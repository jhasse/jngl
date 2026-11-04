// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

/// Experimental Metal backend, see GpuBackend.hpp.
///
/// A 2D backend for the SDL3 platform on macOS and for iOS and tvOS, with the same API as
/// D3D12Renderer (see GpuBackend.hpp). Like it, it doesn't compile GLSL at runtime: all of the
/// shaders, the built-in ones and those of a game's ShaderPrograms, are compiled at build time by
/// jngl-shaderc from GLSL to SPIR-V and translated to the Metal Shading Language, see
/// MslCompiler.hpp and ShaderBytecode. MetalRenderer compiles the MSL when it creates a program.
///
/// This header doesn't include any Objective-C or Metal headers, as it's included by everything
/// that draws, see GpuBackend.hpp. The Metal objects that the structs below hold are retained
/// references (CFBridgingRetain), which are released when they're destroyed through MetalRenderer.
/// Command buffers keep what they use alive, so that can happen in the middle of a frame.
/// MetalRenderer's state lives in its Impl, see MetalRenderer.mm.
/// @file
#pragma once

#include "../gpu/GpuScissor.hpp"
#include "../Renderer.hpp"
#include "../jngl/Rgb.hpp"
#include "../jngl/TextureFilter.hpp"
#include "../spirv/DrawUniforms.hpp"
#include "../spirv/ShaderCompiler.hpp"

#include <array>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

struct SDL_Window;

namespace jngl {

/// GPU resources backing a jngl::Texture on the Metal backend. Created and destroyed through
/// MetalRenderer; owned by jngl::Texture.
struct MetalTexture {
	/// id<MTLTexture>, which destroyTexture() releases
	void* texture = nullptr;
	/// It's sampled with these, see Sprite::setTextureFilter. Mipmaps are sampled like minFilter.
	TextureFilter minFilter = TextureFilter::Bilinear;
	TextureFilter magFilter = TextureFilter::Bilinear;
	int width = 0;
	int height = 0;
	uint32_t mipLevels = 1;
};

/// GPU resources backing a jngl::FrameBuffer on the Metal backend: a texture that can be both
/// rendered into and sampled (\a color).
///
/// An anti-aliased one also has a multisampled texture, which is drawn into instead while
/// anti-aliasing is enabled. Each render pass that draws into it resolves it into \a color when it
/// ends, and it's only kept until the FrameBuffer's done being drawn into: if it's drawn into again
/// without being cleared first, its samples are drawn from \a color, see samplesStale.
struct MetalFramebuffer {
	MetalTexture color;
	/// The other filtering mode, sharing color's texture, see framebufferTexture()
	MetalTexture alternateFilter;
	int width = 0;
	int height = 0;

	void* msaa = nullptr; ///< id<MTLTexture> or nullptr if it isn't anti-aliased
	/// Whether msaa doesn't have color's pixels, as color has been drawn into directly (while
	/// anti-aliasing was disabled) or msaa hasn't been kept, so that msaa has to get them before
	/// it's drawn into again
	bool samplesStale = false;
	/// Whether its textures have 16-bit float channels
	bool hdr = false;
};

/// GPU resources backing a jngl::ShaderProgram on the Metal backend: the compiled shaders and the
/// pipeline states built from them, plus a CPU-side copy of its uniforms (see
/// ShaderProgram::setUniform), which each draw copies, so that setUniform changes in the middle of
/// a frame (e.g. blur pass 0 then 1) are visible to each draw.
struct MetalShaderProgram {
	/// For each render target format and sample count, and blend variant. Each is only built once
	/// it's used.
	static constexpr size_t pipelineCount = 4 * 9;
	/// id<MTLRenderPipelineState>, which destroyShaderProgram() releases
	mutable std::array<void*, pipelineCount> pipelines{};
	void* vertexFunction = nullptr; ///< id<MTLFunction>
	void* fragmentFunction = nullptr; ///< id<MTLFunction>
	std::vector<ShaderUniform> uniforms;
	UniformBlock uniformBlock;
	/// Written by setActiveProgramUniform() while the program is in use
	mutable std::vector<uint8_t> uniformData;
};

class MetalRenderer {
public:
#ifdef IOS
	/// Draws into \a metalLayer, a CAMetalLayer* that's the layer of a UIView (see JNGLView) and
	/// has to outlive the MetalRenderer. Its drawables have the layer's size in pixels.
	explicit MetalRenderer(void* metalLayer);
#else
	/// \a window must have been created with SDL_WINDOW_METAL.
	explicit MetalRenderer(SDL_Window* window);
#endif
	~MetalRenderer();
	MetalRenderer(const MetalRenderer&) = delete;
	MetalRenderer& operator=(const MetalRenderer&) = delete;
	MetalRenderer(MetalRenderer&&) = delete;
	MetalRenderer& operator=(MetalRenderer&&) = delete;

