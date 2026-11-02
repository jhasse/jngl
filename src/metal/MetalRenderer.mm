// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "MetalRenderer.hpp"

#include "../App.hpp"
#include "../jngl/Alpha.hpp"
#include "../jngl/Mat3.hpp"
#include "../jngl/Mat4.hpp"
#include "../log.hpp"
#include "MslCompiler.hpp"

#include "BuiltinBytecode.hpp"

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_metal.h>
#include <algorithm>
#include <bit>
#include <cassert>
#include <cstring>
#include <format>
#include <span>
#include <stdexcept>

// Every public function of MetalRenderer that might create autoreleased objects drains them with
// @autoreleasepool, as nothing else would: SDL's event loop isn't NSApplication's.

namespace jngl {

namespace {

/// The kinds of render targets there are pipeline states for, as a pipeline state only works with
/// render targets that have the same format and number of samples: 8-bit (the window's format, also
/// used by FrameBuffers) or 16-bit float (HDR FrameBuffers), each without or with MSAA
enum class PassKind : uint8_t { Ldr, LdrMsaa, Hdr, HdrMsaa };
constexpr size_t passKindCount = 4;

bool isHdr(const PassKind kind) {
	return kind == PassKind::Hdr || kind == PassKind::HdrMsaa;
}

bool isMultisampled(const PassKind kind) {
	return kind == PassKind::LdrMsaa || kind == PassKind::HdrMsaa;
}

PassKind passKind(const bool hdr, const bool multisampled) {
	if (hdr) {
		return multisampled ? PassKind::HdrMsaa : PassKind::Hdr;
	}
	return multisampled ? PassKind::LdrMsaa : PassKind::Ldr;
}

/// How a pipeline state blends what it draws with what's already there, see Impl::blendVariant()
enum class BlendVariant : uint8_t {
	Composite, ///< alpha composited, too; the default on the window, see BlendMode::Composite
	Premultiplied, ///< see BlendMode::Premultiplied
	Replace, ///< see jngl::disableBlending()
	ReplaceAlphaOnly, ///< see jngl::drawOnlyIntoAlphaChannel()
	AddAlpha, ///< alpha added up; the default while a FrameBuffer's in use, like on OpenGL
	Additive, ///< see BlendMode::Additive
	PremultipliedAdditive, ///< see BlendMode::PremultipliedAdditive
	Multiply, ///< see BlendMode::Multiply
	Screen, ///< see BlendMode::Screen
};
constexpr size_t blendVariantCount = 9;

/// Why a render pass ends, which decides what happens to a multisampled texture that it draws into,
/// see Impl::unbindTarget()
enum class PassEnd : uint8_t {
	Interrupted, ///< something else is drawn into, after which the pass continues
	Read, ///< the frame's texture is read, see MetalRenderer::readPixels
	FrameEnd, ///< the frame's presented
	Popped, ///< the FrameBuffer it draws into is done being drawn into, see popRenderTarget()
};

/// Index into the pipeline state arrays, see MetalShaderProgram::pipelines. Unlike Direct3D 12's,
/// Metal's pipeline states work with every primitive type.
size_t pipelineIndex(const PassKind kind, const BlendVariant variant) {
	return static_cast<size_t>(kind) * blendVariantCount + static_cast<size_t>(variant);
}
static_assert(MetalShaderProgram::pipelineCount == passKindCount * blendVariantCount);

/// Of the window's drawables, as CAMetalLayer doesn't support RGBA, and of FrameBuffers, so that
/// the same pipeline states work for both
constexpr MTLPixelFormat ldrFormat = MTLPixelFormatBGRA8Unorm;
/// Of textures, whose pixels come as RGBA, see toRgba8
constexpr MTLPixelFormat textureFormat = MTLPixelFormatRGBA8Unorm;
/// Of HDR FrameBuffers, which every Mac can render into, blend and sample with linear filtering
constexpr MTLPixelFormat hdrFormat = MTLPixelFormatRGBA16Float;
constexpr NSUInteger msaaSampleCount = 4;

/// Where the shaders have their uniforms, see MslCompiler.hpp
constexpr NSUInteger uniformsIndex = 0;
/// Where the vertex shaders have their vertices, out of the way of what SPIRV-Cross assigns
constexpr NSUInteger verticesIndex = 30;
/// What setVertexBytes and setFragmentBytes can pass at most; anything bigger goes through
/// Impl::vertexBuffers
constexpr size_t maxInlineBytes = 4096;
/// Of what's in Impl::vertexBuffers, which is enough for vertices and uniforms on every Mac
constexpr size_t bufferOffsetAlignment = 256;

/// Turns a four-vertex fan into two triangles, keeping the same diagonal, winding and provoking
/// vertices, as Metal can't draw triangle fans
template <size_t floatsPerVertex>
std::array<float, 6 * floatsPerVertex> triangulateQuad(const float* vertices) {
	constexpr std::array<size_t, 6> indices{ 1, 2, 0, 2, 3, 0 };
	std::array<float, 6 * floatsPerVertex> triangles;
	for (size_t i = 0; i < indices.size(); ++i) {
		std::copy_n(vertices + indices[i] * floatsPerVertex, floatsPerVertex,
		            triangles.data() + i * floatsPerVertex);
	}
	return triangles;
}

/// Like triangulateQuad, preserving the fan's winding and provoking vertices.
template <size_t floatsPerVertex>
void triangulateFan(const float* vertices, const size_t vertexCount,
                    std::vector<float>& triangles) {
	triangles.resize((vertexCount - 2) * 3 * floatsPerVertex);
	float* out = triangles.data();
	for (size_t i = 1; i + 1 < vertexCount; ++i) {
		for (const size_t index : { i, i + 1, size_t{ 0 } }) {
			out = std::copy_n(vertices + index * floatsPerVertex, floatsPerVertex, out);
		}
	}
}

/// A quad covering all of the render target in clip space, with texture coordinates that map the
/// top left corner of the render target to the top left corner of the texture. With the identity
/// as mvp, the textured shader copies a texture this way.
constexpr std::array<float, 24> fullscreenQuad = {
	-1.f, 1.f,  0.f, 0.f, // x, y, u, v
	1.f,  1.f,  1.f, 0.f, //
	-1.f, -1.f, 0.f, 1.f, //
	1.f,  1.f,  1.f, 0.f, //
	1.f,  -1.f, 1.f, 1.f, //
	-1.f, -1.f, 0.f, 1.f, //
};

/// fullscreenQuad without the texture coordinates, for the colored shader
constexpr std::array<float, 12> fullscreenTriangles = {
	-1.f, 1.f, 1.f, 1.f, -1.f, -1.f, 1.f, 1.f, 1.f, -1.f, -1.f, -1.f,
};

void configureBlending(MTLRenderPipelineColorAttachmentDescriptor* const attachment,
                       const BlendVariant variant) {
	attachment.blendingEnabled = YES;
	attachment.sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
	attachment.destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
	attachment.rgbBlendOperation = MTLBlendOperationAdd;
	attachment.sourceAlphaBlendFactor = MTLBlendFactorOne;
	attachment.destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
	attachment.alphaBlendOperation = MTLBlendOperationAdd;
	attachment.writeMask = MTLColorWriteMaskAll;
	switch (variant) {
	case BlendVariant::Composite:
		break;
	case BlendVariant::Premultiplied:
		attachment.sourceRGBBlendFactor = MTLBlendFactorOne;
		break;
	case BlendVariant::Replace:
		attachment.blendingEnabled = NO;
		break;
	case BlendVariant::ReplaceAlphaOnly:
		attachment.blendingEnabled = NO;
		attachment.writeMask = MTLColorWriteMaskAlpha;
		break;
	case BlendVariant::AddAlpha:
		// So that what's drawn into a FrameBuffer is as opaque as the most opaque of it when it's
		// drawn again, see applyBlendMode()
		attachment.destinationAlphaBlendFactor = MTLBlendFactorOne;
		break;
	// The following ones blend alpha like the colors, as glBlendFunc does
	case BlendVariant::Additive:
		attachment.destinationRGBBlendFactor = MTLBlendFactorOne;
		attachment.sourceAlphaBlendFactor = MTLBlendFactorSourceAlpha;
		attachment.destinationAlphaBlendFactor = MTLBlendFactorOne;
		break;
	case BlendVariant::PremultipliedAdditive:
		attachment.sourceRGBBlendFactor = MTLBlendFactorOne;
		attachment.destinationRGBBlendFactor = MTLBlendFactorOne;
		attachment.destinationAlphaBlendFactor = MTLBlendFactorOne;
		break;
	case BlendVariant::Multiply:
		attachment.sourceRGBBlendFactor = MTLBlendFactorDestinationColor;
		attachment.sourceAlphaBlendFactor = MTLBlendFactorDestinationAlpha;
		break;
	case BlendVariant::Screen:
		attachment.sourceRGBBlendFactor = MTLBlendFactorOne;
		attachment.destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceColor;
		break;
	}
}

MTLPrimitiveType toMetalPrimitive(const PrimitiveType type) {
	switch (type) {
	case PrimitiveType::Triangles:
	case PrimitiveType::TriangleFan: // see triangulateFan
		return MTLPrimitiveTypeTriangle;
	case PrimitiveType::TriangleStrip:
		return MTLPrimitiveTypeTriangleStrip;
	case PrimitiveType::Lines:
		return MTLPrimitiveTypeLine;
	}
	return MTLPrimitiveTypeTriangle;
}

size_t alignUp(const size_t value, const size_t alignment) {
	return (value + alignment - 1) & ~(alignment - 1);
}

/// Number of levels down to 1x1, like glGenerateMipmap creates
uint32_t mipLevelCount(const int width, const int height) {
	return static_cast<uint32_t>(std::bit_width(static_cast<uint32_t>(std::max(width, height))));
}

// OpenGL pixel-format enums JNGL passes to Texture/createTexture (avoids including GL headers
// here).
constexpr unsigned int glRGB = 0x1907;
constexpr unsigned int glRGBA = 0x1908;
constexpr unsigned int glBGR = 0x80E0;
constexpr unsigned int glUnsignedByte = 0x1401;

// Repacks pixel data described by an OpenGL \a format into a tightly packed RGBA8 buffer. Pixels
// arrive either as \a rowPointers (one per row) or as a single \a data block.
std::vector<unsigned char> toRgba8(int width, int height, unsigned int format,
                                   const unsigned char* data,
                                   const unsigned char* const* rowPointers) {
	const auto channels = format == glRGBA ? 4 : 3;
	const bool swapRedBlue = format == glBGR;
	std::vector<unsigned char> out(static_cast<size_t>(width) * height * 4);
	for (int y = 0; y < height; ++y) {
		const unsigned char* row =
		    rowPointers ? rowPointers[y] : data + static_cast<size_t>(y) * width * channels;
		for (int x = 0; x < width; ++x) {
			const unsigned char* src = row + static_cast<size_t>(x) * channels;
			unsigned char* dst = out.data() + (static_cast<size_t>(y) * width + x) * 4;
			dst[0] = swapRedBlue ? src[2] : src[0];
			dst[1] = src[1];
			dst[2] = swapRedBlue ? src[0] : src[2];
			dst[3] = channels == 4 ? src[3] : 255;
		}
	}
	return out;
}

/// Index of the sampler in Impl::samplers that filters like this
size_t samplerIndex(const TextureFilter minFilter, const TextureFilter magFilter) {
	return (minFilter == TextureFilter::NearestNeighbor ? 1 : 0) |
	       (magFilter == TextureFilter::NearestNeighbor ? 2 : 0);
}

std::string toString(NSString* const string) {
	return string ? std::string(string.UTF8String) : std::string();
}

/// The MSL that jngl-shaderc has padded with NUL characters
std::string mslSource(const std::span<const uint32_t> words) {
	const auto* const chars = reinterpret_cast<const char*>(words.data());
	return { chars, strnlen(chars, words.size_bytes()) };
}

/// Compiles \a msl and returns its function \a entryPoint, see MslCompiler.hpp
id<MTLFunction> compileFunction(const id<MTLDevice> device, const std::string& msl,
                                const char* const entryPoint) {
	@autoreleasepool {
		MTLCompileOptions* const options = [MTLCompileOptions new];
		options.languageVersion = MTLLanguageVersion2_1; // see MslCompiler.cpp
		NSError* error = nil;
		const id<MTLLibrary> library = [device newLibraryWithSource:@(msl.c_str())
		                                                    options:options
		                                                      error:&error];
		if (!library) {
			throw std::runtime_error("Failed to compile Metal shader: " +
			                         toString(error.localizedDescription) + "\nMSL:\n" + msl);
		}
		const id<MTLFunction> function = [library newFunctionWithName:@(entryPoint)];
		if (!function) {
			throw std::runtime_error(std::string("Metal shader has no function ") + entryPoint +
			                         ".");
		}
		return function;
	}
}

/// Its vertex and fragment shader
struct ShaderFunctions {
	id<MTLFunction> vertex;
	id<MTLFunction> fragment;
};

ShaderFunctions compileFunctions(const id<MTLDevice> device, const ShaderBytecode& program) {
	return { compileFunction(device, mslSource(program.vertex), metalVertexEntryPoint),
		     compileFunction(device, mslSource(program.fragment), metalFragmentEntryPoint) };
}

/// Buffer that's appended to during a frame, see Impl::vertexBuffers
struct UploadBuffer {
	id<MTLBuffer> buffer;
	size_t used = 0;
};

} // namespace

struct MetalRenderer::Impl {
	explicit Impl(SDL_Window*);
	~Impl();
	Impl(const Impl&) = delete;
	Impl& operator=(const Impl&) = delete;
	Impl(Impl&&) = delete;
	Impl& operator=(Impl&&) = delete;

