// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

/// Experimental Vulkan implementation of the Renderer interface, see Renderer.hpp.
///
/// This is a minimal 2D backend for the desktop SDL3 platform: it brings up a Vulkan instance,
/// device and swapchain and can clear the screen and present. Drawing pipelines (sprites/shapes)
/// are built on top of this; see VulkanRenderer.cpp.
/// @file
#pragma once

#include "../Renderer.hpp"
#include "../jngl/Mat4.hpp"
#include "ShaderCompiler.hpp"

#include <array>
#include <cstdint>
#include <functional>
#include <optional>
#include <vector>
#include <vulkan/vulkan.h>

struct SDL_Window;

namespace jngl {

/// GPU resources backing a jngl::Texture on the Vulkan backend. Created and destroyed through
/// VulkanRenderer; owned by jngl::Texture.
struct VulkanTexture {
	VkImage image = VK_NULL_HANDLE;
	VkDeviceMemory memory = VK_NULL_HANDLE;
	VkImageView view = VK_NULL_HANDLE;
	VkSampler sampler = VK_NULL_HANDLE;
	VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
	int width = 0;
	int height = 0;
	uint32_t mipLevels = 1;
};

/// GPU resources backing a jngl::FrameBuffer on the Vulkan backend: a color image that can be both
/// rendered into and sampled (\a color), plus the VkFramebuffer that targets it.
///
/// An anti-aliased one also has a multisampled image, which is drawn into instead while
/// anti-aliasing is enabled and resolved into \a color at the end of each render pass.
struct VulkanFramebuffer {
	VulkanTexture color;
	VkFramebuffer framebuffer = VK_NULL_HANDLE;
	int width = 0;
	int height = 0;

	VkImage msaaImage = VK_NULL_HANDLE; ///< VK_NULL_HANDLE if it isn't anti-aliased
	VkDeviceMemory msaaMemory = VK_NULL_HANDLE;
	VkImageView msaaView = VK_NULL_HANDLE;
	/// Draws into msaaImage and resolves it into color, see OffscreenRenderPasses::msaa
	VkFramebuffer msaaFramebuffer = VK_NULL_HANDLE;
	/// Only msaaImage, for copying color into it, see OffscreenRenderPasses::expand
	VkFramebuffer expandFramebuffer = VK_NULL_HANDLE;
	/// Whether color has been drawn into directly (while anti-aliasing was disabled), so that
	/// msaaImage has to get its pixels before it's drawn into again
	bool samplesStale = false;
	/// Whether its images have 16-bit float channels (VulkanRenderer::hdrFormat)
	bool hdr = false;
};

/// The kinds of render passes there are pipelines for, as a pipeline only works with render passes
/// that have the same color format and number of samples: 8-bit (the swapchain's format, also used
/// by FrameBuffers) or 16-bit float (HDR FrameBuffers), each without or with MSAA
enum class PassKind : uint8_t { Ldr, LdrMsaa, Hdr, HdrMsaa };
constexpr size_t passKindCount = 4;

/// Host-visible, persistently mapped vertex buffer that's appended to during a frame
struct DynamicVertexBuffer {
	VkBuffer buffer = VK_NULL_HANDLE;
	VkDeviceMemory memory = VK_NULL_HANDLE;
	void* mapped = nullptr;
	VkDeviceSize capacity = 0;
	VkDeviceSize used = 0;
};

/// Host-visible, persistently mapped ring of uniform blocks that's appended to during a frame, one
/// per draw with a custom ShaderProgram, bound with a dynamic offset via \a descriptorSet
struct DynamicUniformBuffer {
	VkBuffer buffer = VK_NULL_HANDLE;
	VkDeviceMemory memory = VK_NULL_HANDLE;
	void* mapped = nullptr;
	VkDeviceSize capacity = 0;
	VkDeviceSize used = 0;
	VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
};

/// GPU resources backing a jngl::ShaderProgram on the Vulkan backend: textured pipelines for each
/// blend variant × primitive topology, built from a user-supplied vertex/fragment shader pair, plus
/// an optional CPU-side uniform staging buffer (see ShaderProgram::setUniform). At draw time the
/// staging contents are copied into the per-frame dynamic UBO ring so mid-frame setUniform changes
/// (e.g. blur pass 0 then 1) are visible to each draw.
struct VulkanShaderProgram {
	static constexpr uint32_t topologyCount = 4;
	static constexpr uint32_t blendVariantCount = 4; // see VulkanRenderer::texturedBlendVariant()
	static constexpr uint32_t pipelineCount = topologyCount * blendVariantCount;
	/// Indexed by PassKind. Those for HDR FrameBuffers are only built once they're drawn into, see
	/// VulkanRenderer::ensureProgramPipelines(), from vertexSpirv and fragmentSpirv.
	mutable std::array<std::array<VkPipeline, pipelineCount>, passKindCount> pipelines{};
	std::vector<uint32_t> vertexSpirv;
	std::vector<uint32_t> fragmentSpirv;
	std::vector<ShaderUniform> uniforms;
	uint32_t uniformBlockSize = 0;
	VkBuffer uniformBuffer = VK_NULL_HANDLE; // host-visible staging; not bound to the GPU
	VkDeviceMemory uniformMemory = VK_NULL_HANDLE;
	void* uniformMapped = nullptr;
};

class VulkanRenderer final : public Renderer {
public:
	/// \a nativeWindow must be an SDL_Window* created with the SDL_WINDOW_VULKAN flag.
	explicit VulkanRenderer(void* nativeWindow);
	~VulkanRenderer() override;

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
	std::unique_ptr<VulkanTexture> createTexture(int width, int height, unsigned int format,
	                                             unsigned int type, const unsigned char* data,
	                                             const unsigned char* const* rowPointers,
	                                             bool mipmap = false);
	void destroyTexture(VulkanTexture&);
	/// Replaces the texture's pixels, and regenerates its mipmaps if it has any
	void updateTextureBytes(VulkanTexture&, int width, int height, const unsigned char* rgba);
	/// Gives a texture created without mipmaps a mipmap chain generated from its pixels. Does
	/// nothing if it already has one.
	void enableMipmaps(VulkanTexture&);

