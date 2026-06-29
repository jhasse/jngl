// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "VulkanRenderer.hpp"

#include "../App.hpp"
#include "../jngl/Mat3.hpp"
#include "../log.hpp"
#include "ShaderCompiler.hpp"
#include "vulkan_helper.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <algorithm>
#include <array>
#include <cstring>
#include <limits>
#include <set>
#include <stdexcept>

namespace jngl {

namespace {

constexpr std::array validationLayers = { "VK_LAYER_KHRONOS_validation" };

bool validationLayersSupported() {
	uint32_t count = 0;
	vkEnumerateInstanceLayerProperties(&count, nullptr);
	std::vector<VkLayerProperties> available(count);
	vkEnumerateInstanceLayerProperties(&count, available.data());
	for (const char* const wanted : validationLayers) {
		const bool found = std::ranges::any_of(available, [&](const VkLayerProperties& p) {
			return std::strcmp(p.layerName, wanted) == 0;
		});
		if (!found) {
			return false;
		}
	}
	return true;
}

VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
              VkDebugUtilsMessageTypeFlagsEXT /*type*/,
              const VkDebugUtilsMessengerCallbackDataEXT* data, void* /*userData*/) {
	if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
		internal::error(std::string("Vulkan: ") + data->pMessage);
	} else if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
		internal::debug("Vulkan: {}", data->pMessage);
	} else {
		internal::trace(std::string("Vulkan: ") + data->pMessage);
	}
	return VK_FALSE;
}

} // namespace

VulkanRenderer::VulkanRenderer(void* const nativeWindow)
: window(static_cast<SDL_Window*>(nativeWindow)) {
	createInstance();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapchain();
	createImageViews();
	createPresentSemaphores();
	createRenderPass();
	createFramebuffers();
	createCommandPoolAndBuffers();
	createSyncObjects();
	createColoredPipelines();
	createDescriptorPool();
	createTexturedPipelines();
	createOffscreenRenderPass();
	createVertexBuffers();
	internal::debug("Vulkan renderer initialized ({}x{}).", swapchainExtent.width,
	                swapchainExtent.height);
}

VulkanRenderer::~VulkanRenderer() {
	if (device) {
		vkDeviceWaitIdle(device);
	}
	cleanupSwapchain();
	if (device) {
		for (auto& trashed : framebufferTrash) {
			freeFramebufferResources(trashed.fb);
		}
		framebufferTrash.clear();
		for (auto& vb : vertexBuffers) {
			if (vb.buffer) {
				vkDestroyBuffer(device, vb.buffer, nullptr);
			}
			if (vb.memory) {
				vkUnmapMemory(device, vb.memory);
				vkFreeMemory(device, vb.memory, nullptr);
			}
		}
		for (VkPipeline pipeline : coloredPipelines) {
			if (pipeline) {
				vkDestroyPipeline(device, pipeline, nullptr);
			}
		}
		if (coloredPipelineLayout) {
			vkDestroyPipelineLayout(device, coloredPipelineLayout, nullptr);
		}
		for (VkPipeline pipeline : texturedPipelines) {
			if (pipeline) {
				vkDestroyPipeline(device, pipeline, nullptr);
			}
		}
		if (texturedPipelineLayout) {
			vkDestroyPipelineLayout(device, texturedPipelineLayout, nullptr);
		}
		if (texturedSetLayout) {
			vkDestroyDescriptorSetLayout(device, texturedSetLayout, nullptr);
		}
		if (descriptorPool) {
			vkDestroyDescriptorPool(device, descriptorPool, nullptr);
		}
		for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
			if (i < imageAvailableSemaphores.size()) {
				vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
			}
			if (i < inFlightFences.size()) {
				vkDestroyFence(device, inFlightFences[i], nullptr);
			}
		}
		if (readbackFence) {
			vkDestroyFence(device, readbackFence, nullptr);
		}
		if (renderPass) {
			vkDestroyRenderPass(device, renderPass, nullptr);
		}
		if (loadRenderPass) {
			vkDestroyRenderPass(device, loadRenderPass, nullptr);
		}
		if (offscreenRenderPass) {
			vkDestroyRenderPass(device, offscreenRenderPass, nullptr);
		}
		if (commandPool) {
			vkDestroyCommandPool(device, commandPool, nullptr);
		}
		vkDestroyDevice(device, nullptr);
	}
	if (surface) {
		vkDestroySurfaceKHR(instance, surface, nullptr);
	}
	if (debugMessenger) {
		if (auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
		        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"))) {
			func(instance, debugMessenger, nullptr);
		}
	}
	if (instance) {
		vkDestroyInstance(instance, nullptr);
	}
}

const char* VulkanRenderer::name() const {
	return "Vulkan";
}

void VulkanRenderer::createInstance() {
	validationEnabled = false;
#ifndef NDEBUG
	validationEnabled = validationLayersSupported();
	if (!validationEnabled) {
		internal::debug("Vulkan validation layers requested but not available.");
	}
#endif

	VkApplicationInfo appInfo{ .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO };
	appInfo.pApplicationName = "jngl";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "jngl";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_1;

	// Instance extensions required by SDL to present to its window, plus debug utils.
	uint32_t sdlExtCount = 0;
	const char* const* const sdlExt = SDL_Vulkan_GetInstanceExtensions(&sdlExtCount);
	if (!sdlExt) {
		throw std::runtime_error(std::string("SDL_Vulkan_GetInstanceExtensions: ") + SDL_GetError());
	}
	std::vector<const char*> extensions(sdlExt, sdlExt + sdlExtCount);
	if (validationEnabled) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	VkInstanceCreateInfo createInfo{ .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugInfo{
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT
	};
	debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
	                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
	                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
	                        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
	                        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	debugInfo.pfnUserCallback = debugCallback;

	if (validationEnabled) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
		createInfo.pNext = &debugInfo; // also covers vkCreateInstance/vkDestroyInstance
	}

	VK_CHECK(vkCreateInstance(&createInfo, nullptr, &instance), "vkCreateInstance");

	if (validationEnabled) {
		if (auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
		        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"))) {
			func(instance, &debugInfo, nullptr, &debugMessenger);
		}
	}
}

void VulkanRenderer::createSurface() {
	if (!SDL_Vulkan_CreateSurface(window, instance, nullptr, &surface)) {
		throw std::runtime_error(std::string("SDL_Vulkan_CreateSurface: ") + SDL_GetError());
	}
}

void VulkanRenderer::pickPhysicalDevice() {
	uint32_t count = 0;
	vkEnumeratePhysicalDevices(instance, &count, nullptr);
	if (count == 0) {
		throw std::runtime_error("No Vulkan-capable GPU found.");
	}
	std::vector<VkPhysicalDevice> devices(count);
	vkEnumeratePhysicalDevices(instance, &count, devices.data());

	const auto findQueues = [this](VkPhysicalDevice dev, uint32_t& graphics,
	                                uint32_t& present) -> bool {
		uint32_t familyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(dev, &familyCount, nullptr);
		std::vector<VkQueueFamilyProperties> families(familyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(dev, &familyCount, families.data());
		bool hasGraphics = false;
		bool hasPresent = false;
		for (uint32_t i = 0; i < familyCount; ++i) {
			if (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				graphics = i;
				hasGraphics = true;
			}
			VkBool32 presentSupport = VK_FALSE;
			vkGetPhysicalDeviceSurfaceSupportKHR(dev, i, surface, &presentSupport);
			if (presentSupport) {
				present = i;
				hasPresent = true;
			}
			if (hasGraphics && hasPresent) {
				break;
			}
		}
		return hasGraphics && hasPresent;
	};

	// Prefer a discrete GPU, but accept anything (e.g. lavapipe) that can present.
	VkPhysicalDevice fallback = VK_NULL_HANDLE;
	uint32_t fbGraphics = 0;
	uint32_t fbPresent = 0;
	for (VkPhysicalDevice dev : devices) {
		uint32_t graphics = 0;
		uint32_t present = 0;
		if (!findQueues(dev, graphics, present)) {
			continue;
		}
		VkPhysicalDeviceProperties props;
		vkGetPhysicalDeviceProperties(dev, &props);
		if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			physicalDevice = dev;
			graphicsQueueFamily = graphics;
			presentQueueFamily = present;
			internal::debug("Vulkan: using {}", props.deviceName);
			return;
		}
		if (!fallback) {
			fallback = dev;
			fbGraphics = graphics;
			fbPresent = present;
		}
	}
	if (!fallback) {
		throw std::runtime_error("No Vulkan GPU can present to the window surface.");
	}
	physicalDevice = fallback;
	graphicsQueueFamily = fbGraphics;
	presentQueueFamily = fbPresent;
	VkPhysicalDeviceProperties props;
	vkGetPhysicalDeviceProperties(physicalDevice, &props);
	internal::debug("Vulkan: using {}", props.deviceName);
}