	void createSamplers();
	[[nodiscard]] id<MTLRenderPipelineState> createPipeline(const ShaderFunctions&, bool textured,
	                                                        PassKind, BlendVariant) const;
	enum class Builtin : uint8_t { Colored, Rounded, Textured };
	/// The built-in pipeline state for this, built when it's first used
	id<MTLRenderPipelineState> builtinPipeline(Builtin, PassKind, BlendVariant);
	/// \a program's pipeline state for this, built when it's first used
	id<MTLRenderPipelineState> programPipeline(const MetalShaderProgram&, PassKind,
	                                           BlendVariant) const;
	[[nodiscard]] BlendVariant blendVariant() const;

	/// Copies \a bytes of \a data into the current frame's buffer, growing it if it's full, and
	/// returns where it is
	size_t upload(const void* data, size_t bytes);
	/// Makes \a data the vertices of the next draws
	void bindVertices(const void* data, size_t bytes);
	/// Writes modelview and projection into drawUniforms
	void writeTransform(const UniformBlock& block, const Mat3& modelview);
	/// Makes \a data the uniforms of the next draws
	void bindUniforms(const UniformBlock& block, const std::vector<uint8_t>& data);

	/// Starts recording the frame's command buffer
	void beginCommandBuffer();
	/// The blit encoder of the upload command buffer, which is committed ahead of the frame's, so
	/// that the uploads are done before any of its draws. Starts recording it if it isn't yet.
	id<MTLBlitCommandEncoder> uploadBlit();
	/// The upload command buffer without a blit encoder, for render passes
	id<MTLCommandBuffer> uploadCommands();
	/// Commits the pending uploads
	void submitUploads();
	/// Waits until the GPU's done with everything that's been committed
	void waitIdle();

	/// Gets the drawable's texture, or one that's drawn into instead while the window can't be
	/// seen, and the multisampled texture, if that hasn't happened yet this frame. That's as late as
	/// possible, i.e. once the window's drawn into, as nextDrawable waits for the window server to
	/// hand one back, while what's drawn into FrameBuffers before can be recorded already.
	void acquireFrameTexture();
	/// The multisampled texture that's drawn into instead of the frame's with MSAA, and resolved
	/// into it at the end of each render pass
	void updateMsaaTarget();
	[[nodiscard]] bool swapchainMsaaActive() const;

	/// Begins a render pass into the top of renderTargetStack, or the frame's texture, if there
	/// isn't one yet. Draws and clears call this. With \a clear, the pass clears all of what's
	/// drawn into first, otherwise a pass into the frame's texture does so if screenClearPending.
	void bindTarget(std::optional<MTLClearColor> clear = std::nullopt);
	/// Ends the render pass, which resolves what's been drawn into a multisampled texture. The next
	/// draw calls bindTarget() again.
	///
	/// The samples are only stored if they might be drawn into again, as \a end says: at the
	/// window's size, storing and loading them takes up a lot of the GPU's time. (Not resolving them
	/// when the pass is only interrupted isn't possible, as Metal only lets a pass that has a
	/// texture to resolve into choose between resolving and storing as well.)
	void unbindTarget(PassEnd end = PassEnd::Interrupted);
	/// Ends the render pass, so that the frame's texture has all that's been drawn into the window
	void finishScreen(PassEnd);
	/// Clears all of what's drawn into, whatever the scissor
	void clearTarget(MTLClearColor);
	/// Clears the swap chain's image to the clear color, and the letterboxing's bars to black. Until
	/// it's drawn into, that's only noted in screenClearPending.
	void clearSwapchain();
	/// Fills \a rect of what's drawn into with \a color, without blending
	void fillRect(PixelRect rect, MTLClearColor color);
	/// Draws \a framebuffer's color into all samples of its multisampled texture, in a render pass
	/// of its own, as color is what the pass that draws into the samples resolves them into
	void expandIntoSamples(MetalFramebuffer& framebuffer);
	/// Whether clip space's y has to be inverted for what's drawn now: FrameBuffers are drawn into
	/// upside down like OpenGL FBO textures (see jngl::FrameBuffer::draw), which the Vulkan backend
	/// gets by flipping its viewport, which Metal's viewports can't be. See D3D12Renderer.
	[[nodiscard]] bool flipClipY() const;
	void draw(id<MTLRenderPipelineState>, const UniformBlock&, const std::vector<uint8_t>& uniforms,
	          const void* vertices, size_t vertexCount, size_t stride, PrimitiveType);

	void applyViewport() const;
	void applyScissor();
	/// What drawing into what's drawn on is clipped to, see GpuScissor::clip
	[[nodiscard]] PixelRect currentScissor() const;
	/// The frame's texture's size, or the FrameBuffer's that's drawn on
	[[nodiscard]] std::pair<int, int> currentTargetSize() const;

	/// A texture with \a mipLevels that can be sampled, and which has to be in GPU-only memory
	[[nodiscard]] id<MTLTexture> createTexture(int width, int height, uint32_t mipLevels,
	                                           MTLPixelFormat, MTLTextureUsage,
	                                           NSUInteger samples = 1) const;

