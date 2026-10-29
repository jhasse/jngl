// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "D3D12Renderer.hpp"

#include "../App.hpp"
#include "../StartupProfiler.hpp"
#include "../jngl/Alpha.hpp"
#include "../jngl/Mat3.hpp"
#include "../jngl/Mat4.hpp"
#include "../log.hpp"
#include "HlslCompiler.hpp"

#include "colored.frag.spv.hpp"
#include "colored.vert.spv.hpp"
#include "rounded.frag.spv.hpp"
#include "rounded.vert.spv.hpp"
#include "textured.frag.spv.hpp"
#include "textured.vert.spv.hpp"

#include <SDL3/SDL.h>
#include <algorithm>
#include <bit>
#include <cassert>
#include <cstring>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <climits>
#include <format>
#include <functional>
#include <iterator>
#include <stdexcept>
#include <wrl/client.h>

namespace jngl {

using Microsoft::WRL::ComPtr;

namespace {

/// Throws a std::runtime_error if \a result is a failure, mentioning \a what
void check(const HRESULT result, const char* const what) {
	if (FAILED(result)) {
		throw std::runtime_error(
		    std::format("{} failed (HRESULT 0x{:08x}).", what, static_cast<uint32_t>(result)));
	}
}

/// The kinds of render targets there are pipeline states for, as a pipeline state only works with
/// render targets that have the same format and number of samples: 8-bit (the swapchain's format,
/// also used by FrameBuffers) or 16-bit float (HDR FrameBuffers), each without or with MSAA
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

/// Index into the pipeline state arrays, see D3D12ShaderProgram::pipelines. Triangle fans are
/// drawn as triangle lists, and strips can use the same pipeline states as lists.
size_t pipelineIndex(const PassKind kind, const BlendVariant variant, const PrimitiveType type) {
	const size_t topology = type == PrimitiveType::Lines ? 1 : 0;
	return (static_cast<size_t>(kind) * blendVariantCount + static_cast<size_t>(variant)) * 2 +
	       topology;
}
static_assert(D3D12ShaderProgram::pipelineCount == passKindCount * blendVariantCount * 2);

constexpr DXGI_FORMAT ldrFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
/// Of HDR FrameBuffers, which every Direct3D 12 device can render into, blend and sample with
/// linear filtering
constexpr DXGI_FORMAT hdrFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
constexpr UINT msaaSampleCount = 4;

/// Turns a four-vertex fan into two triangles, keeping the same diagonal, winding and provoking
/// vertices, as Direct3D 12 can't draw triangle fans
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

struct ColoredPushConstants {
	float mvp[16];
	float color[4];
};

struct TexturedPushConstants {
	float mvp[16];
	float lerpColor[4];
	float lerpFactors[4];
};

struct RoundedPushConstants {
	float mvp[16];
	float color[4];
	float size[2];
	float pad[2];
	float cornerRadii[4];
};

static_assert(sizeof(RoundedPushConstants) == 112);
/// The root constants, at b0, hold the push constants of all of the built-in shaders
constexpr UINT rootConstantCount = sizeof(RoundedPushConstants) / 4;

/// Root parameters, see Impl::createRootSignature()
enum RootParameter : UINT {
	RootConstants,
	RootTexture,
	RootSampler,
	RootUniforms,
};

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

/// mvp = projection * modelview (embedded into a 4x4 that leaves z at 0 and w at 1), both
/// column-major (element [col * 4 + row]). With \a flipY, clip space's y is inverted as well, see
/// Impl::flipClipY().
void computeMvp(const Mat4& projection, const Mat3& modelview, const bool flipY, float* const mvp) {
	const float* const m = modelview.data; // column-major: m[C * 3 + R]
	Mat4 embedded;
	embedded.data[0] = m[0];
	embedded.data[1] = m[1];
	embedded.data[2] = 0.f;
	embedded.data[3] = 0.f;
	embedded.data[4] = m[3];
	embedded.data[5] = m[4];
	embedded.data[6] = 0.f;
	embedded.data[7] = 0.f;
	embedded.data[8] = 0.f;
	embedded.data[9] = 0.f;
	embedded.data[10] = 0.f;
	embedded.data[11] = 0.f;
	embedded.data[12] = m[6];
	embedded.data[13] = m[7];
	embedded.data[14] = 0.f;
	embedded.data[15] = 1.f;
	for (int col = 0; col < 4; ++col) {
		for (int row = 0; row < 4; ++row) {
			float sum = 0.f;
			for (int k = 0; k < 4; ++k) {
				sum += projection.data[k * 4 + row] * embedded.data[col * 4 + k];
			}
			mvp[col * 4 + row] = row == 1 && flipY ? -sum : sum;
		}
	}
}

void setIdentity(float* const mvp) {
	const Mat4 identity;
	std::copy(std::begin(identity.data), std::end(identity.data), mvp);
}

D3D12_BLEND_DESC blendDesc(const BlendVariant variant) {
	D3D12_BLEND_DESC desc{};
	auto& state = desc.RenderTarget[0];
	state.BlendEnable = TRUE;
	state.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	state.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	state.BlendOp = D3D12_BLEND_OP_ADD;
	state.SrcBlendAlpha = D3D12_BLEND_ONE;
	state.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
	state.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	state.LogicOp = D3D12_LOGIC_OP_NOOP;
	state.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	switch (variant) {
	case BlendVariant::Composite:
		break;
	case BlendVariant::Premultiplied:
		state.SrcBlend = D3D12_BLEND_ONE;
		break;
	case BlendVariant::Replace:
		state.BlendEnable = FALSE;
		break;
	case BlendVariant::ReplaceAlphaOnly:
		state.BlendEnable = FALSE;
		state.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALPHA;
		break;
	case BlendVariant::AddAlpha:
		// So that what's drawn into a FrameBuffer is as opaque as the most opaque of it when it's
		// drawn again, see applyBlendMode()
		state.DestBlendAlpha = D3D12_BLEND_ONE;
		break;
	// The following ones blend alpha like the colors, as glBlendFunc does
	case BlendVariant::Additive:
		state.DestBlend = D3D12_BLEND_ONE;
		state.SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
		state.DestBlendAlpha = D3D12_BLEND_ONE;
		break;
	case BlendVariant::PremultipliedAdditive:
		state.SrcBlend = D3D12_BLEND_ONE;
		state.DestBlend = D3D12_BLEND_ONE;
		state.DestBlendAlpha = D3D12_BLEND_ONE;
		break;
	case BlendVariant::Multiply:
		state.SrcBlend = D3D12_BLEND_DEST_COLOR;
		state.SrcBlendAlpha = D3D12_BLEND_DEST_ALPHA;
		break;
	case BlendVariant::Screen:
		state.SrcBlend = D3D12_BLEND_ONE;
		state.DestBlend = D3D12_BLEND_INV_SRC_COLOR;
		break;
	}
	return desc;
}

D3D12_PRIMITIVE_TOPOLOGY toD3DTopology(const PrimitiveType type) {
	switch (type) {
	case PrimitiveType::Triangles:
	case PrimitiveType::TriangleFan: // see triangulateFan
		return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	case PrimitiveType::TriangleStrip:
		return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	case PrimitiveType::Lines:
		return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	}
	return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

D3D12_HEAP_PROPERTIES heapProperties(const D3D12_HEAP_TYPE type) {
	D3D12_HEAP_PROPERTIES properties{};
	properties.Type = type;
	return properties;
}

D3D12_RESOURCE_DESC textureDesc(const int width, const int height, const uint32_t mipLevels,
                                const DXGI_FORMAT format, const UINT samples,
                                const D3D12_RESOURCE_FLAGS flags) {
	D3D12_RESOURCE_DESC desc{};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Width = static_cast<UINT64>(width);
	desc.Height = static_cast<UINT>(height);
	desc.DepthOrArraySize = 1;
	desc.MipLevels = static_cast<UINT16>(mipLevels);
	desc.Format = format;
	desc.SampleDesc.Count = samples;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.Flags = flags;
	return desc;
}

D3D12_RESOURCE_DESC bufferDesc(const UINT64 size) {
	D3D12_RESOURCE_DESC desc{};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width = size;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	return desc;
}

/// Records a barrier that moves \a subresource (or all of them) of \a resource from \a before to
/// \a after, if they differ
void transition(ID3D12GraphicsCommandList* const cmd, ID3D12Resource* const resource,
                const D3D12_RESOURCE_STATES before, const D3D12_RESOURCE_STATES after,
                const UINT subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES) {
	if (before == after) {
		return;
	}
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = resource;
	barrier.Transition.Subresource = subresource;
	barrier.Transition.StateBefore = before;
	barrier.Transition.StateAfter = after;
	cmd->ResourceBarrier(1, &barrier);
}

uint64_t alignUp(const uint64_t value, const uint64_t alignment) {
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

/// Index of the sampler in Impl::samplerHeap that filters like this
D3D12Descriptor samplerIndex(const TextureFilter minFilter, const TextureFilter magFilter) {
	return (minFilter == TextureFilter::NearestNeighbor ? 1 : 0) |
	       (magFilter == TextureFilter::NearestNeighbor ? 2 : 0);
}

std::string toUtf8(const wchar_t* const wide) {
	const int size = WideCharToMultiByte(CP_UTF8, 0, wide, -1, nullptr, 0, nullptr, nullptr);
	if (size <= 1) {
		return {};
	}
	std::string utf8(static_cast<size_t>(size - 1), '\0');
	WideCharToMultiByte(CP_UTF8, 0, wide, -1, utf8.data(), size, nullptr, nullptr);
	return utf8;
}

#ifndef NDEBUG
/// Logs the debug layer's messages
void CALLBACK debugCallback(D3D12_MESSAGE_CATEGORY /*category*/,
                            const D3D12_MESSAGE_SEVERITY severity, D3D12_MESSAGE_ID /*id*/,
                            const LPCSTR description, void* /*context*/) {
	if (severity <= D3D12_MESSAGE_SEVERITY_ERROR) {
		internal::error(std::string("Direct3D 12: ") + description);
	} else if (severity == D3D12_MESSAGE_SEVERITY_WARNING) {
		internal::debug("Direct3D 12: {}", description);
	} else {
		internal::trace(std::string("Direct3D 12: ") + description);
	}
}
#endif

/// Descriptors of one type, which are handed out by index and can be given back
class DescriptorHeap {
public:
	void create(ID3D12Device* const device, const D3D12_DESCRIPTOR_HEAP_TYPE type,
	            const uint32_t capacity, const bool shaderVisible) {
		D3D12_DESCRIPTOR_HEAP_DESC desc{};
		desc.Type = type;
		desc.NumDescriptors = capacity;
		desc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
		                           : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		check(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap)), "CreateDescriptorHeap");
		increment = device->GetDescriptorHandleIncrementSize(type);
		this->capacity = capacity;
	}

	[[nodiscard]] D3D12Descriptor allocate() {
		if (!freeList.empty()) {
			const D3D12Descriptor index = freeList.back();
			freeList.pop_back();
			return index;
		}
		if (next == capacity) {
			throw std::runtime_error(
			    std::format("All {} Direct3D 12 descriptors are in use.", capacity));
		}
		return next++;
	}

	void free(const D3D12Descriptor index) {
		if (index != noD3D12Descriptor) {
			freeList.push_back(index);
		}
	}

	[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE cpu(const D3D12Descriptor index) const {
		D3D12_CPU_DESCRIPTOR_HANDLE handle = heap->GetCPUDescriptorHandleForHeapStart();
		handle.ptr += static_cast<SIZE_T>(index) * increment;
		return handle;
	}

	[[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE gpu(const D3D12Descriptor index) const {
		D3D12_GPU_DESCRIPTOR_HANDLE handle = heap->GetGPUDescriptorHandleForHeapStart();
		handle.ptr += static_cast<UINT64>(index) * increment;
		return handle;
	}

	[[nodiscard]] ID3D12DescriptorHeap* get() const {
		return heap.Get();
	}

private:
	ComPtr<ID3D12DescriptorHeap> heap;
	UINT increment = 0;
	uint32_t capacity = 0;
	uint32_t next = 0;
	std::vector<D3D12Descriptor> freeList;
};

/// Persistently mapped buffer in an upload heap that's appended to during a frame
struct UploadBuffer {
	ComPtr<ID3D12Resource> resource;
	uint8_t* mapped = nullptr;
	uint64_t capacity = 0;
	uint64_t used = 0;
};

/// Compiled shaders of a pipeline state
struct ShaderPair {
	std::vector<uint8_t> vertex;
	std::vector<uint8_t> pixel;
};

/// The vertex layouts of the pipeline states: only the position, or with texture coordinates
const std::array<D3D12_INPUT_ELEMENT_DESC, 2> vertexLayout = { {
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
	  0 },
	{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
	  0 },
} };

/// What a top-left based rectangle in pixels of a render target is
struct Rect {
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
};

} // namespace

struct D3D12TimestampPool {
	ComPtr<ID3D12QueryHeap> heap;
	/// The queries' ticks are resolved into this
	ComPtr<ID3D12Resource> readback;
	const uint64_t* ticks = nullptr; ///< readback, mapped
	/// For each query, the value of Impl::fence once it's been written, or 0 if it hasn't been
	std::vector<uint64_t> writtenAt;
};

struct D3D12Renderer::Impl {
	explicit Impl(SDL_Window*);
	~Impl();
	Impl(const Impl&) = delete;
	Impl& operator=(const Impl&) = delete;
	Impl(Impl&&) = delete;
	Impl& operator=(Impl&&) = delete;

	void createDevice();
	void createSwapchain();
	/// Gets the swap chain's buffers and creates their render target views
	void acquireBackBuffers();
	void recreateSwapchain();
	void queryMsaaSupport();
	/// The multisampled render target that's drawn into instead of the back buffer with MSAA, and
	/// resolved into it at the end of the frame
	void createMsaaTarget();
	void destroyMsaaTarget();
	[[nodiscard]] bool swapchainMsaaActive() const;
	void createRootSignature();
	void createSamplers();
	void compileBuiltinShaders();

	[[nodiscard]] ComPtr<ID3D12PipelineState>
	createPipeline(const std::vector<uint8_t>& vertexShader,
	               const std::vector<uint8_t>& pixelShader, bool textured, PassKind, BlendVariant,
	               PrimitiveType) const;
	enum class Builtin : uint8_t { Colored, Rounded, Textured };
	/// The built-in pipeline state for this, built when it's first used
	ID3D12PipelineState* builtinPipeline(Builtin, PassKind, BlendVariant, PrimitiveType);
	/// \a program's pipeline state for this, built when it's first used
	ID3D12PipelineState* programPipeline(const D3D12ShaderProgram&, PassKind, BlendVariant,
	                                     PrimitiveType) const;
	[[nodiscard]] BlendVariant blendVariant() const;

	[[nodiscard]] UploadBuffer createUploadBuffer(uint64_t capacity) const;
	/// Copies \a bytes of vertex data into the current frame's vertex buffer, growing it if it's
	/// full, and returns where it is
	D3D12_GPU_VIRTUAL_ADDRESS uploadVertices(const void* data, uint64_t bytes);
	/// Copies the active program's uniforms into this frame's ring and binds them
	void bindActiveProgramUniforms();

	/// Resets \a list with \a allocator and sets up what all of the draws use
	void beginCommandList(ID3D12GraphicsCommandList* list, ID3D12CommandAllocator* allocator) const;
	/// The upload command list, which is executed ahead of the frame's, so that the uploads are
	/// done before any of its draws. Starts recording it if it isn't yet.
	ID3D12GraphicsCommandList* uploadCommands();
	/// Keeps \a buffer until the uploads that copy from it are done
	void keepStagingBuffer(ComPtr<ID3D12Resource> buffer);
	/// Executes the pending uploads and the frame's command list
	void submit();
	/// Signals the fence after what's been submitted so far, returning the value it'll have then
	uint64_t signal();
	void waitForFence(uint64_t value);
	void waitIdle();
	/// Calls \a destroy once the GPU is done with what might be referenced by the current or an
	/// in-flight frame's command list, see endFrame()
	void destroyLater(std::function<void()> destroy);

	[[nodiscard]] ID3D12Resource* backBuffer() const;
	void transitionBackBuffer(D3D12_RESOURCE_STATES);
	/// Sets what's drawn into to the top of renderTargetStack, or the swapchain, if that hasn't
	/// happened since it changed. Draws and clears call this.
	void bindTarget();
	/// Moves the FrameBuffer that's drawn into back to PIXEL_SHADER_RESOURCE, so that it can be
	/// sampled. The next draw calls bindTarget() again.
	void unbindTarget();
	/// Clears the swap chain's image to the clear color, and the letterboxing's bars to black
	void clearSwapchain();
	/// With MSAA, resolves what's been drawn into the swapchain's image so far
	void resolveSwapchain();
	/// Draws \a framebuffer's color into all samples of its multisampled texture
	void expandIntoSamples(D3D12Framebuffer& framebuffer);
	/// Resolves what's been drawn into the multisampled texture of \a framebuffer into its color
	void resolveFramebuffer(D3D12Framebuffer& framebuffer);
	/// Whether clip space's y has to be inverted for what's drawn now: FrameBuffers are drawn into
	/// upside down like OpenGL FBO textures (see jngl::FrameBuffer::draw), which the Vulkan backend
	/// gets by not flipping its viewport, while Direct3D 12's viewports can't be flipped.
	[[nodiscard]] bool flipClipY() const;
	void draw(ID3D12PipelineState*, const void* push, size_t pushSize,
	          D3D12_GPU_VIRTUAL_ADDRESS vertices, size_t vertexCount, UINT stride, PrimitiveType);

	void applyViewport() const;
	void applyScissor() const;
	[[nodiscard]] Rect effectiveScissor(int targetWidth, int targetHeight) const;
	[[nodiscard]] Rect letterboxScissor() const;
	void syncLetterboxScissor();
	[[nodiscard]] static Rect glScissorToTopLeft(int glX, int glY, int glW, int glH,
	                                             int targetHeight);
	[[nodiscard]] static Rect intersectScissors(Rect a, Rect b);
	/// The swapchain's size, or the FrameBuffer's that's drawn on
	[[nodiscard]] std::pair<int, int> currentTargetSize() const;

	/// Creates a texture that can be sampled, and drawn into to generate its mipmaps if it has
	/// any, in \a state
	ComPtr<ID3D12Resource> createTextureResource(int width, int height, uint32_t mipLevels,
	                                             D3D12_RESOURCE_STATES state) const;
	void createTextureView(D3D12Texture&, DXGI_FORMAT);
	/// Records drawing all of the texture's mip levels from the first one, which has to be in
	/// COPY_DEST like the others. Leaves all of them in PIXEL_SHADER_RESOURCE.
	void recordMipmapGeneration(ID3D12GraphicsCommandList*, const D3D12Texture&);
	void freeTextureResources(const D3D12Texture&);

	/// Maximum number of frames the CPU may be working on ahead of the GPU.
	static constexpr uint32_t maxFramesInFlight = 2;
	static constexpr UINT backBufferCount = 3;

	SDL_Window* window;
	HWND hwnd = nullptr;

	// Released last, after what's been created from it
	ComPtr<ID3D12Device> device;
	ComPtr<IDXGIFactory4> factory;
	ComPtr<IDXGIAdapter1> adapter;
	ComPtr<ID3D12InfoQueue1> infoQueue;
	DWORD debugCallbackCookie = 0;
	ComPtr<ID3D12CommandQueue> queue;
	ComPtr<IDXGISwapChain3> swapchain;
	bool tearingSupported = false;
	int swapchainWidth = 0;
	int swapchainHeight = 0;
	std::array<ComPtr<ID3D12Resource>, backBufferCount> backBuffers;
	std::array<D3D12Descriptor, backBufferCount> backBufferRtvs{};
	std::array<D3D12_RESOURCE_STATES, backBufferCount> backBufferStates{};
	UINT backBufferIndex = 0;
	ComPtr<ID3D12Resource> msaaTarget; ///< see createMsaaTarget()
	D3D12Descriptor msaaTargetRtv = noD3D12Descriptor;

	DescriptorHeap rtvHeap;
	DescriptorHeap srvHeap; ///< shader-visible, see D3D12Texture::srv
	DescriptorHeap samplerHeap; ///< shader-visible, see samplerIndex()
	static constexpr uint32_t rtvCapacity = 4096;
	static constexpr uint32_t srvCapacity = 65536;

	ComPtr<ID3D12RootSignature> rootSignature;
	ShaderPair coloredShaders;
	ShaderPair roundedShaders;
	ShaderPair texturedShaders;
	// Indexed by Builtin and pipelineIndex(), see builtinPipeline()
	std::array<std::array<ComPtr<ID3D12PipelineState>, D3D12ShaderProgram::pipelineCount>, 3>
	    builtinPipelines;
	/// See fullscreenQuad, which never changes, so that it can also be used by the uploads
	UploadBuffer fullscreenQuadBuffer;

	std::array<ComPtr<ID3D12CommandAllocator>, maxFramesInFlight> allocators;
	ComPtr<ID3D12GraphicsCommandList> commandList;
	std::array<ComPtr<ID3D12CommandAllocator>, maxFramesInFlight> uploadAllocators;
	ComPtr<ID3D12GraphicsCommandList> uploadList;
	// Whether uploadList is being recorded and hasn't been executed yet
	bool uploadPending = false;
	// Copied from by the pending uploads
	std::vector<ComPtr<ID3D12Resource>> pendingStagingBuffers;
	// Per frame in flight: copied from by the uploads executed with it, so they're freed once its
	// fence value has been reached
	std::array<std::vector<ComPtr<ID3D12Resource>>, maxFramesInFlight> retiredStagingBuffers;

	ComPtr<ID3D12Fence> fence;
	HANDLE fenceEvent = nullptr;
	uint64_t lastSignaled = 0;
	/// The fence's value once the GPU is done with each frame in flight
	std::array<uint64_t, maxFramesInFlight> frameFenceValues{};

	// Per frame in flight, written directly each frame. `used` is reset in beginFrame and grows as
	// the draw calls append to it, see uploadVertices.
	std::array<UploadBuffer, maxFramesInFlight> vertexBuffers;
	// Buffers that ran full during a frame and were replaced by a bigger one. They're still
	// referenced by that frame's command list, so they're released once its fence value is reached.
	std::array<std::vector<UploadBuffer>, maxFramesInFlight> retiredVertexBuffers;
	// Each draw with a custom ShaderProgram that has uniforms copies them into here, see
	// bindActiveProgramUniforms. Grows like vertexBuffers.
	std::array<UploadBuffer, maxFramesInFlight> uniformBuffers;
	std::array<std::vector<UploadBuffer>, maxFramesInFlight> retiredUniformBuffers;
	// Reused when expanding triangle fans
	std::vector<float> triangulatedVertices;

	// What's been destroyed while it might still be referenced by an in-flight (or not yet
	// submitted) command list, e.g. textures and FrameBuffers, which is actually freed once enough
	// frames have passed, see destroyLater() and endFrame()
	struct PendingDestruction {
		std::function<void()> destroy;
		int framesUntilFree;
	};
	std::vector<PendingDestruction> pendingDestructions;

	uint32_t currentFrame = 0;
	bool frameActive = false;
	// The swapchain has to be resized after the current frame has been presented
	bool swapchainRecreatePending = false;

	// Stack of active FrameBuffers; empty means drawing targets the swapchain.
	std::vector<D3D12Framebuffer*> renderTargetStack;
	// Whether bindTarget() has set up what's drawn into
	bool targetBound = false;
	// Whether bindTarget() has moved the color of the FrameBuffer on top of renderTargetStack to
	// RENDER_TARGET, see unbindTarget()
	bool targetColorIsRenderTarget = false;
	D3D12_CPU_DESCRIPTOR_HANDLE currentRtv{};
	// What's drawn into now, which decides the pipeline states that have to be used
	PassKind currentPassKind = PassKind::Ldr;

	const D3D12ShaderProgram* activeShaderProgram = nullptr;
	bool blendingEnabled = true;
	SpriteBlend spriteBlend = SpriteBlend::Composite;
	bool alphaOnlyWrite = false;

	bool verticalSyncEnabled = true;
	bool msaaSupported = false;
	bool msaaEnabled = true;
	bool msaaRebuildPending = false;
	bool pendingMsaaEnabled = true;

	// Ticks per second of the queue's timestamps, or 0 if they aren't supported
	uint64_t timestampFrequency = 0;

	int framebufferWidth = 0;
	int framebufferHeight = 0;
	int canvasWidth = 0;
	int canvasHeight = 0;
	bool letterboxing = false;
	std::optional<ScissorBox> userScissor;

	Mat4 projection;
	Rgb currentClearColor{ 0, 0, 0 }; // swapchain clear color for the current frame
};

D3D12Renderer::Impl::Impl(SDL_Window* const window) : window(window) {
	internal::StartupProfiler total("D3D12Renderer total");
	hwnd = static_cast<HWND>(SDL_GetPointerProperty(SDL_GetWindowProperties(window),
	                                                SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr));
	if (!hwnd) {
		throw std::runtime_error(std::string("Couldn't get the window's HWND: ") + SDL_GetError());
	}
	{
		internal::StartupProfiler _{ "createDevice" };
		createDevice();
	}
	rtvHeap.create(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, rtvCapacity, false);
	srvHeap.create(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, srvCapacity, true);
	createSamplers();
	{
		internal::StartupProfiler _{ "createSwapchain" };
		createSwapchain();
	}
	queryMsaaSupport();
	if (swapchainMsaaActive()) {
		createMsaaTarget();
	}

	for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
		check(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		                                     IID_PPV_ARGS(&allocators[i])),
		      "CreateCommandAllocator");
		check(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		                                     IID_PPV_ARGS(&uploadAllocators[i])),
		      "CreateCommandAllocator");
	}
	check(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocators[0].Get(), nullptr,
	                                IID_PPV_ARGS(&commandList)),
	      "CreateCommandList");
	check(commandList->Close(), "ID3D12GraphicsCommandList::Close");
	check(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, uploadAllocators[0].Get(),
	                                nullptr, IID_PPV_ARGS(&uploadList)),
	      "CreateCommandList");
	check(uploadList->Close(), "ID3D12GraphicsCommandList::Close");
	check(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)), "CreateFence");
	fenceEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
	if (!fenceEvent) {
		throw std::runtime_error("CreateEvent failed.");
	}
	if (UINT64 frequency = 0; SUCCEEDED(queue->GetTimestampFrequency(&frequency))) {
		timestampFrequency = frequency;
	}

	createRootSignature();
	{
		internal::StartupProfiler _{ "compileBuiltinShaders" };
		compileBuiltinShaders();
	}
	{
		// Only those of the window's default blending, the others are built once they're used
		internal::StartupProfiler _{ "builtinPipeline" };
		for (const auto builtin : { Builtin::Colored, Builtin::Textured }) {
			builtinPipeline(builtin, PassKind::Ldr, BlendVariant::Composite,
			                PrimitiveType::Triangles);
			if (msaaSupported) {
				builtinPipeline(builtin, PassKind::LdrMsaa, BlendVariant::Composite,
				                PrimitiveType::Triangles);
			}
		}
	}
	for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
		vertexBuffers[i] = createUploadBuffer(uint64_t{ 1 } << 20);
		uniformBuffers[i] = createUploadBuffer(uint64_t{ 64 } << 10);
	}
	fullscreenQuadBuffer = createUploadBuffer(sizeof(fullscreenQuad));
	std::memcpy(fullscreenQuadBuffer.mapped, fullscreenQuad.data(), sizeof(fullscreenQuad));
	internal::debug("Direct3D 12 renderer initialized ({}x{}).", swapchainWidth, swapchainHeight);
}