	/// Draw a textured primitive (\a xyuv: interleaved x, y, u, v) modulated by \a color.
	void drawSprite(const VulkanTexture&, const float* xyuv, std::size_t vertexCount, PrimitiveType,
	                const Mat3& modelview, Rgba color);
	/// Draw a textured primitive with each of its channels (RGBA) mixed towards \a lerpColor (and
	/// an alpha of 0) by \a lerpFactors, like OpenGL's texture shader does.
	void drawSprite(const VulkanTexture&, const float* xyuv, std::size_t vertexCount, PrimitiveType,
	                const Mat3& modelview, Rgb lerpColor, std::array<float, 4> lerpFactors);

	/// Reads back a \a width x \a height region of the current frame as tightly packed RGB bytes,
	/// bottom row first (matching OpenGL's glReadPixels). Flushes the pending draws first.
	void readPixels(unsigned char* rgb, int x, int y, int width, int height);

	/// Creates an off-screen render target (see jngl::FrameBuffer), cleared to transparent.
	/// \param antiAliased Multisampled while anti-aliasing is enabled, if it's supported
	/// \param hdr With 16-bit float channels, see hdrFormat
	std::unique_ptr<VulkanFramebuffer> createFramebuffer(int width, int height, bool antiAliased,
	                                                     bool hdr);

	/// Format of HDR FrameBuffers, which every Vulkan implementation can render into, blend and
	/// sample with linear filtering
	static constexpr VkFormat hdrFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
	/// Schedules \a framebuffer's resources for destruction once the GPU is done with them.
	void destroyFramebuffer(VulkanFramebuffer&);

	/// Redirects subsequent draws into \a framebuffer until popRenderTarget() is called. Render
	/// targets nest (FrameBuffers can be used inside one another).
	void pushRenderTarget(VulkanFramebuffer& framebuffer);
	void popRenderTarget();