	/// Maximum number of frames the CPU may be working on ahead of the GPU.
	static constexpr uint32_t maxFramesInFlight = 2;

	SDL_Window* window;
	SDL_MetalView view = nullptr;
	CAMetalLayer* layer = nil;

	id<MTLDevice> device;
	id<MTLCommandQueue> queue;
	/// Waited for by beginFrame(), signaled once the GPU is done with a frame
	dispatch_semaphore_t frameSemaphore;
	std::array<id<MTLSamplerState>, 4> samplers; ///< see samplerIndex()

	// The shaders of the built-in programs, see BuiltinShaders.hpp, indexed by Builtin
	std::array<ShaderFunctions, 3> builtinFunctions;
	// The uniforms of the built-in shaders
	UniformBlock coloredUniforms;
	UniformBlock roundedUniforms;
	UniformBlock texturedUniforms;
	// Reused for writing each draw's uniforms, see prepareDrawUniforms()
	std::vector<uint8_t> drawUniforms;
	/// texturedUniforms that make it copy the texture, see expandIntoSamples()
	std::vector<uint8_t> copyUniforms;
	// Indexed by Builtin and pipelineIndex(), see builtinPipeline()
	std::array<std::array<id<MTLRenderPipelineState>, MetalShaderProgram::pipelineCount>, 3>
	    builtinPipelines;

	id<MTLCommandBuffer> commandBuffer; ///< of the current frame
	id<MTLRenderCommandEncoder> encoder; ///< of the current render pass, see bindTarget()
	id<MTLCommandBuffer> uploadCommandBuffer; ///< see uploadBlit()
	id<MTLBlitCommandEncoder> uploadBlitEncoder;

	// Per frame in flight, written directly each frame. `used` is reset in beginFrame and grows as
	// the draws that don't fit into setVertexBytes append to it. A full one is replaced, as the
	// command buffers that use it keep it alive.
	std::array<UploadBuffer, maxFramesInFlight> vertexBuffers;
	// Reused when expanding triangle fans
	std::vector<float> triangulatedVertices;

	/// nil while the window can't be seen
	id<CAMetalDrawable> drawable;
	/// What's drawn into instead of a drawable while the window can't be seen, see
	/// acquireFrameTexture()
	id<MTLTexture> offscreenTarget;
	/// The drawable's texture or offscreenTarget
	id<MTLTexture> frameTexture;
	id<MTLTexture> msaaTarget; ///< see updateMsaaTarget()
	int swapchainWidth = 0;
	int swapchainHeight = 0;

	uint32_t currentFrame = 0;
	bool frameActive = false;
	// The drawables have to be resized after the current frame has been presented
	bool drawableResizePending = false;

	// Stack of active FrameBuffers; empty means drawing targets the window.
	std::vector<MetalFramebuffer*> renderTargetStack;
	/// Whether the frame's texture still has to be cleared by clearSwapchain(), which the first
	/// pass into it does. Starting a frame with one only to clear it would have its samples stored
	/// and loaded again when the frame first draws into FrameBuffers, as it usually does.
	bool screenClearPending = false;
	/// Whether encoder draws into the window, and into its multisampled texture
	bool encoderOnScreen = false;
	bool encoderOnScreenSamples = false;
	/// The FrameBuffer whose multisampled texture encoder draws into, if it does
	MetalFramebuffer* encoderSamplesOf = nullptr;
	// What's drawn into now, which decides the pipeline states that have to be used
	PassKind currentPassKind = PassKind::Ldr;
	// Whether the scissor doesn't let anything through, which Metal's can't be set to
	bool scissorEmpty = false;

	const MetalShaderProgram* activeShaderProgram = nullptr;
	bool blendingEnabled = true;
	SpriteBlend spriteBlend = SpriteBlend::Composite;
	bool alphaOnlyWrite = false;

	bool verticalSyncEnabled = true;
	bool msaaSupported = false;
	bool msaaEnabled = true;
	bool msaaRebuildPending = false;
	bool pendingMsaaEnabled = true;

	GpuScissor scissor;

	Mat4 projection;
	Rgb currentClearColor{ 0, 0, 0 }; // swapchain clear color for the current frame
};

MetalRenderer::Impl::Impl(SDL_Window* const window) : window(window) {
	device = MTLCreateSystemDefaultDevice();
	if (!device) {
		throw std::runtime_error("No Metal device found.");
	}
	internal::debug("Metal: using {}", toString(device.name));
	view = SDL_Metal_CreateView(window);
	if (!view) {
		throw std::runtime_error(std::string("Couldn't create a Metal view: ") + SDL_GetError());
	}
	layer = (__bridge CAMetalLayer*)SDL_Metal_GetLayer(view);
	layer.device = device;
	layer.pixelFormat = ldrFormat;
	// readPixels copies from the drawables
	layer.framebufferOnly = NO;
	// Without a color space, so that colors written by the shaders reach the screen unmodified,
	// matching JNGL's OpenGL backend whose default framebuffer isn't color managed
	layer.colorspace = nil;
	int width = 0;
	int height = 0;
	SDL_GetWindowSizeInPixels(window, &width, &height);
	layer.drawableSize = CGSizeMake(std::max(width, 1), std::max(height, 1));
	swapchainWidth = std::max(width, 1);
	swapchainHeight = std::max(height, 1);

	queue = [device newCommandQueue];
	frameSemaphore = dispatch_semaphore_create(maxFramesInFlight);
	createSamplers();
	msaaSupported = [device supportsTextureSampleCount:msaaSampleCount];
	if (!msaaSupported) {
		msaaEnabled = false;
	}
	internal::debug("Metal MSAA 4x: {} (enabled: {}).", msaaSupported ? "supported" : "unsupported",
	                msaaEnabled);

	builtinFunctions = { compileFunctions(device, builtin_bytecode::colored),
		                 compileFunctions(device, builtin_bytecode::rounded),
		                 compileFunctions(device, builtin_bytecode::textured) };
	coloredUniforms =
	    uniformBlock(builtin_bytecode::colored.uniforms, builtin_bytecode::colored.uniformBlockSize,
	                 builtin_bytecode::colored.pushConstants);
	roundedUniforms =
	    uniformBlock(builtin_bytecode::rounded.uniforms, builtin_bytecode::rounded.uniformBlockSize,
	                 builtin_bytecode::rounded.pushConstants);
	texturedUniforms = uniformBlock(builtin_bytecode::textured.uniforms,
	                                builtin_bytecode::textured.uniformBlockSize,
	                                builtin_bytecode::textured.pushConstants);
	// Only those of the window's default blending, the others are built once they're used
	for (const auto builtin : { Builtin::Colored, Builtin::Textured }) {
		builtinPipeline(builtin, PassKind::Ldr, BlendVariant::Composite);
		if (msaaSupported) {
			builtinPipeline(builtin, PassKind::LdrMsaa, BlendVariant::Composite);
		}
	}
	for (auto& vertexBuffer : vertexBuffers) {
		vertexBuffer.buffer = [device newBufferWithLength:size_t{ 1 } << 20
		                                          options:MTLResourceStorageModeShared];
	}
	// The identity as modelview and projection, and no mixing towards lerpColor
	copyUniforms.resize(texturedUniforms.size);
	writeUniform(copyUniforms, texturedUniforms.offsets.modelview, Mat3());
	writeUniform(copyUniforms, texturedUniforms.offsets.projection, Mat4());
	internal::debug("Metal renderer initialized ({}x{}).", swapchainWidth, swapchainHeight);
}

MetalRenderer::Impl::~Impl() {
	@autoreleasepool {
		if (frameActive) {
			// Its encoders have to be ended before they're released, and the semaphore has to be
			// back at its initial value before it's released
			unbindTarget();
			const dispatch_semaphore_t semaphore = frameSemaphore;
			[commandBuffer addCompletedHandler:^(id<MTLCommandBuffer>) {
			  dispatch_semaphore_signal(semaphore);
			}];
			submitUploads();
			[commandBuffer commit];
		}
		submitUploads();
		if (frameSemaphore) {
			// Wait until the completion handlers of all frames have been called
			for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
				dispatch_semaphore_wait(frameSemaphore, DISPATCH_TIME_FOREVER);
			}
			for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
				dispatch_semaphore_signal(frameSemaphore);
			}
		}
		drawable = nil;
		frameTexture = nil;
		layer = nil;
	}
	if (view) {
		SDL_Metal_DestroyView(view);
	}
}

void MetalRenderer::Impl::createSamplers() {
	for (const auto minFilter : { TextureFilter::Bilinear, TextureFilter::NearestNeighbor }) {
		for (const auto magFilter : { TextureFilter::Bilinear, TextureFilter::NearestNeighbor }) {
			MTLSamplerDescriptor* const desc = [MTLSamplerDescriptor new];
			// Mipmaps are sampled like the minification, like GL_LINEAR_MIPMAP_LINEAR or
			// GL_NEAREST_MIPMAP_NEAREST
			const bool linearMin = minFilter == TextureFilter::Bilinear;
			desc.minFilter = linearMin ? MTLSamplerMinMagFilterLinear : MTLSamplerMinMagFilterNearest;
			desc.mipFilter = linearMin ? MTLSamplerMipFilterLinear : MTLSamplerMipFilterNearest;
			desc.magFilter = magFilter == TextureFilter::Bilinear ? MTLSamplerMinMagFilterLinear
			                                                      : MTLSamplerMinMagFilterNearest;
			desc.sAddressMode = MTLSamplerAddressModeClampToEdge;
			desc.tAddressMode = MTLSamplerAddressModeClampToEdge;
			samplers[samplerIndex(minFilter, magFilter)] = [device newSamplerStateWithDescriptor:desc];
		}
	}
}