D3D12Renderer::Impl::~Impl() {
	if (queue && fence) {
		waitIdle();
	}
	for (auto& pending : pendingDestructions) {
		pending.destroy();
	}
	pendingDestructions.clear();
	if (infoQueue && debugCallbackCookie) {
		infoQueue->UnregisterMessageCallback(debugCallbackCookie);
	}
	if (fenceEvent) {
		CloseHandle(fenceEvent);
	}
}

void D3D12Renderer::Impl::createDevice() {
	UINT factoryFlags = 0;
#ifndef NDEBUG
	if (ComPtr<ID3D12Debug> debug; SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)))) {
		debug->EnableDebugLayer();
		factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	} else {
		internal::debug("Direct3D 12 debug layer requested but not available.");
	}
#endif
	if (FAILED(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&factory)))) {
		check(CreateDXGIFactory2(0, IID_PPV_ARGS(&factory)), "CreateDXGIFactory2");
	}

	// Prefer the high-performance GPU, but accept anything (e.g. WARP) that supports Direct3D 12
	const auto tryAdapter = [this](IDXGIAdapter1* const candidate) {
		DXGI_ADAPTER_DESC1 desc;
		candidate->GetDesc1(&desc);
		if ((desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) != 0) {
			return false;
		}
		return SUCCEEDED(
		    D3D12CreateDevice(candidate, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)));
	};
	if (ComPtr<IDXGIFactory6> factory6; SUCCEEDED(factory.As(&factory6))) {
		for (UINT i = 0;
		     !device && SUCCEEDED(factory6->EnumAdapterByGpuPreference(
		                    i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)));
		     ++i) {
			tryAdapter(adapter.Get());
		}
	}
	for (UINT i = 0; !device && factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i) {
		tryAdapter(adapter.Get());
	}
	if (!device) {
		internal::debug("No Direct3D 12 GPU found, falling back to WARP.");
		check(factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter)), "EnumWarpAdapter");
		check(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)),
		      "D3D12CreateDevice");
	}
	DXGI_ADAPTER_DESC1 desc;
	adapter->GetDesc1(&desc);
	internal::debug("Direct3D 12: using {}", toUtf8(desc.Description));