	/// Starts recording a command buffer that begins with clearing the window's drawable
	void beginFrame(Rgb clearColor);
	/// Commits the command buffer and presents the drawable
	void endFrame();
	/// See VulkanRenderer::setViewport
	void setViewport(int x, int y, int width, int height);
	/// Resizes the drawables after the current frame
	void onResize(int width, int height);
	/// The projection matrix that the built-in shaders use
	void setProjection(const Mat4&);
	/// See jngl::drawColored
	void drawColored(PrimitiveType, const float* xyVertices, std::size_t vertexCount,
	                 const Mat3& modelview, Rgba color);

	/// Draws a unit square (-0.5..0.5) with per-corner rounding (see jngl::drawRoundedRect).
	void drawRoundedRect(const Mat3& modelview, Rgba color, float sizeX, float sizeY, float topLeft,
	                     float topRight, float bottomLeft, float bottomRight);

	/// Create a texture from pixel data. \a format and \a type are the OpenGL enums JNGL already
	/// uses (GL_RGBA/GL_RGB/GL_BGR and GL_UNSIGNED_BYTE); pixels come either as \a rowPointers (one
	/// pointer per row) or as a single \a data block. Returns nullptr for unsupported formats.
	/// \param mipmap Generate a mipmap chain and sample it trilinearly when minified
	std::unique_ptr<MetalTexture> createTexture(int width, int height, unsigned int format,
	                                            unsigned int type, const unsigned char* data,
	                                            const unsigned char* const* rowPointers,
	                                            bool mipmap = false);
	/// Releases the texture, which the command buffers that use it keep alive until they're done
	void destroyTexture(MetalTexture&);
	/// Replaces the texture's pixels, and regenerates its mipmaps if it has any. The new ones are
	/// used by all of the current frame's draws, also those that have been recorded already.
	void updateTextureBytes(MetalTexture&, int width, int height, const unsigned char* rgba);
	/// Gives a texture created without mipmaps a mipmap chain generated from its pixels. Does
	/// nothing if it already has one.
	void enableMipmaps(MetalTexture&);
	/// Samples the texture with \a minFilter and \a magFilter from now on, see
	/// Sprite::setTextureFilter. Draws recorded earlier in the frame keep the old ones.
	void setTextureFilter(MetalTexture&, TextureFilter minFilter, TextureFilter magFilter);

	/// Draw a textured primitive (\a xyuv: interleaved x, y, u, v) modulated by \a color.
	void drawSprite(const MetalTexture&, const float* xyuv, std::size_t vertexCount, PrimitiveType,
	                const Mat3& modelview, Rgba color);
	/// Draw a textured primitive with each of its channels (RGBA) mixed towards \a lerpColor (and
	/// an alpha of 0) by \a lerpFactors, like OpenGL's texture shader does.
	void drawSprite(const MetalTexture&, const float* xyuv, std::size_t vertexCount, PrimitiveType,
	                const Mat3& modelview, Rgb lerpColor, std::array<float, 4> lerpFactors);