id<MTLRenderPipelineState> MetalRenderer::Impl::createPipeline(const ShaderFunctions& functions,
                                                               const bool textured,
                                                               const PassKind kind,
                                                               const BlendVariant variant) const {
	@autoreleasepool {
		MTLRenderPipelineDescriptor* const desc = [MTLRenderPipelineDescriptor new];
		desc.vertexFunction = functions.vertex;
		desc.fragmentFunction = functions.fragment;
		// Only the position, or with texture coordinates, see ShaderProgram::getAttribLocation
		MTLVertexDescriptor* const vertexDesc = [MTLVertexDescriptor vertexDescriptor];
		vertexDesc.attributes[0].format = MTLVertexFormatFloat2;
		vertexDesc.attributes[0].offset = 0;
		vertexDesc.attributes[0].bufferIndex = verticesIndex;
		if (textured) {
			vertexDesc.attributes[1].format = MTLVertexFormatFloat2;
			vertexDesc.attributes[1].offset = 2 * sizeof(float);
			vertexDesc.attributes[1].bufferIndex = verticesIndex;
		}
		vertexDesc.layouts[verticesIndex].stride = (textured ? 4 : 2) * sizeof(float);
		desc.vertexDescriptor = vertexDesc;
		desc.colorAttachments[0].pixelFormat = isHdr(kind) ? hdrFormat : ldrFormat;
		configureBlending(desc.colorAttachments[0], variant);
		desc.rasterSampleCount = isMultisampled(kind) ? msaaSampleCount : 1;
		NSError* error = nil;
		const id<MTLRenderPipelineState> pipeline =
		    [device newRenderPipelineStateWithDescriptor:desc error:&error];
		if (!pipeline) {
			throw std::runtime_error("Creating a Metal pipeline state failed: " +
			                         toString(error.localizedDescription));
		}
		return pipeline;
	}
}

id<MTLRenderPipelineState> MetalRenderer::Impl::builtinPipeline(const Builtin builtin,
                                                                const PassKind kind,
                                                                const BlendVariant variant) {
	auto& pipeline = builtinPipelines[static_cast<size_t>(builtin)][pipelineIndex(kind, variant)];
	if (!pipeline) {
		pipeline = createPipeline(builtinFunctions[static_cast<size_t>(builtin)],
		                          builtin == Builtin::Textured, kind, variant);
	}
	return pipeline;
}

id<MTLRenderPipelineState> MetalRenderer::Impl::programPipeline(const MetalShaderProgram& program,
                                                                const PassKind kind,
                                                                const BlendVariant variant) const {
	void*& pipeline = program.pipelines[pipelineIndex(kind, variant)];
	if (!pipeline) {
		const ShaderFunctions functions{ (__bridge id<MTLFunction>)program.vertexFunction,
			                             (__bridge id<MTLFunction>)program.fragmentFunction };
		pipeline = (void*)CFBridgingRetain(createPipeline(functions, true, kind, variant));
	}
	return (__bridge id<MTLRenderPipelineState>)pipeline;
}

BlendVariant MetalRenderer::Impl::blendVariant() const {
	if (alphaOnlyWrite) {
		return BlendVariant::ReplaceAlphaOnly;
	}
	if (!blendingEnabled) {
		return BlendVariant::Replace;
	}
	switch (spriteBlend) {
	case SpriteBlend::Composite:
		return BlendVariant::Composite;
	case SpriteBlend::Premultiplied:
		return BlendVariant::Premultiplied;
	case SpriteBlend::AddAlpha:
		return BlendVariant::AddAlpha;
	case SpriteBlend::Additive:
		return BlendVariant::Additive;
	case SpriteBlend::PremultipliedAdditive:
		return BlendVariant::PremultipliedAdditive;
	case SpriteBlend::Multiply:
		return BlendVariant::Multiply;
	case SpriteBlend::Screen:
		return BlendVariant::Screen;
	}
	return BlendVariant::Composite;
}

size_t MetalRenderer::Impl::upload(const void* const data, const size_t bytes) {
	UploadBuffer& vb = vertexBuffers[currentFrame];
	size_t offset = alignUp(vb.used, bufferOffsetAlignment);
	if (offset + bytes > vb.buffer.length) {
		// The draws recorded so far still read from the full buffer, which their command buffer
		// keeps alive. Replace it with a bigger one, which is kept for the following frames.
		size_t capacity = vb.buffer.length * 2;
		while (capacity < bytes) {
			capacity *= 2;
		}
		internal::debug("Growing the Metal vertex buffer of frame {} to {} KiB.", currentFrame,
		                capacity / 1024);
		vb.buffer = [device newBufferWithLength:capacity options:MTLResourceStorageModeShared];
		offset = 0;
	}
	std::memcpy(static_cast<uint8_t*>(vb.buffer.contents) + offset, data, bytes);
	vb.used = offset + bytes;
	return offset;
}

void MetalRenderer::Impl::bindVertices(const void* const data, const size_t bytes) {
	if (bytes <= maxInlineBytes) {
		[encoder setVertexBytes:data length:bytes atIndex:verticesIndex];
		return;
	}
	const size_t offset = upload(data, bytes);
	[encoder setVertexBuffer:vertexBuffers[currentFrame].buffer offset:offset atIndex:verticesIndex];
}

void MetalRenderer::Impl::writeTransform(const UniformBlock& block, const Mat3& modelview) {
	writeUniform(drawUniforms, block.offsets.modelview, modelview);
	if (flipClipY()) {
		Mat4 flipped = projection;
		for (int column = 0; column < 4; ++column) {
			flipped.data[column * 4 + 1] = -flipped.data[column * 4 + 1];
		}
		writeUniform(drawUniforms, block.offsets.projection, flipped);
	} else {
		writeUniform(drawUniforms, block.offsets.projection, projection);
	}
}

void MetalRenderer::Impl::bindUniforms(const UniformBlock& block,
                                       const std::vector<uint8_t>& data) {
	if (block.size == 0) {
		return;
	}
	assert(data.size() == block.size);
	// Push constants or not, they're the same buffer in the shaders, see MslCompiler.hpp
	if (block.size <= maxInlineBytes) {
		[encoder setVertexBytes:data.data() length:block.size atIndex:uniformsIndex];
		[encoder setFragmentBytes:data.data() length:block.size atIndex:uniformsIndex];
		return;
	}
	const size_t offset = upload(data.data(), block.size);
	const id<MTLBuffer> buffer = vertexBuffers[currentFrame].buffer;
	[encoder setVertexBuffer:buffer offset:offset atIndex:uniformsIndex];
	[encoder setFragmentBuffer:buffer offset:offset atIndex:uniformsIndex];
}

void MetalRenderer::Impl::beginCommandBuffer() {
	commandBuffer = [queue commandBuffer];
	commandBuffer.label = @"JNGL frame";
}

id<MTLBlitCommandEncoder> MetalRenderer::Impl::uploadBlit() {
	if (!uploadBlitEncoder) {
		uploadBlitEncoder = [uploadCommands() blitCommandEncoder];
	}
	return uploadBlitEncoder;
}

id<MTLCommandBuffer> MetalRenderer::Impl::uploadCommands() {
	if (!uploadCommandBuffer) {
		uploadCommandBuffer = [queue commandBuffer];
		uploadCommandBuffer.label = @"JNGL uploads";
	}
	if (uploadBlitEncoder) {
		[uploadBlitEncoder endEncoding];
		uploadBlitEncoder = nil;
	}
	return uploadCommandBuffer;
}

void MetalRenderer::Impl::submitUploads() {
	if (!uploadCommandBuffer) {
		return;
	}
	if (uploadBlitEncoder) {
		[uploadBlitEncoder endEncoding];
		uploadBlitEncoder = nil;
	}
	// Executed before whatever's committed after it, as they're all on the same queue
	[uploadCommandBuffer commit];
	uploadCommandBuffer = nil;
}

void MetalRenderer::Impl::waitIdle() {
	submitUploads();
	const id<MTLCommandBuffer> marker = [queue commandBuffer];
	[marker commit];
	[marker waitUntilCompleted];
}

void MetalRenderer::Impl::acquireFrameTexture() {
	if (frameTexture) {
		return;
	}
	// Drawables of a window that can't be seen might not be presented, after which nextDrawable
	// would wait a second each time for one to become available again
	const SDL_WindowFlags hidden = SDL_WINDOW_HIDDEN | SDL_WINDOW_MINIMIZED | SDL_WINDOW_OCCLUDED;
	drawable = (SDL_GetWindowFlags(window) & hidden) == 0 ? [layer nextDrawable] : nil;
	if (drawable) {
		frameTexture = drawable.texture;
	} else {
		const CGSize size = layer.drawableSize;
		const int width = std::max(static_cast<int>(size.width), 1);
		const int height = std::max(static_cast<int>(size.height), 1);
		if (!offscreenTarget || static_cast<int>(offscreenTarget.width) != width ||
		    static_cast<int>(offscreenTarget.height) != height) {
			offscreenTarget =
			    createTexture(width, height, 1, ldrFormat,
			                  MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead);
		}
		frameTexture = offscreenTarget;
	}
	swapchainWidth = static_cast<int>(frameTexture.width);
	swapchainHeight = static_cast<int>(frameTexture.height);
	updateMsaaTarget();
}