	/// Clears the render target that's currently bound (the active FrameBuffer, or the screen),
	/// within the scissor, like glClear. Does nothing if no frame has begun.
	void clearCurrentRenderTarget(Rgba color);

	/// Builds a custom shader program (see jngl::ShaderProgram) from vertex + fragment SPIR-V.
	/// \a uniforms / \a uniformBlockSize describe the fragment UBO (empty if none).
	std::unique_ptr<VulkanShaderProgram>
	createShaderProgram(const std::vector<uint32_t>& vertexSpirv,
	                    const std::vector<uint32_t>& fragmentSpirv,
	                    std::vector<ShaderUniform> uniforms, uint32_t uniformBlockSize);
	void destroyShaderProgram(VulkanShaderProgram&);
	/// Makes \a program (or nullptr for the built-in sprite shader) the one used by drawSprite.
	void setActiveShaderProgram(const VulkanShaderProgram* program);
	/// Writes \a bytes at \a offset into the active program's UBO (must be bound via use()).
	void setActiveProgramUniform(uint32_t offset, const void* data, uint32_t bytes);

	/// Sprite blending state (OpenGL's glEnable/glDisable(GL_BLEND) / glBlendFunc / glColorMask).
	/// Selects among the textured pipeline variants; see texturedBlendVariant().
	void setBlendingEnabled(bool enabled);
	[[nodiscard]] bool isBlendingEnabled() const;
	enum class SpriteBlend : uint8_t { Composite, Premultiplied };
	void setSpriteBlend(SpriteBlend);
	[[nodiscard]] SpriteBlend getSpriteBlend() const;
	void setAlphaOnlyWrite(bool);
	[[nodiscard]] bool isAlphaOnlyWrite() const;

	/// Toggles v-sync by switching the swapchain present mode (FIFO vs mailbox/immediate).
	void setVerticalSync(bool enabled);
	[[nodiscard]] bool getVerticalSync() const;

	/// Whether 4x MSAA is supported for the swapchain and FrameBuffers.
	[[nodiscard]] bool isMultisampleSupported() const;
	void setMultisampleAntiAliasing(bool enabled);
	[[nodiscard]] bool getMultisampleAntiAliasing() const;