#ifndef NDEBUG
	if (SUCCEEDED(device.As(&infoQueue))) {
		// The clear values are optimizations that don't fit every clear
		std::array<D3D12_MESSAGE_ID, 1> denied = {
			D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
		};
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = static_cast<UINT>(denied.size());
		filter.DenyList.pIDList = denied.data();
		infoQueue->PushStorageFilter(&filter);
		infoQueue->RegisterMessageCallback(debugCallback, D3D12_MESSAGE_CALLBACK_FLAG_NONE, nullptr,
		                                   &debugCallbackCookie);
	}
#endif

	D3D12_COMMAND_QUEUE_DESC queueDesc{};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	check(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&queue)), "CreateCommandQueue");

	if (ComPtr<IDXGIFactory5> factory5; SUCCEEDED(factory.As(&factory5))) {
		BOOL allowTearing = FALSE;
		if (SUCCEEDED(factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING,
		                                            &allowTearing, sizeof(allowTearing)))) {
			tearingSupported = allowTearing == TRUE;
		}
	}
}

void D3D12Renderer::Impl::createSwapchain() {
	int width = 0;
	int height = 0;
	SDL_GetWindowSizeInPixels(window, &width, &height);
	DXGI_SWAP_CHAIN_DESC1 desc{};
	desc.Width = static_cast<UINT>(std::max(width, 1));
	desc.Height = static_cast<UINT>(std::max(height, 1));
	// Not an sRGB format, so that colors written by the shaders reach the screen unmodified,
	// matching JNGL's OpenGL backend whose default framebuffer isn't sRGB-encoded.
	desc.Format = ldrFormat;
	desc.SampleDesc.Count = 1;
	// COPY_SOURCE for readPixels, RESOLVE_DEST for MSAA
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount = backBufferCount;
	desc.Scaling = DXGI_SCALING_STRETCH;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	desc.Flags = tearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
	ComPtr<IDXGISwapChain1> swapchain1;
	check(factory->CreateSwapChainForHwnd(queue.Get(), hwnd, &desc, nullptr, nullptr, &swapchain1),
	      "CreateSwapChainForHwnd");
	check(swapchain1.As(&swapchain), "IDXGISwapChain3");
	// SDL handles fullscreen
	factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
	for (auto& rtv : backBufferRtvs) {
		rtv = rtvHeap.allocate();
	}
	acquireBackBuffers();
}

void D3D12Renderer::Impl::acquireBackBuffers() {
	DXGI_SWAP_CHAIN_DESC1 desc;
	check(swapchain->GetDesc1(&desc), "IDXGISwapChain1::GetDesc1");
	swapchainWidth = static_cast<int>(desc.Width);
	swapchainHeight = static_cast<int>(desc.Height);
	for (UINT i = 0; i < backBufferCount; ++i) {
		check(swapchain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i])), "IDXGISwapChain::GetBuffer");
		device->CreateRenderTargetView(backBuffers[i].Get(), nullptr,
		                               rtvHeap.cpu(backBufferRtvs[i]));
		backBufferStates[i] = D3D12_RESOURCE_STATE_PRESENT;
	}
	backBufferIndex = swapchain->GetCurrentBackBufferIndex();
}

void D3D12Renderer::Impl::recreateSwapchain() {
	int width = 0;
	int height = 0;
	SDL_GetWindowSizeInPixels(window, &width, &height);
	while (width == 0 || height == 0) { // minimized: wait until the window has a size again
		SDL_GetWindowSizeInPixels(window, &width, &height);
		SDL_WaitEvent(nullptr);
	}
	waitIdle();
	for (auto& buffer : backBuffers) {
		buffer.Reset();
	}
	check(swapchain->ResizeBuffers(backBufferCount, static_cast<UINT>(width),
	                               static_cast<UINT>(height), ldrFormat,
	                               tearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0),
	      "IDXGISwapChain::ResizeBuffers");
	acquireBackBuffers();
	if (msaaTarget) {
		destroyMsaaTarget();
		createMsaaTarget();
	}
}

void D3D12Renderer::Impl::queryMsaaSupport() {
	msaaSupported = true;
	for (const DXGI_FORMAT format : { ldrFormat, hdrFormat }) {
		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS levels{};
		levels.Format = format;
		levels.SampleCount = msaaSampleCount;
		if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &levels,
		                                       sizeof(levels))) ||
		    levels.NumQualityLevels == 0) {
			msaaSupported = false;
		}
	}
	if (!msaaSupported) {
		msaaEnabled = false;
	}
	internal::debug("Direct3D 12 MSAA 4x: {} (enabled: {}).",
	                msaaSupported ? "supported" : "unsupported", msaaEnabled);
}

void D3D12Renderer::Impl::createMsaaTarget() {
	const auto desc = textureDesc(swapchainWidth, swapchainHeight, 1, ldrFormat, msaaSampleCount,
	                              D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
	const auto heap = heapProperties(D3D12_HEAP_TYPE_DEFAULT);
	check(device->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &desc,
	                                      D3D12_RESOURCE_STATE_RENDER_TARGET, nullptr,
	                                      IID_PPV_ARGS(&msaaTarget)),
	      "CreateCommittedResource");
	msaaTargetRtv = rtvHeap.allocate();
	device->CreateRenderTargetView(msaaTarget.Get(), nullptr, rtvHeap.cpu(msaaTargetRtv));
}

