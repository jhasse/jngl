// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

/// Experimental Direct3D 12 implementation of the Renderer interface, see Renderer.hpp.
///
/// A 2D backend for the SDL3 platform on Windows, with the same API as VulkanRenderer (see
/// GpuBackend.hpp), except that it doesn't compile shaders at runtime: all of them, the built-in
/// ones and those of a game's ShaderPrograms, are compiled at build time by jngl-shaderc from GLSL
/// to SPIR-V, translated to HLSL and compiled to DXBC, see HlslCompiler.hpp and ShaderBytecode.
///
/// This header doesn't include any Windows or Direct3D headers, as it's included by everything
/// that draws, see GpuBackend.hpp. D3D12Renderer's state lives in its Impl, see
/// D3D12Renderer.cpp.
/// @file
#pragma once

#include "../Renderer.hpp"
#include "../jngl/TextureFilter.hpp"
#include "../spirv/DrawUniforms.hpp"
#include "../spirv/ShaderCompiler.hpp"

#include <array>
#include <chrono>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

struct ID3D12PipelineState;
struct ID3D12Resource;

namespace jngl {

/// Index of a descriptor in one of D3D12Renderer's descriptor heaps
using D3D12Descriptor = uint32_t;
constexpr D3D12Descriptor noD3D12Descriptor = UINT32_MAX;

/// GPU resources backing a jngl::Texture on the Direct3D 12 backend. Created and destroyed through
/// D3D12Renderer; owned by jngl::Texture.
///
/// Unless it's being drawn into, uploaded to or copied from by what's being recorded, its resource
/// is in the PIXEL_SHADER_RESOURCE state.
struct D3D12Texture {
	/// Holds a reference, which destroyTexture() releases
	ID3D12Resource* resource = nullptr;
	/// Of all of its mip levels, in the shader-visible heap
	D3D12Descriptor srv = noD3D12Descriptor;
	/// It's sampled with these, see Sprite::setTextureFilter. Mipmaps are sampled like minFilter.
	TextureFilter minFilter = TextureFilter::Bilinear;
	TextureFilter magFilter = TextureFilter::Bilinear;
	int width = 0;
	int height = 0;
	uint32_t mipLevels = 1;
};

/// GPU resources backing a jngl::FrameBuffer on the Direct3D 12 backend: a texture that can be both
/// rendered into and sampled (\a color), with its render target view.
///
/// An anti-aliased one also has a multisampled texture, which is drawn into instead while
/// anti-aliasing is enabled. It's resolved into \a color once the FrameBuffer is done being drawn
/// into, or before it's drawn into without anti-aliasing.
struct D3D12Framebuffer {
	D3D12Texture color;
	/// The other filtering mode, sharing color's resource and view, see framebufferTexture()
	D3D12Texture alternateFilter;
	D3D12Descriptor rtv = noD3D12Descriptor;
	int width = 0;
	int height = 0;

	ID3D12Resource* msaa = nullptr; ///< nullptr if it isn't anti-aliased
	D3D12Descriptor msaaRtv = noD3D12Descriptor;
	/// Whether color has been drawn into directly (while anti-aliasing was disabled), so that msaa
	/// has to get its pixels before it's drawn into again
	bool samplesStale = false;
	/// Whether msaa has been drawn into since it was last resolved into color
	bool resolvePending = false;
	/// Whether its textures have 16-bit float channels
	bool hdr = false;
};

/// GPU resources backing a jngl::ShaderProgram on the Direct3D 12 backend: the compiled shaders and
/// the pipeline states built from them, plus a CPU-side copy of its uniforms (see
/// ShaderProgram::setUniform), which each draw copies, so that setUniform changes in the middle of
/// a frame (e.g. blur pass 0 then 1) are visible to each draw.
struct D3D12ShaderProgram {
	/// For each render target format and sample count, blend variant and primitive topology type
	/// (triangles or lines). Each is only built once it's used.
	static constexpr size_t pipelineCount = 4 * 9 * 2;
	/// Each holds a reference, which destroyShaderProgram() releases
	mutable std::array<ID3D12PipelineState*, pipelineCount> pipelines{};
	std::vector<uint8_t> vertexShader; ///< DXBC
	std::vector<uint8_t> pixelShader; ///< DXBC
	std::vector<ShaderUniform> uniforms;
	UniformBlock uniformBlock;
	/// Written by setActiveProgramUniform() while the program is in use
	mutable std::vector<uint8_t> uniformData;
};

/// The timestamp queries of a jngl::GpuTimer, see D3D12Renderer.cpp
struct D3D12TimestampPool;

class D3D12Renderer final : public Renderer {
public:
	/// \a nativeWindow must be an SDL_Window*, created without SDL_WINDOW_OPENGL.
	explicit D3D12Renderer(void* nativeWindow);
	~D3D12Renderer() override;