	/// Configures letterboxing: black bars outside a centered \a canvasWidth x \a canvasHeight area.
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
	void applyScissor(VkCommandBuffer cmd, VkExtent2D extent) const;
	void applyViewport(VkCommandBuffer cmd, VkExtent2D extent) const;
	void clearSwapchainCanvas(const Rgb& color);
	void syncLetterboxScissor();
	[[nodiscard]] VkRect2D effectiveScissor(VkExtent2D extent) const;
	/// The swapchain's extent, or the FrameBuffer's that's drawn on
	[[nodiscard]] VkExtent2D currentRenderTargetExtent() const;
	[[nodiscard]] VkRect2D letterboxScissorForExtent(VkExtent2D extent) const;
	[[nodiscard]] static VkRect2D intersectScissors(VkRect2D a, VkRect2D b);
	[[nodiscard]] static VkRect2D glScissorToVulkan(int glX, int glY, int glW, int glH,
	                                                int framebufferHeight);
	void queryMsaaSupport();
	[[nodiscard]] bool swapchainMsaaActive() const;
	void createMsaaAttachments();
	void destroyMsaaAttachments();
	void destroyColoredPipelines();
	void destroyTexturedPipelines();
	/// The offscreen render pass that the pipelines of \a kind are built against
	[[nodiscard]] VkRenderPass pipelineRenderPass(PassKind kind) const;
	[[nodiscard]] VkSampleCountFlagBits pipelineSamples(PassKind kind) const;
	/// Builds the built-in pipelines for render passes of \a kind, if they haven't been yet
	void ensurePipelines(PassKind kind);
	/// Builds \a program's pipelines for render passes of \a kind, if they haven't been yet
	void ensureProgramPipelines(const VulkanShaderProgram& program, PassKind kind);
	void rebuildSwapchainRendering();
	void createInstance();
	void createSurface();
	void pickPhysicalDevice();
	void createLogicalDevice();
	void createSwapchain();
	void createImageViews();
	void createRenderPass();
	void createFramebuffers();
	void createCommandPoolAndBuffers();
	void createSyncObjects();
	/// Creates the per-swapchain-image "render finished" semaphores (recreated with the swapchain).
	void createPresentSemaphores();
	void createColoredPipelineLayout();
	void buildColoredPipelines(VkShaderModule vert, VkShaderModule frag, VkRenderPass targetRenderPass,
	                           VkSampleCountFlagBits samples, std::array<VkPipeline, 4>& out);
	void createRoundedPipelineLayout();
	void buildRoundedPipeline(VkShaderModule vert, VkShaderModule frag, VkRenderPass targetRenderPass,
	                          VkSampleCountFlagBits samples, VkPipeline& out);
	void destroyRoundedPipelines();
	void createVertexBuffers();
	[[nodiscard]] DynamicVertexBuffer createVertexBuffer(VkDeviceSize capacity);
	void destroyVertexBuffer(DynamicVertexBuffer&);
	/// Copies \a bytes of vertex data into the current frame's vertex buffer, growing it if it's
	/// full, and returns the buffer and offset to bind
	std::pair<VkBuffer, VkDeviceSize> uploadVertices(const void* data, VkDeviceSize bytes);
	void createUniformBuffers();
	[[nodiscard]] DynamicUniformBuffer createUniformBuffer(VkDeviceSize capacity);
	void destroyUniformBuffer(DynamicUniformBuffer&);
	void createTexturedPipelineLayout();
	/// Builds textured pipelines for every blend variant × topology from the given shader modules.
	void buildTexturedPipelines(VkShaderModule vert, VkShaderModule frag,
	                            VkRenderPass targetRenderPass, VkSampleCountFlagBits samples,
	                            std::array<VkPipeline, VulkanShaderProgram::pipelineCount>& out);
	/// Index into texturedPipelines / VulkanShaderProgram::pipelines for the current blend state.
	[[nodiscard]] uint32_t texturedBlendVariant() const;
	[[nodiscard]] static uint32_t texturedPipelineIndex(uint32_t blendVariant, PrimitiveType type);
	void createDescriptorPool();
	/// Copies the active program's staging UBO into this frame's ring and binds it (dynamic
	/// offset).
	void bindActiveProgramUniforms(VkCommandBuffer cmd);
	/// Creates offscreenRenderPasses. Pipelines are built against these, as they don't change.
	void createOffscreenRenderPasses();
	/// Records a render pass that copies \a framebuffer's color into all samples of its msaaImage
	void expandIntoSamples(VulkanFramebuffer& framebuffer);

	/// Begins the render pass for whatever render target is currently on top of the stack (a
	/// FrameBuffer, or the swapchain when the stack is empty), with the given load operation.
	void beginCurrentRenderPass(VkAttachmentLoadOp loadOp);
	/// Sets up a combined-image-sampler descriptor set sampling \a view with \a sampler.
	[[nodiscard]] VkDescriptorSet allocateTextureDescriptor(VkImageView view, VkSampler sampler);
	/// Immediately destroys a framebuffer's GPU resources (assumes the GPU is no longer using them).
	void freeFramebufferResources(VulkanFramebuffer&);
	/// Creates an RGBA image with \a mipLevels levels which can be sampled and blitted from/to
	void createTextureImage(VulkanTexture&, uint32_t mipLevels);
	/// Creates the view, sampler and descriptor set for the texture's image
	void createTextureDescriptor(VulkanTexture&);
	/// Records blits that fill all of the texture's mip levels from the first one, which has to be
	/// in TRANSFER_DST_OPTIMAL like the others. Leaves all of them in SHADER_READ_ONLY_OPTIMAL.
	static void recordMipmapGeneration(VkCommandBuffer, const VulkanTexture&);