void MetalRenderer::Impl::updateMsaaTarget() {
	if (!swapchainMsaaActive()) {
		msaaTarget = nil;
		return;
	}
	if (!msaaTarget || static_cast<int>(msaaTarget.width) != swapchainWidth ||
	    static_cast<int>(msaaTarget.height) != swapchainHeight) {
		msaaTarget = createTexture(swapchainWidth, swapchainHeight, 1, ldrFormat,
		                           MTLTextureUsageRenderTarget, msaaSampleCount);
	}
}

bool MetalRenderer::Impl::swapchainMsaaActive() const {
	return msaaEnabled && msaaSupported;
}

void MetalRenderer::Impl::bindTarget(std::optional<MTLClearColor> clear) {
	if (encoder) {
		return;
	}
	bool fillCanvas = false; // with the clear color, after clearing the letterboxing's bars
	@autoreleasepool {
		MTLRenderPassDescriptor* const pass = [MTLRenderPassDescriptor renderPassDescriptor];
		MTLRenderPassColorAttachmentDescriptor* const attachment = pass.colorAttachments[0];
		attachment.storeAction = MTLStoreActionStore;
		encoderOnScreen = renderTargetStack.empty();
		if (encoderOnScreen) {
			acquireFrameTexture(); // which decides whether there's msaaTarget
			encoderOnScreenSamples = msaaTarget != nil;
			scissor.sync(swapchainWidth, swapchainHeight);
			if (screenClearPending && !clear) {
				const Rgb& c = currentClearColor;
				fillCanvas = scissor.isLetterboxed();
				clear = fillCanvas ? MTLClearColorMake(0, 0, 0, 1)
				                   : MTLClearColorMake(c.getRed(), c.getGreen(), c.getBlue(), 1);
			}
			screenClearPending = false;
			if (msaaTarget) {
				currentPassKind = PassKind::LdrMsaa;
				attachment.texture = msaaTarget;
				attachment.resolveTexture = frameTexture;
				attachment.storeAction = MTLStoreActionUnknown; // see unbindTarget()
			} else {
				currentPassKind = PassKind::Ldr;
				attachment.texture = frameTexture;
			}
		} else {
			MetalFramebuffer& target = *renderTargetStack.back();
			// Like on OpenGL, where an anti-aliased FrameBuffer is always drawn into multisampled,
			// but only smoothed while GL_MULTISAMPLE is enabled
			const bool multisampled =
			    target.msaa && (msaaRebuildPending ? pendingMsaaEnabled : msaaEnabled);
			currentPassKind = passKind(target.hdr, multisampled);
			if (multisampled) {
				if (target.samplesStale && !clear) {
					expandIntoSamples(target);
				}
				target.samplesStale = false;
				encoderSamplesOf = &target;
				attachment.texture = (__bridge id<MTLTexture>)target.msaa;
				attachment.resolveTexture = (__bridge id<MTLTexture>)target.color.texture;
				attachment.storeAction = MTLStoreActionUnknown; // see unbindTarget()
			} else {
				if (target.msaa) {
					target.samplesStale = true;
				}
				attachment.texture = (__bridge id<MTLTexture>)target.color.texture;
			}
		}
		if (clear) {
			attachment.loadAction = MTLLoadActionClear;
			attachment.clearColor = *clear;
		} else {
			attachment.loadAction = MTLLoadActionLoad;
		}
		encoder = [commandBuffer renderCommandEncoderWithDescriptor:pass];
	}
	applyViewport();
	applyScissor();
	if (fillCanvas) {
		const PixelRect canvas = scissor.letterbox(swapchainWidth, swapchainHeight);
		if (canvas.width > 0 && canvas.height > 0) {
			const Rgb& c = currentClearColor;
			fillRect(canvas, MTLClearColorMake(c.getRed(), c.getGreen(), c.getBlue(), 1));
		}
	}
}

void MetalRenderer::Impl::unbindTarget(const PassEnd end) {
	if (!encoder) {
		return;
	}
	if (encoderOnScreenSamples) {
		// Nothing loads the samples again once the frame's done
		[encoder setColorStoreAction:end == PassEnd::FrameEnd
		                                 ? MTLStoreActionMultisampleResolve
		                                 : MTLStoreActionStoreAndMultisampleResolve
		                     atIndex:0];
	} else if (encoderSamplesOf) {
		if (end == PassEnd::Popped) {
			// Usually, it's cleared before it's drawn into again. If it isn't, its samples are
			// drawn from what they're resolved into now, see bindTarget().
			[encoder setColorStoreAction:MTLStoreActionMultisampleResolve atIndex:0];
			encoderSamplesOf->samplesStale = true;
		} else {
			[encoder setColorStoreAction:MTLStoreActionStoreAndMultisampleResolve atIndex:0];
		}
	}
	[encoder endEncoding];
	encoder = nil;
	encoderOnScreen = false;
	encoderOnScreenSamples = false;
	encoderSamplesOf = nullptr;
}

void MetalRenderer::Impl::finishScreen(const PassEnd end) {
	if (encoder && !encoderOnScreen) {
		unbindTarget();
	}
	if (!encoder && screenClearPending) {
		// A pass into the window that does nothing but its pending clear
		std::vector<MetalFramebuffer*> stack;
		std::swap(stack, renderTargetStack);
		bindTarget();
		std::swap(stack, renderTargetStack);
	}
	unbindTarget(end);
}

void MetalRenderer::Impl::clearTarget(const MTLClearColor color) {
	if (!encoder) {
		bindTarget(color);
		return;
	}
	const auto [width, height] = currentTargetSize();
	fillRect({ 0, 0, width, height }, color);
}

void MetalRenderer::Impl::clearSwapchain() {
	if (!encoder) {
		screenClearPending = true; // see bindTarget()
		return;
	}
	const Rgb& c = currentClearColor;
	const MTLClearColor color = MTLClearColorMake(c.getRed(), c.getGreen(), c.getBlue(), 1);
	if (!scissor.isLetterboxed()) {
		clearTarget(color);
		return;
	}
	clearTarget(MTLClearColorMake(0, 0, 0, 1));
	const PixelRect canvas = scissor.letterbox(swapchainWidth, swapchainHeight);
	if (canvas.width > 0 && canvas.height > 0) {
		fillRect(canvas, color);
	}
}

void MetalRenderer::Impl::fillRect(const PixelRect rect, const MTLClearColor color) {
	bindTarget();
	[encoder setScissorRect:{ static_cast<NSUInteger>(rect.x), static_cast<NSUInteger>(rect.y),
	                          static_cast<NSUInteger>(rect.width),
	                          static_cast<NSUInteger>(rect.height) }];
	const UniformBlock& block = coloredUniforms;
	auto& uniforms = prepareDrawUniforms(drawUniforms, block);
	writeUniform(uniforms, block.offsets.modelview, Mat3());
	writeUniform(uniforms, block.offsets.projection, Mat4());
	writeUniform(uniforms, block.offsets.color,
	             std::array{ static_cast<float>(color.red), static_cast<float>(color.green),
	                         static_cast<float>(color.blue), static_cast<float>(color.alpha) });
	[encoder setRenderPipelineState:builtinPipeline(Builtin::Colored, currentPassKind,
	                                                BlendVariant::Replace)];
	bindUniforms(block, uniforms);
	bindVertices(fullscreenTriangles.data(), sizeof(fullscreenTriangles));
	[encoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:6];
	applyScissor();
}

void MetalRenderer::Impl::expandIntoSamples(MetalFramebuffer& fb) {
	assert(!encoder);
	MTLRenderPassDescriptor* const pass = [MTLRenderPassDescriptor renderPassDescriptor];
	pass.colorAttachments[0].texture = (__bridge id<MTLTexture>)fb.msaa;
	pass.colorAttachments[0].loadAction = MTLLoadActionDontCare;
	pass.colorAttachments[0].storeAction = MTLStoreActionStore;
	const id<MTLRenderCommandEncoder> expand =
	    [commandBuffer renderCommandEncoderWithDescriptor:pass];
	// Samples each texel at its center into all of its pixel's samples, as it's drawn without
	// blending
	[expand setRenderPipelineState:builtinPipeline(Builtin::Textured, passKind(fb.hdr, true),
	                                               BlendVariant::Replace)];
	[expand setFragmentTexture:(__bridge id<MTLTexture>)fb.color.texture atIndex:0];
	[expand setFragmentSamplerState:samplers[samplerIndex(fb.color.minFilter, fb.color.magFilter)]
	                        atIndex:0];
	if (!copyUniforms.empty()) {
		[expand setVertexBytes:copyUniforms.data() length:copyUniforms.size() atIndex:uniformsIndex];
		[expand setFragmentBytes:copyUniforms.data()
		                  length:copyUniforms.size()
		                 atIndex:uniformsIndex];
	}
	[expand setVertexBytes:fullscreenQuad.data() length:sizeof(fullscreenQuad) atIndex:verticesIndex];
	[expand drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:6];
	[expand endEncoding];
}

bool MetalRenderer::Impl::flipClipY() const {
	return !renderTargetStack.empty();
}