	[[nodiscard]] const char* name() const override;
	void beginFrame(Rgb clearColor) override;
	void endFrame() override;
	void setViewport(int x, int y, int width, int height) override;
	void onResize(int width, int height) override;
	void setProjection(const Mat4&) override;
	void drawColored(PrimitiveType, const float* xyVertices, std::size_t vertexCount,
	                 const Mat3& modelview, Rgba color) override;

	/// Draws a unit square (-0.5..0.5) with per-corner rounding (see jngl::drawRoundedRect).
	void drawRoundedRect(const Mat3& modelview, Rgba color, float sizeX, float sizeY, float topLeft,
	                     float topRight, float bottomLeft, float bottomRight);

	/// Create a texture from pixel data. \a format and \a type are the OpenGL enums JNGL already
	/// uses (GL_RGBA/GL_RGB/GL_BGR and GL_UNSIGNED_BYTE); pixels come either as \a rowPointers (one
	/// pointer per row) or as a single \a data block. Returns nullptr for unsupported formats.
	/// \param mipmap Generate a mipmap chain and sample it trilinearly when minified
	std::unique_ptr<D3D12Texture> createTexture(int width, int height, unsigned int format,
	                                            unsigned int type, const unsigned char* data,
	                                            const unsigned char* const* rowPointers,
	                                            bool mipmap = false);
	/// Frees the texture's resources once the GPU is done with them
	void destroyTexture(D3D12Texture&);
	/// Replaces the texture's pixels, and regenerates its mipmaps if it has any. The new ones are
	/// used by all of the current frame's draws, also those that have been recorded already.
	void updateTextureBytes(D3D12Texture&, int width, int height, const unsigned char* rgba);
	/// Gives a texture created without mipmaps a mipmap chain generated from its pixels. Does
	/// nothing if it already has one.
	void enableMipmaps(D3D12Texture&);
	/// Samples the texture with \a minFilter and \a magFilter from now on, see
	/// Sprite::setTextureFilter. Draws recorded earlier in the frame keep the old ones.
	void setTextureFilter(D3D12Texture&, TextureFilter minFilter, TextureFilter magFilter);

	/// Draw a textured primitive (\a xyuv: interleaved x, y, u, v) modulated by \a color.
	void drawSprite(const D3D12Texture&, const float* xyuv, std::size_t vertexCount, PrimitiveType,
	                const Mat3& modelview, Rgba color);
	/// Draw a textured primitive with each of its channels (RGBA) mixed towards \a lerpColor (and
	/// an alpha of 0) by \a lerpFactors, like OpenGL's texture shader does.
	void drawSprite(const D3D12Texture&, const float* xyuv, std::size_t vertexCount, PrimitiveType,
	                const Mat3& modelview, Rgb lerpColor, std::array<float, 4> lerpFactors);

	/// Reads back a \a width x \a height region of the current frame as tightly packed RGB bytes,
	/// bottom row first (matching OpenGL's glReadPixels). Waits for the pending draws first.
	void readPixels(unsigned char* rgb, int x, int y, int width, int height);