	void cleanupSwapchain();
	void recreateSwapchain();
	[[nodiscard]] VkPresentModeKHR choosePresentMode() const;

	[[nodiscard]] uint32_t findMemoryType(uint32_t typeFilter,
	                                      VkMemoryPropertyFlags properties) const;

	/// Records and submits \a record on a transient command buffer, waiting for completion. Used
	/// for one-off transfers like texture uploads.
	void submitOneTime(const std::function<void(VkCommandBuffer)>& record);

	/// Maximum number of frames the CPU may be working on ahead of the GPU.
	static constexpr uint32_t maxFramesInFlight = 2;

	SDL_Window* window;

	VkInstance instance = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
	VkSurfaceKHR surface = VK_NULL_HANDLE;

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	uint32_t graphicsQueueFamily = 0;
	uint32_t presentQueueFamily = 0;
	VkDevice device = VK_NULL_HANDLE;
	VkQueue graphicsQueue = VK_NULL_HANDLE;
	VkQueue presentQueue = VK_NULL_HANDLE;

	VkSwapchainKHR swapchain = VK_NULL_HANDLE;
	VkFormat swapchainImageFormat = VK_FORMAT_UNDEFINED;
	VkExtent2D swapchainExtent{};
	std::vector<VkImage> swapchainImages;
	std::vector<VkImageView> swapchainImageViews;
	// For renderPass and loadRenderPass: with MSAA, the multisampled image and the swapchain image
	// it's resolved into
	std::vector<VkFramebuffer> swapchainFramebuffers;
	struct MsaaColorAttachment {
		VkImage image = VK_NULL_HANDLE;
		VkDeviceMemory memory = VK_NULL_HANDLE;
		VkImageView view = VK_NULL_HANDLE;
	};
	std::vector<MsaaColorAttachment> msaaColorAttachments;

	VkRenderPass renderPass = VK_NULL_HANDLE;
	// Same as renderPass but with a LOAD (instead of CLEAR) load operation, used to resume a frame
	// after readPixels has flushed it mid-way or a FrameBuffer has been drawn into. With MSAA, it
	// continues drawing into the multisampled image.
	VkRenderPass loadRenderPass = VK_NULL_HANDLE;
	// Render passes used when drawing into a FrameBuffer, for each of its formats: the swapchain's
	// (so that the pipelines built against them work with the swapchain's render passes, too) and
	// hdrFormat. Indexed by VulkanFramebuffer::hdr.
	struct OffscreenRenderPasses {
		// LOAD; clearing is done explicitly
		VkRenderPass single = VK_NULL_HANDLE;
		// Same for an anti-aliased FrameBuffer: draws into its multisampled image (LOAD) and
		// resolves that into its color image at the end. If MSAA is supported.
		VkRenderPass msaa = VK_NULL_HANDLE;
		// Only the multisampled image of an anti-aliased FrameBuffer, see expandIntoSamples
		VkRenderPass expand = VK_NULL_HANDLE;
	};
	std::array<OffscreenRenderPasses, 2> offscreenRenderPasses{};
	void createOffscreenRenderPasses(VkFormat, OffscreenRenderPasses&);
	// Stack of active FrameBuffers; empty means drawing targets the swapchain.
	std::vector<VulkanFramebuffer*> renderTargetStack;
	// FrameBuffers destroyed (or textures replaced, see enableMipmaps) while their resources might
	// still be referenced by an in-flight (or not-yet-submitted) command buffer. They're actually
	// freed once enough frames have passed, see destroyFramebuffer / endFrame.
	struct TrashedFramebuffer {
		VulkanFramebuffer fb;
		int framesUntilFree;
	};
	std::vector<TrashedFramebuffer> framebufferTrash;
	VkCommandPool commandPool = VK_NULL_HANDLE;
	std::vector<VkCommandBuffer> commandBuffers;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	VkFence readbackFence = VK_NULL_HANDLE; // signalled by the readPixels flush submit