void MetalRenderer::Impl::draw(const id<MTLRenderPipelineState> pipeline,
                               const UniformBlock& block, const std::vector<uint8_t>& uniforms,
                               const void* const vertices, const size_t vertexCount,
                               const size_t stride, const PrimitiveType type) {
	if (scissorEmpty) {
		return;
	}
	[encoder setRenderPipelineState:pipeline];
	bindUniforms(block, uniforms);
	bindVertices(vertices, vertexCount * stride);
	[encoder drawPrimitives:toMetalPrimitive(type) vertexStart:0 vertexCount:vertexCount];
}

void MetalRenderer::Impl::applyViewport() const {
	MTLViewport viewport{ 0, 0, 0, 0, 0, 1 };
	if (renderTargetStack.empty()) {
		// Like glViewport, at the bottom left. When the drawables haven't been resized after a
		// resize of the window yet, this might not cover all of it.
		const int fbWidth = scissor.getFramebufferWidth();
		const int fbHeight = scissor.getFramebufferHeight();
		viewport.width = fbWidth > 0 ? fbWidth : swapchainWidth;
		viewport.height = fbHeight > 0 ? fbHeight : swapchainHeight;
		viewport.originY = swapchainHeight - viewport.height;
	} else {
		viewport.width = renderTargetStack.back()->width;
		viewport.height = renderTargetStack.back()->height;
	}
	[encoder setViewport:viewport];
}

void MetalRenderer::Impl::applyScissor() {
	const PixelRect clip = currentScissor();
	const auto [width, height] = currentTargetSize();
	// Metal's has to be within the render target and can't be empty
	const int x = std::clamp(clip.x, 0, width);
	const int y = std::clamp(clip.y, 0, height);
	const int w = std::min(clip.width, width - x);
	const int h = std::min(clip.height, height - y);
	scissorEmpty = w <= 0 || h <= 0;
	if (!scissorEmpty) {
		[encoder setScissorRect:{ static_cast<NSUInteger>(x), static_cast<NSUInteger>(y),
		                          static_cast<NSUInteger>(w), static_cast<NSUInteger>(h) }];
	}
}

PixelRect MetalRenderer::Impl::currentScissor() const {
	const auto [width, height] = currentTargetSize();
	return scissor.clip(width, height, renderTargetStack.empty());
}

std::pair<int, int> MetalRenderer::Impl::currentTargetSize() const {
	if (renderTargetStack.empty()) {
		return { swapchainWidth, swapchainHeight };
	}
	return { renderTargetStack.back()->width, renderTargetStack.back()->height };
}

id<MTLTexture> MetalRenderer::Impl::createTexture(const int width, const int height,
                                                  const uint32_t mipLevels,
                                                  const MTLPixelFormat format,
                                                  const MTLTextureUsage usage,
                                                  const NSUInteger samples) const {
	@autoreleasepool {
		MTLTextureDescriptor* const desc =
		    [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:format
		                                                       width:static_cast<NSUInteger>(width)
		                                                      height:static_cast<NSUInteger>(height)
		                                                   mipmapped:NO];
		desc.mipmapLevelCount = mipLevels;
		desc.usage = usage;
		desc.storageMode = MTLStorageModePrivate;
		if (samples > 1) {
			desc.textureType = MTLTextureType2DMultisample;
			desc.sampleCount = samples;
		}
		const id<MTLTexture> texture = [device newTextureWithDescriptor:desc];
		if (!texture) {
			throw std::runtime_error(
			    std::format("Creating a {}x{} Metal texture failed.", width, height));
		}
		return texture;
	}
}

MetalRenderer::MetalRenderer(SDL_Window* const window) {
	@autoreleasepool {
		impl = std::make_unique<Impl>(window);
	}
}

MetalRenderer::~MetalRenderer() = default;

void MetalRenderer::beginFrame(const Rgb clearColor) {
	@autoreleasepool {
		if (impl->frameActive) {
			// E.g. by showWindow, before the first swapBuffers. Like glClear on OpenGL, clear
			// what's being drawn on instead.
			impl->currentClearColor = clearColor;
			if (impl->renderTargetStack.empty()) {
				impl->clearSwapchain(); // with the letterboxing's bars
			} else {
				clearCurrentRenderTarget(Rgba(clearColor, Alpha(1)));
			}
			return;
		}
		// The GPU is done with this frame's buffers once it's done with the frame before last
		dispatch_semaphore_wait(impl->frameSemaphore, DISPATCH_TIME_FOREVER);
		impl->vertexBuffers[impl->currentFrame].used = 0;
		impl->beginCommandBuffer();

		impl->currentClearColor = clearColor;
		impl->renderTargetStack.clear();
		impl->frameActive = true;
		impl->clearSwapchain();
	}
}

void MetalRenderer::endFrame() {
	if (!impl->frameActive) {
		return;
	}
	@autoreleasepool {
		impl->finishScreen(PassEnd::FrameEnd);
		if (impl->drawable) {
			[impl->commandBuffer presentDrawable:impl->drawable];
		}
		const dispatch_semaphore_t semaphore = impl->frameSemaphore;
		[impl->commandBuffer addCompletedHandler:^(id<MTLCommandBuffer>) {
		  dispatch_semaphore_signal(semaphore);
		}];
		impl->submitUploads();
		[impl->commandBuffer commit];
		impl->commandBuffer = nil;
		impl->drawable = nil;
		impl->frameTexture = nil;

		impl->frameActive = false;
		impl->currentFrame = (impl->currentFrame + 1) % Impl::maxFramesInFlight;

		if (impl->drawableResizePending) {
			impl->drawableResizePending = false;
			int width = 0;
			int height = 0;
			SDL_GetWindowSizeInPixels(impl->window, &width, &height);
			if (width > 0 && height > 0) { // not while it's minimized
				impl->layer.drawableSize = CGSizeMake(width, height);
			}
		}
		if (impl->msaaRebuildPending) {
			impl->msaaRebuildPending = false;
			impl->msaaEnabled = impl->pendingMsaaEnabled;
		}
	}
}

void MetalRenderer::setViewport(const int x, const int y, const int width, const int height) {
	assert(x == 0 && y == 0);
	(void)x;
	(void)y;
	impl->scissor.setFramebufferSize(width, height);
	if (impl->encoder && impl->renderTargetStack.empty()) {
		impl->applyViewport();
	}
}

void MetalRenderer::onResize(int /*width*/, int /*height*/) {
	impl->drawableResizePending = true;
}

void MetalRenderer::setProjection(const Mat4& projection) {
	impl->projection = projection;
}

void MetalRenderer::drawColored(PrimitiveType type, const float* xyVertices,
                                std::size_t vertexCount, const Mat3& modelview, const Rgba color) {
	if (!impl->frameActive || vertexCount == 0) {
		return;
	}
	std::array<float, 12> triangles;
	if (type == PrimitiveType::TriangleFan && vertexCount == 4) {
		triangles = triangulateQuad<2>(xyVertices);
		xyVertices = triangles.data();
		vertexCount = 6;
		type = PrimitiveType::Triangles;
	} else if (type == PrimitiveType::TriangleFan) {
		if (vertexCount < 3) {
			return;
		}
		triangulateFan<2>(xyVertices, vertexCount, impl->triangulatedVertices);
		xyVertices = impl->triangulatedVertices.data();
		vertexCount = (vertexCount - 2) * 3;
		type = PrimitiveType::Triangles;
	}

	impl->bindTarget();
	const UniformBlock& block = impl->coloredUniforms;
	auto& uniforms = prepareDrawUniforms(impl->drawUniforms, block);
	impl->writeTransform(block, modelview);
	writeUniform(uniforms, block.offsets.color,
	             std::array{ color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha() });
	impl->draw(impl->builtinPipeline(Impl::Builtin::Colored, impl->currentPassKind,
	                                 impl->blendVariant()),
	           block, uniforms, xyVertices, vertexCount, 2 * sizeof(float), type);
}

void MetalRenderer::drawRoundedRect(const Mat3& modelview, const Rgba color, const float sizeX,
                                    const float sizeY, const float topLeft, const float topRight,
                                    const float bottomLeft, const float bottomRight) {
	if (!impl->frameActive) {
		return;
	}
	static constexpr std::array<float, 12> unitSquare = { .5f, -.5f, .5f,  .5f, -.5f, -.5f,
		                                                  .5f, .5f,  -.5f, .5f, -.5f, -.5f };
	impl->bindTarget();
	const UniformBlock& block = impl->roundedUniforms;
	auto& uniforms = prepareDrawUniforms(impl->drawUniforms, block);
	impl->writeTransform(block, modelview);
	writeUniform(uniforms, block.offsets.color,
	             std::array{ color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha() });
	writeUniform(uniforms, block.offsets.size, std::array{ sizeX, sizeY });
	writeUniform(uniforms, block.offsets.cornerRadii,
	             std::array{ topLeft, topRight, bottomLeft, bottomRight });
	impl->draw(impl->builtinPipeline(Impl::Builtin::Rounded, impl->currentPassKind,
	                                 impl->blendVariant()),
	           block, uniforms, unitSquare.data(), 6, 2 * sizeof(float), PrimitiveType::Triangles);
}