	/// "Direct3D 12" and the adapter, with its driver version, see jngl::getGraphicsApi()
	[[nodiscard]] std::string getGraphicsApi() const;

	/// Whether the GPU's time can be measured with timestamp queries, see jngl::GpuTimer
	[[nodiscard]] bool areTimestampsSupported() const;
	using TimestampPool = D3D12TimestampPool*;
	/// A pool of \a count timestamp queries, none of which have been written
	[[nodiscard]] TimestampPool createTimestampPool(uint32_t count);
	/// Destroys \a pool once the GPU is done with it
	void destroyTimestampPool(TimestampPool pool);
	/// Records writing the GPU's time into \a query once what's been drawn before is done. Returns
	/// false if there's no frame to record it into.
	bool writeTimestamp(TimestampPool pool, uint32_t query);
	/// The time between the timestamps \a first and \a first + 1, once both have been written, or
	/// nullopt. Then forgets them, so that they can be written again.
	std::optional<std::chrono::nanoseconds> takeElapsedTime(TimestampPool pool, uint32_t first);

	/// Creates an off-screen render target (see jngl::FrameBuffer), cleared to transparent.
	/// \param antiAliased Multisampled while anti-aliasing is enabled, if it's supported
	/// \param hdr With 16-bit float channels
	std::unique_ptr<D3D12Framebuffer> createFramebuffer(int width, int height, bool antiAliased,
	                                                    bool hdr);
	/// Schedules \a framebuffer's resources for destruction once the GPU is done with them.
	void destroyFramebuffer(D3D12Framebuffer&);
	/// Samples the framebuffer with the requested filter, without changing its default one.
	const D3D12Texture& framebufferTexture(D3D12Framebuffer&, TextureFilter);

	/// Redirects subsequent draws into \a framebuffer until popRenderTarget() is called. Render
	/// targets nest (FrameBuffers can be used inside one another).
	void pushRenderTarget(D3D12Framebuffer& framebuffer);
	void popRenderTarget();

	/// Clears the render target that's currently bound (the active FrameBuffer, or the screen),
	/// within the scissor, like glClear. Does nothing if no frame has begun.
	void clearCurrentRenderTarget(Rgba color);

	/// Builds a custom shader program (see jngl::ShaderProgram) from DXBC compiled by jngl-shaderc
	std::unique_ptr<D3D12ShaderProgram> createShaderProgram(const ShaderBytecode&);
	void destroyShaderProgram(D3D12ShaderProgram&);
	/// Makes \a program (or nullptr for the built-in sprite shader) the one used by drawSprite.
	void setActiveShaderProgram(const D3D12ShaderProgram* program);
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

	/// Toggles v-sync, i.e. whether presenting waits for the vertical blank.
	void setVerticalSync(bool enabled);
	[[nodiscard]] bool getVerticalSync() const;

	/// Whether 4x MSAA is supported for the swapchain and FrameBuffers.
	[[nodiscard]] bool isMultisampleSupported() const;
	void setMultisampleAntiAliasing(bool enabled);
	[[nodiscard]] bool getMultisampleAntiAliasing() const;

	/// Configures letterboxing: black bars outside a centered \a canvasWidth x \a canvasHeight
	/// area.
	void updateLetterboxing(int framebufferWidth, int framebufferHeight, int canvasWidth,
	                        int canvasHeight);

	/// OpenGL-style scissor box (origin bottom-left) in the pixels of the current render target
	struct ScissorBox {
		int x = 0;
		int y = 0;
		int width = 0;
		int height = 0;
	};
	/// Clips drawing to \a box (see jngl::applyScissor), or only to the window's letterboxing
	void setScissor(std::optional<ScissorBox> box);
	[[nodiscard]] int getFramebufferWidth() const;
	[[nodiscard]] int getFramebufferHeight() const;

private:
	struct Impl;
	std::unique_ptr<Impl> impl;
};

} // namespace jngl