	/// Reads back a \a width x \a height region of the current frame as tightly packed RGB bytes,
	/// bottom row first (matching OpenGL's glReadPixels). Waits for the pending draws first.
	void readPixels(unsigned char* rgb, int x, int y, int width, int height);

	/// "Metal", the GPU and the version of macOS or iOS, see jngl::getGraphicsApi()
	[[nodiscard]] std::string getGraphicsApi() const;

	/// Creates an off-screen render target (see jngl::FrameBuffer), cleared to transparent.
	/// \param antiAliased Multisampled while anti-aliasing is enabled, if it's supported
	/// \param hdr With 16-bit float channels
	std::unique_ptr<MetalFramebuffer> createFramebuffer(int width, int height, bool antiAliased,
	                                                    bool hdr);
	/// Releases \a framebuffer's textures, which the command buffers that use them keep alive
	void destroyFramebuffer(MetalFramebuffer&);
	/// Samples the framebuffer with the requested filter, without changing its default one.
	const MetalTexture& framebufferTexture(MetalFramebuffer&, TextureFilter);

	/// Redirects subsequent draws into \a framebuffer until popRenderTarget() is called. Render
	/// targets nest (FrameBuffers can be used inside one another).
	void pushRenderTarget(MetalFramebuffer& framebuffer);
	void popRenderTarget();

	/// Clears the render target that's currently bound (the active FrameBuffer, or the screen),
	/// within the scissor, like glClear. Does nothing if no frame has begun.
	void clearCurrentRenderTarget(Rgba color);

	/// Builds a custom shader program (see jngl::ShaderProgram) from MSL translated by jngl-shaderc
	std::unique_ptr<MetalShaderProgram> createShaderProgram(const ShaderBytecode&);
	void destroyShaderProgram(MetalShaderProgram&);
	/// Makes \a program (or nullptr for the built-in sprite shader) the one used by drawSprite.
	void setActiveShaderProgram(const MetalShaderProgram* program);
	/// Writes \a bytes at \a offset into the active program's uniforms (must be bound via use()).
	void setActiveProgramUniform(uint32_t offset, const void* data, uint32_t bytes);

	/// Blending state of everything that's drawn (OpenGL's glEnable/glDisable(GL_BLEND) /
	/// glBlendFunc / glColorMask), which selects among the pipeline states' blend variants.
	void setBlendingEnabled(bool enabled);
	[[nodiscard]] bool isBlendingEnabled() const;
	enum class SpriteBlend : uint8_t {
		Composite,
		Premultiplied,
		AddAlpha,
		Additive,
		PremultipliedAdditive,
		Multiply,
		Screen,
	};
	void setSpriteBlend(SpriteBlend);
	[[nodiscard]] SpriteBlend getSpriteBlend() const;
	void setAlphaOnlyWrite(bool);
	[[nodiscard]] bool isAlphaOnlyWrite() const;

	/// Toggles v-sync, i.e. whether presenting waits for the vertical blank. Always on on iOS.
	void setVerticalSync(bool enabled);
	[[nodiscard]] bool getVerticalSync() const;

	/// Whether 4x MSAA is supported for the window and FrameBuffers.
	[[nodiscard]] bool isMultisampleSupported() const;
	void setMultisampleAntiAliasing(bool enabled);
	[[nodiscard]] bool getMultisampleAntiAliasing() const;

	/// Configures letterboxing: black bars outside a centered \a canvasWidth x \a canvasHeight
	/// area.
	void updateLetterboxing(int framebufferWidth, int framebufferHeight, int canvasWidth,
	                        int canvasHeight);

	/// Clips drawing to \a box (see jngl::applyScissor), or only to the window's letterboxing
	void setScissor(std::optional<ScissorBox> box);

private:
	struct Impl;
	std::unique_ptr<Impl> impl;
};

} // namespace jngl