void VulkanRenderer::createLogicalDevice() {
	const std::set<uint32_t> uniqueFamilies = { graphicsQueueFamily, presentQueueFamily };
	std::vector<VkDeviceQueueCreateInfo> queueInfos;
	const float priority = 1.f;
	for (const uint32_t family : uniqueFamilies) {
		VkDeviceQueueCreateInfo info{ .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
		info.queueFamilyIndex = family;
		info.queueCount = 1;
		info.pQueuePriorities = &priority;
		queueInfos.push_back(info);
	}

	const std::array deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	VkPhysicalDeviceFeatures features{};

	VkDeviceCreateInfo createInfo{ .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
	createInfo.pQueueCreateInfos = queueInfos.data();
	createInfo.pEnabledFeatures = &features;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();
	if (validationEnabled) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}

	VK_CHECK(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device), "vkCreateDevice");
	vkGetDeviceQueue(device, graphicsQueueFamily, 0, &graphicsQueue);
	vkGetDeviceQueue(device, presentQueueFamily, 0, &presentQueue);
}

void VulkanRenderer::createSwapchain() {
	VkSurfaceCapabilitiesKHR caps;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &caps);

	// Surface format: prefer 8-bit BGRA sRGB, otherwise take whatever is offered.
	uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
	std::vector<VkSurfaceFormatKHR> formats(formatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats.data());
	// Prefer a plain (non-sRGB) 8-bit BGRA format so that colors written by the shaders reach the
	// screen unmodified, matching JNGL's OpenGL backend whose default framebuffer isn't sRGB-encoded.
	VkSurfaceFormatKHR chosenFormat = formats.front();
	for (const auto& f : formats) {
		if (f.format == VK_FORMAT_B8G8R8A8_UNORM &&
		    f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			chosenFormat = f;
			break;
		}
	}

	// Present mode: FIFO is always available and is v-synced.
	const VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;

	// Extent.
	if (caps.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		swapchainExtent = caps.currentExtent;
	} else {
		int w = 0;
		int h = 0;
		SDL_GetWindowSizeInPixels(window, &w, &h);
		swapchainExtent.width = std::clamp(static_cast<uint32_t>(w), caps.minImageExtent.width,
		                                   caps.maxImageExtent.width);
		swapchainExtent.height = std::clamp(static_cast<uint32_t>(h), caps.minImageExtent.height,
		                                    caps.maxImageExtent.height);
	}

	uint32_t imageCount = caps.minImageCount + 1;
	if (caps.maxImageCount > 0 && imageCount > caps.maxImageCount) {
		imageCount = caps.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{ .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = chosenFormat.format;
	createInfo.imageColorSpace = chosenFormat.colorSpace;
	createInfo.imageExtent = swapchainExtent;
	createInfo.imageArrayLayers = 1;
	// TRANSFER_SRC is needed so readPixels can copy the rendered image back to the CPU.
	createInfo.imageUsage =
	    VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

	const std::array<uint32_t, 2> families = { graphicsQueueFamily, presentQueueFamily };
	if (graphicsQueueFamily != presentQueueFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = families.data();
	} else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}
	createInfo.preTransform = caps.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	VK_CHECK(vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapchain),
	         "vkCreateSwapchainKHR");

	vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
	swapchainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapchainImages.data());
	swapchainImageFormat = chosenFormat.format;
}