std::unique_ptr<MetalTexture>
MetalRenderer::createTexture(const int width, const int height, const unsigned int format,
                             const unsigned int type, const unsigned char* const data,
                             const unsigned char* const* const rowPointers, const bool mipmap) {
	if (type != glUnsignedByte || (format != glRGBA && format != glRGB && format != glBGR)) {
		internal::error("Metal backend: unsupported texture format/type (0x{:x}/0x{:x}).", format,
		                type);
		return nullptr;
	}

	auto texture = std::make_unique<MetalTexture>();
	texture->width = width;
	texture->height = height;
	texture->mipLevels = mipmap ? mipLevelCount(width, height) : 1;
	@autoreleasepool {
		// Mipmaps are generated by drawing into the levels
		const MTLTextureUsage usage = texture->mipLevels > 1
		                                  ? MTLTextureUsageShaderRead | MTLTextureUsageRenderTarget
		                                  : MTLTextureUsageShaderRead;
		texture->texture = (void*)CFBridgingRetain(
		    impl->createTexture(width, height, texture->mipLevels, textureFormat, usage));
	}
	if (data || rowPointers) {
		updateTextureBytes(*texture, width, height,
		                   toRgba8(width, height, format, data, rowPointers).data());
	}
	if (App::isPixelArt()) {
		texture->magFilter = TextureFilter::NearestNeighbor;
	}
	return texture;
}

void MetalRenderer::updateTextureBytes(MetalTexture& texture, const int width, const int height,
                                       const unsigned char* const rgba) {
	if (!texture.texture || width <= 0 || height <= 0) {
		return;
	}
	@autoreleasepool {
		const size_t rowSize = static_cast<size_t>(width) * 4;
		const id<MTLBuffer> staging =
		    [impl->device newBufferWithBytes:rgba
		                              length:rowSize * height
		                             options:MTLResourceStorageModeShared];
		const id<MTLTexture> target = (__bridge id<MTLTexture>)texture.texture;
		// After earlier frames are done sampling the old pixels, as Metal tracks the hazard
		const id<MTLBlitCommandEncoder> blit = impl->uploadBlit();
		[blit copyFromBuffer:staging
		           sourceOffset:0
		      sourceBytesPerRow:rowSize
		    sourceBytesPerImage:rowSize * height
		             sourceSize:MTLSizeMake(static_cast<NSUInteger>(width),
		                                    static_cast<NSUInteger>(height), 1)
		              toTexture:target
		       destinationSlice:0
		       destinationLevel:0
		      destinationOrigin:MTLOriginMake(0, 0, 0)];
		if (texture.mipLevels > 1) {
			[blit generateMipmapsForTexture:target];
		}
	}
}

void MetalRenderer::enableMipmaps(MetalTexture& texture) {
	const uint32_t mipLevels = mipLevelCount(texture.width, texture.height);
	if (texture.mipLevels > 1 || mipLevels == 1 || !texture.texture) {
		return;
	}
	@autoreleasepool {
		// The number of levels of a texture can't be changed, so copy the pixels into a new one
		const id<MTLTexture> old = (__bridge id<MTLTexture>)texture.texture;
		const id<MTLTexture> mipmapped =
		    impl->createTexture(texture.width, texture.height, mipLevels, textureFormat,
		                        MTLTextureUsageShaderRead | MTLTextureUsageRenderTarget);
		const id<MTLBlitCommandEncoder> blit = impl->uploadBlit();
		[blit copyFromTexture:old
		             sourceSlice:0
		             sourceLevel:0
		            sourceOrigin:MTLOriginMake(0, 0, 0)
		              sourceSize:MTLSizeMake(old.width, old.height, 1)
		               toTexture:mipmapped
		        destinationSlice:0
		        destinationLevel:0
		       destinationOrigin:MTLOriginMake(0, 0, 0)];
		[blit generateMipmapsForTexture:mipmapped];
		// Draws recorded earlier in this frame still sample the old texture, which their command
		// buffer keeps alive
		MetalTexture copy = texture;
		destroyTexture(copy);
		texture.texture = (void*)CFBridgingRetain(mipmapped);
		texture.mipLevels = mipLevels;
	}
}

void MetalRenderer::setTextureFilter(MetalTexture& texture, const TextureFilter minFilter,
                                     const TextureFilter magFilter) {
	// The sampler is picked for each draw, see samplerIndex()
	texture.minFilter = minFilter;
	texture.magFilter = magFilter;
}

void MetalRenderer::destroyTexture(MetalTexture& texture) {
	if (texture.texture) {
		CFRelease(texture.texture);
	}
	texture = {};
}

void MetalRenderer::drawSprite(const MetalTexture& texture, const float* const xyuv,
                               const std::size_t vertexCount, const PrimitiveType type,
                               const Mat3& modelview, const Rgba color) {
	// Multiplying with color is the same as mixing towards black by 1 - color
	drawSprite(texture, xyuv, vertexCount, type, modelview, Rgb(0, 0, 0),
	           { 1.f - color.getRed(), 1.f - color.getGreen(), 1.f - color.getBlue(),
	             1.f - color.getAlpha() });
}

void MetalRenderer::drawSprite(const MetalTexture& texture, const float* xyuv,
                               std::size_t vertexCount, PrimitiveType type, const Mat3& modelview,
                               const Rgb lerpColor, const std::array<float, 4> lerpFactors) {
	if (!impl->frameActive || vertexCount == 0 || !texture.texture) {
		return;
	}
	std::array<float, 24> triangles;
	if (type == PrimitiveType::TriangleFan && vertexCount == 4) {
		triangles = triangulateQuad<4>(xyuv);
		xyuv = triangles.data();
		vertexCount = 6;
		type = PrimitiveType::Triangles;
	} else if (type == PrimitiveType::TriangleFan) {
		if (vertexCount < 3) {
			return;
		}
		triangulateFan<4>(xyuv, vertexCount, impl->triangulatedVertices);
		xyuv = impl->triangulatedVertices.data();
		vertexCount = (vertexCount - 2) * 3;
		type = PrimitiveType::Triangles;
	}

	impl->bindTarget();
	const MetalShaderProgram* const program = impl->activeShaderProgram;
	const UniformBlock& block = program ? program->uniformBlock : impl->texturedUniforms;
	auto& uniforms = prepareDrawUniforms(impl->drawUniforms, block, program);
	impl->writeTransform(block, modelview);
	if (!program) { // a ShaderProgram's other uniforms are its own, like on OpenGL
		writeUniform(uniforms, block.offsets.lerpColor,
		             std::array{ lerpColor.getRed(), lerpColor.getGreen(), lerpColor.getBlue() });
		writeUniform(uniforms, block.offsets.lerpFactors, lerpFactors);
	}

	const BlendVariant variant = impl->blendVariant();
	const id<MTLRenderPipelineState> pipeline =
	    program ? impl->programPipeline(*program, impl->currentPassKind, variant)
	            : impl->builtinPipeline(Impl::Builtin::Textured, impl->currentPassKind, variant);
	[impl->encoder setFragmentTexture:(__bridge id<MTLTexture>)texture.texture atIndex:0];
	[impl->encoder
	    setFragmentSamplerState:impl->samplers[samplerIndex(texture.minFilter, texture.magFilter)]
	                    atIndex:0];
	impl->draw(pipeline, block, uniforms, xyuv, vertexCount, 4 * sizeof(float), type);
}

void MetalRenderer::readPixels(unsigned char* const rgb, const int x, const int y, const int width,
                               const int height) {
	const size_t pixelCount = static_cast<size_t>(width) * height;
	if (impl->frameActive) {
		@autoreleasepool {
			impl->acquireFrameTexture(); // for its size
		}
	}
	if (!impl->frameActive || x < 0 || y < 0 || x + width > impl->swapchainWidth ||
	    y + height > impl->swapchainHeight || pixelCount == 0) {
		std::memset(rgb, 0, pixelCount * 3);
		return;
	}
	@autoreleasepool {
		// Copy the requested region into a buffer the CPU can read, after the draws recorded so
		// far
		impl->finishScreen(PassEnd::Read);
		const size_t rowSize = static_cast<size_t>(width) * 4;
		const id<MTLBuffer> readback =
		    [impl->device newBufferWithLength:rowSize * height
		                              options:MTLResourceStorageModeShared];
		const id<MTLBlitCommandEncoder> blit = [impl->commandBuffer blitCommandEncoder];
		// x and y are from the bottom left like glReadPixels
		const int top = impl->swapchainHeight - y - height;
		[blit copyFromTexture:impl->frameTexture
		                 sourceSlice:0
		                 sourceLevel:0
		                sourceOrigin:MTLOriginMake(static_cast<NSUInteger>(x),
		                                           static_cast<NSUInteger>(top), 0)
		                  sourceSize:MTLSizeMake(static_cast<NSUInteger>(width),
		                                         static_cast<NSUInteger>(height), 1)
		                    toBuffer:readback
		           destinationOffset:0
		      destinationBytesPerRow:rowSize
		    destinationBytesPerImage:rowSize * height];
		[blit endEncoding];
		impl->submitUploads();
		[impl->commandBuffer commit];
		[impl->commandBuffer waitUntilCompleted];

		const auto* const src8 = static_cast<const unsigned char*>(readback.contents);
		// Output RGB from BGRA, bottom row first (OpenGL convention)
		for (int row = 0; row < height; ++row) {
			const unsigned char* srcRow = src8 + static_cast<size_t>(height - 1 - row) * rowSize;
			unsigned char* dstRow = rgb + static_cast<size_t>(row) * width * 3;
			for (int col = 0; col < width; ++col) {
				dstRow[col * 3 + 0] = srcRow[col * 4 + 2];
				dstRow[col * 3 + 1] = srcRow[col * 4 + 1];
				dstRow[col * 3 + 2] = srcRow[col * 4 + 0];
			}
		}

		// Resume the frame, the GPU is done with everything that's been recorded
		impl->beginCommandBuffer();
	}
}