void D3D12Renderer::Impl::destroyMsaaTarget() {
	msaaTarget.Reset();
	rtvHeap.free(msaaTargetRtv);
	msaaTargetRtv = noD3D12Descriptor;
}

bool D3D12Renderer::Impl::swapchainMsaaActive() const {
	return msaaEnabled && msaaSupported;
}

void D3D12Renderer::Impl::createRootSignature() {
	std::array<D3D12_ROOT_PARAMETER, 4> parameters{};
	parameters[RootConstants].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	parameters[RootConstants].Constants.ShaderRegister = 0;
	parameters[RootConstants].Constants.Num32BitValues = rootConstantCount;
	parameters[RootConstants].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_DESCRIPTOR_RANGE textureRange{};
	textureRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	textureRange.NumDescriptors = 1;
	parameters[RootTexture].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	parameters[RootTexture].DescriptorTable.NumDescriptorRanges = 1;
	parameters[RootTexture].DescriptorTable.pDescriptorRanges = &textureRange;
	parameters[RootTexture].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_DESCRIPTOR_RANGE samplerRange{};
	samplerRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
	samplerRange.NumDescriptors = 1;
	parameters[RootSampler].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	parameters[RootSampler].DescriptorTable.NumDescriptorRanges = 1;
	parameters[RootSampler].DescriptorTable.pDescriptorRanges = &samplerRange;
	parameters[RootSampler].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	parameters[RootUniforms].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	parameters[RootUniforms].Descriptor.ShaderRegister = 1;
	parameters[RootUniforms].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_DESC desc{};
	desc.NumParameters = static_cast<UINT>(parameters.size());
	desc.pParameters = parameters.data();
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	ComPtr<ID3DBlob> serialized;
	ComPtr<ID3DBlob> errors;
	if (FAILED(D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &serialized,
	                                       &errors))) {
		throw std::runtime_error(
		    "D3D12SerializeRootSignature failed: " +
		    std::string(errors ? static_cast<const char*>(errors->GetBufferPointer()) : ""));
	}
	check(device->CreateRootSignature(0, serialized->GetBufferPointer(),
	                                  serialized->GetBufferSize(), IID_PPV_ARGS(&rootSignature)),
	      "CreateRootSignature");
}

void D3D12Renderer::Impl::createSamplers() {
	samplerHeap.create(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 4, true);
	for (const auto minFilter : { TextureFilter::Bilinear, TextureFilter::NearestNeighbor }) {
		for (const auto magFilter : { TextureFilter::Bilinear, TextureFilter::NearestNeighbor }) {
			D3D12_SAMPLER_DESC desc{};
			// Mipmaps are sampled like the minification, like GL_LINEAR_MIPMAP_LINEAR or
			// GL_NEAREST_MIPMAP_NEAREST
			const bool linearMin = minFilter == TextureFilter::Bilinear;
			const bool linearMag = magFilter == TextureFilter::Bilinear;
			if (linearMin) {
				desc.Filter = linearMag ? D3D12_FILTER_MIN_MAG_MIP_LINEAR
				                        : D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
			} else {
				desc.Filter = linearMag ? D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT
				                        : D3D12_FILTER_MIN_MAG_MIP_POINT;
			}
			desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			desc.MaxLOD = D3D12_FLOAT32_MAX;
			const D3D12Descriptor index = samplerIndex(minFilter, magFilter);
			device->CreateSampler(&desc, samplerHeap.cpu(index));
		}
	}
}

void D3D12Renderer::Impl::compileBuiltinShaders() {
	coloredShaders = {
		compileSpirvToDxbc(spirv_shaders::colored_vert, Shader::Type::VERTEX, "colored.vert"),
		compileSpirvToDxbc(spirv_shaders::colored_frag, Shader::Type::FRAGMENT, "colored.frag"),
	};
	roundedShaders = {
		compileSpirvToDxbc(spirv_shaders::rounded_vert, Shader::Type::VERTEX, "rounded.vert"),
		compileSpirvToDxbc(spirv_shaders::rounded_frag, Shader::Type::FRAGMENT, "rounded.frag"),
	};
	texturedShaders = {
		compileSpirvToDxbc(spirv_shaders::textured_vert, Shader::Type::VERTEX, "textured.vert"),
		compileSpirvToDxbc(spirv_shaders::textured_frag, Shader::Type::FRAGMENT, "textured.frag"),
	};
}

ComPtr<ID3D12PipelineState>
D3D12Renderer::Impl::createPipeline(const std::vector<uint8_t>& vertexShader,
                                    const std::vector<uint8_t>& pixelShader, const bool textured,
                                    const PassKind kind, const BlendVariant variant,
                                    const PrimitiveType type) const {
	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{};
	desc.pRootSignature = rootSignature.Get();
	desc.VS = { vertexShader.data(), vertexShader.size() };
	desc.PS = { pixelShader.data(), pixelShader.size() };
	desc.BlendState = blendDesc(variant);
	desc.SampleMask = UINT_MAX;
	desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	desc.RasterizerState.DepthClipEnable = TRUE;
	desc.InputLayout = { vertexLayout.data(), textured ? 2u : 1u };
	desc.PrimitiveTopologyType = type == PrimitiveType::Lines
	                                 ? D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE
	                                 : D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	desc.NumRenderTargets = 1;
	desc.RTVFormats[0] = isHdr(kind) ? hdrFormat : ldrFormat;
	desc.SampleDesc.Count = isMultisampled(kind) ? msaaSampleCount : 1;
	ComPtr<ID3D12PipelineState> pipeline;
	check(device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipeline)),
	      "CreateGraphicsPipelineState");
	return pipeline;
}

ID3D12PipelineState* D3D12Renderer::Impl::builtinPipeline(const Builtin builtin,
                                                          const PassKind kind,
                                                          const BlendVariant variant,
                                                          const PrimitiveType type) {
	auto& pipeline =
	    builtinPipelines[static_cast<size_t>(builtin)][pipelineIndex(kind, variant, type)];
	if (!pipeline) {
		const ShaderPair& shaders = builtin == Builtin::Colored   ? coloredShaders
		                            : builtin == Builtin::Rounded ? roundedShaders
		                                                          : texturedShaders;
		pipeline = createPipeline(shaders.vertex, shaders.pixel, builtin == Builtin::Textured, kind,
		                          variant, type);
	}
	return pipeline.Get();
}

ID3D12PipelineState* D3D12Renderer::Impl::programPipeline(const D3D12ShaderProgram& program,
                                                          const PassKind kind,
                                                          const BlendVariant variant,
                                                          const PrimitiveType type) const {
	auto& pipeline = program.pipelines[pipelineIndex(kind, variant, type)];
	if (!pipeline) {
		pipeline =
		    createPipeline(program.vertexShader, program.pixelShader, true, kind, variant, type)
		        .Detach();
	}
	return pipeline;
}

BlendVariant D3D12Renderer::Impl::blendVariant() const {
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

UploadBuffer D3D12Renderer::Impl::createUploadBuffer(const uint64_t capacity) const {
	UploadBuffer buffer;
	const auto heap = heapProperties(D3D12_HEAP_TYPE_UPLOAD);
	const auto desc = bufferDesc(capacity);
	check(device->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &desc,
	                                      D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
	                                      IID_PPV_ARGS(&buffer.resource)),
	      "CreateCommittedResource");
	const D3D12_RANGE noRead{ 0, 0 };
	void* mapped = nullptr;
	check(buffer.resource->Map(0, &noRead, &mapped), "ID3D12Resource::Map");
	buffer.mapped = static_cast<uint8_t*>(mapped);
	buffer.capacity = capacity;
	return buffer;
}

D3D12_GPU_VIRTUAL_ADDRESS D3D12Renderer::Impl::uploadVertices(const void* const data,
                                                              const uint64_t bytes) {
	UploadBuffer& vb = vertexBuffers[currentFrame];
	if (vb.used + bytes > vb.capacity) {
		// The draws recorded so far still read from the full buffer, so it can't be resized in
		// place. Replace it with a bigger one, which is kept for the following frames.
		uint64_t capacity = vb.capacity * 2;
		while (capacity < bytes) {
			capacity *= 2;
		}
		internal::debug("Growing the Direct3D 12 vertex buffer of frame {} to {} KiB.",
		                currentFrame, capacity / 1024);
		retiredVertexBuffers[currentFrame].push_back(std::move(vb));
		vb = createUploadBuffer(capacity);
	}
	const uint64_t offset = vb.used;
	std::memcpy(vb.mapped + offset, data, bytes);
	vb.used += alignUp(bytes, 16);
	return vb.resource->GetGPUVirtualAddress() + offset;
}

