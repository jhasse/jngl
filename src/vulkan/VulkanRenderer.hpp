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
#include <vector>
#include <vulkan/vulkan.h>

struct SDL_Window;

namespace jngl {

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

private:
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
	void createColoredPipelines();
	void createVertexBuffers();

	void cleanupSwapchain();
	void recreateSwapchain();

	[[nodiscard]] uint32_t findMemoryType(uint32_t typeFilter,
	                                      VkMemoryPropertyFlags properties) const;

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

	VkRenderPass renderPass = VK_NULL_HANDLE;
	VkCommandPool commandPool = VK_NULL_HANDLE;
	std::vector<VkCommandBuffer> commandBuffers;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;

	// Pipeline drawing single-color geometry with the built-in "simple" shader. There's one
	// pipeline per primitive topology (they only differ in input assembly state); they share the
	// layout and shader modules.
	VkPipelineLayout coloredPipelineLayout = VK_NULL_HANDLE;
	std::array<VkPipeline, 4> coloredPipelines{}; // indexed by PrimitiveType

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

	bool validationEnabled = false;

	Mat4 projection;
};

} // namespace jngl