std::string MetalRenderer::getGraphicsApi() const {
	@autoreleasepool {
		// E.g. "Metal (Apple M2 Pro) 26.1.0", like Vulkan's "(NVIDIA) 580.95.05", as there's no
		// driver version but that of macOS
		const NSOperatingSystemVersion version = [NSProcessInfo processInfo].operatingSystemVersion;
		return std::format("Metal ({}) {}.{}.{}", toString(impl->device.name), version.majorVersion,
		                   version.minorVersion, version.patchVersion);
	}
}

std::unique_ptr<MetalFramebuffer> MetalRenderer::createFramebuffer(const int width,
                                                                   const int height,
                                                                   const bool antiAliased,
                                                                   const bool hdr) {
	auto fb = std::make_unique<MetalFramebuffer>();
	fb->width = width;
	fb->height = height;
	fb->hdr = hdr;
	const MTLPixelFormat format = hdr ? hdrFormat : ldrFormat;
	const TextureFilter filter =
	    App::isPixelArt() ? TextureFilter::NearestNeighbor : TextureFilter::Bilinear;
	fb->color.width = width;
	fb->color.height = height;
	fb->color.minFilter = filter;
	fb->color.magFilter = filter;
	@autoreleasepool {
		const id<MTLTexture> color =
		    impl->createTexture(width, height, 1, format,
		                        MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead);
		fb->color.texture = (void*)CFBridgingRetain(color);
		id<MTLTexture> msaa = nil;
		if (antiAliased && impl->msaaSupported) {
			msaa = impl->createTexture(width, height, 1, format, MTLTextureUsageRenderTarget,
			                           msaaSampleCount);
			fb->msaa = (void*)CFBridgingRetain(msaa);
		}

		// Clear both to transparent white, before anything that's committed after the uploads
		const MTLClearColor transparent = MTLClearColorMake(1, 1, 1, 0);
		for (const id<MTLTexture>& texture : { color, msaa }) {
			if (!texture) {
				continue;
			}
			MTLRenderPassDescriptor* const pass = [MTLRenderPassDescriptor renderPassDescriptor];
			pass.colorAttachments[0].texture = texture;
			pass.colorAttachments[0].loadAction = MTLLoadActionClear;
			pass.colorAttachments[0].clearColor = transparent;
			pass.colorAttachments[0].storeAction = MTLStoreActionStore;
			[[impl->uploadCommands() renderCommandEncoderWithDescriptor:pass] endEncoding];
		}
	}
	return fb;
}

void MetalRenderer::destroyFramebuffer(MetalFramebuffer& fb) {
	// The command buffers that use them keep them alive, jngl::FrameBuffers can be destroyed
	// mid-frame
	destroyTexture(fb.color);
	if (fb.msaa) {
		CFRelease(fb.msaa);
	}
	fb = {};
}

const MetalTexture& MetalRenderer::framebufferTexture(MetalFramebuffer& fb,
                                                      const TextureFilter filter) {
	if (filter == fb.color.minFilter) {
		return fb.color;
	}
	// Doesn't own the texture
	fb.alternateFilter = fb.color;
	fb.alternateFilter.minFilter = filter;
	fb.alternateFilter.magFilter = filter;
	return fb.alternateFilter;
}

void MetalRenderer::pushRenderTarget(MetalFramebuffer& framebuffer) {
	if (!impl->frameActive) {
		return;
	}
	impl->unbindTarget();
	impl->renderTargetStack.push_back(&framebuffer);
}

void MetalRenderer::popRenderTarget() {
	if (!impl->frameActive || impl->renderTargetStack.empty()) {
		return;
	}
	impl->unbindTarget(PassEnd::Popped);
	impl->renderTargetStack.pop_back();
}

void MetalRenderer::clearCurrentRenderTarget(const Rgba color) {
	if (!impl->frameActive) {
		return;
	}
	if (impl->renderTargetStack.empty()) {
		@autoreleasepool {
			impl->acquireFrameTexture(); // for its size
		}
		impl->scissor.sync(impl->swapchainWidth, impl->swapchainHeight);
	}
	const PixelRect scissor = impl->currentScissor();
	if (scissor.width <= 0 || scissor.height <= 0) {
		return;
	}
	const MTLClearColor values =
	    MTLClearColorMake(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
	@autoreleasepool {
		const auto [width, height] = impl->currentTargetSize();
		if (!impl->encoder && scissor.x == 0 && scissor.y == 0 && scissor.width == width &&
		    scissor.height == height) {
			impl->bindTarget(values); // cheaper than drawing over all of it
		} else {
			impl->fillRect(scissor, values);
		}
	}
}

std::unique_ptr<MetalShaderProgram>
MetalRenderer::createShaderProgram(const ShaderBytecode& bytecode) {
	auto program = std::make_unique<MetalShaderProgram>();
	@autoreleasepool {
		const ShaderFunctions functions = compileFunctions(impl->device, bytecode);
		program->vertexFunction = (void*)CFBridgingRetain(functions.vertex);
		program->fragmentFunction = (void*)CFBridgingRetain(functions.fragment);
	}
	program->uniformBlock =
	    uniformBlock(bytecode.uniforms, bytecode.uniformBlockSize, bytecode.pushConstants);
	program->uniforms = shaderUniforms(bytecode);
	program->uniformData.resize(program->uniformBlock.size);
	try {
		// Fails here rather than when it's first drawn with if the shaders don't fit together
		impl->programPipeline(*program, PassKind::Ldr, BlendVariant::Composite);
	} catch (...) {
		destroyShaderProgram(*program);
		throw;
	}
	return program;
}

void MetalRenderer::destroyShaderProgram(MetalShaderProgram& program) {
	// The command buffers that use the pipeline states keep them alive
	for (void*& pipeline : program.pipelines) {
		if (pipeline) {
			CFRelease(pipeline);
			pipeline = nullptr;
		}
	}
	for (void** function : { &program.vertexFunction, &program.fragmentFunction }) {
		if (*function) {
			CFRelease(*function);
			*function = nullptr;
		}
	}
	if (impl->activeShaderProgram == &program) {
		impl->activeShaderProgram = nullptr;
	}
}

void MetalRenderer::setActiveShaderProgram(const MetalShaderProgram* const program) {
	impl->activeShaderProgram = program;
}

void MetalRenderer::setActiveProgramUniform(const uint32_t offset, const void* const data,
                                            const uint32_t bytes) {
	assert(impl->activeShaderProgram);
	assert(offset + bytes <= impl->activeShaderProgram->uniformData.size());
	// Only copied from on the CPU, see prepareDrawUniforms()
	std::memcpy(impl->activeShaderProgram->uniformData.data() + offset, data, bytes);
}

void MetalRenderer::setBlendingEnabled(const bool enabled) {
	impl->blendingEnabled = enabled;
}

bool MetalRenderer::isBlendingEnabled() const {
	return impl->blendingEnabled;
}

void MetalRenderer::setSpriteBlend(const SpriteBlend blend) {
	impl->spriteBlend = blend;
}

MetalRenderer::SpriteBlend MetalRenderer::getSpriteBlend() const {
	return impl->spriteBlend;
}

void MetalRenderer::setAlphaOnlyWrite(const bool alphaOnly) {
	impl->alphaOnlyWrite = alphaOnly;
}

bool MetalRenderer::isAlphaOnlyWrite() const {
	return impl->alphaOnlyWrite;
}

void MetalRenderer::setVerticalSync(const bool enabled) {
	impl->verticalSyncEnabled = enabled;
	impl->layer.displaySyncEnabled = enabled;
}

bool MetalRenderer::getVerticalSync() const {
	return impl->verticalSyncEnabled;
}

bool MetalRenderer::isMultisampleSupported() const {
	return impl->msaaSupported;
}

void MetalRenderer::setMultisampleAntiAliasing(const bool enabled) {
	if (!impl->msaaSupported) {
		impl->msaaEnabled = false;
		impl->msaaRebuildPending = false;
		return;
	}
	if (impl->msaaEnabled == enabled && !impl->msaaRebuildPending) {
		return;
	}
	if (impl->frameActive) {
		// What's been drawn so far is in the current target, so switch after the frame
		impl->pendingMsaaEnabled = enabled;
		impl->msaaRebuildPending = true;
		return;
	}
	// The next frame creates or drops the multisampled target, see updateMsaaTarget()
	impl->msaaEnabled = enabled;
	impl->msaaRebuildPending = false;
}

bool MetalRenderer::getMultisampleAntiAliasing() const {
	if (!impl->msaaSupported) {
		return false;
	}
	return impl->msaaRebuildPending ? impl->pendingMsaaEnabled : impl->msaaEnabled;
}

void MetalRenderer::updateLetterboxing(const int fbWidth, const int fbHeight, const int canvasW,
                                       const int canvasH) {
	impl->scissor.setLetterboxing(fbWidth, fbHeight, canvasW, canvasH);
	if (impl->encoder && impl->renderTargetStack.empty()) {
		impl->applyScissor();
	}
}

void MetalRenderer::setScissor(const std::optional<ScissorBox> box) {
	impl->scissor.setBox(box);
	if (impl->encoder) {
		impl->applyScissor();
	}
}

} // namespace jngl
