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

#include <array>
#include <cstdint>
#include <functional>
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
};

/// GPU resources backing a jngl::FrameBuffer on the Vulkan backend: a color image that can be both
/// rendered into and sampled (\a color), plus the VkFramebuffer that targets it.
struct VulkanFramebuffer {
	VulkanTexture color;
	VkFramebuffer framebuffer = VK_NULL_HANDLE;
	int width = 0;
	int height = 0;
};

/// GPU resources backing a jngl::ShaderProgram on the Vulkan backend: one textured pipeline per
/// primitive topology, built from a user-supplied vertex/fragment shader pair.
struct VulkanShaderProgram {
	std::array<VkPipeline, 4> pipelines{}; // indexed by PrimitiveType
	std::array<VkPipeline, 4> msaaPipelines{}; // swapchain MSAA, when active at creation
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

	/// Create a texture from pixel data. \a format and \a type are the OpenGL enums JNGL already
	/// uses (GL_RGBA/GL_RGB/GL_BGR and GL_UNSIGNED_BYTE); pixels come either as \a rowPointers (one
	/// pointer per row) or as a single \a data block. Returns nullptr for unsupported formats.
	std::unique_ptr<VulkanTexture> createTexture(int width, int height, unsigned int format,
	                                             unsigned int type, const unsigned char* data,
	                                             const unsigned char* const* rowPointers);
	void destroyTexture(VulkanTexture&);
	void updateTextureBytes(VulkanTexture&, int width, int height, const unsigned char* rgba);

	/// Draw a textured primitive (\a xyuv: interleaved x, y, u, v) modulated by \a color.
	void drawSprite(const VulkanTexture&, const float* xyuv, std::size_t vertexCount, PrimitiveType,
	                const Mat3& modelview, Rgba color);

	/// Reads back a \a width x \a height region of the current frame as tightly packed RGB bytes,
	/// bottom row first (matching OpenGL's glReadPixels). Flushes the pending draws first.
	void readPixels(unsigned char* rgb, int x, int y, int width, int height);

	/// Creates an off-screen render target (see jngl::FrameBuffer), cleared to transparent.
	std::unique_ptr<VulkanFramebuffer> createFramebuffer(int width, int height);
	/// Schedules \a framebuffer's resources for destruction once the GPU is done with them.
	void destroyFramebuffer(VulkanFramebuffer&);

	/// Redirects subsequent draws into \a framebuffer until popRenderTarget() is called. Render
	/// targets nest (FrameBuffers can be used inside one another).
	void pushRenderTarget(VulkanFramebuffer& framebuffer);
	void popRenderTarget();

	/// Clears the render target that's currently bound (the active FrameBuffer, or the screen).
	void clearCurrentRenderTarget(Rgba color);

	/// Builds a custom shader program (see jngl::ShaderProgram) from vertex + fragment SPIR-V.
	std::unique_ptr<VulkanShaderProgram> createShaderProgram(const std::vector<uint32_t>& vertexSpirv,
	                                                         const std::vector<uint32_t>& fragmentSpirv);
	void destroyShaderProgram(VulkanShaderProgram&);
	/// Makes \a program (or nullptr for the built-in sprite shader) the one used by drawSprite.
	void setActiveShaderProgram(const VulkanShaderProgram* program);

	/// Toggles v-sync by switching the swapchain present mode (FIFO vs mailbox/immediate).
	void setVerticalSync(bool enabled);
	[[nodiscard]] bool getVerticalSync() const;

	/// Whether 4x MSAA is supported for the swapchain.
	[[nodiscard]] bool isMultisampleSupported() const;
	void setMultisampleAntiAliasing(bool enabled);
	[[nodiscard]] bool getMultisampleAntiAliasing() const;

private:
	void queryMsaaSupport();
	[[nodiscard]] bool swapchainMsaaActive() const;
	void createMsaaAttachments();
	void destroyMsaaAttachments();
	void destroyColoredPipelines();
	void destroyTexturedPipelines();
	void destroyColoredMsaaPipelines();
	void destroyTexturedMsaaPipelines();
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
	void createColoredPipelines();
	void buildColoredPipelines(VkShaderModule vert, VkShaderModule frag, VkRenderPass targetRenderPass,
	                           VkSampleCountFlagBits samples, std::array<VkPipeline, 4>& out);
	void createVertexBuffers();
	void createTexturedPipelines();
	/// Builds one textured pipeline per primitive topology from the given shader modules into \a out.
	void buildTexturedPipelines(VkShaderModule vert, VkShaderModule frag, VkRenderPass targetRenderPass,
	                            VkSampleCountFlagBits samples, std::array<VkPipeline, 4>& out);
	void createDescriptorPool();
	void createOffscreenRenderPass();