void D3D12Renderer::Impl::bindActiveProgramUniforms() {
	if (!activeShaderProgram || activeShaderProgram->uniformBlockSize == 0) {
		return;
	}
	const uint64_t size = alignUp(activeShaderProgram->uniformBlockSize,
	                              D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
	UploadBuffer& ub = uniformBuffers[currentFrame];
	uint64_t offset = alignUp(ub.used, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
	if (offset + size > ub.capacity) {
		// See uploadVertices
		uint64_t capacity = ub.capacity * 2;
		while (capacity < size) {
			capacity *= 2;
		}
		internal::debug("Growing the Direct3D 12 uniform buffer of frame {} to {} KiB.",
		                currentFrame, capacity / 1024);
		retiredUniformBuffers[currentFrame].push_back(std::move(ub));
		ub = createUploadBuffer(capacity);
		offset = 0;
	}
	std::memcpy(ub.mapped + offset, activeShaderProgram->uniformData.data(),
	            activeShaderProgram->uniformBlockSize);
	ub.used = offset + size;
	commandList->SetGraphicsRootConstantBufferView(RootUniforms,
	                                               ub.resource->GetGPUVirtualAddress() + offset);
}

void D3D12Renderer::Impl::beginCommandList(ID3D12GraphicsCommandList* const list,
                                           ID3D12CommandAllocator* const allocator) const {
	check(allocator->Reset(), "ID3D12CommandAllocator::Reset");
	check(list->Reset(allocator, nullptr), "ID3D12GraphicsCommandList::Reset");
	const std::array<ID3D12DescriptorHeap*, 2> heaps = { srvHeap.get(), samplerHeap.get() };
	list->SetDescriptorHeaps(static_cast<UINT>(heaps.size()), heaps.data());
	list->SetGraphicsRootSignature(rootSignature.Get());
}

ID3D12GraphicsCommandList* D3D12Renderer::Impl::uploadCommands() {
	if (!uploadPending) {
		if (!frameActive) {
			// Its last submission might still be running, beginFrame() would wait for it anyway
			waitForFence(frameFenceValues[currentFrame]);
		}
		beginCommandList(uploadList.Get(), uploadAllocators[currentFrame].Get());
		uploadPending = true;
	}
	return uploadList.Get();
}

void D3D12Renderer::Impl::keepStagingBuffer(ComPtr<ID3D12Resource> buffer) {
	pendingStagingBuffers.push_back(std::move(buffer));
}

void D3D12Renderer::Impl::submit() {
	check(commandList->Close(), "ID3D12GraphicsCommandList::Close");
	std::array<ID3D12CommandList*, 2> lists{};
	UINT count = 0;
	if (uploadPending) {
		check(uploadList->Close(), "ID3D12GraphicsCommandList::Close");
		uploadPending = false;
		lists[count++] = uploadList.Get();
		auto& retired = retiredStagingBuffers[currentFrame];
		std::move(pendingStagingBuffers.begin(), pendingStagingBuffers.end(),
		          std::back_inserter(retired));
		pendingStagingBuffers.clear();
	}
	lists[count++] = commandList.Get();
	queue->ExecuteCommandLists(count, lists.data());
}

uint64_t D3D12Renderer::Impl::signal() {
	check(queue->Signal(fence.Get(), ++lastSignaled), "ID3D12CommandQueue::Signal");
	return lastSignaled;
}

void D3D12Renderer::Impl::waitForFence(const uint64_t value) {
	if (fence->GetCompletedValue() < value) {
		check(fence->SetEventOnCompletion(value, fenceEvent), "ID3D12Fence::SetEventOnCompletion");
		WaitForSingleObject(fenceEvent, INFINITE);
	}
}

void D3D12Renderer::Impl::waitIdle() {
	waitForFence(signal());
}

void D3D12Renderer::Impl::destroyLater(std::function<void()> destroy) {
	pendingDestructions.push_back({ std::move(destroy), static_cast<int>(maxFramesInFlight) + 1 });
}

ID3D12Resource* D3D12Renderer::Impl::backBuffer() const {
	return backBuffers[backBufferIndex].Get();
}

void D3D12Renderer::Impl::transitionBackBuffer(const D3D12_RESOURCE_STATES state) {
	transition(commandList.Get(), backBuffer(), backBufferStates[backBufferIndex], state);
	backBufferStates[backBufferIndex] = state;
}

void D3D12Renderer::Impl::bindTarget() {
	if (targetBound) {
		return;
	}
	ID3D12GraphicsCommandList* const cmd = commandList.Get();
	if (renderTargetStack.empty()) {
		syncLetterboxScissor();
		if (swapchainMsaaActive()) {
			currentPassKind = PassKind::LdrMsaa;
			currentRtv = rtvHeap.cpu(msaaTargetRtv);
		} else {
			currentPassKind = PassKind::Ldr;
			transitionBackBuffer(D3D12_RESOURCE_STATE_RENDER_TARGET);
			currentRtv = rtvHeap.cpu(backBufferRtvs[backBufferIndex]);
		}
	} else {
		D3D12Framebuffer& target = *renderTargetStack.back();
		// Like on OpenGL, where an anti-aliased FrameBuffer is always drawn into multisampled, but
		// only smoothed while GL_MULTISAMPLE is enabled
		const bool multisampled =
		    target.msaa && (msaaRebuildPending ? pendingMsaaEnabled : msaaEnabled);
		currentPassKind = passKind(target.hdr, multisampled);
		if (multisampled) {
			if (target.samplesStale) {
				expandIntoSamples(target);
			}
			target.resolvePending = true;
			currentRtv = rtvHeap.cpu(target.msaaRtv);
		} else {
			if (target.resolvePending) {
				resolveFramebuffer(target); // before what's drawn now, which it'd overwrite
			}
			if (target.msaa) {
				target.samplesStale = true;
			}
			transition(cmd, target.color.resource, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			           D3D12_RESOURCE_STATE_RENDER_TARGET);
			targetColorIsRenderTarget = true;
			currentRtv = rtvHeap.cpu(target.rtv);
		}
	}
	cmd->OMSetRenderTargets(1, &currentRtv, FALSE, nullptr);
	targetBound = true;
	applyViewport();
	applyScissor();
}

void D3D12Renderer::Impl::unbindTarget() {
	if (targetBound && targetColorIsRenderTarget) {
		transition(commandList.Get(), renderTargetStack.back()->color.resource,
		           D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	}
	targetColorIsRenderTarget = false;
	targetBound = false;
}

void D3D12Renderer::Impl::clearSwapchain() {
	bindTarget();
	const Rgb& c = currentClearColor;
	const std::array<float, 4> color = { c.getRed(), c.getGreen(), c.getBlue(), 1.f };
	if (!letterboxing) {
		commandList->ClearRenderTargetView(currentRtv, color.data(), 0, nullptr);
		return;
	}
	const std::array<float, 4> black = { 0.f, 0.f, 0.f, 1.f };
	commandList->ClearRenderTargetView(currentRtv, black.data(), 0, nullptr);
	const Rect canvas = letterboxScissor();
	if (canvas.width > 0 && canvas.height > 0) {
		const D3D12_RECT rect{ canvas.x, canvas.y, canvas.x + canvas.width,
			                   canvas.y + canvas.height };
		commandList->ClearRenderTargetView(currentRtv, color.data(), 1, &rect);
	}
}

void D3D12Renderer::Impl::resolveSwapchain() {
	if (!swapchainMsaaActive()) {
		return; // it's been drawn into directly
	}
	ID3D12GraphicsCommandList* const cmd = commandList.Get();
	transition(cmd, msaaTarget.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET,
	           D3D12_RESOURCE_STATE_RESOLVE_SOURCE);
	transitionBackBuffer(D3D12_RESOURCE_STATE_RESOLVE_DEST);
	cmd->ResolveSubresource(backBuffer(), 0, msaaTarget.Get(), 0, ldrFormat);
	transition(cmd, msaaTarget.Get(), D3D12_RESOURCE_STATE_RESOLVE_SOURCE,
	           D3D12_RESOURCE_STATE_RENDER_TARGET);
}

void D3D12Renderer::Impl::expandIntoSamples(D3D12Framebuffer& fb) {
	ID3D12GraphicsCommandList* const cmd = commandList.Get();
	const D3D12_CPU_DESCRIPTOR_HANDLE rtv = rtvHeap.cpu(fb.msaaRtv);
	cmd->OMSetRenderTargets(1, &rtv, FALSE, nullptr);
	const D3D12_VIEWPORT viewport{
		0.f, 0.f, static_cast<float>(fb.width), static_cast<float>(fb.height), 0.f, 1.f
	};
	cmd->RSSetViewports(1, &viewport);
	const D3D12_RECT scissor{ 0, 0, fb.width, fb.height };
	cmd->RSSetScissorRects(1, &scissor);
	// Samples each texel at its center into all of its pixel's samples, as it's drawn without
	// blending
	TexturedPushConstants push{};
	setIdentity(push.mvp);
	const PassKind kind = passKind(fb.hdr, true);
	cmd->SetPipelineState(
	    builtinPipeline(Builtin::Textured, kind, BlendVariant::Replace, PrimitiveType::Triangles));
	cmd->SetGraphicsRootDescriptorTable(RootTexture, srvHeap.gpu(fb.color.srv));
	cmd->SetGraphicsRootDescriptorTable(
	    RootSampler, samplerHeap.gpu(samplerIndex(fb.color.minFilter, fb.color.magFilter)));
	cmd->SetGraphicsRoot32BitConstants(RootConstants, sizeof(push) / 4, &push, 0);
	cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	const D3D12_VERTEX_BUFFER_VIEW view{ fullscreenQuadBuffer.resource->GetGPUVirtualAddress(),
		                                 sizeof(fullscreenQuad), 4 * sizeof(float) };
	cmd->IASetVertexBuffers(0, 1, &view);
	cmd->DrawInstanced(6, 1, 0, 0);
	fb.samplesStale = false;
}

void D3D12Renderer::Impl::resolveFramebuffer(D3D12Framebuffer& fb) {
	ID3D12GraphicsCommandList* const cmd = commandList.Get();
	transition(cmd, fb.msaa, D3D12_RESOURCE_STATE_RENDER_TARGET,
	           D3D12_RESOURCE_STATE_RESOLVE_SOURCE);
	transition(cmd, fb.color.resource, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
	           D3D12_RESOURCE_STATE_RESOLVE_DEST);
	cmd->ResolveSubresource(fb.color.resource, 0, fb.msaa, 0, fb.hdr ? hdrFormat : ldrFormat);
	transition(cmd, fb.color.resource, D3D12_RESOURCE_STATE_RESOLVE_DEST,
	           D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	transition(cmd, fb.msaa, D3D12_RESOURCE_STATE_RESOLVE_SOURCE,
	           D3D12_RESOURCE_STATE_RENDER_TARGET);
	fb.resolvePending = false;
}

bool D3D12Renderer::Impl::flipClipY() const {
	return !renderTargetStack.empty();
}

void D3D12Renderer::Impl::draw(ID3D12PipelineState* const pipeline, const void* const push,
                               const size_t pushSize, const D3D12_GPU_VIRTUAL_ADDRESS vertices,
                               const size_t vertexCount, const UINT stride,
                               const PrimitiveType type) {
	ID3D12GraphicsCommandList* const cmd = commandList.Get();
	cmd->SetPipelineState(pipeline);
	cmd->SetGraphicsRoot32BitConstants(RootConstants, static_cast<UINT>(pushSize / 4), push, 0);
	cmd->IASetPrimitiveTopology(toD3DTopology(type));
	const D3D12_VERTEX_BUFFER_VIEW view{ vertices, static_cast<UINT>(vertexCount * stride),
		                                 stride };
	cmd->IASetVertexBuffers(0, 1, &view);
	cmd->DrawInstanced(static_cast<UINT>(vertexCount), 1, 0, 0);
}

void D3D12Renderer::Impl::applyViewport() const {
	D3D12_VIEWPORT viewport{};
	viewport.MaxDepth = 1.f;
	if (renderTargetStack.empty()) {
		// Like glViewport, at the bottom left. When the swapchain hasn't been resized after a
		// resize of the window yet, this might not cover all of it.
		viewport.Width =
		    static_cast<float>(framebufferWidth > 0 ? framebufferWidth : swapchainWidth);
		viewport.Height =
		    static_cast<float>(framebufferHeight > 0 ? framebufferHeight : swapchainHeight);
		viewport.TopLeftY = static_cast<float>(swapchainHeight) - viewport.Height;
	} else {
		viewport.Width = static_cast<float>(renderTargetStack.back()->width);
		viewport.Height = static_cast<float>(renderTargetStack.back()->height);
	}
	commandList->RSSetViewports(1, &viewport);
}

void D3D12Renderer::Impl::applyScissor() const {
	const auto [width, height] = currentTargetSize();
	const Rect scissor = effectiveScissor(width, height);
	const D3D12_RECT rect{ scissor.x, scissor.y, scissor.x + scissor.width,
		                   scissor.y + scissor.height };
	commandList->RSSetScissorRects(1, &rect);
}

Rect D3D12Renderer::Impl::glScissorToTopLeft(const int glX, const int glY, const int glW,
                                             const int glH, const int targetHeight) {
	return { std::max(glX, 0), std::max(targetHeight - glY - glH, 0), std::max(glW, 0),
		     std::max(glH, 0) };
}

Rect D3D12Renderer::Impl::intersectScissors(const Rect a, const Rect b) {
	const int x1 = std::max(a.x, b.x);
	const int y1 = std::max(a.y, b.y);
	const int x2 = std::min(a.x + a.width, b.x + b.width);
	const int y2 = std::min(a.y + a.height, b.y + b.height);
	if (x2 > x1 && y2 > y1) {
		return { x1, y1, x2 - x1, y2 - y1 };
	}
	return {};
}

Rect D3D12Renderer::Impl::letterboxScissor() const {
	// Centered in the viewport, which is at the bottom left of the swapchain, see setViewport
	const int x = (framebufferWidth - canvasWidth) / 2;
	const int y = (framebufferHeight - canvasHeight) / 2;
	return intersectScissors(glScissorToTopLeft(x, y, canvasWidth, canvasHeight, swapchainHeight),
	                         { 0, 0, swapchainWidth, swapchainHeight });
}

Rect D3D12Renderer::Impl::effectiveScissor(const int targetWidth, const int targetHeight) const {
	Rect base{ 0, 0, targetWidth, targetHeight };
	if (renderTargetStack.empty() && letterboxing) {
		base = letterboxScissor();
	}
	if (!userScissor) {
		return base;
	}
	if (renderTargetStack.empty()) {
		return intersectScissors(base, glScissorToTopLeft(userScissor->x, userScissor->y,
		                                                  userScissor->width, userScissor->height,
		                                                  targetHeight));
	}
	// FrameBuffers are drawn into upside down (see flipClipY), so their rows already start at the
	// bottom like OpenGL's
	return intersectScissors(base,
	                         { std::max(userScissor->x, 0), std::max(userScissor->y, 0),
	                           std::max(userScissor->width, 0), std::max(userScissor->height, 0) });
}

void D3D12Renderer::Impl::syncLetterboxScissor() {
	if (canvasWidth <= 0 || canvasHeight <= 0 || swapchainWidth == 0) {
		return;
	}
	if (framebufferWidth <= 0 || framebufferHeight <= 0) { // setViewport hasn't been called yet
		framebufferWidth = swapchainWidth;
		framebufferHeight = swapchainHeight;
	}
	letterboxing = canvasWidth != framebufferWidth || canvasHeight != framebufferHeight;
}

std::pair<int, int> D3D12Renderer::Impl::currentTargetSize() const {
	if (renderTargetStack.empty()) {
		return { swapchainWidth, swapchainHeight };
	}
	return { renderTargetStack.back()->width, renderTargetStack.back()->height };
}

ComPtr<ID3D12Resource>
D3D12Renderer::Impl::createTextureResource(const int width, const int height,
                                           const uint32_t mipLevels,
                                           const D3D12_RESOURCE_STATES state) const {
	const auto desc = textureDesc(width, height, mipLevels, ldrFormat, 1,
	                              mipLevels > 1 ? D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
	                                            : D3D12_RESOURCE_FLAG_NONE);
	const auto heap = heapProperties(D3D12_HEAP_TYPE_DEFAULT);
	ComPtr<ID3D12Resource> resource;
	check(device->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &desc, state, nullptr,
	                                      IID_PPV_ARGS(&resource)),
	      "CreateCommittedResource");
	return resource;
}

void D3D12Renderer::Impl::createTextureView(D3D12Texture& texture, const DXGI_FORMAT format) {
	texture.srv = srvHeap.allocate();
	D3D12_SHADER_RESOURCE_VIEW_DESC desc{};
	desc.Format = format;
	desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	desc.Texture2D.MipLevels = texture.mipLevels;
	device->CreateShaderResourceView(texture.resource, &desc, srvHeap.cpu(texture.srv));
}

void D3D12Renderer::Impl::recordMipmapGeneration(ID3D12GraphicsCommandList* const cmd,
                                                 const D3D12Texture& texture) {
	transition(cmd, texture.resource, D3D12_RESOURCE_STATE_COPY_DEST,
	           D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, 0);
	cmd->SetPipelineState(builtinPipeline(Builtin::Textured, PassKind::Ldr, BlendVariant::Replace,
	                                      PrimitiveType::Triangles));
	cmd->SetGraphicsRootDescriptorTable(
	    RootSampler,
	    samplerHeap.gpu(samplerIndex(TextureFilter::Bilinear, TextureFilter::Bilinear)));
	TexturedPushConstants push{};
	setIdentity(push.mvp);
	cmd->SetGraphicsRoot32BitConstants(RootConstants, sizeof(push) / 4, &push, 0);
	cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	const D3D12_VERTEX_BUFFER_VIEW view{ fullscreenQuadBuffer.resource->GetGPUVirtualAddress(),
		                                 sizeof(fullscreenQuad), 4 * sizeof(float) };
	cmd->IASetVertexBuffers(0, 1, &view);
	// Views of single mip levels: sampled ones are only needed until the GPU is done, render
	// target views are only read while recording
	std::vector<D3D12Descriptor> srvs;
	const D3D12Descriptor rtv = rtvHeap.allocate();
	int width = texture.width;
	int height = texture.height;
	for (uint32_t level = 1; level < texture.mipLevels; ++level) {
		width = std::max(width / 2, 1);
		height = std::max(height / 2, 1);
		transition(cmd, texture.resource, D3D12_RESOURCE_STATE_COPY_DEST,
		           D3D12_RESOURCE_STATE_RENDER_TARGET, level);

		const D3D12Descriptor srv = srvHeap.allocate();
		srvs.push_back(srv);
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = ldrFormat;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Texture2D.MostDetailedMip = level - 1;
		srvDesc.Texture2D.MipLevels = 1;
		device->CreateShaderResourceView(texture.resource, &srvDesc, srvHeap.cpu(srv));
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		rtvDesc.Format = ldrFormat;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = level;
		device->CreateRenderTargetView(texture.resource, &rtvDesc, rtvHeap.cpu(rtv));

		const D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap.cpu(rtv);
		cmd->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
		const D3D12_VIEWPORT viewport{
			0.f, 0.f, static_cast<float>(width), static_cast<float>(height), 0.f, 1.f
		};
		cmd->RSSetViewports(1, &viewport);
		const D3D12_RECT scissor{ 0, 0, width, height };
		cmd->RSSetScissorRects(1, &scissor);
		cmd->SetGraphicsRootDescriptorTable(RootTexture, srvHeap.gpu(srv));
		cmd->DrawInstanced(6, 1, 0, 0);
		transition(cmd, texture.resource, D3D12_RESOURCE_STATE_RENDER_TARGET,
		           D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, level);
	}
	rtvHeap.free(rtv);
	destroyLater([this, srvs = std::move(srvs)]() {
		for (const D3D12Descriptor srv : srvs) {
			srvHeap.free(srv);
		}
	});
}

void D3D12Renderer::Impl::freeTextureResources(const D3D12Texture& texture) {
	srvHeap.free(texture.srv);
	if (texture.resource) {
		texture.resource->Release();
	}
}

D3D12Renderer::D3D12Renderer(void* const nativeWindow)
: impl(std::make_unique<Impl>(static_cast<SDL_Window*>(nativeWindow))) {
}

D3D12Renderer::~D3D12Renderer() = default;

const char* D3D12Renderer::name() const {
	return "Direct3D 12";
}

void D3D12Renderer::beginFrame(const Rgb clearColor) {
	if (impl->frameActive) {
		// E.g. by showWindow, before the first swapBuffers. Like glClear on OpenGL, clear what's
		// being drawn on instead.
		impl->currentClearColor = clearColor;
		if (impl->renderTargetStack.empty()) {
			impl->clearSwapchain(); // with the letterboxing's bars
		} else {
			clearCurrentRenderTarget(Rgba(clearColor, Alpha(1)));
		}
		return;
	}
	const uint32_t frame = impl->currentFrame;
	impl->waitForFence(impl->frameFenceValues[frame]);
	// The GPU is done with this frame's previous command list, and so with the buffers it read
	impl->retiredVertexBuffers[frame].clear();
	impl->retiredUniformBuffers[frame].clear();
	impl->retiredStagingBuffers[frame].clear();

	impl->backBufferIndex = impl->swapchain->GetCurrentBackBufferIndex();
	impl->beginCommandList(impl->commandList.Get(), impl->allocators[frame].Get());
	impl->vertexBuffers[frame].used = 0;
	impl->uniformBuffers[frame].used = 0;

	impl->currentClearColor = clearColor;
	impl->renderTargetStack.clear();
	impl->targetBound = false;
	impl->targetColorIsRenderTarget = false;
	impl->frameActive = true;
	impl->clearSwapchain();
}

void D3D12Renderer::endFrame() {
	if (!impl->frameActive) {
		return;
	}
	static bool firstPresent = true;
	internal::StartupProfiler firstPresentProfiler(firstPresent ? "first Present" : "");
	firstPresent = false;
	impl->unbindTarget();
	impl->resolveSwapchain();
	impl->transitionBackBuffer(D3D12_RESOURCE_STATE_PRESENT);
	impl->submit();
	const bool tearing = !impl->verticalSyncEnabled && impl->tearingSupported;
	const HRESULT result = impl->swapchain->Present(impl->verticalSyncEnabled ? 1 : 0,
	                                                tearing ? DXGI_PRESENT_ALLOW_TEARING : 0);
	impl->frameFenceValues[impl->currentFrame] = impl->signal();
	if (result == DXGI_ERROR_DEVICE_REMOVED || result == DXGI_ERROR_DEVICE_RESET) {
		throw std::runtime_error(
		    std::format("The Direct3D 12 device has been removed (HRESULT 0x{:08x}).",
		                static_cast<uint32_t>(impl->device->GetDeviceRemovedReason())));
	}
	check(result, "IDXGISwapChain::Present");

	impl->frameActive = false;
	impl->currentFrame = (impl->currentFrame + 1) % Impl::maxFramesInFlight;

	// Free what's been destroyed when the frames that could have used it have all completed. A
	// frame's beginFrame() waits for the one that used its slot before.
	auto& pending = impl->pendingDestructions;
	for (auto it = pending.begin(); it != pending.end();) {
		if (--it->framesUntilFree <= 0) {
			it->destroy();
			it = pending.erase(it);
		} else {
			++it;
		}
	}

	if (impl->swapchainRecreatePending) {
		impl->swapchainRecreatePending = false;
		impl->recreateSwapchain();
	}
	if (impl->msaaRebuildPending) {
		impl->msaaRebuildPending = false;
		impl->msaaEnabled = impl->pendingMsaaEnabled;
		impl->waitIdle();
		impl->destroyMsaaTarget();
		if (impl->swapchainMsaaActive()) {
			impl->createMsaaTarget();
		}
	}
}

void D3D12Renderer::setViewport(const int x, const int y, const int width, const int height) {
	assert(x == 0 && y == 0);
	(void)x;
	(void)y;
	impl->framebufferWidth = width;
	impl->framebufferHeight = height;
	if (impl->targetBound && impl->renderTargetStack.empty()) {
		impl->applyViewport();
	}
}

void D3D12Renderer::onResize(int /*width*/, int /*height*/) {
	impl->swapchainRecreatePending = true;
}

void D3D12Renderer::setProjection(const Mat4& projection) {
	impl->projection = projection;
}

void D3D12Renderer::drawColored(PrimitiveType type, const float* xyVertices,
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
	const auto vertices = impl->uploadVertices(xyVertices, vertexCount * 2 * sizeof(float));

	impl->bindTarget();
	ColoredPushConstants push{};
	computeMvp(impl->projection, modelview, impl->flipClipY(), push.mvp);
	push.color[0] = color.getRed();
	push.color[1] = color.getGreen();
	push.color[2] = color.getBlue();
	push.color[3] = color.getAlpha();
	impl->draw(impl->builtinPipeline(Impl::Builtin::Colored, impl->currentPassKind,
	                                 impl->blendVariant(), type),
	           &push, sizeof(push), vertices, vertexCount, 2 * sizeof(float), type);
}

void D3D12Renderer::drawRoundedRect(const Mat3& modelview, const Rgba color, const float sizeX,
                                    const float sizeY, const float topLeft, const float topRight,
                                    const float bottomLeft, const float bottomRight) {
	if (!impl->frameActive) {
		return;
	}
	static constexpr std::array<float, 12> unitSquare = { .5f, -.5f, .5f,  .5f, -.5f, -.5f,
		                                                  .5f, .5f,  -.5f, .5f, -.5f, -.5f };
	const auto vertices = impl->uploadVertices(unitSquare.data(), sizeof(unitSquare));

	impl->bindTarget();
	RoundedPushConstants push{};
	computeMvp(impl->projection, modelview, impl->flipClipY(), push.mvp);
	push.color[0] = color.getRed();
	push.color[1] = color.getGreen();
	push.color[2] = color.getBlue();
	push.color[3] = color.getAlpha();
	push.size[0] = sizeX;
	push.size[1] = sizeY;
	push.cornerRadii[0] = topLeft;
	push.cornerRadii[1] = topRight;
	push.cornerRadii[2] = bottomLeft;
	push.cornerRadii[3] = bottomRight;
	impl->draw(impl->builtinPipeline(Impl::Builtin::Rounded, impl->currentPassKind,
	                                 impl->blendVariant(), PrimitiveType::Triangles),
	           &push, sizeof(push), vertices, 6, 2 * sizeof(float), PrimitiveType::Triangles);
}

std::unique_ptr<D3D12Texture>
D3D12Renderer::createTexture(const int width, const int height, const unsigned int format,
                             const unsigned int type, const unsigned char* const data,
                             const unsigned char* const* const rowPointers, const bool mipmap) {
	internal::StartupProfiler _{ "createTexture" };
	if (type != glUnsignedByte || (format != glRGBA && format != glRGB && format != glBGR)) {
		internal::error("Direct3D 12 backend: unsupported texture format/type (0x{:x}/0x{:x}).",
		                format, type);
		return nullptr;
	}

	auto texture = std::make_unique<D3D12Texture>();
	texture->width = width;
	texture->height = height;
	texture->mipLevels = mipmap ? mipLevelCount(width, height) : 1;
	texture->resource = impl->createTextureResource(width, height, texture->mipLevels,
	                                                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
	                        .Detach();
	impl->createTextureView(*texture, ldrFormat);
	if (data || rowPointers) {
		updateTextureBytes(*texture, width, height,
		                   toRgba8(width, height, format, data, rowPointers).data());
	}
	if (App::isPixelArt()) {
		texture->magFilter = TextureFilter::NearestNeighbor;
	}
	return texture;
}

void D3D12Renderer::updateTextureBytes(D3D12Texture& texture, const int width, const int height,
                                       const unsigned char* const rgba) {
	const auto desc = textureDesc(width, height, 1, ldrFormat, 1, D3D12_RESOURCE_FLAG_NONE);
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
	UINT rows = 0;
	UINT64 rowSize = 0;
	UINT64 size = 0;
	impl->device->GetCopyableFootprints(&desc, 0, 1, 0, &footprint, &rows, &rowSize, &size);
	UploadBuffer staging = impl->createUploadBuffer(size);
	for (UINT row = 0; row < rows; ++row) {
		std::memcpy(staging.mapped + footprint.Offset + row * footprint.Footprint.RowPitch,
		            rgba + static_cast<size_t>(row) * width * 4, static_cast<size_t>(rowSize));
	}

	ID3D12GraphicsCommandList* const cmd = impl->uploadCommands();
	// After earlier frames are done sampling the old pixels, as the queue runs it after them
	transition(cmd, texture.resource, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
	           D3D12_RESOURCE_STATE_COPY_DEST);
	D3D12_TEXTURE_COPY_LOCATION dst{};
	dst.pResource = texture.resource;
	dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	dst.SubresourceIndex = 0;
	D3D12_TEXTURE_COPY_LOCATION src{};
	src.pResource = staging.resource.Get();
	src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	src.PlacedFootprint = footprint;
	cmd->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
	if (texture.mipLevels > 1) {
		impl->recordMipmapGeneration(cmd, texture);
	} else {
		transition(cmd, texture.resource, D3D12_RESOURCE_STATE_COPY_DEST,
		           D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	}
	impl->keepStagingBuffer(std::move(staging.resource));
}

void D3D12Renderer::enableMipmaps(D3D12Texture& texture) {
	const uint32_t mipLevels = mipLevelCount(texture.width, texture.height);
	if (texture.mipLevels > 1 || mipLevels == 1 || !texture.resource) {
		return;
	}
	// The number of levels of a texture can't be changed, so copy the pixels into a new one
	D3D12Texture mipmapped = texture;
	mipmapped.mipLevels = mipLevels;
	mipmapped.resource = impl->createTextureResource(texture.width, texture.height, mipLevels,
	                                                 D3D12_RESOURCE_STATE_COPY_DEST)
	                         .Detach();
	ID3D12GraphicsCommandList* const cmd = impl->uploadCommands();
	transition(cmd, texture.resource, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
	           D3D12_RESOURCE_STATE_COPY_SOURCE);
	D3D12_TEXTURE_COPY_LOCATION dst{};
	dst.pResource = mipmapped.resource;
	dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	D3D12_TEXTURE_COPY_LOCATION src{};
	src.pResource = texture.resource;
	src.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	cmd->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
	// Draws recorded earlier in this frame still sample the old texture, as the uploads are done
	// before them
	transition(cmd, texture.resource, D3D12_RESOURCE_STATE_COPY_SOURCE,
	           D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	impl->recordMipmapGeneration(cmd, mipmapped);
	impl->createTextureView(mipmapped, ldrFormat);
	// The old texture and its view might still be used by the current or an in-flight command list
	destroyTexture(texture);
	texture = mipmapped;
}

void D3D12Renderer::setTextureFilter(D3D12Texture& texture, const TextureFilter minFilter,
                                     const TextureFilter magFilter) {
	// The sampler is picked for each draw, see samplerIndex()
	texture.minFilter = minFilter;
	texture.magFilter = magFilter;
}

void D3D12Renderer::destroyTexture(D3D12Texture& texture) {
	impl->destroyLater([impl = impl.get(), texture]() { impl->freeTextureResources(texture); });
	texture = {}; // the copy owns the resource now
}

void D3D12Renderer::drawSprite(const D3D12Texture& texture, const float* const xyuv,
                               const std::size_t vertexCount, const PrimitiveType type,
                               const Mat3& modelview, const Rgba color) {
	// Multiplying with color is the same as mixing towards black by 1 - color
	drawSprite(texture, xyuv, vertexCount, type, modelview, Rgb(0, 0, 0),
	           { 1.f - color.getRed(), 1.f - color.getGreen(), 1.f - color.getBlue(),
	             1.f - color.getAlpha() });
}

void D3D12Renderer::drawSprite(const D3D12Texture& texture, const float* xyuv,
                               std::size_t vertexCount, PrimitiveType type, const Mat3& modelview,
                               const Rgb lerpColor, const std::array<float, 4> lerpFactors) {
	if (!impl->frameActive || vertexCount == 0 || !texture.resource) {
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
	const auto vertices = impl->uploadVertices(xyuv, vertexCount * 4 * sizeof(float));

	impl->bindTarget();
	TexturedPushConstants push{};
	computeMvp(impl->projection, modelview, impl->flipClipY(), push.mvp);
	push.lerpColor[0] = lerpColor.getRed();
	push.lerpColor[1] = lerpColor.getGreen();
	push.lerpColor[2] = lerpColor.getBlue();
	std::copy(lerpFactors.begin(), lerpFactors.end(), push.lerpFactors);

	const BlendVariant variant = impl->blendVariant();
	ID3D12PipelineState* const pipeline =
	    impl->activeShaderProgram
	        ? impl->programPipeline(*impl->activeShaderProgram, impl->currentPassKind, variant,
	                                type)
	        : impl->builtinPipeline(Impl::Builtin::Textured, impl->currentPassKind, variant, type);
	ID3D12GraphicsCommandList* const cmd = impl->commandList.Get();
	cmd->SetGraphicsRootDescriptorTable(RootTexture, impl->srvHeap.gpu(texture.srv));
	cmd->SetGraphicsRootDescriptorTable(
	    RootSampler, impl->samplerHeap.gpu(samplerIndex(texture.minFilter, texture.magFilter)));
	impl->bindActiveProgramUniforms();
	impl->draw(pipeline, &push, sizeof(push), vertices, vertexCount, 4 * sizeof(float), type);
}

void D3D12Renderer::readPixels(unsigned char* const rgb, const int x, const int y, const int width,
                               const int height) {
	const size_t pixelCount = static_cast<size_t>(width) * height;
	if (!impl->frameActive || x < 0 || y < 0 || x + width > impl->swapchainWidth ||
	    y + height > impl->swapchainHeight) {
		std::memset(rgb, 0, pixelCount * 3);
		return;
	}

	// Copy the requested region into a buffer the CPU can read, after the draws recorded so far
	const UINT rowPitch = static_cast<UINT>(
	    alignUp(static_cast<uint64_t>(width) * 4, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT));
	const auto heap = heapProperties(D3D12_HEAP_TYPE_READBACK);
	const auto desc = bufferDesc(static_cast<UINT64>(rowPitch) * height);
	ComPtr<ID3D12Resource> readback;
	check(impl->device->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &desc,
	                                            D3D12_RESOURCE_STATE_COPY_DEST, nullptr,
	                                            IID_PPV_ARGS(&readback)),
	      "CreateCommittedResource");
	impl->unbindTarget();
	impl->resolveSwapchain();
	impl->transitionBackBuffer(D3D12_RESOURCE_STATE_COPY_SOURCE);
	D3D12_TEXTURE_COPY_LOCATION dst{};
	dst.pResource = readback.Get();
	dst.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	dst.PlacedFootprint.Footprint = { ldrFormat, static_cast<UINT>(width),
		                              static_cast<UINT>(height), 1, rowPitch };
	D3D12_TEXTURE_COPY_LOCATION src{};
	src.pResource = impl->backBuffer();
	src.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	// x and y are from the bottom left like glReadPixels
	const UINT top = static_cast<UINT>(impl->swapchainHeight - y - height);
	const D3D12_BOX box{ static_cast<UINT>(x),
		                 top,
		                 0,
		                 static_cast<UINT>(x + width),
		                 top + static_cast<UINT>(height),
		                 1 };
	impl->commandList->CopyTextureRegion(&dst, 0, 0, 0, &src, &box);
	impl->submit();
	impl->waitIdle();

	const D3D12_RANGE range{ 0, static_cast<SIZE_T>(desc.Width) };
	void* mapped = nullptr;
	check(readback->Map(0, &range, &mapped), "ID3D12Resource::Map");
	const auto* const src8 = static_cast<const unsigned char*>(mapped);
	// Output RGB, bottom row first (OpenGL convention)
	for (int row = 0; row < height; ++row) {
		const unsigned char* srcRow = src8 + static_cast<size_t>(height - 1 - row) * rowPitch;
		unsigned char* dstRow = rgb + static_cast<size_t>(row) * width * 3;
		for (int col = 0; col < width; ++col) {
			dstRow[col * 3 + 0] = srcRow[col * 4 + 0];
			dstRow[col * 3 + 1] = srcRow[col * 4 + 1];
			dstRow[col * 3 + 2] = srcRow[col * 4 + 2];
		}
	}
	const D3D12_RANGE noWrite{ 0, 0 };
	readback->Unmap(0, &noWrite);

	// Resume the frame, the GPU is done with everything that's been recorded
	impl->beginCommandList(impl->commandList.Get(), impl->allocators[impl->currentFrame].Get());
}

std::string D3D12Renderer::getGraphicsApi() const {
	DXGI_ADAPTER_DESC1 desc;
	impl->adapter->GetDesc1(&desc);
	// E.g. "Direct3D 12 (NVIDIA GeForce RTX 4070) 32.0.15.6094", like Vulkan's "(NVIDIA) 580.95.05"
	std::string api = "Direct3D 12 (" + toUtf8(desc.Description) + ")";
	if (LARGE_INTEGER version;
	    SUCCEEDED(impl->adapter->CheckInterfaceSupport(__uuidof(IDXGIDevice), &version))) {
		api += std::format(" {}.{}.{}.{}", HIWORD(version.HighPart), LOWORD(version.HighPart),
		                   HIWORD(version.LowPart), LOWORD(version.LowPart));
	}
	return api;
}

bool D3D12Renderer::areTimestampsSupported() const {
	return impl->timestampFrequency > 0;
}

D3D12Renderer::TimestampPool D3D12Renderer::createTimestampPool(const uint32_t count) {
	auto pool = std::make_unique<D3D12TimestampPool>();
	D3D12_QUERY_HEAP_DESC heapDesc{};
	heapDesc.Type = D3D12_QUERY_HEAP_TYPE_TIMESTAMP;
	heapDesc.Count = count;
	check(impl->device->CreateQueryHeap(&heapDesc, IID_PPV_ARGS(&pool->heap)), "CreateQueryHeap");
	const auto heap = heapProperties(D3D12_HEAP_TYPE_READBACK);
	const auto desc = bufferDesc(uint64_t{ count } * sizeof(uint64_t));
	check(impl->device->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &desc,
	                                            D3D12_RESOURCE_STATE_COPY_DEST, nullptr,
	                                            IID_PPV_ARGS(&pool->readback)),
	      "CreateCommittedResource");
	void* mapped = nullptr;
	check(pool->readback->Map(0, nullptr, &mapped), "ID3D12Resource::Map");
	pool->ticks = static_cast<const uint64_t*>(mapped);
	pool->writtenAt.resize(count);
	return pool.release();
}

void D3D12Renderer::destroyTimestampPool(const TimestampPool pool) {
	impl->destroyLater([pool]() { delete pool; });
}

bool D3D12Renderer::writeTimestamp(const TimestampPool pool, const uint32_t query) {
	if (!impl->frameActive) {
		return false;
	}
	ID3D12GraphicsCommandList* const cmd = impl->commandList.Get();
	cmd->EndQuery(pool->heap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, query);
	cmd->ResolveQueryData(pool->heap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, query, 1,
	                      pool->readback.Get(), uint64_t{ query } * sizeof(uint64_t));
	// It's done once the next signal(), after this command list has been executed, has been
	pool->writtenAt[query] = impl->lastSignaled + 1;
	return true;
}

std::optional<std::chrono::nanoseconds> D3D12Renderer::takeElapsedTime(const TimestampPool pool,
                                                                       const uint32_t first) {
	const uint64_t written = std::max(pool->writtenAt[first], pool->writtenAt[first + 1]);
	if (pool->writtenAt[first] == 0 || pool->writtenAt[first + 1] == 0 ||
	    impl->fence->GetCompletedValue() < written) {
		return std::nullopt;
	}
	pool->writtenAt[first] = 0;
	pool->writtenAt[first + 1] = 0;
	const uint64_t elapsed = pool->ticks[first + 1] - pool->ticks[first];
	return std::chrono::nanoseconds(static_cast<std::chrono::nanoseconds::rep>(
	    static_cast<double>(elapsed) * 1e9 / static_cast<double>(impl->timestampFrequency)));
}

std::unique_ptr<D3D12Framebuffer> D3D12Renderer::createFramebuffer(const int width,
                                                                   const int height,
                                                                   const bool antiAliased,
                                                                   const bool hdr) {
	auto fb = std::make_unique<D3D12Framebuffer>();
	fb->width = width;
	fb->height = height;
	fb->hdr = hdr;
	const DXGI_FORMAT format = hdr ? hdrFormat : ldrFormat;
	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = format;
	clearValue.Color[0] = clearValue.Color[1] = clearValue.Color[2] = 1.f; // transparent white
	const auto heap = heapProperties(D3D12_HEAP_TYPE_DEFAULT);
	auto desc = textureDesc(width, height, 1, format, 1, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
	ComPtr<ID3D12Resource> color;
	check(impl->device->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &desc,
	                                            D3D12_RESOURCE_STATE_RENDER_TARGET, &clearValue,
	                                            IID_PPV_ARGS(&color)),
	      "CreateCommittedResource");
	fb->color.resource = color.Detach();
	fb->color.width = width;
	fb->color.height = height;
	impl->createTextureView(fb->color, format);
	fb->rtv = impl->rtvHeap.allocate();
	impl->device->CreateRenderTargetView(fb->color.resource, nullptr, impl->rtvHeap.cpu(fb->rtv));
	const TextureFilter filter =
	    App::isPixelArt() ? TextureFilter::NearestNeighbor : TextureFilter::Bilinear;
	fb->color.minFilter = filter;
	fb->color.magFilter = filter;

	// Clear to transparent and move to PIXEL_SHADER_RESOURCE, which it's in while it isn't drawn
	// into
	ID3D12GraphicsCommandList* const cmd = impl->uploadCommands();
	cmd->ClearRenderTargetView(impl->rtvHeap.cpu(fb->rtv), clearValue.Color, 0, nullptr);
	transition(cmd, fb->color.resource, D3D12_RESOURCE_STATE_RENDER_TARGET,
	           D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	if (!antiAliased || !impl->msaaSupported) {
		return fb;
	}
	desc.SampleDesc.Count = msaaSampleCount;
	ComPtr<ID3D12Resource> msaa;
	check(impl->device->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &desc,
	                                            D3D12_RESOURCE_STATE_RENDER_TARGET, &clearValue,
	                                            IID_PPV_ARGS(&msaa)),
	      "CreateCommittedResource");
	fb->msaa = msaa.Detach();
	fb->msaaRtv = impl->rtvHeap.allocate();
	impl->device->CreateRenderTargetView(fb->msaa, nullptr, impl->rtvHeap.cpu(fb->msaaRtv));
	// Clear it like the color texture. It stays in RENDER_TARGET.
	cmd->ClearRenderTargetView(impl->rtvHeap.cpu(fb->msaaRtv), clearValue.Color, 0, nullptr);
	return fb;
}

void D3D12Renderer::destroyFramebuffer(D3D12Framebuffer& fb) {
	// The framebuffer may still be referenced by the current (not yet submitted) or an in-flight
	// command list - jngl::FrameBuffers can be destroyed mid-frame.
	impl->destroyLater([impl = impl.get(), fb]() {
		impl->freeTextureResources(fb.color);
		impl->rtvHeap.free(fb.rtv);
		if (fb.msaa) {
			fb.msaa->Release();
		}
		impl->rtvHeap.free(fb.msaaRtv);
	});
	fb = {}; // the copy owns the resources now
}

const D3D12Texture& D3D12Renderer::framebufferTexture(D3D12Framebuffer& fb,
                                                      const TextureFilter filter) {
	if (filter == fb.color.minFilter) {
		return fb.color;
	}
	// Doesn't own the resource or its view
	fb.alternateFilter = fb.color;
	fb.alternateFilter.minFilter = filter;
	fb.alternateFilter.magFilter = filter;
	return fb.alternateFilter;
}

void D3D12Renderer::pushRenderTarget(D3D12Framebuffer& framebuffer) {
	if (!impl->frameActive) {
		return;
	}
	impl->unbindTarget();
	impl->renderTargetStack.push_back(&framebuffer);
}

void D3D12Renderer::popRenderTarget() {
	if (!impl->frameActive || impl->renderTargetStack.empty()) {
		return;
	}
	impl->unbindTarget();
	D3D12Framebuffer& popped = *impl->renderTargetStack.back();
	impl->renderTargetStack.pop_back();
	if (popped.resolvePending) {
		impl->resolveFramebuffer(popped); // as it's done being drawn into, like on OpenGL
	}
}

void D3D12Renderer::clearCurrentRenderTarget(const Rgba color) {
	if (!impl->frameActive) {
		return;
	}
	impl->bindTarget();
	const auto [width, height] = impl->currentTargetSize();
	const Rect scissor = impl->effectiveScissor(width, height);
	if (scissor.width <= 0 || scissor.height <= 0) {
		return; // without any rectangles ClearRenderTargetView would clear all of it
	}
	const std::array<float, 4> values = { color.getRed(), color.getGreen(), color.getBlue(),
		                                  color.getAlpha() };
	const D3D12_RECT rect{ scissor.x, scissor.y, scissor.x + scissor.width,
		                   scissor.y + scissor.height };
	impl->commandList->ClearRenderTargetView(impl->currentRtv, values.data(), 1, &rect);
}

std::unique_ptr<D3D12ShaderProgram> D3D12Renderer::createShaderProgram(
    const std::vector<uint32_t>& vertexSpirv, const std::vector<uint32_t>& fragmentSpirv,
    std::vector<ShaderUniform> uniforms, const uint32_t uniformBlockSize) {
	auto program = std::make_unique<D3D12ShaderProgram>();
	program->vertexShader = compileSpirvToDxbc(vertexSpirv, Shader::Type::VERTEX, "vertex");
	program->pixelShader = compileSpirvToDxbc(fragmentSpirv, Shader::Type::FRAGMENT, "fragment");
	program->uniforms = std::move(uniforms);
	program->uniformBlockSize = uniformBlockSize;
	program->uniformData.resize(uniformBlockSize);
	// Fails here rather than when it's first drawn with if the shaders don't fit together
	impl->programPipeline(*program, PassKind::Ldr, BlendVariant::Composite,
	                      PrimitiveType::Triangles);
	return program;
}

void D3D12Renderer::destroyShaderProgram(D3D12ShaderProgram& program) {
	// The pipeline states might still be used by the current or an in-flight command list
	impl->destroyLater([pipelines = program.pipelines]() {
		for (ID3D12PipelineState* const pipeline : pipelines) {
			if (pipeline) {
				pipeline->Release();
			}
		}
	});
	program.pipelines = {};
	if (impl->activeShaderProgram == &program) {
		impl->activeShaderProgram = nullptr;
	}
}

void D3D12Renderer::setActiveShaderProgram(const D3D12ShaderProgram* const program) {
	impl->activeShaderProgram = program;
}

void D3D12Renderer::setActiveProgramUniform(const uint32_t offset, const void* const data,
                                            const uint32_t bytes) {
	assert(impl->activeShaderProgram);
	assert(offset + bytes <= impl->activeShaderProgram->uniformBlockSize);
	// Only copied from on the CPU, see bindActiveProgramUniforms()
	std::memcpy(impl->activeShaderProgram->uniformData.data() + offset, data, bytes);
}

void D3D12Renderer::setBlendingEnabled(const bool enabled) {
	impl->blendingEnabled = enabled;
}

bool D3D12Renderer::isBlendingEnabled() const {
	return impl->blendingEnabled;
}

void D3D12Renderer::setSpriteBlend(const SpriteBlend blend) {
	impl->spriteBlend = blend;
}

D3D12Renderer::SpriteBlend D3D12Renderer::getSpriteBlend() const {
	return impl->spriteBlend;
}

void D3D12Renderer::setAlphaOnlyWrite(const bool alphaOnly) {
	impl->alphaOnlyWrite = alphaOnly;
}

bool D3D12Renderer::isAlphaOnlyWrite() const {
	return impl->alphaOnlyWrite;
}

void D3D12Renderer::setVerticalSync(const bool enabled) {
	// Present() is told each frame
	impl->verticalSyncEnabled = enabled;
}

bool D3D12Renderer::getVerticalSync() const {
	return impl->verticalSyncEnabled;
}

bool D3D12Renderer::isMultisampleSupported() const {
	return impl->msaaSupported;
}

void D3D12Renderer::setMultisampleAntiAliasing(const bool enabled) {
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
	impl->msaaEnabled = enabled;
	impl->msaaRebuildPending = false;
	impl->waitIdle();
	impl->destroyMsaaTarget();
	if (impl->swapchainMsaaActive()) {
		impl->createMsaaTarget();
	}
}

bool D3D12Renderer::getMultisampleAntiAliasing() const {
	if (!impl->msaaSupported) {
		return false;
	}
	return impl->msaaRebuildPending ? impl->pendingMsaaEnabled : impl->msaaEnabled;
}

void D3D12Renderer::updateLetterboxing(const int fbWidth, const int fbHeight, const int canvasW,
                                       const int canvasH) {
	impl->framebufferWidth = fbWidth;
	impl->framebufferHeight = fbHeight;
	impl->canvasWidth = canvasW;
	impl->canvasHeight = canvasH;
	impl->letterboxing = canvasW != fbWidth || canvasH != fbHeight;
	if (impl->targetBound && impl->renderTargetStack.empty()) {
		impl->applyScissor();
	}
}

void D3D12Renderer::setScissor(const std::optional<ScissorBox> box) {
	impl->userScissor = box;
	if (impl->targetBound) {
		impl->applyScissor();
	}
}

int D3D12Renderer::getFramebufferWidth() const {
	return impl->framebufferWidth;
}

int D3D12Renderer::getFramebufferHeight() const {
	return impl->framebufferHeight;
}

} // namespace jngl