void VulkanRenderer::createImageViews() {
	swapchainImageViews.resize(swapchainImages.size());
	for (size_t i = 0; i < swapchainImages.size(); ++i) {
		VkImageViewCreateInfo info{ .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		info.image = swapchainImages[i];
		info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		info.format = swapchainImageFormat;
		info.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
			                VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
		info.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		VK_CHECK(vkCreateImageView(device, &info, nullptr, &swapchainImageViews[i]),
		         "vkCreateImageView");
	}
}

void VulkanRenderer::createRenderPass() {
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapchainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorRef{ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorRef;

	// Use the same two dependencies as the offscreen render pass so that all three render passes
	// (swapchain CLEAR, swapchain LOAD, offscreen) are mutually compatible and the colored/textured
	// pipelines - created against `renderPass` - can be used inside any of them.
	std::array<VkSubpassDependency, 2> deps{};
	deps[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	deps[0].dstSubpass = 0;
	deps[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	deps[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
	deps[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	deps[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	deps[1].srcSubpass = 0;
	deps[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	deps[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	deps[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	deps[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	deps[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	VkRenderPassCreateInfo info{ .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
	info.attachmentCount = 1;
	info.pAttachments = &colorAttachment;
	info.subpassCount = 1;
	info.pSubpasses = &subpass;
	info.dependencyCount = static_cast<uint32_t>(deps.size());
	info.pDependencies = deps.data();
	VK_CHECK(vkCreateRenderPass(device, &info, nullptr, &renderPass), "vkCreateRenderPass");

	// A variant that loads (keeps) the existing contents instead of clearing, used to resume the
	// swapchain frame after readPixels or a FrameBuffer flushed it. The image is back in
	// PRESENT_SRC_KHR by then.
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	VK_CHECK(vkCreateRenderPass(device, &info, nullptr, &loadRenderPass), "vkCreateRenderPass");
}

void VulkanRenderer::createOffscreenRenderPass() {
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapchainImageFormat; // same format keeps the pipelines compatible
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkAttachmentReference colorRef{ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorRef;

	// Make sure sampling of the framebuffer (in a later draw) waits for our writes, and our writes
	// wait for any previous sampling.
	std::array<VkSubpassDependency, 2> deps{};
	deps[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	deps[0].dstSubpass = 0;
	deps[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	deps[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
	deps[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	deps[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	deps[1].srcSubpass = 0;
	deps[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	deps[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	deps[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	deps[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	deps[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	VkRenderPassCreateInfo info{ .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
	info.attachmentCount = 1;
	info.pAttachments = &colorAttachment;
	info.subpassCount = 1;
	info.pSubpasses = &subpass;
	info.dependencyCount = static_cast<uint32_t>(deps.size());
	info.pDependencies = deps.data();
	VK_CHECK(vkCreateRenderPass(device, &info, nullptr, &offscreenRenderPass),
	         "vkCreateRenderPass");
}

void VulkanRenderer::createFramebuffers() {
	swapchainFramebuffers.resize(swapchainImageViews.size());
	for (size_t i = 0; i < swapchainImageViews.size(); ++i) {
		const std::array attachments = { swapchainImageViews[i] };
		VkFramebufferCreateInfo info{ .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
		info.renderPass = renderPass;
		info.attachmentCount = static_cast<uint32_t>(attachments.size());
		info.pAttachments = attachments.data();
		info.width = swapchainExtent.width;
		info.height = swapchainExtent.height;
		info.layers = 1;
		VK_CHECK(vkCreateFramebuffer(device, &info, nullptr, &swapchainFramebuffers[i]),
		         "vkCreateFramebuffer");
	}
}

void VulkanRenderer::createCommandPoolAndBuffers() {
	VkCommandPoolCreateInfo poolInfo{ .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = graphicsQueueFamily;
	VK_CHECK(vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool),
	         "vkCreateCommandPool");

	commandBuffers.resize(maxFramesInFlight);
	VkCommandBufferAllocateInfo allocInfo{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO
	};
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
	VK_CHECK(vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()),
	         "vkAllocateCommandBuffers");
}

void VulkanRenderer::createSyncObjects() {
	imageAvailableSemaphores.resize(maxFramesInFlight);
	inFlightFences.resize(maxFramesInFlight);

	VkSemaphoreCreateInfo semInfo{ .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	VkFenceCreateInfo fenceInfo{ .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
		VK_CHECK(vkCreateSemaphore(device, &semInfo, nullptr, &imageAvailableSemaphores[i]),
		         "vkCreateSemaphore");
		VK_CHECK(vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]),
		         "vkCreateFence");
	}
	VkFenceCreateInfo unsignaled{ .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
	VK_CHECK(vkCreateFence(device, &unsignaled, nullptr, &readbackFence), "vkCreateFence");
}

void VulkanRenderer::createPresentSemaphores() {
	// The semaphore the present operation waits on must be one per swapchain image, not per
	// frame-in-flight: a swapchain image's present may still be reading the semaphore when the next
	// frame-in-flight reuses it. (See the Vulkan swapchain-semaphore-reuse guidance.)
	VkSemaphoreCreateInfo semInfo{ .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	renderFinishedSemaphores.resize(swapchainImages.size());
	for (auto& semaphore : renderFinishedSemaphores) {
		VK_CHECK(vkCreateSemaphore(device, &semInfo, nullptr, &semaphore), "vkCreateSemaphore");
	}
}

void VulkanRenderer::cleanupSwapchain() {
	if (!device) {
		return;
	}
	for (VkSemaphore semaphore : renderFinishedSemaphores) {
		vkDestroySemaphore(device, semaphore, nullptr);
	}
	renderFinishedSemaphores.clear();
	for (VkFramebuffer fb : swapchainFramebuffers) {
		vkDestroyFramebuffer(device, fb, nullptr);
	}
	swapchainFramebuffers.clear();
	for (VkImageView view : swapchainImageViews) {
		vkDestroyImageView(device, view, nullptr);
	}
	swapchainImageViews.clear();
	if (swapchain) {
		vkDestroySwapchainKHR(device, swapchain, nullptr);
		swapchain = VK_NULL_HANDLE;
	}
}

void VulkanRenderer::recreateSwapchain() {
	int w = 0;
	int h = 0;
	SDL_GetWindowSizeInPixels(window, &w, &h);
	while (w == 0 || h == 0) { // minimized: wait until the window has a size again
		SDL_GetWindowSizeInPixels(window, &w, &h);
		SDL_WaitEvent(nullptr);
	}
	vkDeviceWaitIdle(device);
	cleanupSwapchain();
	createSwapchain();
	createImageViews();
	createPresentSemaphores();
	createFramebuffers();
}

void VulkanRenderer::beginFrame(const Rgb clearColor) {
	vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE,
	                std::numeric_limits<uint64_t>::max());

	const VkResult acquire = vkAcquireNextImageKHR(
	    device, swapchain, std::numeric_limits<uint64_t>::max(),
	    imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &currentImageIndex);
	if (acquire == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapchain();
		frameActive = false;
		return;
	}
	if (acquire != VK_SUCCESS && acquire != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("vkAcquireNextImageKHR failed");
	}

	vkResetFences(device, 1, &inFlightFences[currentFrame]);

	vertexBuffers[currentFrame].used = 0;
	imageAvailableWaited = false;

	const VkCommandBuffer cmd = commandBuffers[currentFrame];
	vkResetCommandBuffer(cmd, 0);
	VkCommandBufferBeginInfo beginInfo{ .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	VK_CHECK(vkBeginCommandBuffer(cmd, &beginInfo), "vkBeginCommandBuffer");

	currentClearColor = clearColor;
	renderTargetStack.clear();
	frameActive = true;
	beginCurrentRenderPass(VK_ATTACHMENT_LOAD_OP_CLEAR);
}

void VulkanRenderer::beginCurrentRenderPass(const VkAttachmentLoadOp loadOp) {
	const VkCommandBuffer cmd = commandBuffers[currentFrame];
	VkRenderPassBeginInfo rpInfo{ .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
	VkExtent2D extent{};
	VkClearValue clear{};
	clear.color = { { currentClearColor.getRed(), currentClearColor.getGreen(),
		              currentClearColor.getBlue(), 1.f } };
	if (renderTargetStack.empty()) {
		rpInfo.renderPass = loadOp == VK_ATTACHMENT_LOAD_OP_CLEAR ? renderPass : loadRenderPass;
		rpInfo.framebuffer = swapchainFramebuffers[currentImageIndex];
		extent = swapchainExtent;
		if (loadOp == VK_ATTACHMENT_LOAD_OP_CLEAR) {
			rpInfo.clearValueCount = 1;
			rpInfo.pClearValues = &clear;
		}
	} else {
		const VulkanFramebuffer* target = renderTargetStack.back();
		rpInfo.renderPass = offscreenRenderPass; // always LOAD; clearing is explicit
		rpInfo.framebuffer = target->framebuffer;
		extent = { static_cast<uint32_t>(target->width), static_cast<uint32_t>(target->height) };
	}
	rpInfo.renderArea.extent = extent;
	vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

	// For the swapchain, flip the viewport vertically (negative height, core since Vulkan 1.1) so
	// that JNGL's projection matrix - written for OpenGL's clip space with +Y up - maps to the
	// screen the same way it does on the OpenGL backend. FrameBuffers are left un-flipped so their
	// image ends up "upside down" like an OpenGL FBO texture, which jngl::FrameBuffer::draw undoes
	// with a scale(1, -1).
	VkViewport viewport;
	if (renderTargetStack.empty()) {
		viewport = { 0.f, static_cast<float>(extent.height), static_cast<float>(extent.width),
			         -static_cast<float>(extent.height), 0.f, 1.f };
	} else {
		viewport = { 0.f, 0.f, static_cast<float>(extent.width), static_cast<float>(extent.height),
			         0.f, 1.f };
	}
	vkCmdSetViewport(cmd, 0, 1, &viewport);
	VkRect2D scissor{ { 0, 0 }, extent };
	vkCmdSetScissor(cmd, 0, 1, &scissor);
}

void VulkanRenderer::endFrame() {
	if (!frameActive) {
		return; // swapchain was recreated in beginFrame; skip this frame
	}
	const VkCommandBuffer cmd = commandBuffers[currentFrame];
	vkCmdEndRenderPass(cmd);
	VK_CHECK(vkEndCommandBuffer(cmd), "vkEndCommandBuffer");

	VkSubmitInfo submit{ .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO };
	const std::array<VkSemaphore, 1> waitSemaphores = { imageAvailableSemaphores[currentFrame] };
	const std::array<VkPipelineStageFlags, 1> waitStages = {
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
	};
	// If readPixels already flushed the frame it consumed the imageAvailable semaphore, so don't
	// wait on it again here.
	submit.waitSemaphoreCount = imageAvailableWaited ? 0 : 1;
	submit.pWaitSemaphores = waitSemaphores.data();
	submit.pWaitDstStageMask = waitStages.data();
	submit.commandBufferCount = 1;
	submit.pCommandBuffers = &cmd;
	const std::array<VkSemaphore, 1> signalSemaphores = {
		renderFinishedSemaphores[currentImageIndex]
	};
	submit.signalSemaphoreCount = 1;
	submit.pSignalSemaphores = signalSemaphores.data();

	VK_CHECK(vkQueueSubmit(graphicsQueue, 1, &submit, inFlightFences[currentFrame]),
	         "vkQueueSubmit");

	VkPresentInfoKHR present{ .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
	present.waitSemaphoreCount = 1;
	present.pWaitSemaphores = signalSemaphores.data();
	present.swapchainCount = 1;
	present.pSwapchains = &swapchain;
	present.pImageIndices = &currentImageIndex;

	const VkResult result = vkQueuePresentKHR(presentQueue, &present);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
		framebufferResized = false;
		recreateSwapchain();
	} else if (result != VK_SUCCESS) {
		throw std::runtime_error("vkQueuePresentKHR failed");
	}

	frameActive = false;
	currentFrame = (currentFrame + 1) % maxFramesInFlight;

	// Free trashed framebuffers whose frames have all been submitted and completed.
	for (auto it = framebufferTrash.begin(); it != framebufferTrash.end();) {
		if (--it->framesUntilFree <= 0) {
			freeFramebufferResources(it->fb);
			it = framebufferTrash.erase(it);
		} else {
			++it;
		}
	}
}

void VulkanRenderer::setViewport(int /*x*/, int /*y*/, int /*width*/, int /*height*/) {
	// The viewport is set per frame to the full swapchain extent in beginFrame(). JNGL's
	// letterboxing is applied through the projection matrix and (later) a scissor rectangle.
}

void VulkanRenderer::onResize(int /*width*/, int /*height*/) {
	framebufferResized = true;
}

void VulkanRenderer::setProjection(const Mat4& p) {
	projection = p;
}

namespace {

// Built-in shader drawing single-color 2D geometry. The transform and color come in as push
// constants so a single pipeline can serve every shape draw. Mirrors ShaderCache's "simple" GLSL
// shader, but combines modelview and projection into one matrix computed on the CPU.
constexpr const char* coloredVertexShader = R"(#version 450
layout(location = 0) in vec2 position;
layout(push_constant) uniform Push {
	mat4 mvp;
	vec4 color;
} pc;
void main() {
	gl_Position = pc.mvp * vec4(position, 0.0, 1.0);
})";

constexpr const char* coloredFragmentShader = R"(#version 450
layout(push_constant) uniform Push {
	mat4 mvp;
	vec4 color;
} pc;
layout(location = 0) out vec4 outColor;
void main() {
	outColor = pc.color;
})";

struct ColoredPushConstants {
	float mvp[16];
	float color[4];
};

VkShaderModule createShaderModule(VkDevice device, const std::vector<uint32_t>& spirv) {
	VkShaderModuleCreateInfo info{ .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
	info.codeSize = spirv.size() * sizeof(uint32_t);
	info.pCode = spirv.data();
	VkShaderModule module = VK_NULL_HANDLE;
	VK_CHECK(vkCreateShaderModule(device, &info, nullptr, &module), "vkCreateShaderModule");
	return module;
}

VkPrimitiveTopology toVkTopology(const PrimitiveType type) {
	switch (type) {
	case PrimitiveType::Triangles:
		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	case PrimitiveType::TriangleStrip:
		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
	case PrimitiveType::TriangleFan:
		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
	case PrimitiveType::Lines:
		return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
	}
	return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
}

} // namespace

uint32_t VulkanRenderer::findMemoryType(const uint32_t typeFilter,
                                        const VkMemoryPropertyFlags properties) const {
	VkPhysicalDeviceMemoryProperties memProps;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProps);
	for (uint32_t i = 0; i < memProps.memoryTypeCount; ++i) {
		if ((typeFilter & (1U << i)) != 0 &&
		    (memProps.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}
	throw std::runtime_error("No suitable Vulkan memory type found.");
}

void VulkanRenderer::createColoredPipelines() {
	const VkShaderModule vert = createShaderModule(
	    device, compileGlslToSpirv(coloredVertexShader, VK_SHADER_STAGE_VERTEX_BIT, "colored.vert"));
	const VkShaderModule frag = createShaderModule(
	    device,
	    compileGlslToSpirv(coloredFragmentShader, VK_SHADER_STAGE_FRAGMENT_BIT, "colored.frag"));

	VkPushConstantRange pushRange{};
	pushRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushRange.offset = 0;
	pushRange.size = sizeof(ColoredPushConstants);

	VkPipelineLayoutCreateInfo layoutInfo{ .sType =
		                                       VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
	layoutInfo.pushConstantRangeCount = 1;
	layoutInfo.pPushConstantRanges = &pushRange;
	VK_CHECK(vkCreatePipelineLayout(device, &layoutInfo, nullptr, &coloredPipelineLayout),
	         "vkCreatePipelineLayout");

	const std::array<VkPipelineShaderStageCreateInfo, 2> stages = {
		VkPipelineShaderStageCreateInfo{
		    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		    .stage = VK_SHADER_STAGE_VERTEX_BIT,
		    .module = vert,
		    .pName = "main" },
		VkPipelineShaderStageCreateInfo{
		    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		    .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
		    .module = frag,
		    .pName = "main" },
	};

	VkVertexInputBindingDescription binding{};
	binding.binding = 0;
	binding.stride = 2 * sizeof(float);
	binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	VkVertexInputAttributeDescription attribute{};
	attribute.location = 0;
	attribute.binding = 0;
	attribute.format = VK_FORMAT_R32G32_SFLOAT;
	attribute.offset = 0;
	VkPipelineVertexInputStateCreateInfo vertexInput{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO
	};
	vertexInput.vertexBindingDescriptionCount = 1;
	vertexInput.pVertexBindingDescriptions = &binding;
	vertexInput.vertexAttributeDescriptionCount = 1;
	vertexInput.pVertexAttributeDescriptions = &attribute;

	VkPipelineViewportStateCreateInfo viewportState{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO
	};
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rasterizer{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO
	};
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.cullMode = VK_CULL_MODE_NONE;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.lineWidth = 1.f;

	VkPipelineMultisampleStateCreateInfo multisampling{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO
	};
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	// Standard alpha blending, matching the OpenGL backend's glBlendFunc(GL_SRC_ALPHA,
	// GL_ONE_MINUS_SRC_ALPHA).
	VkPipelineColorBlendAttachmentState blendAttachment{};
	blendAttachment.blendEnable = VK_TRUE;
	blendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	blendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	blendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	blendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	blendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	blendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
	blendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
	                                 VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	VkPipelineColorBlendStateCreateInfo colorBlending{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO
	};
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &blendAttachment;

	const std::array<VkDynamicState, 2> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT,
		                                                  VK_DYNAMIC_STATE_SCISSOR };
	VkPipelineDynamicStateCreateInfo dynamicState{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO
	};
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	const std::array<PrimitiveType, 4> topologies = { PrimitiveType::Triangles,
		                                              PrimitiveType::TriangleStrip,
		                                              PrimitiveType::TriangleFan,
		                                              PrimitiveType::Lines };
	for (const PrimitiveType type : topologies) {
		VkPipelineInputAssemblyStateCreateInfo inputAssembly{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO
		};
		inputAssembly.topology = toVkTopology(type);

		VkGraphicsPipelineCreateInfo pipelineInfo{
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO
		};
		pipelineInfo.stageCount = static_cast<uint32_t>(stages.size());
		pipelineInfo.pStages = stages.data();
		pipelineInfo.pVertexInputState = &vertexInput;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = coloredPipelineLayout;
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = 0;
		VK_CHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr,
		                                   &coloredPipelines[static_cast<size_t>(type)]),
		         "vkCreateGraphicsPipelines");
	}

	vkDestroyShaderModule(device, frag, nullptr);
	vkDestroyShaderModule(device, vert, nullptr);
}

void VulkanRenderer::createVertexBuffers() {
	// 1 MiB per frame is plenty for JNGL's 2D shape geometry; drawColored reports (once) if a frame
	// ever needs more.
	constexpr VkDeviceSize capacity = 1u << 20u;
	vertexBuffers.resize(maxFramesInFlight);
	for (auto& vb : vertexBuffers) {
		VkBufferCreateInfo bufferInfo{ .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bufferInfo.size = capacity;
		bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		VK_CHECK(vkCreateBuffer(device, &bufferInfo, nullptr, &vb.buffer), "vkCreateBuffer");

		VkMemoryRequirements memReq;
		vkGetBufferMemoryRequirements(device, vb.buffer, &memReq);
		VkMemoryAllocateInfo allocInfo{ .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
		allocInfo.allocationSize = memReq.size;
		allocInfo.memoryTypeIndex =
		    findMemoryType(memReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		                                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		VK_CHECK(vkAllocateMemory(device, &allocInfo, nullptr, &vb.memory), "vkAllocateMemory");
		VK_CHECK(vkBindBufferMemory(device, vb.buffer, vb.memory, 0), "vkBindBufferMemory");
		VK_CHECK(vkMapMemory(device, vb.memory, 0, capacity, 0, &vb.mapped), "vkMapMemory");
		vb.capacity = capacity;
	}
}

void VulkanRenderer::drawColored(const PrimitiveType type, const float* const xyVertices,
                                 const std::size_t vertexCount, const Mat3& modelview,
                                 const Rgba color) {
	if (!frameActive || vertexCount == 0) {
		return;
	}
	DynamicVertexBuffer& vb = vertexBuffers[currentFrame];
	const VkDeviceSize bytes = vertexCount * 2 * sizeof(float);
	if (vb.used + bytes > vb.capacity) {
		if (!vertexBufferOverflowReported) {
			internal::error("Vulkan per-frame vertex buffer exhausted; some shapes won't be drawn.");
			vertexBufferOverflowReported = true;
		}
		return;
	}
	const VkDeviceSize offset = vb.used;
	std::memcpy(static_cast<char*>(vb.mapped) + offset, xyVertices, bytes);
	// Keep the next vertex block 4-byte aligned (it already is, but stay explicit).
	vb.used += (bytes + 3) & ~VkDeviceSize{ 3 };

	// Combine modelview (3x3) and the projection matrix into a single 4x4 transform on the CPU. The
	// modelview maps (x, y) -> (x', y'); embed it into a 4x4 that leaves z at 0 and w at 1.
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

	ColoredPushConstants push{};
	// mvp = projection * embedded, both column-major (element [col * 4 + row]).
	for (int col = 0; col < 4; ++col) {
		for (int row = 0; row < 4; ++row) {
			float sum = 0.f;
			for (int k = 0; k < 4; ++k) {
				sum += projection.data[k * 4 + row] * embedded.data[col * 4 + k];
			}
			push.mvp[col * 4 + row] = sum;
		}
	}
	push.color[0] = color.getRed();
	push.color[1] = color.getGreen();
	push.color[2] = color.getBlue();
	push.color[3] = color.getAlpha();

	const VkCommandBuffer cmd = commandBuffers[currentFrame];
	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
	                  coloredPipelines[static_cast<size_t>(type)]);
	vkCmdBindVertexBuffers(cmd, 0, 1, &vb.buffer, &offset);
	vkCmdPushConstants(cmd, coloredPipelineLayout,
	                   VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(push),
	                   &push);
	vkCmdDraw(cmd, static_cast<uint32_t>(vertexCount), 1, 0, 0);
}

namespace {

// Texture shader: samples a texture and modulates by the push-constant color. Mirrors ShaderCache's
// GLSL texture shader. Vertices carry position (xy) and texture coordinates (uv).
constexpr const char* texturedVertexShader = R"(#version 450
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 inTexCoord;
layout(push_constant) uniform Push {
	mat4 mvp;
	vec4 color;
} pc;
layout(location = 0) out vec2 texCoord;
void main() {
	gl_Position = pc.mvp * vec4(position, 0.0, 1.0);
	texCoord = inTexCoord;
})";

constexpr const char* texturedFragmentShader = R"(#version 450
layout(set = 0, binding = 0) uniform sampler2D tex;
layout(push_constant) uniform Push {
	mat4 mvp;
	vec4 color;
} pc;
layout(location = 0) in vec2 texCoord;
layout(location = 0) out vec4 outColor;
void main() {
	outColor = texture(tex, texCoord) * pc.color;
})";

// OpenGL pixel-format enums JNGL passes to Texture/createTexture (avoids including GL headers here).
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

} // namespace

void VulkanRenderer::createDescriptorPool() {
	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSize.descriptorCount = 1024;
	VkDescriptorPoolCreateInfo info{ .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
	info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	info.maxSets = 1024;
	info.poolSizeCount = 1;
	info.pPoolSizes = &poolSize;
	VK_CHECK(vkCreateDescriptorPool(device, &info, nullptr, &descriptorPool),
	         "vkCreateDescriptorPool");
}

void VulkanRenderer::createTexturedPipelines() {
	VkDescriptorSetLayoutBinding samplerBinding{};
	samplerBinding.binding = 0;
	samplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerBinding.descriptorCount = 1;
	samplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	VkDescriptorSetLayoutCreateInfo setLayoutInfo{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO
	};
	setLayoutInfo.bindingCount = 1;
	setLayoutInfo.pBindings = &samplerBinding;
	VK_CHECK(vkCreateDescriptorSetLayout(device, &setLayoutInfo, nullptr, &texturedSetLayout),
	         "vkCreateDescriptorSetLayout");

	const VkShaderModule vert = createShaderModule(
	    device,
	    compileGlslToSpirv(texturedVertexShader, VK_SHADER_STAGE_VERTEX_BIT, "textured.vert"));
	const VkShaderModule frag = createShaderModule(
	    device,
	    compileGlslToSpirv(texturedFragmentShader, VK_SHADER_STAGE_FRAGMENT_BIT, "textured.frag"));

	VkPushConstantRange pushRange{};
	pushRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushRange.size = sizeof(ColoredPushConstants);
	VkPipelineLayoutCreateInfo layoutInfo{ .sType =
		                                       VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
	layoutInfo.setLayoutCount = 1;
	layoutInfo.pSetLayouts = &texturedSetLayout;
	layoutInfo.pushConstantRangeCount = 1;
	layoutInfo.pPushConstantRanges = &pushRange;
	VK_CHECK(vkCreatePipelineLayout(device, &layoutInfo, nullptr, &texturedPipelineLayout),
	         "vkCreatePipelineLayout");

	buildTexturedPipelines(vert, frag, texturedPipelines);

	vkDestroyShaderModule(device, frag, nullptr);
	vkDestroyShaderModule(device, vert, nullptr);
}

void VulkanRenderer::buildTexturedPipelines(const VkShaderModule vert, const VkShaderModule frag,
                                            std::array<VkPipeline, 4>& out) {
	const std::array<VkPipelineShaderStageCreateInfo, 2> stages = {
		VkPipelineShaderStageCreateInfo{
		    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		    .stage = VK_SHADER_STAGE_VERTEX_BIT,
		    .module = vert,
		    .pName = "main" },
		VkPipelineShaderStageCreateInfo{
		    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		    .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
		    .module = frag,
		    .pName = "main" },
	};

	VkVertexInputBindingDescription binding{};
	binding.binding = 0;
	binding.stride = 4 * sizeof(float); // x, y, u, v
	binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	const std::array<VkVertexInputAttributeDescription, 2> attributes = {
		VkVertexInputAttributeDescription{ 0, 0, VK_FORMAT_R32G32_SFLOAT, 0 },
		VkVertexInputAttributeDescription{ 1, 0, VK_FORMAT_R32G32_SFLOAT, 2 * sizeof(float) },
	};
	VkPipelineVertexInputStateCreateInfo vertexInput{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO
	};
	vertexInput.vertexBindingDescriptionCount = 1;
	vertexInput.pVertexBindingDescriptions = &binding;
	vertexInput.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size());
	vertexInput.pVertexAttributeDescriptions = attributes.data();

	VkPipelineViewportStateCreateInfo viewportState{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO
	};
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rasterizer{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO
	};
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.cullMode = VK_CULL_MODE_NONE;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.lineWidth = 1.f;

	VkPipelineMultisampleStateCreateInfo multisampling{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO
	};
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState blendAttachment{};
	blendAttachment.blendEnable = VK_TRUE;
	blendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	blendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	blendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	blendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	blendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	blendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
	blendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
	                                 VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	VkPipelineColorBlendStateCreateInfo colorBlending{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO
	};
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &blendAttachment;

	const std::array<VkDynamicState, 2> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT,
		                                                  VK_DYNAMIC_STATE_SCISSOR };
	VkPipelineDynamicStateCreateInfo dynamicState{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO
	};
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	const std::array<PrimitiveType, 4> topologies = { PrimitiveType::Triangles,
		                                              PrimitiveType::TriangleStrip,
		                                              PrimitiveType::TriangleFan,
		                                              PrimitiveType::Lines };
	for (const PrimitiveType type : topologies) {
		VkPipelineInputAssemblyStateCreateInfo inputAssembly{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO
		};
		inputAssembly.topology = toVkTopology(type);

		VkGraphicsPipelineCreateInfo pipelineInfo{
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO
		};
		pipelineInfo.stageCount = static_cast<uint32_t>(stages.size());
		pipelineInfo.pStages = stages.data();
		pipelineInfo.pVertexInputState = &vertexInput;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = texturedPipelineLayout;
		pipelineInfo.renderPass = renderPass;
		VK_CHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr,
		                                   &out[static_cast<size_t>(type)]),
		         "vkCreateGraphicsPipelines");
	}
}

std::unique_ptr<VulkanShaderProgram> VulkanRenderer::createShaderProgram(
    const std::vector<uint32_t>& vertexSpirv, const std::vector<uint32_t>& fragmentSpirv) {
	auto program = std::make_unique<VulkanShaderProgram>();
	const VkShaderModule vert = createShaderModule(device, vertexSpirv);
	const VkShaderModule frag = createShaderModule(device, fragmentSpirv);
	buildTexturedPipelines(vert, frag, program->pipelines);
	vkDestroyShaderModule(device, frag, nullptr);
	vkDestroyShaderModule(device, vert, nullptr);
	return program;
}

void VulkanRenderer::destroyShaderProgram(VulkanShaderProgram& program) {
	if (!device) {
		return;
	}
	vkDeviceWaitIdle(device);
	for (VkPipeline pipeline : program.pipelines) {
		if (pipeline) {
			vkDestroyPipeline(device, pipeline, nullptr);
		}
	}
}

void VulkanRenderer::setActiveShaderProgram(const VulkanShaderProgram* program) {
	activeShaderProgram = program;
}

void VulkanRenderer::submitOneTime(const std::function<void(VkCommandBuffer)>& record) {
	VkCommandBufferAllocateInfo allocInfo{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO
	};
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;
	VkCommandBuffer cmd = VK_NULL_HANDLE;
	VK_CHECK(vkAllocateCommandBuffers(device, &allocInfo, &cmd), "vkAllocateCommandBuffers");

	VkCommandBufferBeginInfo beginInfo{ .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	VK_CHECK(vkBeginCommandBuffer(cmd, &beginInfo), "vkBeginCommandBuffer");
	record(cmd);
	VK_CHECK(vkEndCommandBuffer(cmd), "vkEndCommandBuffer");

	VkSubmitInfo submit{ .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO };
	submit.commandBufferCount = 1;
	submit.pCommandBuffers = &cmd;
	VK_CHECK(vkQueueSubmit(graphicsQueue, 1, &submit, VK_NULL_HANDLE), "vkQueueSubmit");
	vkQueueWaitIdle(graphicsQueue);
	vkFreeCommandBuffers(device, commandPool, 1, &cmd);
}

std::unique_ptr<VulkanTexture> VulkanRenderer::createTexture(const int width, const int height,
                                                             const unsigned int format,
                                                             const unsigned int type,
                                                             const unsigned char* const data,
                                                             const unsigned char* const* const
                                                                 rowPointers) {
	if (type != glUnsignedByte || (format != glRGBA && format != glRGB && format != glBGR)) {
		internal::error("Vulkan backend: unsupported texture format/type (0x{:x}/0x{:x}).", format,
		                type);
		return nullptr;
	}

	auto texture = std::make_unique<VulkanTexture>();
	texture->width = width;
	texture->height = height;

	VkImageCreateInfo imageInfo{ .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	imageInfo.extent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1 };
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	VK_CHECK(vkCreateImage(device, &imageInfo, nullptr, &texture->image), "vkCreateImage");

	VkMemoryRequirements memReq;
	vkGetImageMemoryRequirements(device, texture->image, &memReq);
	VkMemoryAllocateInfo allocInfo{ .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
	allocInfo.allocationSize = memReq.size;
	allocInfo.memoryTypeIndex =
	    findMemoryType(memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	VK_CHECK(vkAllocateMemory(device, &allocInfo, nullptr, &texture->memory), "vkAllocateMemory");
	VK_CHECK(vkBindImageMemory(device, texture->image, texture->memory, 0), "vkBindImageMemory");

	if (data || rowPointers) {
		updateTextureBytes(*texture, width, height,
		                   toRgba8(width, height, format, data, rowPointers).data());
	} else {
		// No initial pixels (e.g. a render target): just move it to the shader-read layout.
		submitOneTime([&](VkCommandBuffer cmd) {
			VkImageMemoryBarrier barrier{ .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
			barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.image = texture->image;
			barrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			                     VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1,
			                     &barrier);
		});
	}

	VkImageViewCreateInfo viewInfo{ .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	viewInfo.image = texture->image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	viewInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	VK_CHECK(vkCreateImageView(device, &viewInfo, nullptr, &texture->view), "vkCreateImageView");

	const VkFilter filter = App::isPixelArt() ? VK_FILTER_NEAREST : VK_FILTER_LINEAR;
	VkSamplerCreateInfo samplerInfo{ .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
	samplerInfo.magFilter = filter;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	VK_CHECK(vkCreateSampler(device, &samplerInfo, nullptr, &texture->sampler), "vkCreateSampler");

	VkDescriptorSetAllocateInfo dsAlloc{ .sType =
		                                     VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
	dsAlloc.descriptorPool = descriptorPool;
	dsAlloc.descriptorSetCount = 1;
	dsAlloc.pSetLayouts = &texturedSetLayout;
	VK_CHECK(vkAllocateDescriptorSets(device, &dsAlloc, &texture->descriptorSet),
	         "vkAllocateDescriptorSets");

	VkDescriptorImageInfo imgInfo{};
	imgInfo.sampler = texture->sampler;
	imgInfo.imageView = texture->view;
	imgInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	VkWriteDescriptorSet write{ .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
	write.dstSet = texture->descriptorSet;
	write.dstBinding = 0;
	write.descriptorCount = 1;
	write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	write.pImageInfo = &imgInfo;
	vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);

	return texture;
}

void VulkanRenderer::updateTextureBytes(VulkanTexture& texture, const int width, const int height,
                                        const unsigned char* const rgba) {
	const VkDeviceSize size = static_cast<VkDeviceSize>(width) * height * 4;

	VkBufferCreateInfo bufferInfo{ .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	bufferInfo.size = size;
	bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	VkBuffer staging = VK_NULL_HANDLE;
	VK_CHECK(vkCreateBuffer(device, &bufferInfo, nullptr, &staging), "vkCreateBuffer");
	VkMemoryRequirements memReq;
	vkGetBufferMemoryRequirements(device, staging, &memReq);
	VkMemoryAllocateInfo allocInfo{ .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
	allocInfo.allocationSize = memReq.size;
	allocInfo.memoryTypeIndex = findMemoryType(
	    memReq.memoryTypeBits,
	    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	VkDeviceMemory stagingMemory = VK_NULL_HANDLE;
	VK_CHECK(vkAllocateMemory(device, &allocInfo, nullptr, &stagingMemory), "vkAllocateMemory");
	VK_CHECK(vkBindBufferMemory(device, staging, stagingMemory, 0), "vkBindBufferMemory");
	void* mapped = nullptr;
	VK_CHECK(vkMapMemory(device, stagingMemory, 0, size, 0, &mapped), "vkMapMemory");
	std::memcpy(mapped, rgba, size);
	vkUnmapMemory(device, stagingMemory);

	submitOneTime([&](VkCommandBuffer cmd) {
		VkImageMemoryBarrier toTransfer{ .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		toTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		toTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		toTransfer.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		toTransfer.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		toTransfer.image = texture.image;
		toTransfer.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		toTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
		                     0, 0, nullptr, 0, nullptr, 1, &toTransfer);

		VkBufferImageCopy region{};
		region.imageSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
		region.imageExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1 };
		vkCmdCopyBufferToImage(cmd, staging, texture.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
		                       &region);

		VkImageMemoryBarrier toShader{ .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		toShader.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		toShader.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		toShader.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		toShader.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		toShader.image = texture.image;
		toShader.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		toShader.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		toShader.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT,
		                     VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1,
		                     &toShader);
	});

	vkDestroyBuffer(device, staging, nullptr);
	vkFreeMemory(device, stagingMemory, nullptr);
}

void VulkanRenderer::destroyTexture(VulkanTexture& texture) {
	if (!device) {
		return;
	}
	vkDeviceWaitIdle(device);
	if (texture.descriptorSet) {
		vkFreeDescriptorSets(device, descriptorPool, 1, &texture.descriptorSet);
	}
	if (texture.sampler) {
		vkDestroySampler(device, texture.sampler, nullptr);
	}
	if (texture.view) {
		vkDestroyImageView(device, texture.view, nullptr);
	}
	if (texture.image) {
		vkDestroyImage(device, texture.image, nullptr);
	}
	if (texture.memory) {
		vkFreeMemory(device, texture.memory, nullptr);
	}
}

void VulkanRenderer::drawSprite(const VulkanTexture& texture, const float* const xyuv,
                                const std::size_t vertexCount, const PrimitiveType type,
                                const Mat3& modelview, const Rgba color) {
	if (!frameActive || vertexCount == 0) {
		return;
	}
	DynamicVertexBuffer& vb = vertexBuffers[currentFrame];
	const VkDeviceSize bytes = vertexCount * 4 * sizeof(float);
	if (vb.used + bytes > vb.capacity) {
		if (!vertexBufferOverflowReported) {
			internal::error("Vulkan per-frame vertex buffer exhausted; some sprites won't be drawn.");
			vertexBufferOverflowReported = true;
		}
		return;
	}
	const VkDeviceSize offset = vb.used;
	std::memcpy(static_cast<char*>(vb.mapped) + offset, xyuv, bytes);
	vb.used += (bytes + 3) & ~VkDeviceSize{ 3 };

	ColoredPushConstants push{};
	const float* const m = modelview.data;
	Mat4 embedded;
	embedded.data[0] = m[0];
	embedded.data[1] = m[1];
	embedded.data[4] = m[3];
	embedded.data[5] = m[4];
	embedded.data[12] = m[6];
	embedded.data[13] = m[7];
	for (int col = 0; col < 4; ++col) {
		for (int row = 0; row < 4; ++row) {
			float sum = 0.f;
			for (int k = 0; k < 4; ++k) {
				sum += projection.data[k * 4 + row] * embedded.data[col * 4 + k];
			}
			push.mvp[col * 4 + row] = sum;
		}
	}
	push.color[0] = color.getRed();
	push.color[1] = color.getGreen();
	push.color[2] = color.getBlue();
	push.color[3] = color.getAlpha();

	const VkCommandBuffer cmd = commandBuffers[currentFrame];
	const VkPipeline pipeline = activeShaderProgram
	                                ? activeShaderProgram->pipelines[static_cast<size_t>(type)]
	                                : texturedPipelines[static_cast<size_t>(type)];
	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, texturedPipelineLayout, 0, 1,
	                        &texture.descriptorSet, 0, nullptr);
	vkCmdBindVertexBuffers(cmd, 0, 1, &vb.buffer, &offset);
	vkCmdPushConstants(cmd, texturedPipelineLayout,
	                   VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(push),
	                   &push);
	vkCmdDraw(cmd, static_cast<uint32_t>(vertexCount), 1, 0, 0);
}

void VulkanRenderer::readPixels(unsigned char* const rgb, const int x, const int y, const int width,
                                const int height) {
	const size_t pixelCount = static_cast<size_t>(width) * height;
	if (!frameActive) {
		std::memset(rgb, 0, pixelCount * 3);
		return;
	}

	// Flush the draws recorded so far: end the render pass and submit, so the swapchain image
	// actually holds the rendered content we're about to read back.
	const VkCommandBuffer cmd = commandBuffers[currentFrame];
	vkCmdEndRenderPass(cmd);
	VK_CHECK(vkEndCommandBuffer(cmd), "vkEndCommandBuffer");

	const VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSubmitInfo submit{ .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO };
	if (!imageAvailableWaited) {
		submit.waitSemaphoreCount = 1;
		submit.pWaitSemaphores = &imageAvailableSemaphores[currentFrame];
		submit.pWaitDstStageMask = &waitStage;
	}
	submit.commandBufferCount = 1;
	submit.pCommandBuffers = &cmd;
	vkResetFences(device, 1, &readbackFence);
	VK_CHECK(vkQueueSubmit(graphicsQueue, 1, &submit, readbackFence), "vkQueueSubmit");
	vkWaitForFences(device, 1, &readbackFence, VK_TRUE, std::numeric_limits<uint64_t>::max());
	imageAvailableWaited = true;

	// Copy the requested region into a host-visible buffer.
	const VkDeviceSize size = pixelCount * 4;
	VkBufferCreateInfo bufferInfo{ .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	bufferInfo.size = size;
	bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	VkBuffer staging = VK_NULL_HANDLE;
	VK_CHECK(vkCreateBuffer(device, &bufferInfo, nullptr, &staging), "vkCreateBuffer");
	VkMemoryRequirements memReq;
	vkGetBufferMemoryRequirements(device, staging, &memReq);
	VkMemoryAllocateInfo allocInfo{ .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
	allocInfo.allocationSize = memReq.size;
	allocInfo.memoryTypeIndex = findMemoryType(
	    memReq.memoryTypeBits,
	    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	VkDeviceMemory stagingMemory = VK_NULL_HANDLE;
	VK_CHECK(vkAllocateMemory(device, &allocInfo, nullptr, &stagingMemory), "vkAllocateMemory");
	VK_CHECK(vkBindBufferMemory(device, staging, stagingMemory, 0), "vkBindBufferMemory");

	VkImage image = swapchainImages[currentImageIndex];
	submitOneTime([&](VkCommandBuffer c) {
		VkImageMemoryBarrier toSrc{ .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		toSrc.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		toSrc.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		toSrc.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		toSrc.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		toSrc.image = image;
		toSrc.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		toSrc.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		vkCmdPipelineBarrier(c, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
		                     0, nullptr, 0, nullptr, 1, &toSrc);

		VkBufferImageCopy region{};
		region.imageSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
		region.imageOffset = { x, y, 0 };
		region.imageExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1 };
		vkCmdCopyImageToBuffer(c, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, staging, 1, &region);

		// Move the image back to PRESENT_SRC_KHR, which is what the resumed loadRenderPass expects
		// as its initial layout.
		VkImageMemoryBarrier toPresent{ .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		toPresent.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		toPresent.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		toPresent.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		toPresent.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		toPresent.image = image;
		toPresent.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		toPresent.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		vkCmdPipelineBarrier(c, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
		                     0, 0, nullptr, 0, nullptr, 1, &toPresent);
	});

	void* mapped = nullptr;
	VK_CHECK(vkMapMemory(device, stagingMemory, 0, size, 0, &mapped), "vkMapMemory");
	const auto* src = static_cast<const unsigned char*>(mapped);
	// Output RGB, bottom row first (OpenGL convention) and swap B/R (swapchain is BGRA).
	for (int row = 0; row < height; ++row) {
		const unsigned char* srcRow =
		    src + static_cast<size_t>(height - 1 - row) * width * 4;
		unsigned char* dstRow = rgb + static_cast<size_t>(row) * width * 3;
		for (int col = 0; col < width; ++col) {
			dstRow[col * 3 + 0] = srcRow[col * 4 + 2];
			dstRow[col * 3 + 1] = srcRow[col * 4 + 1];
			dstRow[col * 3 + 2] = srcRow[col * 4 + 0];
		}
	}
	vkUnmapMemory(device, stagingMemory);
	vkDestroyBuffer(device, staging, nullptr);
	vkFreeMemory(device, stagingMemory, nullptr);

	// Resume the frame: begin a fresh command buffer and a LOAD render pass that keeps the pixels
	// we just read, so subsequent draws (and endFrame's present) continue seamlessly.
	vkResetCommandBuffer(cmd, 0);
	VkCommandBufferBeginInfo beginInfo{ .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	VK_CHECK(vkBeginCommandBuffer(cmd, &beginInfo), "vkBeginCommandBuffer");
	beginCurrentRenderPass(VK_ATTACHMENT_LOAD_OP_LOAD);
}

VkDescriptorSet VulkanRenderer::allocateTextureDescriptor(const VkImageView view,
                                                          const VkSampler sampler) {
	VkDescriptorSetAllocateInfo dsAlloc{ .sType =
		                                     VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
	dsAlloc.descriptorPool = descriptorPool;
	dsAlloc.descriptorSetCount = 1;
	dsAlloc.pSetLayouts = &texturedSetLayout;
	VkDescriptorSet set = VK_NULL_HANDLE;
	VK_CHECK(vkAllocateDescriptorSets(device, &dsAlloc, &set), "vkAllocateDescriptorSets");

	VkDescriptorImageInfo imgInfo{};
	imgInfo.sampler = sampler;
	imgInfo.imageView = view;
	imgInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	VkWriteDescriptorSet write{ .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
	write.dstSet = set;
	write.dstBinding = 0;
	write.descriptorCount = 1;
	write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	write.pImageInfo = &imgInfo;
	vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
	return set;
}

std::unique_ptr<VulkanFramebuffer> VulkanRenderer::createFramebuffer(const int width,
                                                                     const int height) {
	auto fb = std::make_unique<VulkanFramebuffer>();
	fb->width = width;
	fb->height = height;
	fb->color.width = width;
	fb->color.height = height;

	VkImageCreateInfo imageInfo{ .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.format = swapchainImageFormat;
	imageInfo.extent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1 };
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT |
	                  VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	VK_CHECK(vkCreateImage(device, &imageInfo, nullptr, &fb->color.image), "vkCreateImage");

	VkMemoryRequirements memReq;
	vkGetImageMemoryRequirements(device, fb->color.image, &memReq);
	VkMemoryAllocateInfo allocInfo{ .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
	allocInfo.allocationSize = memReq.size;
	allocInfo.memoryTypeIndex =
	    findMemoryType(memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	VK_CHECK(vkAllocateMemory(device, &allocInfo, nullptr, &fb->color.memory), "vkAllocateMemory");
	VK_CHECK(vkBindImageMemory(device, fb->color.image, fb->color.memory, 0), "vkBindImageMemory");

	// Clear to transparent and move to SHADER_READ_ONLY (the offscreen render pass's initial
	// layout, and the layout it's sampled from).
	submitOneTime([&](VkCommandBuffer c) {
		VkImageMemoryBarrier toDst{ .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		toDst.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		toDst.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		toDst.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		toDst.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		toDst.image = fb->color.image;
		toDst.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		toDst.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		vkCmdPipelineBarrier(c, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
		                     0, nullptr, 0, nullptr, 1, &toDst);
		VkClearColorValue clearColor{ { 1.f, 1.f, 1.f, 0.f } };
		VkImageSubresourceRange range{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		vkCmdClearColorImage(c, fb->color.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearColor, 1,
		                     &range);
		VkImageMemoryBarrier toRead{ .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		toRead.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		toRead.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		toRead.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		toRead.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		toRead.image = fb->color.image;
		toRead.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		toRead.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		toRead.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		vkCmdPipelineBarrier(c, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		                     0, 0, nullptr, 0, nullptr, 1, &toRead);
	});

	VkImageViewCreateInfo viewInfo{ .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	viewInfo.image = fb->color.image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = swapchainImageFormat;
	viewInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	VK_CHECK(vkCreateImageView(device, &viewInfo, nullptr, &fb->color.view), "vkCreateImageView");

	const VkFilter filter = App::isPixelArt() ? VK_FILTER_NEAREST : VK_FILTER_LINEAR;
	VkSamplerCreateInfo samplerInfo{ .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
	samplerInfo.magFilter = filter;
	samplerInfo.minFilter = filter;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	VK_CHECK(vkCreateSampler(device, &samplerInfo, nullptr, &fb->color.sampler), "vkCreateSampler");

	fb->color.descriptorSet = allocateTextureDescriptor(fb->color.view, fb->color.sampler);

	const std::array<VkImageView, 1> attachments = { fb->color.view };
	VkFramebufferCreateInfo fbInfo{ .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
	fbInfo.renderPass = offscreenRenderPass;
	fbInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	fbInfo.pAttachments = attachments.data();
	fbInfo.width = static_cast<uint32_t>(width);
	fbInfo.height = static_cast<uint32_t>(height);
	fbInfo.layers = 1;
	VK_CHECK(vkCreateFramebuffer(device, &fbInfo, nullptr, &fb->framebuffer),
	         "vkCreateFramebuffer");
	return fb;
}

void VulkanRenderer::freeFramebufferResources(VulkanFramebuffer& fb) {
	if (fb.framebuffer) {
		vkDestroyFramebuffer(device, fb.framebuffer, nullptr);
		fb.framebuffer = VK_NULL_HANDLE;
	}
	destroyTexture(fb.color);
}

void VulkanRenderer::destroyFramebuffer(VulkanFramebuffer& fb) {
	if (!device) {
		return;
	}
	// The framebuffer may still be referenced by the current (not yet submitted) or an in-flight
	// command buffer - jngl::FrameBuffers can be destroyed mid-frame. Hold on to the resources and
	// free them once enough frames have been submitted that the GPU is guaranteed to be done.
	framebufferTrash.push_back({ fb, static_cast<int>(maxFramesInFlight) + 1 });
	fb = {}; // the trashed copy now owns the handles
}

void VulkanRenderer::pushRenderTarget(VulkanFramebuffer& framebuffer) {
	if (!frameActive) {
		return;
	}
	vkCmdEndRenderPass(commandBuffers[currentFrame]);
	renderTargetStack.push_back(&framebuffer);
	beginCurrentRenderPass(VK_ATTACHMENT_LOAD_OP_LOAD);
}

void VulkanRenderer::popRenderTarget() {
	if (!frameActive || renderTargetStack.empty()) {
		return;
	}
	vkCmdEndRenderPass(commandBuffers[currentFrame]);
	renderTargetStack.pop_back();
	beginCurrentRenderPass(VK_ATTACHMENT_LOAD_OP_LOAD);
}

void VulkanRenderer::clearCurrentRenderTarget(const Rgba color) {
	if (!frameActive) {
		return;
	}
	VkClearAttachment attachment{};
	attachment.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	attachment.colorAttachment = 0;
	attachment.clearValue.color = { { color.getRed(), color.getGreen(), color.getBlue(),
		                              color.getAlpha() } };
	const VkExtent2D extent =
	    renderTargetStack.empty()
	        ? swapchainExtent
	        : VkExtent2D{ static_cast<uint32_t>(renderTargetStack.back()->width),
		                  static_cast<uint32_t>(renderTargetStack.back()->height) };
	VkClearRect rect{};
	rect.rect = { { 0, 0 }, extent };
	rect.baseArrayLayer = 0;
	rect.layerCount = 1;
	vkCmdClearAttachments(commandBuffers[currentFrame], 1, &attachment, 1, &rect);
}

} // namespace jngl