	/// Begins the render pass for whatever render target is currently on top of the stack (a
	/// FrameBuffer, or the swapchain when the stack is empty), with the given load operation.
	void beginCurrentRenderPass(VkAttachmentLoadOp loadOp);
	/// Sets up a combined-image-sampler descriptor set sampling \a view with \a sampler.
	[[nodiscard]] VkDescriptorSet allocateTextureDescriptor(VkImageView view, VkSampler sampler);
	/// Immediately destroys a framebuffer's GPU resources (assumes the GPU is no longer using them).
	void freeFramebufferResources(VulkanFramebuffer&);

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
	std::vector<VkFramebuffer> swapchainFramebuffers;
	// Single-sample framebuffers for the LOAD swapchain pass when MSAA is active (e.g. readPixels).
	std::vector<VkFramebuffer> loadSwapchainFramebuffers;
	struct MsaaColorAttachment {
		VkImage image = VK_NULL_HANDLE;
		VkDeviceMemory memory = VK_NULL_HANDLE;
		VkImageView view = VK_NULL_HANDLE;
	};
	std::vector<MsaaColorAttachment> msaaColorAttachments;

	VkRenderPass renderPass = VK_NULL_HANDLE;
	// Same as renderPass but with a LOAD (instead of CLEAR) load operation, used to resume a frame
	// after readPixels has flushed it mid-way.
	VkRenderPass loadRenderPass = VK_NULL_HANDLE;
	// Render pass used when drawing into a FrameBuffer (LOAD; clearing is done explicitly). Shares
	// the swapchain's color format so the colored/textured pipelines stay compatible with it.
	VkRenderPass offscreenRenderPass = VK_NULL_HANDLE;
	// Stack of active FrameBuffers; empty means drawing targets the swapchain.
	std::vector<VulkanFramebuffer*> renderTargetStack;
	// FrameBuffers destroyed while their resources might still be referenced by an in-flight (or
	// not-yet-submitted) command buffer. They're actually freed once enough frames have passed, see
	// destroyFramebuffer / endFrame.
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
	std::array<VkPipeline, 4> coloredPipelines{}; // indexed by PrimitiveType
	std::array<VkPipeline, 4> coloredMsaaPipelines{}; // swapchain MSAA, when active

	// Pipeline drawing textured geometry (sprites, text) with the built-in texture shader. One per
	// primitive topology; they share the layout, descriptor set layout and shader modules.
	VkDescriptorSetLayout texturedSetLayout = VK_NULL_HANDLE;
	VkPipelineLayout texturedPipelineLayout = VK_NULL_HANDLE;
	std::array<VkPipeline, 4> texturedPipelines{}; // indexed by PrimitiveType
	std::array<VkPipeline, 4> texturedMsaaPipelines{}; // swapchain MSAA, when active
	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
	// Custom shader program currently bound via ShaderProgram::use(); nullptr = built-in shader.
	const VulkanShaderProgram* activeShaderProgram = nullptr;

	// Host-visible vertex buffer per frame in flight, written directly each frame. `used` is reset
	// in beginFrame and grows as drawColored appends geometry.
	struct DynamicVertexBuffer {
		VkBuffer buffer = VK_NULL_HANDLE;
		VkDeviceMemory memory = VK_NULL_HANDLE;
		void* mapped = nullptr;
		VkDeviceSize capacity = 0;
		VkDeviceSize used = 0;
	};
	std::vector<DynamicVertexBuffer> vertexBuffers;

	uint32_t currentFrame = 0;
	uint32_t currentImageIndex = 0;
	bool frameActive = false;
	bool framebufferResized = false;
	bool vertexBufferOverflowReported = false;
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
	// True while the active swapchain render pass uses multisampled attachments.
	bool swapchainUsesMsaaPass = false;

	Mat4 projection;
	Rgb currentClearColor{ 0, 0, 0 }; // swapchain clear color for the current frame
};

} // namespace jngl