	// Pipeline drawing single-color geometry with the built-in "simple" shader. There's one
	// pipeline per primitive topology (they only differ in input assembly state); they share the
	// layout and shader modules.
	VkPipelineLayout coloredPipelineLayout = VK_NULL_HANDLE;
	// Indexed by PassKind and PrimitiveType, see ensurePipelines()
	std::array<std::array<VkPipeline, 4>, passKindCount> coloredPipelines{};

	VkPipelineLayout roundedPipelineLayout = VK_NULL_HANDLE;
	std::array<VkPipeline, passKindCount> roundedPipelines{};

	// Pipeline drawing textured geometry (sprites, text) with the built-in texture shader.
	// set 0 = combined image sampler; set 1 = custom-uniform UBO (used only by user
	// ShaderPrograms). Pipelines: blendVariant × PrimitiveType, see texturedPipelineIndex().
	VkDescriptorSetLayout texturedSetLayout = VK_NULL_HANDLE;
	VkDescriptorSetLayout uniformsSetLayout = VK_NULL_HANDLE;
	VkPipelineLayout texturedPipelineLayout = VK_NULL_HANDLE;
	std::array<std::array<VkPipeline, VulkanShaderProgram::pipelineCount>, passKindCount>
	    texturedPipelines{};
	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
	// Custom shader program currently bound via ShaderProgram::use(); nullptr = built-in shader.
	const VulkanShaderProgram* activeShaderProgram = nullptr;
	bool blendingEnabled = true;
	SpriteBlend spriteBlend = SpriteBlend::Composite;
	bool alphaOnlyWrite = false;

	// Host-visible vertex buffer per frame in flight, written directly each frame. `used` is reset
	// in beginFrame and grows as the draw calls append geometry, see uploadVertices.
	std::vector<DynamicVertexBuffer> vertexBuffers;
	// Buffers that ran full during a frame and were replaced by a bigger one. They're still
	// referenced by that frame's command buffer, so they're destroyed once its fence has signaled.
	std::vector<std::vector<DynamicVertexBuffer>> retiredVertexBuffers;

	// Per-frame dynamic UBO ring: each drawSprite with custom uniforms snapshots staging into here
	// so mid-frame setUniform updates don't overwrite earlier draws still pending on the GPU. Grows
	// like vertexBuffers, see bindActiveProgramUniforms.
	std::vector<DynamicUniformBuffer> uniformBuffers;
	std::vector<std::vector<DynamicUniformBuffer>> retiredUniformBuffers;
	VkDeviceSize minUniformBufferOffsetAlignment = 256;
	static constexpr VkDeviceSize uniformDynamicRange = 256; // max UBO size we bind per draw

	uint32_t currentFrame = 0;
	uint32_t currentImageIndex = 0;
	bool frameActive = false;
	// The swapchain has to be recreated after the current frame has been presented, e.g. as the
	// window has been resized or v-sync toggled
	bool swapchainRecreatePending = false;
	// Set once the current frame's imageAvailable semaphore has been waited on (by a readPixels
	// flush), so endFrame doesn't wait on it a second time.
	bool imageAvailableWaited = false;

	bool validationEnabled = false;
	bool verticalSyncEnabled = true;
	bool msaaSupported = false;
	bool msaaEnabled = true;
	bool msaaRebuildPending = false;
	bool pendingMsaaEnabled = true;
	VkSampleCountFlagBits msaaSampleCount = VK_SAMPLE_COUNT_1_BIT;
	// What the active render pass (the swapchain's or a FrameBuffer's) draws into, which decides
	// the pipelines that have to be used
	PassKind currentPassKind = PassKind::Ldr;

	int framebufferWidth = 0;
	int framebufferHeight = 0;
	int canvasWidth = 0;
	int canvasHeight = 0;
	bool letterboxing = false;
	std::optional<ScissorBox> userScissor;

	Mat4 projection;
	Rgb currentClearColor{ 0, 0, 0 }; // swapchain clear color for the current frame
};

} // namespace jngl
