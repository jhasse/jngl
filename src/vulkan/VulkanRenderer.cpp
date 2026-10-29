// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "VulkanRenderer.hpp"

#include "../App.hpp"
#include "../StartupProfiler.hpp"
#include "../jngl/Alpha.hpp"
#include "../jngl/Mat3.hpp"
#include "../log.hpp"
#include "vulkan_helper.hpp"

#include "BuiltinBytecode.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <algorithm>
#include <array>
#include <bit>
#include <cassert>
#include <cstring>
#include <limits>
#include <set>
#include <span>
#include <tuple>
#include <stdexcept>

namespace jngl {

namespace {

/// A Vulkan struct of type T with everything zeroed but its sType, which is \a type
template <class T> T vkStruct(const VkStructureType type) {
	T value{};
	value.sType = type;
	return value;
}

/// Metal emulates triangle fans with extra GPU work. Expand the common four-vertex fan
/// on the CPU, keeping the same diagonal, winding and provoking vertices.
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
void triangulateFan(const float* vertices, const size_t vertexCount, std::vector<float>& triangles) {
	triangles.resize((vertexCount - 2) * 3 * floatsPerVertex);
	float* out = triangles.data();
	for (size_t i = 1; i + 1 < vertexCount; ++i) {
		for (const size_t index : { i, i + 1, size_t{ 0 } }) {
			out = std::copy_n(vertices + index * floatsPerVertex, floatsPerVertex, out);
		}
	}
}

/// The stage of a pipeline that runs \a module's main function
VkPipelineShaderStageCreateInfo shaderStage(const VkShaderStageFlagBits stage,
                                            const VkShaderModule module) {
	auto info = vkStruct<VkPipelineShaderStageCreateInfo>(
	    VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO);
	info.stage = stage;
	info.module = module;
	info.pName = "main";
	return info;
}

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

/// The same for all render passes, as it's part of what makes them compatible with the pipelines.
/// Drawing waits for an earlier render pass's drawing (which a LOAD continues) and sampling (of a
/// FrameBuffer that's drawn into now), and later sampling and drawing wait for it.
std::array<VkSubpassDependency, 2> subpassDependencies() {
	std::array<VkSubpassDependency, 2> deps{};
	deps[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	deps[0].dstSubpass = 0;
	deps[1].srcSubpass = 0;
	deps[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	for (auto& dep : deps) {
		dep.srcStageMask =
		    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dep.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dep.dstStageMask =
		    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
		                    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;
	}
	return deps;
}

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
	internal::StartupProfiler total("VulkanRenderer total");
	{
		internal::StartupProfiler _{ "createInstance" };
		createInstance();
	}
	{
		internal::StartupProfiler _{ "createSurface" };
		createSurface();
	}
	{
		internal::StartupProfiler _{ "pickPhysicalDevice" };
		pickPhysicalDevice();
	}
	{
		internal::StartupProfiler _{ "createLogicalDevice" };
		createLogicalDevice();
	}
	{
		internal::StartupProfiler _{ "queryMsaaSupport" };
		queryMsaaSupport();
	}
	{
		internal::StartupProfiler _{ "createSwapchain" };
		createSwapchain();
	}
	{
		internal::StartupProfiler _{ "createImageViews" };
		createImageViews();
	}
	{
		internal::StartupProfiler _{ "createPresentSemaphores" };
		createPresentSemaphores();
	}
	{
		internal::StartupProfiler _{ "createRenderPass" };
		createRenderPass();
	}
	{
		internal::StartupProfiler _{ "createOffscreenRenderPasses" };
		createOffscreenRenderPasses();
	}
	{
		internal::StartupProfiler _{ "createFramebuffers" };
		createFramebuffers();
	}
	{
		internal::StartupProfiler _{ "createCommandPoolAndBuffers" };
		createCommandPoolAndBuffers();
	}
	{
		internal::StartupProfiler _{ "createSyncObjects" };
		createSyncObjects();
	}
	{
		internal::StartupProfiler _{ "createDescriptorPool" };
		createDescriptorPool();
	}
	{
		internal::StartupProfiler _{ "createPipelineLayout" };
		createPipelineLayout();
	}
	coloredUniforms =
	    uniformBlock(builtin_bytecode::colored.uniforms, builtin_bytecode::colored.uniformBlockSize,
	                 builtin_bytecode::colored.pushConstants);
	roundedUniforms =
	    uniformBlock(builtin_bytecode::rounded.uniforms, builtin_bytecode::rounded.uniformBlockSize,
	                 builtin_bytecode::rounded.pushConstants);
	texturedUniforms = uniformBlock(builtin_bytecode::textured.uniforms,
	                                builtin_bytecode::textured.uniformBlockSize,
	                                builtin_bytecode::textured.pushConstants);
	{
		// Only those of the window's default blending, the others are built once they're used
		internal::StartupProfiler _{ "ensurePipelines" };
		ensurePipelines(PassKind::Ldr, BlendVariant::Composite);
		if (msaaSupported) {
			ensurePipelines(PassKind::LdrMsaa, BlendVariant::Composite);
		}
	}
	{
		internal::StartupProfiler _{ "createVertexBuffers" };
		createVertexBuffers();
	}
	{
		internal::StartupProfiler _{ "createUniformBuffers" };
		createUniformBuffers();
	}
	internal::debug("Vulkan renderer initialized ({}x{}).", swapchainExtent.width,
	                swapchainExtent.height);
}

VulkanRenderer::~VulkanRenderer() {
	if (device) {
		vkDeviceWaitIdle(device);
	}
	cleanupSwapchain();
	if (device) {
		for (auto& pending : pendingDestructions) {
			pending.destroy();
		}
		pendingDestructions.clear();
		for (const auto& staging : pendingStagingBuffers) {
			vkDestroyBuffer(device, staging.buffer, nullptr);
			vkFreeMemory(device, staging.memory, nullptr);
		}
		for (const auto& retired : retiredStagingBuffers) {
			for (const auto& staging : retired) {
				vkDestroyBuffer(device, staging.buffer, nullptr);
				vkFreeMemory(device, staging.memory, nullptr);
			}
		}
		for (auto& vb : vertexBuffers) {
			destroyVertexBuffer(vb);
		}
		for (auto& retired : retiredVertexBuffers) {
			for (auto& vb : retired) {
				destroyVertexBuffer(vb);
			}
		}
		for (auto& ub : uniformBuffers) {
			destroyUniformBuffer(ub);
		}
		for (auto& retired : retiredUniformBuffers) {
			for (auto& ub : retired) {
				destroyUniformBuffer(ub);
			}
		}
		destroyColoredPipelines();
		destroyRoundedPipelines();
		destroyTexturedPipelines();
		if (pipelineLayout) {
			vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
		}
		if (texturedSetLayout) {
			vkDestroyDescriptorSetLayout(device, texturedSetLayout, nullptr);
		}
		if (uniformsSetLayout) {
			vkDestroyDescriptorSetLayout(device, uniformsSetLayout, nullptr);
		}
		for (const auto& pool : descriptorPools) {
			vkDestroyDescriptorPool(device, pool.pool, nullptr);
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
		if (resolveRenderPass) {
			vkDestroyRenderPass(device, resolveRenderPass, nullptr);
		}
		if (clearResolveRenderPass) {
			vkDestroyRenderPass(device, clearResolveRenderPass, nullptr);
		}
		for (const auto& passes : offscreenRenderPasses) {
			for (VkRenderPass pass :
			     { passes.single, passes.singleClear, passes.msaa, passes.msaaClear, passes.resolve,
			       passes.resolveClear, passes.expand }) {
				if (pass) {
					vkDestroyRenderPass(device, pass, nullptr);
				}
			}
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

	auto appInfo = vkStruct<VkApplicationInfo>(VK_STRUCTURE_TYPE_APPLICATION_INFO);
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

	auto createInfo = vkStruct<VkInstanceCreateInfo>(VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO);
	createInfo.pApplicationInfo = &appInfo;
	// MoltenVK needs an explicit opt-in to enumerate its GPUs. Query support rather than
	// assuming all macOS drivers need it: KosmicKrisp is a conformant implementation.
	uint32_t extensionCount = 0;
	VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr),
	         "vkEnumerateInstanceExtensionProperties");
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
	                                                availableExtensions.data()),
	         "vkEnumerateInstanceExtensionProperties");
	if (std::ranges::any_of(availableExtensions, [](const VkExtensionProperties& extension) {
		return std::strcmp(extension.extensionName,
		                   VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME) == 0;
	})) {
		if (std::ranges::none_of(extensions, [](const char* extension) {
			return std::strcmp(extension, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME) == 0;
		})) {
			extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
		}
		createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
	}
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	auto debugInfo = vkStruct<VkDebugUtilsMessengerCreateInfoEXT>(
	    VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT);
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
		auto info = vkStruct<VkDeviceQueueCreateInfo>(VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO);
		info.queueFamilyIndex = family;
		info.queueCount = 1;
		info.pQueuePriorities = &priority;
		queueInfos.push_back(info);
	}

	std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	VkPhysicalDeviceFeatures features{};

	// Timestamps for jngl::GpuTimer, if the graphics queue can write them and they can be reset
	// from the host
	uint32_t familyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyCount, nullptr);
	std::vector<VkQueueFamilyProperties> families(familyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyCount, families.data());
	const uint32_t timestampValidBits = families[graphicsQueueFamily].timestampValidBits;
	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount,
	                                     extensions.data());
	const bool portabilitySubset =
	    std::ranges::any_of(extensions, [](const VkExtensionProperties& extension) {
		return std::strcmp(extension.extensionName, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME) == 0;
	});
	auto portabilityFeatures = vkStruct<VkPhysicalDevicePortabilitySubsetFeaturesKHR>(
	    VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR);
	if (portabilitySubset) {
		deviceExtensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
		auto supported =
		    vkStruct<VkPhysicalDeviceFeatures2>(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2);
		supported.pNext = &portabilityFeatures;
		vkGetPhysicalDeviceFeatures2(physicalDevice, &supported);
		if (!portabilityFeatures.triangleFans) {
			throw std::runtime_error("The Vulkan driver does not support triangle fans.");
		}
		// Triangle fans are core on conformant drivers, but must be explicitly enabled on
		// portability drivers. Leave the other optional portability features disabled.
		portabilityFeatures = vkStruct<VkPhysicalDevicePortabilitySubsetFeaturesKHR>(
		    VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR);
		portabilityFeatures.triangleFans = VK_TRUE;
	}
	VkPhysicalDeviceProperties props;
	vkGetPhysicalDeviceProperties(physicalDevice, &props);
	auto hostQueryReset = vkStruct<VkPhysicalDeviceHostQueryResetFeaturesEXT>(
	    VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES_EXT);
	if (props.apiVersion >= VK_API_VERSION_1_1 &&
	    std::ranges::any_of(extensions, [](const VkExtensionProperties& extension) {
		return std::strcmp(extension.extensionName, VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME) == 0;
	})) {
		auto features2 =
		    vkStruct<VkPhysicalDeviceFeatures2>(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2);
		features2.pNext = &hostQueryReset;
		vkGetPhysicalDeviceFeatures2(physicalDevice, &features2);
	}
	const bool timestamps = timestampValidBits > 0 && hostQueryReset.hostQueryReset == VK_TRUE;
	if (timestamps) {
		deviceExtensions.push_back(VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME);
	}

	auto createInfo = vkStruct<VkDeviceCreateInfo>(VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO);
	if (portabilitySubset) {
		createInfo.pNext = &portabilityFeatures;
	}
	if (timestamps) {
		hostQueryReset.pNext = portabilitySubset ? &portabilityFeatures : nullptr;
		createInfo.pNext = &hostQueryReset;
	}
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
	createInfo.pQueueCreateInfos = queueInfos.data();
	createInfo.pEnabledFeatures = &features;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	VK_CHECK(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device), "vkCreateDevice");
	vkGetDeviceQueue(device, graphicsQueueFamily, 0, &graphicsQueue);
	vkGetDeviceQueue(device, presentQueueFamily, 0, &presentQueue);

	if (timestamps) {
		resetQueryPool = reinterpret_cast<PFN_vkResetQueryPoolEXT>(
		    vkGetDeviceProcAddr(device, "vkResetQueryPoolEXT"));
		timestampPeriod = props.limits.timestampPeriod;
		timestampMask =
		    timestampValidBits >= 64 ? ~uint64_t{ 0 } : (uint64_t{ 1 } << timestampValidBits) - 1;
	}
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

	const VkPresentModeKHR presentMode = choosePresentMode();

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

	auto createInfo =
	    vkStruct<VkSwapchainCreateInfoKHR>(VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR);
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
		auto info = vkStruct<VkImageViewCreateInfo>(VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO);
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
	// The swapchain image, or with MSAA, the multisampled image that's resolved into it at the end
	// of the frame
	std::array<VkAttachmentDescription, 2> attachments{};
	attachments[0].format = swapchainImageFormat;
	attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	uint32_t attachmentCount = 1;

	VkAttachmentReference colorRef{ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
	VkAttachmentReference resolveRef{ 1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorRef;

	const bool useMsaa = swapchainMsaaActive();
	if (useMsaa) {
		// Only resolve when the target finishes drawing. Intermediate passes preserve the
		// samples without resolving, e.g. while a nested FrameBuffer is drawn into.
		attachments[1] = attachments[0];
		attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[1].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		// Stored, as the frame's later render passes (e.g. after a FrameBuffer has been drawn
		// into) continue drawing into it
		attachments[0].samples = msaaSampleCount;
		attachments[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		attachmentCount = 2;
	}

	const auto deps = subpassDependencies();
	auto info = vkStruct<VkRenderPassCreateInfo>(VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO);
	info.attachmentCount = attachmentCount;
	info.pAttachments = attachments.data();
	info.subpassCount = 1;
	info.pSubpasses = &subpass;
	info.dependencyCount = static_cast<uint32_t>(deps.size());
	info.pDependencies = deps.data();
	VK_CHECK(vkCreateRenderPass(device, &info, nullptr, &renderPass), "vkCreateRenderPass");

	// The LOAD variant, which continues the frame after it's been interrupted, e.g. by readPixels
	// or a FrameBuffer. It uses the same framebuffers.
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
	if (useMsaa) {
		attachments[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		attachments[1].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	} else {
		attachments[0].initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	}
	VK_CHECK(vkCreateRenderPass(device, &info, nullptr, &loadRenderPass), "vkCreateRenderPass");

	if (!useMsaa) {
		return;
	}
	// The same framebuffers work with it, as the resolve attachments don't matter for the
	// compatibility of render passes with a single subpass
	attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[1].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	subpass.pResolveAttachments = &resolveRef;
	VK_CHECK(vkCreateRenderPass(device, &info, nullptr, &resolveRenderPass), "vkCreateRenderPass");
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	VK_CHECK(vkCreateRenderPass(device, &info, nullptr, &clearResolveRenderPass),
	         "vkCreateRenderPass");
}

void VulkanRenderer::createOffscreenRenderPasses() {
	for (const bool hdr : { false, true }) {
		createOffscreenRenderPasses(hdr ? hdrFormat : swapchainImageFormat,
		                            offscreenRenderPasses[hdr ? 1 : 0]);
	}
}

void VulkanRenderer::createOffscreenRenderPasses(const VkFormat format,
                                                 OffscreenRenderPasses& passes) {
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = format;
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

	const auto deps = subpassDependencies();

	auto info = vkStruct<VkRenderPassCreateInfo>(VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO);
	info.attachmentCount = 1;
	info.pAttachments = &colorAttachment;
	info.subpassCount = 1;
	info.pSubpasses = &subpass;
	info.dependencyCount = static_cast<uint32_t>(deps.size());
	info.pDependencies = deps.data();
	VK_CHECK(vkCreateRenderPass(device, &info, nullptr, &passes.single), "vkCreateRenderPass");
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	VK_CHECK(vkCreateRenderPass(device, &info, nullptr, &passes.singleClear), "vkCreateRenderPass");
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;

	if (!msaaSupported) {
		return;
	}
	std::array<VkAttachmentDescription, 2> attachments{};
	attachments[0] = colorAttachment;
	attachments[0].samples = msaaSampleCount;
	attachments[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	attachments[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	// In the framebuffer, but not used, so that it keeps what was resolved into it last, see
	// passes.resolve
	attachments[1] = colorAttachment;
	attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	info.attachmentCount = static_cast<uint32_t>(attachments.size());
	info.pAttachments = attachments.data();
	VK_CHECK(vkCreateRenderPass(device, &info, nullptr, &passes.msaa), "vkCreateRenderPass");
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	VK_CHECK(vkCreateRenderPass(device, &info, nullptr, &passes.msaaClear), "vkCreateRenderPass");
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;

	// The same framebuffers work with it, as the resolve attachments don't matter for the
	// compatibility of render passes with a single subpass
	attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE; // all of it is resolved into
	VkAttachmentReference resolveRef{ 1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
	subpass.pResolveAttachments = &resolveRef;
	VK_CHECK(vkCreateRenderPass(device, &info, nullptr, &passes.resolve), "vkCreateRenderPass");
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	VK_CHECK(vkCreateRenderPass(device, &info, nullptr, &passes.resolveClear),
	         "vkCreateRenderPass");
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;

	// Pipelines built against passes.msaa work with this one too, as the resolve
	// attachments don't matter for the compatibility of render passes with a single subpass
	VkAttachmentDescription samplesAttachment = attachments[0];
	samplesAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // all of it is overwritten
	samplesAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	subpass.pResolveAttachments = nullptr;
	info.attachmentCount = 1;
	info.pAttachments = &samplesAttachment;
	VK_CHECK(vkCreateRenderPass(device, &info, nullptr, &passes.expand), "vkCreateRenderPass");
}

void VulkanRenderer::createMsaaAttachments() {
	destroyMsaaAttachments();
	if (!swapchainMsaaActive()) {
		return;
	}
	msaaColorAttachments.resize(swapchainImages.size());
	for (size_t i = 0; i < swapchainImages.size(); ++i) {
		MsaaColorAttachment& msaa = msaaColorAttachments[i];
		auto imageInfo = vkStruct<VkImageCreateInfo>(VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO);
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.format = swapchainImageFormat;
		imageInfo.extent = { swapchainExtent.width, swapchainExtent.height, 1 };
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.samples = msaaSampleCount;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		// Not transient, as it's stored between the render passes of a frame
		imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		VK_CHECK(vkCreateImage(device, &imageInfo, nullptr, &msaa.image), "vkCreateImage");

		VkMemoryRequirements memReq;
		vkGetImageMemoryRequirements(device, msaa.image, &memReq);
		auto allocInfo = vkStruct<VkMemoryAllocateInfo>(VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO);
		allocInfo.allocationSize = memReq.size;
		allocInfo.memoryTypeIndex = findMemoryType(
		    memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		VK_CHECK(vkAllocateMemory(device, &allocInfo, nullptr, &msaa.memory), "vkAllocateMemory");
		VK_CHECK(vkBindImageMemory(device, msaa.image, msaa.memory, 0), "vkBindImageMemory");

		auto viewInfo = vkStruct<VkImageViewCreateInfo>(VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO);
		viewInfo.image = msaa.image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = swapchainImageFormat;
		viewInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		VK_CHECK(vkCreateImageView(device, &viewInfo, nullptr, &msaa.view), "vkCreateImageView");
	}
}

void VulkanRenderer::destroyMsaaAttachments() {
	if (!device) {
		return;
	}
	for (auto& msaa : msaaColorAttachments) {
		if (msaa.view) {
			vkDestroyImageView(device, msaa.view, nullptr);
		}
		if (msaa.image) {
			vkDestroyImage(device, msaa.image, nullptr);
		}
		if (msaa.memory) {
			vkFreeMemory(device, msaa.memory, nullptr);
		}
		msaa = {};
	}
	msaaColorAttachments.clear();
}

void VulkanRenderer::createFramebuffers() {
	createMsaaAttachments();
	swapchainFramebuffers.resize(swapchainImageViews.size());
	for (size_t i = 0; i < swapchainImageViews.size(); ++i) {
		// The multisampled image first, see createRenderPass()
		std::vector<VkImageView> attachments;
		if (swapchainMsaaActive()) {
			attachments.push_back(msaaColorAttachments[i].view);
		}
		attachments.push_back(swapchainImageViews[i]);
		auto info = vkStruct<VkFramebufferCreateInfo>(VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO);
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
	auto poolInfo = vkStruct<VkCommandPoolCreateInfo>(VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO);
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = graphicsQueueFamily;
	VK_CHECK(vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool),
	         "vkCreateCommandPool");

	commandBuffers.resize(maxFramesInFlight);
	auto allocInfo =
	    vkStruct<VkCommandBufferAllocateInfo>(VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO);
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
	VK_CHECK(vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()),
	         "vkAllocateCommandBuffers");

	uploadCommandBuffers.resize(maxFramesInFlight);
	VK_CHECK(vkAllocateCommandBuffers(device, &allocInfo, uploadCommandBuffers.data()),
	         "vkAllocateCommandBuffers");
	retiredStagingBuffers.resize(maxFramesInFlight);
}

void VulkanRenderer::createSyncObjects() {
	imageAvailableSemaphores.resize(maxFramesInFlight);
	inFlightFences.resize(maxFramesInFlight);

	auto semInfo = vkStruct<VkSemaphoreCreateInfo>(VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO);
	auto fenceInfo = vkStruct<VkFenceCreateInfo>(VK_STRUCTURE_TYPE_FENCE_CREATE_INFO);
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
		VK_CHECK(vkCreateSemaphore(device, &semInfo, nullptr, &imageAvailableSemaphores[i]),
		         "vkCreateSemaphore");
		VK_CHECK(vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]),
		         "vkCreateFence");
	}
	auto unsignaled = vkStruct<VkFenceCreateInfo>(VK_STRUCTURE_TYPE_FENCE_CREATE_INFO);
	VK_CHECK(vkCreateFence(device, &unsignaled, nullptr, &readbackFence), "vkCreateFence");
}

void VulkanRenderer::createPresentSemaphores() {
	// The semaphore the present operation waits on must be one per swapchain image, not per
	// frame-in-flight: a swapchain image's present may still be reading the semaphore when the next
	// frame-in-flight reuses it. (See the Vulkan swapchain-semaphore-reuse guidance.)
	auto semInfo = vkStruct<VkSemaphoreCreateInfo>(VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO);
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
	destroyMsaaAttachments();
	for (VkImageView view : swapchainImageViews) {
		vkDestroyImageView(device, view, nullptr);
	}
	swapchainImageViews.clear();
	if (swapchain) {
		vkDestroySwapchainKHR(device, swapchain, nullptr);
		swapchain = VK_NULL_HANDLE;
	}
}

VkPresentModeKHR VulkanRenderer::choosePresentMode() const {
	if (verticalSyncEnabled) {
		return VK_PRESENT_MODE_FIFO_KHR;
	}
	uint32_t count = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, nullptr);
	std::vector<VkPresentModeKHR> modes(count);
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, modes.data());
	for (const VkPresentModeKHR mode : modes) {
		if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return mode;
		}
	}
	for (const VkPresentModeKHR mode : modes) {
		if (mode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
			return mode;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}

void VulkanRenderer::setVerticalSync(const bool enabled) {
	if (verticalSyncEnabled == enabled) {
		return;
	}
	verticalSyncEnabled = enabled;
	if (!device) {
		return;
	}
	if (frameActive) {
		// The frame's command buffer uses the swapchain's framebuffers, so wait until it's been
		// presented
		swapchainRecreatePending = true;
		return;
	}
	vkDeviceWaitIdle(device);
	recreateSwapchain();
}

bool VulkanRenderer::getVerticalSync() const {
	return verticalSyncEnabled;
}

void VulkanRenderer::queryMsaaSupport() {
	VkPhysicalDeviceProperties props;
	vkGetPhysicalDeviceProperties(physicalDevice, &props);
	const VkSampleCountFlags counts = props.limits.framebufferColorSampleCounts;
	if ((counts & VK_SAMPLE_COUNT_4_BIT) != 0) {
		msaaSampleCount = VK_SAMPLE_COUNT_4_BIT;
		msaaSupported = true;
	} else {
		msaaSampleCount = VK_SAMPLE_COUNT_1_BIT;
		msaaSupported = false;
	}
	if (!msaaSupported) {
		msaaEnabled = false;
	}
	internal::debug("Vulkan MSAA 4x: {} (enabled: {}).", msaaSupported ? "supported" : "unsupported",
	                msaaEnabled);
}

bool VulkanRenderer::swapchainMsaaActive() const {
	return msaaEnabled && msaaSupported;
}

bool VulkanRenderer::isMultisampleSupported() const {
	return msaaSupported;
}

void VulkanRenderer::setMultisampleAntiAliasing(const bool enabled) {
	if (!msaaSupported) {
		msaaEnabled = false;
		msaaRebuildPending = false;
		return;
	}
	if (msaaEnabled == enabled && !msaaRebuildPending) {
		return;
	}
	if (frameActive) {
		pendingMsaaEnabled = enabled;
		msaaRebuildPending = true;
		return;
	}
	msaaEnabled = enabled;
	if (!device) {
		return;
	}
	rebuildSwapchainRendering();
}

bool VulkanRenderer::getMultisampleAntiAliasing() const {
	if (!msaaSupported) {
		return false;
	}
	return msaaRebuildPending ? pendingMsaaEnabled : msaaEnabled;
}

void VulkanRenderer::destroyColoredPipelines() {
	if (!device) {
		return;
	}
	for (auto& pipelines : coloredPipelines) {
		for (VkPipeline& pipeline : pipelines) {
			if (pipeline) {
				vkDestroyPipeline(device, pipeline, nullptr);
				pipeline = VK_NULL_HANDLE;
			}
		}
	}
}

void VulkanRenderer::destroyTexturedPipelines() {
	if (!device) {
		return;
	}
	for (auto& pipelines : texturedPipelines) {
		for (VkPipeline& pipeline : pipelines) {
			if (pipeline) {
				vkDestroyPipeline(device, pipeline, nullptr);
				pipeline = VK_NULL_HANDLE;
			}
		}
	}
}

void VulkanRenderer::rebuildSwapchainRendering() {
	vkDeviceWaitIdle(device);
	if (frameActive) {
		vkResetCommandBuffer(commandBuffers[currentFrame], 0);
		frameActive = false;
		renderPassActive = false;
		currentPassKind = PassKind::Ldr;
	}
	for (VkFramebuffer fb : swapchainFramebuffers) {
		vkDestroyFramebuffer(device, fb, nullptr);
	}
	swapchainFramebuffers.clear();
	destroyMsaaAttachments();
	if (renderPass) {
		vkDestroyRenderPass(device, renderPass, nullptr);
		renderPass = VK_NULL_HANDLE;
	}
	if (loadRenderPass) {
		vkDestroyRenderPass(device, loadRenderPass, nullptr);
		loadRenderPass = VK_NULL_HANDLE;
	}
	if (resolveRenderPass) {
		vkDestroyRenderPass(device, resolveRenderPass, nullptr);
		resolveRenderPass = VK_NULL_HANDLE;
	}
	if (clearResolveRenderPass) {
		vkDestroyRenderPass(device, clearResolveRenderPass, nullptr);
		clearResolveRenderPass = VK_NULL_HANDLE;
	}
	// The pipelines are built against the offscreen render passes, which are compatible with
	// either kind of swapchain render pass, so they're kept
	createRenderPass();
	createFramebuffers();
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
	if (frameActive) {
		// E.g. by showWindow, before the first swapBuffers. Like glClear on OpenGL, clear what's
		// being drawn on instead.
		currentClearColor = clearColor;
		if (renderTargetStack.empty()) {
			endCurrentRenderPass();
			beginCurrentRenderPass(VK_ATTACHMENT_LOAD_OP_CLEAR); // with the letterboxing's bars
		} else {
			clearCurrentRenderTarget(Rgba(clearColor, Alpha(1)));
		}
		return;
	}
	vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE,
	                std::numeric_limits<uint64_t>::max());
	// The GPU is done with this frame's previous command buffer, and so with the buffers it read
	for (auto& vb : retiredVertexBuffers[currentFrame]) {
		destroyVertexBuffer(vb);
	}
	retiredVertexBuffers[currentFrame].clear();
	for (auto& ub : retiredUniformBuffers[currentFrame]) {
		destroyUniformBuffer(ub);
	}
	retiredUniformBuffers[currentFrame].clear();
	for (const auto& staging : retiredStagingBuffers[currentFrame]) {
		vkDestroyBuffer(device, staging.buffer, nullptr);
		vkFreeMemory(device, staging.memory, nullptr);
	}
	retiredStagingBuffers[currentFrame].clear();

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
	uniformBuffers[currentFrame].used = 0;
	usedSecondaryCommands[currentFrame] = 0;
	imageAvailableWaited = false;

	const VkCommandBuffer cmd = commandBuffers[currentFrame];
	vkResetCommandBuffer(cmd, 0);
	auto beginInfo =
	    vkStruct<VkCommandBufferBeginInfo>(VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO);
	VK_CHECK(vkBeginCommandBuffer(cmd, &beginInfo), "vkBeginCommandBuffer");

	currentClearColor = clearColor;
	renderTargetStack.clear();
	frameActive = true;
	beginCurrentRenderPass(VK_ATTACHMENT_LOAD_OP_CLEAR);
}

void VulkanRenderer::beginCurrentRenderPass(const VkAttachmentLoadOp loadOp) {
	assert(!renderPassActive);
	pendingLoadOp = loadOp;
	pendingClearColor = Rgba(currentClearColor, Alpha(1));
}

bool VulkanRenderer::endCurrentRenderPass(const bool resolve) {
	bool resolvedSwapchain = false;
	// An empty LOAD pass has no effect. A clear still has to happen even without any draws.
	if (pendingLoadOp == VK_ATTACHMENT_LOAD_OP_CLEAR) {
		ensureRenderPass();
	}
	if (renderPassActive) {
		const VkCommandBuffer cmd = commandBuffers[currentFrame];
		if (renderCommands != cmd) {
			VK_CHECK(vkEndCommandBuffer(renderCommands), "vkEndCommandBuffer");
			if (resolve && renderTargetStack.empty()) {
				deferredPass.renderPass =
				    deferredPass.clearValueCount ? clearResolveRenderPass : resolveRenderPass;
				resolvedSwapchain = true;
			} else if (resolve) {
				auto& target = *renderTargetStack.back();
				const auto& passes = offscreenRenderPasses[target.hdr ? 1 : 0];
				deferredPass.renderPass =
				    deferredPass.clearValueCount ? passes.resolveClear : passes.resolve;
				target.resolvePending = false;
			}
			vkCmdBeginRenderPass(cmd, &deferredPass, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
			vkCmdExecuteCommands(cmd, 1, &renderCommands);
		}
		vkCmdEndRenderPass(cmd);
		renderPassActive = false;
	}
	return resolvedSwapchain;
}

void VulkanRenderer::ensureRenderPass() {
	if (renderPassActive) {
		return;
	}
	const VkAttachmentLoadOp loadOp = pendingLoadOp;
	const VkCommandBuffer cmd = commandBuffers[currentFrame];
	auto rpInfo = vkStruct<VkRenderPassBeginInfo>(VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO);
	VkExtent2D extent{};
	VkClearValue clear{};
	if (renderTargetStack.empty()) {
		syncLetterboxScissor();
	}
	const bool letterboxClear =
	    renderTargetStack.empty() && letterboxing && loadOp == VK_ATTACHMENT_LOAD_OP_CLEAR;
	const Rgba passClearColor = letterboxClear ? Rgba(0.f, 0.f, 0.f, 1.f) : pendingClearColor;
	clear.color = { { passClearColor.getRed(), passClearColor.getGreen(), passClearColor.getBlue(),
		              passClearColor.getAlpha() } };
	if (renderTargetStack.empty()) {
		currentPassKind = swapchainMsaaActive() ? PassKind::LdrMsaa : PassKind::Ldr;
		rpInfo.framebuffer = swapchainFramebuffers[currentImageIndex];
		if (loadOp == VK_ATTACHMENT_LOAD_OP_CLEAR) {
			rpInfo.renderPass = renderPass;
			rpInfo.clearValueCount = 1; // only the first attachment is cleared
			rpInfo.pClearValues = &clear;
		} else {
			rpInfo.renderPass = loadRenderPass;
		}
		extent = swapchainExtent;
	} else {
		VulkanFramebuffer* target = renderTargetStack.back();
		// Like on OpenGL, where an anti-aliased FrameBuffer is always drawn into multisampled, but
		// only smoothed while GL_MULTISAMPLE is enabled
		const bool multisampled = target->msaaImage && getMultisampleAntiAliasing();
		currentPassKind = passKind(target->hdr, multisampled);
		const auto& passes = offscreenRenderPasses[target->hdr ? 1 : 0];
		if (multisampled) {
			if (target->samplesStale && loadOp != VK_ATTACHMENT_LOAD_OP_CLEAR) {
				expandIntoSamples(*target);
			}
			target->samplesStale = false;
			rpInfo.renderPass =
			    loadOp == VK_ATTACHMENT_LOAD_OP_CLEAR ? passes.msaaClear : passes.msaa;
			rpInfo.framebuffer = target->msaaFramebuffer;
			target->resolvePending = true;
		} else {
			if (target->resolvePending && loadOp != VK_ATTACHMENT_LOAD_OP_CLEAR) {
				resolveFramebuffer(*target); // before what's drawn now, which it'd overwrite
			}
			target->resolvePending = false;
			if (target->msaaImage) {
				target->samplesStale = true;
			}
			rpInfo.renderPass =
			    loadOp == VK_ATTACHMENT_LOAD_OP_CLEAR ? passes.singleClear : passes.single;
			rpInfo.framebuffer = target->framebuffer;
		}
		extent = { static_cast<uint32_t>(target->width), static_cast<uint32_t>(target->height) };
		if (loadOp == VK_ATTACHMENT_LOAD_OP_CLEAR) {
			rpInfo.clearValueCount = 1;
			rpInfo.pClearValues = &clear;
		}
	}
	rpInfo.renderArea.extent = extent;
	renderCommands = cmd;
	if (isMultisampled(currentPassKind)) {
		auto& buffers = secondaryCommands[currentFrame];
		const size_t index = usedSecondaryCommands[currentFrame]++;
		if (index == buffers.size()) {
			auto alloc = vkStruct<VkCommandBufferAllocateInfo>(
			    VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO);
			alloc.commandPool = commandPool;
			alloc.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
			alloc.commandBufferCount = 1;
			VkCommandBuffer secondary;
			VK_CHECK(vkAllocateCommandBuffers(device, &alloc, &secondary),
			         "vkAllocateCommandBuffers");
			buffers.push_back(secondary);
		}
		renderCommands = buffers[index];
		VK_CHECK(vkResetCommandBuffer(renderCommands, 0), "vkResetCommandBuffer");
		auto inheritance = vkStruct<VkCommandBufferInheritanceInfo>(
		    VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO);
		inheritance.renderPass = rpInfo.renderPass;
		inheritance.framebuffer = rpInfo.framebuffer;
		auto begin =
		    vkStruct<VkCommandBufferBeginInfo>(VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO);
		begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT |
		              VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
		begin.pInheritanceInfo = &inheritance;
		VK_CHECK(vkBeginCommandBuffer(renderCommands, &begin), "vkBeginCommandBuffer");
		deferredClear = clear;
		deferredPass = rpInfo;
		deferredPass.pClearValues = &deferredClear;
	} else {
		vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);
	}
	renderPassActive = true;
	pendingLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;

	// For the swapchain, flip the viewport vertically (negative height, core since Vulkan 1.1) so
	// that JNGL's projection matrix - written for OpenGL's clip space with +Y up - maps to the
	// screen the same way it does on the OpenGL backend. FrameBuffers are left un-flipped so their
	// image ends up "upside down" like an OpenGL FBO texture, which jngl::FrameBuffer::draw undoes
	// with a scale(1, -1).
	applyViewport(renderCommands, extent);
	applyScissor(renderCommands, extent);
	if (letterboxClear) {
		clearSwapchainCanvas(currentClearColor);
	}
}

void VulkanRenderer::resolveSwapchain() {
	if (!resolveRenderPass) {
		return; // without MSAA, it's been drawn into directly
	}
	auto rpInfo = vkStruct<VkRenderPassBeginInfo>(VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO);
	rpInfo.renderPass = resolveRenderPass;
	rpInfo.framebuffer = swapchainFramebuffers[currentImageIndex];
	rpInfo.renderArea.extent = swapchainExtent;
	const VkCommandBuffer cmd = commandBuffers[currentFrame];
	vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdEndRenderPass(cmd);
}

VkRect2D VulkanRenderer::glScissorToVulkan(const int glX, const int glY, const int glW,
                                           const int glH, const int fbHeight) {
	VkRect2D rect{};
	rect.offset.x = static_cast<uint32_t>(std::max(glX, 0));
	rect.offset.y = static_cast<uint32_t>(std::max(fbHeight - glY - glH, 0));
	rect.extent.width = static_cast<uint32_t>(std::max(glW, 0));
	rect.extent.height = static_cast<uint32_t>(std::max(glH, 0));
	return rect;
}

VkRect2D VulkanRenderer::intersectScissors(const VkRect2D a, const VkRect2D b) {
	const int ax1 = static_cast<int>(a.offset.x);
	const int ay1 = static_cast<int>(a.offset.y);
	const int ax2 = ax1 + static_cast<int>(a.extent.width);
	const int ay2 = ay1 + static_cast<int>(a.extent.height);
	const int bx1 = static_cast<int>(b.offset.x);
	const int by1 = static_cast<int>(b.offset.y);
	const int bx2 = bx1 + static_cast<int>(b.extent.width);
	const int by2 = by1 + static_cast<int>(b.extent.height);
	const int x1 = std::max(ax1, bx1);
	const int y1 = std::max(ay1, by1);
	const int x2 = std::min(ax2, bx2);
	const int y2 = std::min(ay2, by2);
	VkRect2D out{};
	if (x2 > x1 && y2 > y1) {
		out.offset = { static_cast<int32_t>(x1), static_cast<int32_t>(y1) };
		out.extent = { static_cast<uint32_t>(x2 - x1), static_cast<uint32_t>(y2 - y1) };
	}
	return out;
}

VkRect2D VulkanRenderer::letterboxScissorForExtent(const VkExtent2D extent) const {
	// Centered in the viewport, which is at the bottom left of the swapchain, see setViewport
	const int x = (framebufferWidth - canvasWidth) / 2;
	const int y = (framebufferHeight - canvasHeight) / 2;
	VkRect2D rect =
	    glScissorToVulkan(x, y, canvasWidth, canvasHeight, static_cast<int>(extent.height));
	return intersectScissors(rect, VkRect2D{ { 0, 0 }, extent });
}

VkRect2D VulkanRenderer::effectiveScissor(const VkExtent2D extent) const {
	VkRect2D base{ { 0, 0 }, extent };
	if (renderTargetStack.empty() && letterboxing) {
		base = letterboxScissorForExtent(extent);
	}
	if (!userScissor) {
		return base;
	}
	if (renderTargetStack.empty()) {
		return intersectScissors(base, glScissorToVulkan(userScissor->x, userScissor->y,
		                                                 userScissor->width, userScissor->height,
		                                                 static_cast<int>(extent.height)));
	}
	// FrameBuffers aren't flipped vertically (see beginCurrentRenderPass), so their rows already
	// start at the bottom like OpenGL's
	VkRect2D rect{};
	rect.offset.x = std::max(userScissor->x, 0);
	rect.offset.y = std::max(userScissor->y, 0);
	rect.extent.width = static_cast<uint32_t>(std::max(userScissor->width, 0));
	rect.extent.height = static_cast<uint32_t>(std::max(userScissor->height, 0));
	return intersectScissors(base, rect);
}

void VulkanRenderer::applyScissor(const VkCommandBuffer cmd, const VkExtent2D extent) const {
	const VkRect2D scissor = effectiveScissor(extent);
	vkCmdSetScissor(cmd, 0, 1, &scissor);
}

void VulkanRenderer::syncLetterboxScissor() {
	if (canvasWidth <= 0 || canvasHeight <= 0 || swapchainExtent.width == 0) {
		return;
	}
	if (framebufferWidth <= 0 || framebufferHeight <= 0) { // setViewport hasn't been called yet
		framebufferWidth = static_cast<int>(swapchainExtent.width);
		framebufferHeight = static_cast<int>(swapchainExtent.height);
	}
	letterboxing = canvasWidth != framebufferWidth || canvasHeight != framebufferHeight;
}

void VulkanRenderer::clearSwapchainCanvas(const Rgb& color) {
	if (!frameActive || !letterboxing) {
		return;
	}
	VkClearAttachment attachment{};
	attachment.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	attachment.colorAttachment = 0;
	attachment.clearValue.color = { { color.getRed(), color.getGreen(), color.getBlue(), 1.f } };
	VkClearRect rect{};
	rect.rect = letterboxScissorForExtent(swapchainExtent);
	rect.baseArrayLayer = 0;
	rect.layerCount = 1;
	vkCmdClearAttachments(renderCommands, 1, &attachment, 1, &rect);
}

void VulkanRenderer::updateLetterboxing(const int fbWidth, const int fbHeight, const int canvasW,
                                        const int canvasH) {
	framebufferWidth = fbWidth;
	framebufferHeight = fbHeight;
	canvasWidth = canvasW;
	canvasHeight = canvasH;
	letterboxing = canvasWidth != fbWidth || canvasHeight != fbHeight;
	if (renderPassActive && renderTargetStack.empty()) {
		applyScissor(renderCommands, swapchainExtent);
	}
}

VkExtent2D VulkanRenderer::currentRenderTargetExtent() const {
	if (renderTargetStack.empty()) {
		return swapchainExtent;
	}
	return { static_cast<uint32_t>(renderTargetStack.back()->width),
		     static_cast<uint32_t>(renderTargetStack.back()->height) };
}

void VulkanRenderer::setScissor(const std::optional<ScissorBox> box) {
	userScissor = box;
	if (renderPassActive) {
		applyScissor(renderCommands, currentRenderTargetExtent());
	}
}

int VulkanRenderer::getFramebufferWidth() const {
	return framebufferWidth;
}

int VulkanRenderer::getFramebufferHeight() const {
	return framebufferHeight;
}

void VulkanRenderer::endFrame() {
	if (!frameActive) {
		return; // swapchain was recreated in beginFrame; skip this frame
	}
	static bool firstPresent = true;
	internal::StartupProfiler firstPresentProfiler(
	    firstPresent ? "first vkQueuePresentKHR" : "");
	firstPresent = false;
	const VkCommandBuffer cmd = commandBuffers[currentFrame];
	if (!endCurrentRenderPass(true)) {
		resolveSwapchain();
	}
	VK_CHECK(vkEndCommandBuffer(cmd), "vkEndCommandBuffer");

	auto submit = vkStruct<VkSubmitInfo>(VK_STRUCTURE_TYPE_SUBMIT_INFO);
	const std::array<VkSemaphore, 1> waitSemaphores = { imageAvailableSemaphores[currentFrame] };
	const std::array<VkPipelineStageFlags, 1> waitStages = {
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
	};
	// If readPixels already flushed the frame it consumed the imageAvailable semaphore, so don't
	// wait on it again here.
	submit.waitSemaphoreCount = imageAvailableWaited ? 0 : 1;
	submit.pWaitSemaphores = waitSemaphores.data();
	submit.pWaitDstStageMask = waitStages.data();
	const auto commands = withPendingUploads(cmd);
	submit.commandBufferCount = static_cast<uint32_t>(commands.size());
	submit.pCommandBuffers = commands.data();
	const std::array<VkSemaphore, 1> signalSemaphores = {
		renderFinishedSemaphores[currentImageIndex]
	};
	submit.signalSemaphoreCount = 1;
	submit.pSignalSemaphores = signalSemaphores.data();

	VK_CHECK(vkQueueSubmit(graphicsQueue, 1, &submit, inFlightFences[currentFrame]),
	         "vkQueueSubmit");

	auto present = vkStruct<VkPresentInfoKHR>(VK_STRUCTURE_TYPE_PRESENT_INFO_KHR);
	present.waitSemaphoreCount = 1;
	present.pWaitSemaphores = signalSemaphores.data();
	present.swapchainCount = 1;
	present.pSwapchains = &swapchain;
	present.pImageIndices = &currentImageIndex;

	const VkResult result = vkQueuePresentKHR(presentQueue, &present);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
	    swapchainRecreatePending) {
		swapchainRecreatePending = false;
		recreateSwapchain();
	} else if (result != VK_SUCCESS) {
		throw std::runtime_error("vkQueuePresentKHR failed");
	}

	frameActive = false;
	currentFrame = (currentFrame + 1) % maxFramesInFlight;

	// Free what's been destroyed when the frames that could have used it have all completed. A
	// frame's beginFrame() waits for the one that used its slot before.
	for (auto it = pendingDestructions.begin(); it != pendingDestructions.end();) {
		if (--it->framesUntilFree <= 0) {
			it->destroy();
			it = pendingDestructions.erase(it);
		} else {
			++it;
		}
	}

	if (msaaRebuildPending) {
		msaaRebuildPending = false;
		msaaEnabled = pendingMsaaEnabled;
		rebuildSwapchainRendering();
	}
}

void VulkanRenderer::setViewport(const int x, const int y, const int width, const int height) {
	assert(x == 0 && y == 0);
	(void)x;
	(void)y;
	framebufferWidth = width;
	framebufferHeight = height;
	if (renderPassActive && renderTargetStack.empty()) {
		applyViewport(renderCommands, swapchainExtent);
	}
}

void VulkanRenderer::applyViewport(const VkCommandBuffer cmd, const VkExtent2D extent) const {
	VkViewport viewport;
	if (renderTargetStack.empty()) {
		// Like glViewport, at the bottom left. When the swapchain hasn't been recreated after a
		// resize yet, this might not cover all of it.
		const auto width = static_cast<float>(
		    framebufferWidth > 0 ? framebufferWidth : static_cast<int>(extent.width));
		const auto height = static_cast<float>(
		    framebufferHeight > 0 ? framebufferHeight : static_cast<int>(extent.height));
		viewport = { 0.f, static_cast<float>(extent.height), width, -height, 0.f, 1.f };
	} else {
		viewport = { 0.f, 0.f, static_cast<float>(extent.width), static_cast<float>(extent.height),
			         0.f, 1.f };
	}
	vkCmdSetViewport(cmd, 0, 1, &viewport);
}

void VulkanRenderer::onResize(int /*width*/, int /*height*/) {
	swapchainRecreatePending = true;
}

void VulkanRenderer::setProjection(const Mat4& p) {
	projection = p;
}

namespace {

VkShaderModule createShaderModule(VkDevice device, const std::span<const uint32_t> spirv) {
	auto info = vkStruct<VkShaderModuleCreateInfo>(VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO);
	info.codeSize = spirv.size() * sizeof(uint32_t);
	info.pCode = spirv.data();
	VkShaderModule module = VK_NULL_HANDLE;
	VK_CHECK(vkCreateShaderModule(device, &info, nullptr, &module), "vkCreateShaderModule");
	return module;
}

/// Shader modules that are only needed while pipelines are built from them
struct ShaderModules {
	ShaderModules(VkDevice device, const std::span<const uint32_t> vertexSpirv,
	              const std::span<const uint32_t> fragmentSpirv)
	: device(device), vert(createShaderModule(device, vertexSpirv)),
	  frag(createShaderModule(device, fragmentSpirv)) {
	}
	~ShaderModules() {
		vkDestroyShaderModule(device, frag, nullptr);
		vkDestroyShaderModule(device, vert, nullptr);
	}
	ShaderModules(const ShaderModules&) = delete;
	ShaderModules& operator=(const ShaderModules&) = delete;
	ShaderModules(ShaderModules&&) = delete;
	ShaderModules& operator=(ShaderModules&&) = delete;

	VkDevice device;
	VkShaderModule vert;
	VkShaderModule frag;
};

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

VkPipelineColorBlendAttachmentState blendAttachmentState(const BlendVariant variant) {
	VkPipelineColorBlendAttachmentState state{};
	state.blendEnable = VK_TRUE;
	state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	state.colorBlendOp = VK_BLEND_OP_ADD;
	state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	state.alphaBlendOp = VK_BLEND_OP_ADD;
	state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
	                       VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	switch (variant) {
	case BlendVariant::Composite:
		break;
	case BlendVariant::Premultiplied:
		state.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		break;
	case BlendVariant::Replace:
		state.blendEnable = VK_FALSE;
		break;
	case BlendVariant::ReplaceAlphaOnly:
		state.blendEnable = VK_FALSE;
		state.colorWriteMask = VK_COLOR_COMPONENT_A_BIT;
		break;
	case BlendVariant::AddAlpha:
		// So that what's drawn into a FrameBuffer is as opaque as the most opaque of it when it's
		// drawn again, see applyBlendMode()
		state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		break;
	// The following ones blend alpha like the colors, as glBlendFunc does
	case BlendVariant::Additive:
		state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
		state.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		break;
	case BlendVariant::PremultipliedAdditive:
		state.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
		state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		break;
	case BlendVariant::Multiply:
		state.srcColorBlendFactor = VK_BLEND_FACTOR_DST_COLOR;
		state.srcAlphaBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
		break;
	case BlendVariant::Screen:
		state.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
		break;
	}
	return state;
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

VkRenderPass VulkanRenderer::pipelineRenderPass(const PassKind kind) const {
	const auto& passes = offscreenRenderPasses[isHdr(kind) ? 1 : 0];
	return isMultisampled(kind) ? passes.msaa : passes.single;
}

VkSampleCountFlagBits VulkanRenderer::pipelineSamples(const PassKind kind) const {
	return isMultisampled(kind) ? msaaSampleCount : VK_SAMPLE_COUNT_1_BIT;
}

void VulkanRenderer::ensurePipelines(const PassKind kind, const BlendVariant variant) {
	const auto k = static_cast<size_t>(kind);
	if (coloredPipelines[k][pipelineIndex(variant, PrimitiveType::Triangles)]) {
		return;
	}
	const VkRenderPass pass = pipelineRenderPass(kind);
	const VkSampleCountFlagBits samples = pipelineSamples(kind);
	{
		const ShaderModules colored(device, builtin_bytecode::colored.vertex,
		                            builtin_bytecode::colored.fragment);
		buildColoredPipelines(colored.vert, colored.frag, pass, samples, variant,
		                      coloredPipelines[k]);
	}
	{
		const ShaderModules rounded(device, builtin_bytecode::rounded.vertex,
		                            builtin_bytecode::rounded.fragment);
		buildRoundedPipeline(rounded.vert, rounded.frag, pass, samples, variant,
		                     roundedPipelines[k][static_cast<size_t>(variant)]);
	}
	const ShaderModules textured(device, builtin_bytecode::textured.vertex,
	                             builtin_bytecode::textured.fragment);
	buildTexturedPipelines(textured.vert, textured.frag, pass, samples, variant,
	                       texturedPipelines[k]);
}

void VulkanRenderer::ensureProgramPipelines(const VulkanShaderProgram& program, const PassKind kind,
                                            const BlendVariant variant) {
	auto& pipelines = program.pipelines[static_cast<size_t>(kind)];
	if (pipelines[pipelineIndex(variant, PrimitiveType::Triangles)]) {
		return;
	}
	const ShaderModules modules(device, program.vertexSpirv, program.fragmentSpirv);
	buildTexturedPipelines(modules.vert, modules.frag, pipelineRenderPass(kind),
	                       pipelineSamples(kind), variant, pipelines);
}

void VulkanRenderer::buildColoredPipelines(
    const VkShaderModule vert, const VkShaderModule frag, const VkRenderPass targetRenderPass,
    const VkSampleCountFlagBits samples, const BlendVariant variant,
    std::array<VkPipeline, VulkanShaderProgram::pipelineCount>& out) {
	const std::array stages = { shaderStage(VK_SHADER_STAGE_VERTEX_BIT, vert),
		                        shaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, frag) };

	VkVertexInputBindingDescription binding{};
	binding.binding = 0;
	binding.stride = 2 * sizeof(float);
	binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	VkVertexInputAttributeDescription attribute{};
	attribute.location = 0;
	attribute.binding = 0;
	attribute.format = VK_FORMAT_R32G32_SFLOAT;
	attribute.offset = 0;
	auto vertexInput = vkStruct<VkPipelineVertexInputStateCreateInfo>(
	    VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO);
	vertexInput.vertexBindingDescriptionCount = 1;
	vertexInput.pVertexBindingDescriptions = &binding;
	vertexInput.vertexAttributeDescriptionCount = 1;
	vertexInput.pVertexAttributeDescriptions = &attribute;

	auto viewportState = vkStruct<VkPipelineViewportStateCreateInfo>(
	    VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO);
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	auto rasterizer = vkStruct<VkPipelineRasterizationStateCreateInfo>(
	    VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO);
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.cullMode = VK_CULL_MODE_NONE;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.lineWidth = 1.f;

	auto multisampling = vkStruct<VkPipelineMultisampleStateCreateInfo>(
	    VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO);
	multisampling.rasterizationSamples = samples;

	const VkPipelineColorBlendAttachmentState blendAttachment = blendAttachmentState(variant);
	auto colorBlending = vkStruct<VkPipelineColorBlendStateCreateInfo>(
	    VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO);
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &blendAttachment;

	const std::array<VkDynamicState, 2> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT,
		                                                  VK_DYNAMIC_STATE_SCISSOR };
	auto dynamicState = vkStruct<VkPipelineDynamicStateCreateInfo>(
	    VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO);
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	const std::array<PrimitiveType, 4> topologies = { PrimitiveType::Triangles,
		                                              PrimitiveType::TriangleStrip,
		                                              PrimitiveType::TriangleFan,
		                                              PrimitiveType::Lines };
	for (const PrimitiveType type : topologies) {
		auto inputAssembly = vkStruct<VkPipelineInputAssemblyStateCreateInfo>(
		    VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO);
		inputAssembly.topology = toVkTopology(type);

		auto pipelineInfo =
		    vkStruct<VkGraphicsPipelineCreateInfo>(VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO);
		pipelineInfo.stageCount = static_cast<uint32_t>(stages.size());
		pipelineInfo.pStages = stages.data();
		pipelineInfo.pVertexInputState = &vertexInput;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = pipelineLayout;
		pipelineInfo.renderPass = targetRenderPass;
		pipelineInfo.subpass = 0;
		VK_CHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr,
		                                   &out[pipelineIndex(variant, type)]),
		         "vkCreateGraphicsPipelines");
	}
}

void VulkanRenderer::destroyRoundedPipelines() {
	if (!device) {
		return;
	}
	for (auto& pipelines : roundedPipelines) {
		for (VkPipeline& pipeline : pipelines) {
			if (pipeline) {
				vkDestroyPipeline(device, pipeline, nullptr);
				pipeline = VK_NULL_HANDLE;
			}
		}
	}
}

void VulkanRenderer::buildRoundedPipeline(const VkShaderModule vert, const VkShaderModule frag,
                                          const VkRenderPass targetRenderPass,
                                          const VkSampleCountFlagBits samples,
                                          const BlendVariant variant, VkPipeline& out) {
	const std::array stages = { shaderStage(VK_SHADER_STAGE_VERTEX_BIT, vert),
		                        shaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, frag) };

	VkVertexInputBindingDescription binding{};
	binding.binding = 0;
	binding.stride = 2 * sizeof(float);
	binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	VkVertexInputAttributeDescription attribute{};
	attribute.location = 0;
	attribute.binding = 0;
	attribute.format = VK_FORMAT_R32G32_SFLOAT;
	attribute.offset = 0;
	auto vertexInput = vkStruct<VkPipelineVertexInputStateCreateInfo>(
	    VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO);
	vertexInput.vertexBindingDescriptionCount = 1;
	vertexInput.pVertexBindingDescriptions = &binding;
	vertexInput.vertexAttributeDescriptionCount = 1;
	vertexInput.pVertexAttributeDescriptions = &attribute;

	auto inputAssembly = vkStruct<VkPipelineInputAssemblyStateCreateInfo>(
	    VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO);
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	auto viewportState = vkStruct<VkPipelineViewportStateCreateInfo>(
	    VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO);
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	auto rasterizer = vkStruct<VkPipelineRasterizationStateCreateInfo>(
	    VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO);
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.cullMode = VK_CULL_MODE_NONE;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.lineWidth = 1.f;

	auto multisampling = vkStruct<VkPipelineMultisampleStateCreateInfo>(
	    VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO);
	multisampling.rasterizationSamples = samples;

	const VkPipelineColorBlendAttachmentState blendAttachment = blendAttachmentState(variant);
	auto colorBlending = vkStruct<VkPipelineColorBlendStateCreateInfo>(
	    VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO);
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &blendAttachment;

	const std::array<VkDynamicState, 2> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT,
		                                                  VK_DYNAMIC_STATE_SCISSOR };
	auto dynamicState = vkStruct<VkPipelineDynamicStateCreateInfo>(
	    VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO);
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	auto pipelineInfo =
	    vkStruct<VkGraphicsPipelineCreateInfo>(VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO);
	pipelineInfo.stageCount = static_cast<uint32_t>(stages.size());
	pipelineInfo.pStages = stages.data();
	pipelineInfo.pVertexInputState = &vertexInput;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = targetRenderPass;
	pipelineInfo.subpass = 0;
	VK_CHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &out),
	         "vkCreateGraphicsPipelines");
}

void VulkanRenderer::createVertexBuffers() {
	// 1 MiB per frame is plenty for most frames, uploadVertices grows it when it isn't.
	constexpr VkDeviceSize capacity = 1u << 20u;
	vertexBuffers.resize(maxFramesInFlight);
	retiredVertexBuffers.resize(maxFramesInFlight);
	for (auto& vb : vertexBuffers) {
		vb = createVertexBuffer(capacity);
	}
}

DynamicVertexBuffer VulkanRenderer::createVertexBuffer(const VkDeviceSize capacity) {
	DynamicVertexBuffer vb;
	auto bufferInfo = vkStruct<VkBufferCreateInfo>(VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO);
	bufferInfo.size = capacity;
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	VK_CHECK(vkCreateBuffer(device, &bufferInfo, nullptr, &vb.buffer), "vkCreateBuffer");

	VkMemoryRequirements memReq;
	vkGetBufferMemoryRequirements(device, vb.buffer, &memReq);
	auto allocInfo = vkStruct<VkMemoryAllocateInfo>(VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO);
	allocInfo.allocationSize = memReq.size;
	allocInfo.memoryTypeIndex =
	    findMemoryType(memReq.memoryTypeBits,
	                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	VK_CHECK(vkAllocateMemory(device, &allocInfo, nullptr, &vb.memory), "vkAllocateMemory");
	VK_CHECK(vkBindBufferMemory(device, vb.buffer, vb.memory, 0), "vkBindBufferMemory");
	VK_CHECK(vkMapMemory(device, vb.memory, 0, capacity, 0, &vb.mapped), "vkMapMemory");
	vb.capacity = capacity;
	return vb;
}

void VulkanRenderer::destroyVertexBuffer(DynamicVertexBuffer& vb) {
	if (vb.buffer) {
		vkDestroyBuffer(device, vb.buffer, nullptr);
	}
	if (vb.memory) {
		vkUnmapMemory(device, vb.memory);
		vkFreeMemory(device, vb.memory, nullptr);
	}
	vb = {};
}

std::pair<VkBuffer, VkDeviceSize> VulkanRenderer::uploadVertices(const void* const data,
                                                                 const VkDeviceSize bytes) {
	DynamicVertexBuffer& vb = vertexBuffers[currentFrame];
	if (vb.used + bytes > vb.capacity) {
		// The draws recorded so far still read from the full buffer, so it can't be resized in
		// place. Replace it with one twice as big, which is kept for the following frames.
		VkDeviceSize capacity = vb.capacity * 2;
		while (capacity < bytes) {
			capacity *= 2;
		}
		internal::debug("Growing the Vulkan vertex buffer of frame {} to {} KiB.", currentFrame,
		                capacity / 1024);
		retiredVertexBuffers[currentFrame].push_back(vb);
		vb = createVertexBuffer(capacity);
	}
	const VkDeviceSize offset = vb.used;
	std::memcpy(static_cast<char*>(vb.mapped) + offset, data, bytes);
	// Keep the next vertex block 4-byte aligned (it already is, but stay explicit).
	vb.used += (bytes + 3) & ~VkDeviceSize{ 3 };
	return { vb.buffer, offset };
}

void VulkanRenderer::createUniformBuffers() {
	VkPhysicalDeviceProperties props;
	vkGetPhysicalDeviceProperties(physicalDevice, &props);
	minUniformBufferOffsetAlignment = props.limits.minUniformBufferOffsetAlignment;

	// Each custom-shader draw takes its UBO's size, padded to minUniformBufferOffsetAlignment;
	// 256 KiB covers a thousand of them per frame, bindActiveProgramUniforms grows it if needed.
	constexpr VkDeviceSize capacity = 1u << 18u;
	uniformBuffers.resize(maxFramesInFlight);
	retiredUniformBuffers.resize(maxFramesInFlight);
	for (auto& ub : uniformBuffers) {
		ub = createUniformBuffer(capacity);
	}
}

DynamicUniformBuffer VulkanRenderer::createUniformBuffer(const VkDeviceSize capacity) {
	DynamicUniformBuffer ub;
	auto bufferInfo = vkStruct<VkBufferCreateInfo>(VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO);
	bufferInfo.size = capacity;
	bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	VK_CHECK(vkCreateBuffer(device, &bufferInfo, nullptr, &ub.buffer), "vkCreateBuffer");

	VkMemoryRequirements memReq;
	vkGetBufferMemoryRequirements(device, ub.buffer, &memReq);
	auto allocInfo = vkStruct<VkMemoryAllocateInfo>(VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO);
	allocInfo.allocationSize = memReq.size;
	allocInfo.memoryTypeIndex =
	    findMemoryType(memReq.memoryTypeBits,
	                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	VK_CHECK(vkAllocateMemory(device, &allocInfo, nullptr, &ub.memory), "vkAllocateMemory");
	VK_CHECK(vkBindBufferMemory(device, ub.buffer, ub.memory, 0), "vkBindBufferMemory");
	VK_CHECK(vkMapMemory(device, ub.memory, 0, capacity, 0, &ub.mapped), "vkMapMemory");
	ub.capacity = capacity;

	std::tie(ub.descriptorSet, ub.descriptorPool) = allocateDescriptorSet(uniformsSetLayout);
	VkDescriptorBufferInfo bufferDesc{};
	bufferDesc.buffer = ub.buffer;
	bufferDesc.offset = 0;
	bufferDesc.range = uniformDynamicRange;
	auto write = vkStruct<VkWriteDescriptorSet>(VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET);
	write.dstSet = ub.descriptorSet;
	write.dstBinding = 0;
	write.descriptorCount = 1;
	write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	write.pBufferInfo = &bufferDesc;
	vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
	return ub;
}

void VulkanRenderer::destroyUniformBuffer(DynamicUniformBuffer& ub) {
	if (ub.descriptorSet) {
		freeDescriptorSet(ub.descriptorSet, ub.descriptorPool, uniformsSetLayout);
	}
	if (ub.buffer) {
		vkDestroyBuffer(device, ub.buffer, nullptr);
	}
	if (ub.memory) {
		vkUnmapMemory(device, ub.memory);
		vkFreeMemory(device, ub.memory, nullptr);
	}
	ub = {};
}

std::vector<uint8_t>&
VulkanRenderer::prepareDrawUniforms(const UniformBlock& block,
                                    const VulkanShaderProgram* const program) {
	if (program) {
		drawUniforms.assign(program->uniformData.begin(), program->uniformData.end());
	} else {
		// Not zeroed, as the built-in shaders' draws write all of their uniforms
		drawUniforms.resize(block.size);
	}
	return drawUniforms;
}

void VulkanRenderer::bindUniforms(const VkCommandBuffer cmd, const UniformBlock& block,
                                  const std::vector<uint8_t>& data,
                                  const VkDescriptorSet textureSet) {
	if (block.size == 0 || block.pushConstants) {
		if (textureSet) {
			vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
			                        &textureSet, 0, nullptr);
		}
		if (block.size > 0) {
			vkCmdPushConstants(cmd, pipelineLayout,
			                   VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
			                   block.size, data.data());
		}
		return;
	}
	assert(data.size() == block.size);
	assert(block.size <= uniformDynamicRange);
	DynamicUniformBuffer& ub = uniformBuffers[currentFrame];
	VkDeviceSize aligned =
	    (ub.used + minUniformBufferOffsetAlignment - 1) & ~(minUniformBufferOffsetAlignment - 1);
	if (aligned + uniformDynamicRange > ub.capacity) {
		// The draws recorded so far still read from the full buffer, so it can't be resized in
		// place. Replace it with one twice as big, which is kept for the following frames.
		internal::debug("Growing the Vulkan uniform buffer of frame {} to {} KiB.", currentFrame,
		                ub.capacity * 2 / 1024);
		retiredUniformBuffers[currentFrame].push_back(ub);
		ub = createUniformBuffer(ub.capacity * 2);
		aligned = 0;
	}
	std::memcpy(static_cast<char*>(ub.mapped) + aligned, data.data(), block.size);
	// The next draw's may start within uniformDynamicRange, which only has to fit into the buffer
	ub.used = aligned + block.size;
	const uint32_t dynamicOffset = static_cast<uint32_t>(aligned);
	if (textureSet) { // both with one call
		const std::array sets = { textureSet, ub.descriptorSet };
		vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0,
		                        static_cast<uint32_t>(sets.size()), sets.data(), 1, &dynamicOffset);
	} else {
		vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 1, 1,
		                        &ub.descriptorSet, 1, &dynamicOffset);
	}
}

void VulkanRenderer::drawColored(PrimitiveType type, const float* xyVertices,
                                 std::size_t vertexCount, const Mat3& modelview, const Rgba color) {
	if (!frameActive || vertexCount == 0) {
		return;
	}
	std::array<float, 12> triangles;
	if (type == PrimitiveType::TriangleFan && vertexCount == 4) {
		triangles = triangulateQuad<2>(xyVertices);
		xyVertices = triangles.data();
		vertexCount = 6;
		type = PrimitiveType::Triangles;
	} else if (type == PrimitiveType::TriangleFan && vertexCount >= 3) {
		triangulateFan<2>(xyVertices, vertexCount, triangulatedVertices);
		xyVertices = triangulatedVertices.data();
		vertexCount = (vertexCount - 2) * 3;
		type = PrimitiveType::Triangles;
	}
	const VkDeviceSize bytes = vertexCount * 2 * sizeof(float);
	const auto [vertexBuffer, offset] = uploadVertices(xyVertices, bytes);

	ensureRenderPass(); // before the uniforms, as it might draw itself, see expandIntoSamples
	auto& uniforms = prepareDrawUniforms(coloredUniforms, nullptr);
	writeUniform(uniforms, coloredUniforms.offsets.modelview, modelview);
	writeUniform(uniforms, coloredUniforms.offsets.projection, projection);
	writeUniform(uniforms, coloredUniforms.offsets.color,
	             std::array{ color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha() });

	const VkCommandBuffer cmd = renderCommands;
	const BlendVariant variant = blendVariant();
	ensurePipelines(currentPassKind, variant);
	vkCmdBindPipeline(
	    cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
	    coloredPipelines[static_cast<size_t>(currentPassKind)][pipelineIndex(variant, type)]);
	vkCmdBindVertexBuffers(cmd, 0, 1, &vertexBuffer, &offset);
	bindUniforms(cmd, coloredUniforms, uniforms);
	vkCmdDraw(cmd, static_cast<uint32_t>(vertexCount), 1, 0, 0);
}

void VulkanRenderer::drawRoundedRect(const Mat3& modelview, const Rgba color, const float sizeX,
                                     const float sizeY, const float topLeft, const float topRight,
                                     const float bottomLeft, const float bottomRight) {
	if (!frameActive) {
		return;
	}
	static constexpr std::array<float, 12> unitSquare = { .5f, -.5f, .5f,  .5f, -.5f, -.5f,
		                                                  .5f, .5f,  -.5f, .5f, -.5f, -.5f };
	const VkDeviceSize bytes = unitSquare.size() * sizeof(float);
	const auto [vertexBuffer, offset] = uploadVertices(unitSquare.data(), bytes);

	ensureRenderPass(); // before the uniforms, as it might draw itself, see expandIntoSamples
	auto& uniforms = prepareDrawUniforms(roundedUniforms, nullptr);
	const DrawUniformOffsets& offsets = roundedUniforms.offsets;
	writeUniform(uniforms, offsets.modelview, modelview);
	writeUniform(uniforms, offsets.projection, projection);
	writeUniform(uniforms, offsets.color,
	             std::array{ color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha() });
	writeUniform(uniforms, offsets.size, std::array{ sizeX, sizeY });
	writeUniform(uniforms, offsets.cornerRadii,
	             std::array{ topLeft, topRight, bottomLeft, bottomRight });

	const VkCommandBuffer cmd = renderCommands;
	const BlendVariant variant = blendVariant();
	ensurePipelines(currentPassKind, variant);
	const VkPipeline pipeline =
	    roundedPipelines[static_cast<size_t>(currentPassKind)][static_cast<size_t>(variant)];
	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	vkCmdBindVertexBuffers(cmd, 0, 1, &vertexBuffer, &offset);
	bindUniforms(cmd, roundedUniforms, uniforms);
	vkCmdDraw(cmd, 6, 1, 0, 0);
}

namespace {

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
	const std::array<VkDescriptorPoolSize, 2> poolSizes = { {
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, descriptorPoolTextures },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, descriptorPoolUniformBuffers },
	} };
	auto info = vkStruct<VkDescriptorPoolCreateInfo>(VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO);
	info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	info.maxSets = descriptorPoolTextures + descriptorPoolUniformBuffers;
	info.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	info.pPoolSizes = poolSizes.data();
	VkDescriptorPool pool = VK_NULL_HANDLE;
	VK_CHECK(vkCreateDescriptorPool(device, &info, nullptr, &pool), "vkCreateDescriptorPool");
	descriptorPools.push_back({ .pool = pool });
}

std::pair<VkDescriptorSet, VkDescriptorPool>
VulkanRenderer::allocateDescriptorSet(const VkDescriptorSetLayout layout) {
	auto dsAlloc =
	    vkStruct<VkDescriptorSetAllocateInfo>(VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO);
	dsAlloc.descriptorSetCount = 1;
	dsAlloc.pSetLayouts = &layout;
	VkDescriptorSet set = VK_NULL_HANDLE;
	const bool texture = layout == texturedSetLayout;
	const auto hasRoom = [texture](const DescriptorPool& pool) {
		return texture ? pool.textures < descriptorPoolTextures
		               : pool.uniformBuffers < descriptorPoolUniformBuffers;
	};
	const auto count = [texture](DescriptorPool& pool) {
		++(texture ? pool.textures : pool.uniformBuffers);
	};
	// Earlier pools might have room again, as descriptor sets are freed into them
	for (auto& pool : descriptorPools) {
		if (!hasRoom(pool)) {
			continue;
		}
		dsAlloc.descriptorPool = pool.pool;
		const VkResult result = vkAllocateDescriptorSets(device, &dsAlloc, &set);
		if (result == VK_SUCCESS) {
			count(pool);
			return { set, pool.pool };
		}
		// Even if there's room, it might be too fragmented
		if (result != VK_ERROR_OUT_OF_POOL_MEMORY && result != VK_ERROR_FRAGMENTED_POOL) {
			VK_CHECK(result, "vkAllocateDescriptorSets");
		}
	}
	internal::debug("All {} Vulkan descriptor pools are full, adding another one.",
	                descriptorPools.size());
	createDescriptorPool();
	auto& pool = descriptorPools.back();
	dsAlloc.descriptorPool = pool.pool;
	VK_CHECK(vkAllocateDescriptorSets(device, &dsAlloc, &set), "vkAllocateDescriptorSets");
	count(pool);
	return { set, pool.pool };
}

void VulkanRenderer::freeDescriptorSet(const VkDescriptorSet set, const VkDescriptorPool pool,
                                       const VkDescriptorSetLayout layout) {
	vkFreeDescriptorSets(device, pool, 1, &set);
	const auto it = std::ranges::find(descriptorPools, pool, &DescriptorPool::pool);
	assert(it != descriptorPools.end());
	--(layout == texturedSetLayout ? it->textures : it->uniformBuffers);
}

void VulkanRenderer::createPipelineLayout() {
	if (!texturedSetLayout) {
		VkDescriptorSetLayoutBinding samplerBinding{};
		samplerBinding.binding = 0;
		samplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerBinding.descriptorCount = 1;
		samplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		auto setLayoutInfo = vkStruct<VkDescriptorSetLayoutCreateInfo>(
		    VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO);
		setLayoutInfo.bindingCount = 1;
		setLayoutInfo.pBindings = &samplerBinding;
		VK_CHECK(vkCreateDescriptorSetLayout(device, &setLayoutInfo, nullptr, &texturedSetLayout),
		         "vkCreateDescriptorSetLayout");
	}
	if (!uniformsSetLayout) {
		VkDescriptorSetLayoutBinding uboBinding{};
		uboBinding.binding = 0;
		uboBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		uboBinding.descriptorCount = 1;
		uboBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		auto setLayoutInfo = vkStruct<VkDescriptorSetLayoutCreateInfo>(
		    VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO);
		setLayoutInfo.bindingCount = 1;
		setLayoutInfo.pBindings = &uboBinding;
		VK_CHECK(vkCreateDescriptorSetLayout(device, &setLayoutInfo, nullptr, &uniformsSetLayout),
		         "vkCreateDescriptorSetLayout");
	}

	VkPushConstantRange pushRange{};
	pushRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushRange.size = maxPushConstantsSize;
	const std::array<VkDescriptorSetLayout, 2> setLayouts = { texturedSetLayout,
		                                                      uniformsSetLayout };
	auto layoutInfo =
	    vkStruct<VkPipelineLayoutCreateInfo>(VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO);
	layoutInfo.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
	layoutInfo.pSetLayouts = setLayouts.data();
	layoutInfo.pushConstantRangeCount = 1;
	layoutInfo.pPushConstantRanges = &pushRange;
	VK_CHECK(vkCreatePipelineLayout(device, &layoutInfo, nullptr, &pipelineLayout),
	         "vkCreatePipelineLayout");
}

void VulkanRenderer::buildTexturedPipelines(
    const VkShaderModule vert, const VkShaderModule frag, const VkRenderPass targetRenderPass,
    const VkSampleCountFlagBits samples, const BlendVariant variant,
    std::array<VkPipeline, VulkanShaderProgram::pipelineCount>& out) {
	const std::array stages = { shaderStage(VK_SHADER_STAGE_VERTEX_BIT, vert),
		                        shaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, frag) };

	VkVertexInputBindingDescription binding{};
	binding.binding = 0;
	binding.stride = 4 * sizeof(float); // x, y, u, v
	binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	const std::array<VkVertexInputAttributeDescription, 2> attributes = {
		VkVertexInputAttributeDescription{ 0, 0, VK_FORMAT_R32G32_SFLOAT, 0 },
		VkVertexInputAttributeDescription{ 1, 0, VK_FORMAT_R32G32_SFLOAT, 2 * sizeof(float) },
	};
	auto vertexInput = vkStruct<VkPipelineVertexInputStateCreateInfo>(
	    VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO);
	vertexInput.vertexBindingDescriptionCount = 1;
	vertexInput.pVertexBindingDescriptions = &binding;
	vertexInput.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size());
	vertexInput.pVertexAttributeDescriptions = attributes.data();

	auto viewportState = vkStruct<VkPipelineViewportStateCreateInfo>(
	    VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO);
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	auto rasterizer = vkStruct<VkPipelineRasterizationStateCreateInfo>(
	    VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO);
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.cullMode = VK_CULL_MODE_NONE;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.lineWidth = 1.f;

	auto multisampling = vkStruct<VkPipelineMultisampleStateCreateInfo>(
	    VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO);
	multisampling.rasterizationSamples = samples;

	const std::array<VkDynamicState, 2> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT,
		                                                  VK_DYNAMIC_STATE_SCISSOR };
	auto dynamicState = vkStruct<VkPipelineDynamicStateCreateInfo>(
	    VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO);
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	const std::array<PrimitiveType, 4> topologies = { PrimitiveType::Triangles,
		                                              PrimitiveType::TriangleStrip,
		                                              PrimitiveType::TriangleFan,
		                                              PrimitiveType::Lines };

	const VkPipelineColorBlendAttachmentState blendAttachment = blendAttachmentState(variant);
	auto colorBlending = vkStruct<VkPipelineColorBlendStateCreateInfo>(
	    VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO);
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &blendAttachment;

	for (const PrimitiveType type : topologies) {
		auto inputAssembly = vkStruct<VkPipelineInputAssemblyStateCreateInfo>(
		    VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO);
		inputAssembly.topology = toVkTopology(type);

		auto pipelineInfo =
		    vkStruct<VkGraphicsPipelineCreateInfo>(VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO);
		pipelineInfo.stageCount = static_cast<uint32_t>(stages.size());
		pipelineInfo.pStages = stages.data();
		pipelineInfo.pVertexInputState = &vertexInput;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = pipelineLayout;
		pipelineInfo.renderPass = targetRenderPass;
		VK_CHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr,
		                                   &out[pipelineIndex(variant, type)]),
		         "vkCreateGraphicsPipelines");
	}
}

BlendVariant VulkanRenderer::blendVariant() const {
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

size_t VulkanRenderer::pipelineIndex(const BlendVariant variant, const PrimitiveType type) {
	return static_cast<size_t>(variant) * VulkanShaderProgram::topologyCount +
	       static_cast<size_t>(type);
}

void VulkanRenderer::setBlendingEnabled(const bool enabled) {
	blendingEnabled = enabled;
}

bool VulkanRenderer::isBlendingEnabled() const {
	return blendingEnabled;
}

void VulkanRenderer::setSpriteBlend(const SpriteBlend blend) {
	spriteBlend = blend;
}

VulkanRenderer::SpriteBlend VulkanRenderer::getSpriteBlend() const {
	return spriteBlend;
}

void VulkanRenderer::setAlphaOnlyWrite(const bool alphaOnly) {
	alphaOnlyWrite = alphaOnly;
}

bool VulkanRenderer::isAlphaOnlyWrite() const {
	return alphaOnlyWrite;
}

std::unique_ptr<VulkanShaderProgram> VulkanRenderer::createShaderProgram(CompiledProgram compiled) {
	auto program = std::make_unique<VulkanShaderProgram>();
	program->vertexSpirv = std::move(compiled.vertexSpirv);
	program->fragmentSpirv = std::move(compiled.fragmentSpirv);
	program->uniformBlock =
	    uniformBlock(compiled.uniforms, compiled.uniformBlockSize, compiled.pushConstants);
	program->uniforms = std::move(compiled.uniforms);
	if (program->uniformBlock.size > uniformDynamicRange) {
		throw std::runtime_error("Shader uniform block (" +
		                         std::to_string(program->uniformBlock.size) +
		                         " bytes) exceeds VulkanRenderer::uniformDynamicRange (" +
		                         std::to_string(uniformDynamicRange) + ").");
	}
	program->uniformData.resize(program->uniformBlock.size);
	ensureProgramPipelines(*program, PassKind::Ldr, BlendVariant::Composite);
	if (msaaSupported) {
		ensureProgramPipelines(*program, PassKind::LdrMsaa, BlendVariant::Composite);
	}
	return program;
}

std::unique_ptr<VulkanShaderProgram>
VulkanRenderer::createShaderProgram(const ShaderBytecode& bytecode) {
	CompiledProgram compiled;
	compiled.vertexSpirv.assign(bytecode.vertex.begin(), bytecode.vertex.end());
	compiled.fragmentSpirv.assign(bytecode.fragment.begin(), bytecode.fragment.end());
	compiled.uniforms = shaderUniforms(bytecode);
	compiled.uniformBlockSize = bytecode.uniformBlockSize;
	compiled.pushConstants = bytecode.pushConstants;
	return createShaderProgram(std::move(compiled));
}

void VulkanRenderer::destroyShaderProgram(VulkanShaderProgram& program) {
	if (!device) {
		return;
	}
	// The pipelines might still be used by the current or an in-flight command buffer
	destroyLater([this, pipelines = program.pipelines]() {
		for (const auto& kindPipelines : pipelines) {
			for (const VkPipeline pipeline : kindPipelines) {
				if (pipeline) {
					vkDestroyPipeline(device, pipeline, nullptr);
				}
			}
		}
	});
	program.pipelines = {};
	if (activeShaderProgram == &program) {
		activeShaderProgram = nullptr;
	}
}

void VulkanRenderer::setActiveShaderProgram(const VulkanShaderProgram* program) {
	activeShaderProgram = program;
}

void VulkanRenderer::setActiveProgramUniform(const uint32_t offset, const void* const data,
                                             const uint32_t bytes) {
	assert(activeShaderProgram);
	assert(offset + bytes <= activeShaderProgram->uniformData.size());
	// Only copied from on the CPU, see prepareDrawUniforms()
	std::memcpy(activeShaderProgram->uniformData.data() + offset, data, bytes);
}

void VulkanRenderer::submitOneTime(const std::function<void(VkCommandBuffer)>& record) {
	auto allocInfo =
	    vkStruct<VkCommandBufferAllocateInfo>(VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO);
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;
	VkCommandBuffer cmd = VK_NULL_HANDLE;
	VK_CHECK(vkAllocateCommandBuffers(device, &allocInfo, &cmd), "vkAllocateCommandBuffers");

	auto beginInfo =
	    vkStruct<VkCommandBufferBeginInfo>(VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO);
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	VK_CHECK(vkBeginCommandBuffer(cmd, &beginInfo), "vkBeginCommandBuffer");
	record(cmd);
	VK_CHECK(vkEndCommandBuffer(cmd), "vkEndCommandBuffer");

	auto submit = vkStruct<VkSubmitInfo>(VK_STRUCTURE_TYPE_SUBMIT_INFO);
	submit.commandBufferCount = 1;
	submit.pCommandBuffers = &cmd;
	VK_CHECK(vkQueueSubmit(graphicsQueue, 1, &submit, VK_NULL_HANDLE), "vkQueueSubmit");
	vkQueueWaitIdle(graphicsQueue);
	vkFreeCommandBuffers(device, commandPool, 1, &cmd);
}

void VulkanRenderer::recordUpload(const std::function<void(VkCommandBuffer)>& record) {
	const VkCommandBuffer cmd = uploadCommandBuffers[currentFrame];
	if (!uploadPending) {
		if (!frameActive) {
			// Its last submission might still be running, beginFrame() would wait for it anyway
			vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE,
			                std::numeric_limits<uint64_t>::max());
		}
		auto beginInfo =
		    vkStruct<VkCommandBufferBeginInfo>(VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO);
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		VK_CHECK(vkBeginCommandBuffer(cmd, &beginInfo), "vkBeginCommandBuffer");
		uploadPending = true;
	}
	record(cmd);
}

void VulkanRenderer::keepStagingBuffer(const VkBuffer buffer, const VkDeviceMemory memory) {
	pendingStagingBuffers.push_back({ buffer, memory });
}

std::vector<VkCommandBuffer>
VulkanRenderer::withPendingUploads(const VkCommandBuffer frameCommands) {
	if (!uploadPending) {
		return { frameCommands };
	}
	const VkCommandBuffer uploads = uploadCommandBuffers[currentFrame];
	VK_CHECK(vkEndCommandBuffer(uploads), "vkEndCommandBuffer");
	uploadPending = false;
	auto& retired = retiredStagingBuffers[currentFrame];
	retired.insert(retired.end(), pendingStagingBuffers.begin(), pendingStagingBuffers.end());
	pendingStagingBuffers.clear();
	return { uploads, frameCommands };
}

void VulkanRenderer::destroyLater(std::function<void()> destroy) {
	pendingDestructions.push_back({ std::move(destroy), static_cast<int>(maxFramesInFlight) + 1 });
}

namespace {
VkFilter toVkFilter(const TextureFilter filter) {
	return filter == TextureFilter::NearestNeighbor ? VK_FILTER_NEAREST : VK_FILTER_LINEAR;
}

/// Number of levels down to 1x1, like glGenerateMipmap creates
uint32_t mipLevelCount(const int width, const int height) {
	return static_cast<uint32_t>(std::bit_width(static_cast<uint32_t>(std::max(width, height))));
}
} // namespace

std::unique_ptr<VulkanTexture>
VulkanRenderer::createTexture(const int width, const int height, const unsigned int format,
                              const unsigned int type, const unsigned char* const data,
                              const unsigned char* const* const rowPointers, const bool mipmap) {
	internal::StartupProfiler _{ "createTexture" };
	if (type != glUnsignedByte || (format != glRGBA && format != glRGB && format != glBGR)) {
		internal::error("Vulkan backend: unsupported texture format/type (0x{:x}/0x{:x}).", format,
		                type);
		return nullptr;
	}

	auto texture = std::make_unique<VulkanTexture>();
	texture->width = width;
	texture->height = height;
	createTextureImage(*texture, mipmap ? mipLevelCount(width, height) : 1);

	if (data || rowPointers) {
		updateTextureBytes(*texture, width, height,
		                   toRgba8(width, height, format, data, rowPointers).data());
	} else {
		// No initial pixels (e.g. a render target): just move it to the shader-read layout.
		recordUpload([&](VkCommandBuffer cmd) {
			auto barrier = vkStruct<VkImageMemoryBarrier>(VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER);
			barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.image = texture->image;
			barrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, texture->mipLevels, 0, 1 };
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			                     VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1,
			                     &barrier);
		});
	}

	if (App::isPixelArt()) {
		texture->magFilter = VK_FILTER_NEAREST;
	}
	createTextureDescriptor(*texture);
	return texture;
}

void VulkanRenderer::createTextureImage(VulkanTexture& texture, const uint32_t mipLevels) {
	texture.mipLevels = mipLevels;
	auto imageInfo = vkStruct<VkImageCreateInfo>(VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO);
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	imageInfo.extent = { static_cast<uint32_t>(texture.width),
		                 static_cast<uint32_t>(texture.height), 1 };
	imageInfo.mipLevels = mipLevels;
	imageInfo.arrayLayers = 1;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	// TRANSFER_SRC for generating the mipmaps, and for copying the pixels to a new image when
	// they're enabled later on, see enableMipmaps. VK_FORMAT_R8G8B8A8_UNORM always supports
	// linear blits.
	imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
	                  VK_IMAGE_USAGE_SAMPLED_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	VK_CHECK(vkCreateImage(device, &imageInfo, nullptr, &texture.image), "vkCreateImage");

	VkMemoryRequirements memReq;
	vkGetImageMemoryRequirements(device, texture.image, &memReq);
	auto allocInfo = vkStruct<VkMemoryAllocateInfo>(VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO);
	allocInfo.allocationSize = memReq.size;
	allocInfo.memoryTypeIndex =
	    findMemoryType(memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	VK_CHECK(vkAllocateMemory(device, &allocInfo, nullptr, &texture.memory), "vkAllocateMemory");
	VK_CHECK(vkBindImageMemory(device, texture.image, texture.memory, 0), "vkBindImageMemory");
}

void VulkanRenderer::createTextureDescriptor(VulkanTexture& texture) {
	auto viewInfo = vkStruct<VkImageViewCreateInfo>(VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO);
	viewInfo.image = texture.image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	viewInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, texture.mipLevels, 0, 1 };
	VK_CHECK(vkCreateImageView(device, &viewInfo, nullptr, &texture.view), "vkCreateImageView");
	createTextureSampler(texture);
	allocateTextureDescriptor(texture);
}

void VulkanRenderer::createTextureSampler(VulkanTexture& texture) {
	auto samplerInfo = vkStruct<VkSamplerCreateInfo>(VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO);
	samplerInfo.magFilter = texture.magFilter;
	samplerInfo.minFilter = texture.minFilter;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	// like GL_LINEAR_MIPMAP_LINEAR or GL_NEAREST_MIPMAP_NEAREST
	samplerInfo.mipmapMode = texture.minFilter == VK_FILTER_NEAREST
	                             ? VK_SAMPLER_MIPMAP_MODE_NEAREST
	                             : VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.maxLod = static_cast<float>(texture.mipLevels - 1);
	VK_CHECK(vkCreateSampler(device, &samplerInfo, nullptr, &texture.sampler), "vkCreateSampler");
}

void VulkanRenderer::recordMipmapGeneration(const VkCommandBuffer cmd,
                                            const VulkanTexture& texture) {
	auto barrier = vkStruct<VkImageMemoryBarrier>(VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER);
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = texture.image;
	barrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	auto width = static_cast<int32_t>(texture.width);
	auto height = static_cast<int32_t>(texture.height);
	for (uint32_t level = 1; level < texture.mipLevels; ++level) {
		// The previous level has been written, now it's read from
		barrier.subresourceRange.baseMipLevel = level - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
		                     0, nullptr, 0, nullptr, 1, &barrier);

		const int32_t nextWidth = std::max(width / 2, 1);
		const int32_t nextHeight = std::max(height / 2, 1);
		VkImageBlit blit{};
		blit.srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, level - 1, 0, 1 };
		blit.srcOffsets[1] = { width, height, 1 };
		blit.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, level, 0, 1 };
		blit.dstOffsets[1] = { nextWidth, nextHeight, 1 };
		vkCmdBlitImage(cmd, texture.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, texture.image,
		               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT,
		                     VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1,
		                     &barrier);
		width = nextWidth;
		height = nextHeight;
	}
	// The last level has only been written
	barrier.subresourceRange.baseMipLevel = texture.mipLevels - 1;
	barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
	                     0, 0, nullptr, 0, nullptr, 1, &barrier);
}

void VulkanRenderer::updateTextureBytes(VulkanTexture& texture, const int width, const int height,
                                        const unsigned char* const rgba) {
	const VkDeviceSize size = static_cast<VkDeviceSize>(width) * height * 4;

	auto bufferInfo = vkStruct<VkBufferCreateInfo>(VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO);
	bufferInfo.size = size;
	bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	VkBuffer staging = VK_NULL_HANDLE;
	VK_CHECK(vkCreateBuffer(device, &bufferInfo, nullptr, &staging), "vkCreateBuffer");
	VkMemoryRequirements memReq;
	vkGetBufferMemoryRequirements(device, staging, &memReq);
	auto allocInfo = vkStruct<VkMemoryAllocateInfo>(VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO);
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

	recordUpload([&](VkCommandBuffer cmd) {
		auto toTransfer = vkStruct<VkImageMemoryBarrier>(VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER);
		toTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		toTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		toTransfer.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		toTransfer.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		toTransfer.image = texture.image;
		toTransfer.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, texture.mipLevels, 0, 1 };
		toTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		// After earlier frames are done sampling the old pixels
		vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		                     VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1,
		                     &toTransfer);

		VkBufferImageCopy region{};
		region.imageSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
		region.imageExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1 };
		vkCmdCopyBufferToImage(cmd, staging, texture.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
		                       &region);
		if (texture.mipLevels > 1) {
			recordMipmapGeneration(cmd, texture);
			return;
		}

		auto toShader = vkStruct<VkImageMemoryBarrier>(VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER);
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

	keepStagingBuffer(staging, stagingMemory);
}

void VulkanRenderer::enableMipmaps(VulkanTexture& texture) {
	const uint32_t mipLevels = mipLevelCount(texture.width, texture.height);
	if (texture.mipLevels > 1 || mipLevels == 1 || !texture.image) {
		return;
	}
	// The number of levels of an image can't be changed, so copy the pixels into a new one
	VulkanTexture mipmapped;
	mipmapped.width = texture.width;
	mipmapped.height = texture.height;
	mipmapped.minFilter = texture.minFilter;
	mipmapped.magFilter = texture.magFilter;
	createTextureImage(mipmapped, mipLevels);
	recordUpload([&](VkCommandBuffer cmd) {
		VkImageMemoryBarrier barriers[2]{};
		for (auto& barrier : barriers) {
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		}
		barriers[0].image = texture.image;
		barriers[0].subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		barriers[0].oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barriers[0].newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barriers[0].dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barriers[1].image = mipmapped.image;
		barriers[1].subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, mipLevels, 0, 1 };
		barriers[1].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		barriers[1].newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barriers[1].dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		                     VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 2,
		                     barriers);

		VkImageCopy region{};
		region.srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
		region.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
		region.extent = { static_cast<uint32_t>(texture.width),
			              static_cast<uint32_t>(texture.height), 1 };
		vkCmdCopyImage(cmd, texture.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, mipmapped.image,
		               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		// Draws recorded earlier in this frame still sample the old image, as the uploads are done
		// before them
		barriers[0].oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barriers[0].newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barriers[0].srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barriers[0].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT,
		                     VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1,
		                     barriers);

		recordMipmapGeneration(cmd, mipmapped);
	});
	createTextureDescriptor(mipmapped);
	// The old image and descriptor set might still be used by the current or an in-flight command
	// buffer
	destroyTexture(texture);
	texture = mipmapped;
}

void VulkanRenderer::setTextureFilter(VulkanTexture& texture, const TextureFilter minification,
                                      const TextureFilter magnification) {
	const VkFilter minFilter = toVkFilter(minification);
	const VkFilter magFilter = toVkFilter(magnification);
	if ((texture.minFilter == minFilter && texture.magFilter == magFilter) || !texture.view) {
		return;
	}
	// The old sampler and descriptor set might still be used by the current or an in-flight
	// command buffer
	VulkanTexture old;
	old.sampler = texture.sampler;
	old.descriptorSet = texture.descriptorSet;
	old.descriptorPool = texture.descriptorPool;
	destroyTexture(old);
	texture.minFilter = minFilter;
	texture.magFilter = magFilter;
	createTextureSampler(texture);
	allocateTextureDescriptor(texture);
}

void VulkanRenderer::destroyTexture(VulkanTexture& texture) {
	if (!device) {
		return;
	}
	destroyLater([this, texture]() { freeTextureResources(texture); });
	texture = {}; // the copy owns the handles now
}

void VulkanRenderer::freeTextureResources(const VulkanTexture& texture) {
	if (texture.descriptorSet) {
		freeDescriptorSet(texture.descriptorSet, texture.descriptorPool, texturedSetLayout);
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
	// Multiplying with color is the same as mixing towards black by 1 - color
	drawSprite(texture, xyuv, vertexCount, type, modelview, Rgb(0, 0, 0),
	           { 1.f - color.getRed(), 1.f - color.getGreen(), 1.f - color.getBlue(),
	             1.f - color.getAlpha() });
}

void VulkanRenderer::drawSprite(const VulkanTexture& texture, const float* xyuv,
                                std::size_t vertexCount, PrimitiveType type, const Mat3& modelview,
                                const Rgb lerpColor, const std::array<float, 4> lerpFactors) {
	if (!frameActive || vertexCount == 0) {
		return;
	}
	std::array<float, 24> triangles;
	if (type == PrimitiveType::TriangleFan && vertexCount == 4) {
		triangles = triangulateQuad<4>(xyuv);
		xyuv = triangles.data();
		vertexCount = 6;
		type = PrimitiveType::Triangles;
	} else if (type == PrimitiveType::TriangleFan && vertexCount >= 3) {
		triangulateFan<4>(xyuv, vertexCount, triangulatedVertices);
		xyuv = triangulatedVertices.data();
		vertexCount = (vertexCount - 2) * 3;
		type = PrimitiveType::Triangles;
	}
	const VkDeviceSize bytes = vertexCount * 4 * sizeof(float);
	const auto [vertexBuffer, offset] = uploadVertices(xyuv, bytes);

	const UniformBlock& block =
	    activeShaderProgram ? activeShaderProgram->uniformBlock : texturedUniforms;
	ensureRenderPass(); // before the uniforms, as it might draw itself, see expandIntoSamples
	auto& uniforms = prepareDrawUniforms(block, activeShaderProgram);
	writeUniform(uniforms, block.offsets.modelview, modelview);
	writeUniform(uniforms, block.offsets.projection, projection);
	if (!activeShaderProgram) { // a ShaderProgram's other uniforms are its own, like on OpenGL
		writeUniform(uniforms, block.offsets.lerpColor,
		             std::array{ lerpColor.getRed(), lerpColor.getGreen(), lerpColor.getBlue() });
		writeUniform(uniforms, block.offsets.lerpFactors, lerpFactors);
	}

	const VkCommandBuffer cmd = renderCommands;
	const BlendVariant variant = blendVariant();
	const size_t index = pipelineIndex(variant, type);
	const auto kind = static_cast<size_t>(currentPassKind);
	VkPipeline pipeline = VK_NULL_HANDLE;
	if (activeShaderProgram) {
		ensureProgramPipelines(*activeShaderProgram, currentPassKind, variant);
		pipeline = activeShaderProgram->pipelines[kind][index];
	} else {
		ensurePipelines(currentPassKind, variant);
		pipeline = texturedPipelines[kind][index];
	}
	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	bindUniforms(cmd, block, uniforms, texture.descriptorSet);
	vkCmdBindVertexBuffers(cmd, 0, 1, &vertexBuffer, &offset);
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
	endCurrentRenderPass();
	resolveSwapchain();
	VK_CHECK(vkEndCommandBuffer(cmd), "vkEndCommandBuffer");

	const VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	auto submit = vkStruct<VkSubmitInfo>(VK_STRUCTURE_TYPE_SUBMIT_INFO);
	if (!imageAvailableWaited) {
		submit.waitSemaphoreCount = 1;
		submit.pWaitSemaphores = &imageAvailableSemaphores[currentFrame];
		submit.pWaitDstStageMask = &waitStage;
	}
	const auto commands = withPendingUploads(cmd);
	submit.commandBufferCount = static_cast<uint32_t>(commands.size());
	submit.pCommandBuffers = commands.data();
	vkResetFences(device, 1, &readbackFence);
	VK_CHECK(vkQueueSubmit(graphicsQueue, 1, &submit, readbackFence), "vkQueueSubmit");
	vkWaitForFences(device, 1, &readbackFence, VK_TRUE, std::numeric_limits<uint64_t>::max());
	imageAvailableWaited = true;

	// Copy the requested region into a host-visible buffer.
	const VkDeviceSize size = pixelCount * 4;
	auto bufferInfo = vkStruct<VkBufferCreateInfo>(VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO);
	bufferInfo.size = size;
	bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	VkBuffer staging = VK_NULL_HANDLE;
	VK_CHECK(vkCreateBuffer(device, &bufferInfo, nullptr, &staging), "vkCreateBuffer");
	VkMemoryRequirements memReq;
	vkGetBufferMemoryRequirements(device, staging, &memReq);
	auto allocInfo = vkStruct<VkMemoryAllocateInfo>(VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO);
	allocInfo.allocationSize = memReq.size;
	allocInfo.memoryTypeIndex = findMemoryType(
	    memReq.memoryTypeBits,
	    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	VkDeviceMemory stagingMemory = VK_NULL_HANDLE;
	VK_CHECK(vkAllocateMemory(device, &allocInfo, nullptr, &stagingMemory), "vkAllocateMemory");
	VK_CHECK(vkBindBufferMemory(device, staging, stagingMemory, 0), "vkBindBufferMemory");

	VkImage image = swapchainImages[currentImageIndex];
	submitOneTime([&](VkCommandBuffer c) {
		auto toSrc = vkStruct<VkImageMemoryBarrier>(VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER);
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
		// x and y are from the bottom left like glReadPixels
		region.imageOffset = { x, static_cast<int32_t>(swapchainExtent.height) - y - height, 0 };
		region.imageExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1 };
		vkCmdCopyImageToBuffer(c, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, staging, 1, &region);

		// Move the image back to what the resumed loadRenderPass expects as its initial layout
		auto toPresent = vkStruct<VkImageMemoryBarrier>(VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER);
		toPresent.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		toPresent.newLayout = resolveRenderPass ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		                                        : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
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
	auto beginInfo =
	    vkStruct<VkCommandBufferBeginInfo>(VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO);
	VK_CHECK(vkBeginCommandBuffer(cmd, &beginInfo), "vkBeginCommandBuffer");
	beginCurrentRenderPass(VK_ATTACHMENT_LOAD_OP_LOAD);
}

std::string VulkanRenderer::getGraphicsApi() const {
	VkPhysicalDeviceProperties props;
	vkGetPhysicalDeviceProperties(physicalDevice, &props);
	std::string api = "Vulkan " + std::to_string(VK_API_VERSION_MAJOR(props.apiVersion)) + "." +
	                  std::to_string(VK_API_VERSION_MINOR(props.apiVersion)) + "." +
	                  std::to_string(VK_API_VERSION_PATCH(props.apiVersion));
	// Core since Vulkan 1.2, but we ask for 1.1. It can be queried without enabling it.
	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount,
	                                     extensions.data());
	if (std::ranges::none_of(extensions, [](const VkExtensionProperties& extension) {
		return std::strcmp(extension.extensionName, VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME) == 0;
	})) {
		return api;
	}
	auto driver = vkStruct<VkPhysicalDeviceDriverPropertiesKHR>(
	    VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES_KHR);
	auto props2 =
	    vkStruct<VkPhysicalDeviceProperties2>(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2);
	props2.pNext = &driver;
	vkGetPhysicalDeviceProperties2(physicalDevice, &props2);
	// E.g. "(NVIDIA) 580.95.05", like OpenGL's "NVIDIA 580.95.05"
	return api + " (" + driver.driverName + ") " + driver.driverInfo;
}

bool VulkanRenderer::areTimestampsSupported() const {
	return resetQueryPool != nullptr;
}

VkQueryPool VulkanRenderer::createTimestampPool(const uint32_t count) {
	auto info = vkStruct<VkQueryPoolCreateInfo>(VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO);
	info.queryType = VK_QUERY_TYPE_TIMESTAMP;
	info.queryCount = count;
	VkQueryPool pool = VK_NULL_HANDLE;
	VK_CHECK(vkCreateQueryPool(device, &info, nullptr, &pool), "vkCreateQueryPool");
	resetQueryPool(device, pool, 0, count);
	return pool;
}

void VulkanRenderer::destroyTimestampPool(const VkQueryPool pool) {
	if (!device) {
		return;
	}
	destroyLater([this, pool]() { vkDestroyQueryPool(device, pool, nullptr); });
}

bool VulkanRenderer::writeTimestamp(const VkQueryPool pool, const uint32_t query) {
	if (!frameActive) {
		return false;
	}
	vkCmdWriteTimestamp(renderPassActive ? renderCommands : commandBuffers[currentFrame],
	                    VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, pool, query);
	return true;
}

std::optional<std::chrono::nanoseconds> VulkanRenderer::takeElapsedTime(const VkQueryPool pool,
                                                                        const uint32_t first) {
	std::array<uint64_t, 2> ticks{};
	const VkResult result =
	    vkGetQueryPoolResults(device, pool, first, 2, sizeof(ticks), ticks.data(), sizeof(uint64_t),
	                          VK_QUERY_RESULT_64_BIT);
	if (result == VK_NOT_READY) {
		return std::nullopt;
	}
	VK_CHECK(result, "vkGetQueryPoolResults");
	// The commands that wrote them are done, as their results are there
	resetQueryPool(device, pool, first, 2);
	const uint64_t elapsed = (ticks[1] - ticks[0]) & timestampMask;
	return std::chrono::nanoseconds(
	    static_cast<std::chrono::nanoseconds::rep>(static_cast<double>(elapsed) * timestampPeriod));
}

void VulkanRenderer::allocateTextureDescriptor(VulkanTexture& texture) {
	std::tie(texture.descriptorSet, texture.descriptorPool) =
	    allocateDescriptorSet(texturedSetLayout);

	VkDescriptorImageInfo imgInfo{};
	imgInfo.sampler = texture.sampler;
	imgInfo.imageView = texture.view;
	imgInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	auto write = vkStruct<VkWriteDescriptorSet>(VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET);
	write.dstSet = texture.descriptorSet;
	write.dstBinding = 0;
	write.descriptorCount = 1;
	write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	write.pImageInfo = &imgInfo;
	vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
}

std::unique_ptr<VulkanFramebuffer> VulkanRenderer::createFramebuffer(const int width,
                                                                     const int height,
                                                                     const bool antiAliased,
                                                                     const bool hdr) {
	auto fb = std::make_unique<VulkanFramebuffer>();
	fb->width = width;
	fb->height = height;
	fb->hdr = hdr;
	const VkFormat format = hdr ? hdrFormat : swapchainImageFormat;
	const auto& passes = offscreenRenderPasses[hdr ? 1 : 0];
	fb->color.width = width;
	fb->color.height = height;

	auto imageInfo = vkStruct<VkImageCreateInfo>(VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO);
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.format = format;
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
	auto allocInfo = vkStruct<VkMemoryAllocateInfo>(VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO);
	allocInfo.allocationSize = memReq.size;
	allocInfo.memoryTypeIndex =
	    findMemoryType(memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	VK_CHECK(vkAllocateMemory(device, &allocInfo, nullptr, &fb->color.memory), "vkAllocateMemory");
	VK_CHECK(vkBindImageMemory(device, fb->color.image, fb->color.memory, 0), "vkBindImageMemory");

	// Clear to transparent and move to SHADER_READ_ONLY (the offscreen render pass's initial
	// layout, and the layout it's sampled from).
	recordUpload([&](VkCommandBuffer c) {
		auto toDst = vkStruct<VkImageMemoryBarrier>(VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER);
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
		auto toRead = vkStruct<VkImageMemoryBarrier>(VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER);
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

	auto viewInfo = vkStruct<VkImageViewCreateInfo>(VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO);
	viewInfo.image = fb->color.image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	VK_CHECK(vkCreateImageView(device, &viewInfo, nullptr, &fb->color.view), "vkCreateImageView");

	const VkFilter filter = App::isPixelArt() ? VK_FILTER_NEAREST : VK_FILTER_LINEAR;
	fb->color.minFilter = filter;
	fb->color.magFilter = filter;
	auto samplerInfo = vkStruct<VkSamplerCreateInfo>(VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO);
	samplerInfo.magFilter = filter;
	samplerInfo.minFilter = filter;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	VK_CHECK(vkCreateSampler(device, &samplerInfo, nullptr, &fb->color.sampler), "vkCreateSampler");

	allocateTextureDescriptor(fb->color);

	const std::array<VkImageView, 1> attachments = { fb->color.view };
	auto fbInfo = vkStruct<VkFramebufferCreateInfo>(VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO);
	fbInfo.renderPass = passes.single;
	fbInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	fbInfo.pAttachments = attachments.data();
	fbInfo.width = static_cast<uint32_t>(width);
	fbInfo.height = static_cast<uint32_t>(height);
	fbInfo.layers = 1;
	VK_CHECK(vkCreateFramebuffer(device, &fbInfo, nullptr, &fb->framebuffer),
	         "vkCreateFramebuffer");

	if (!antiAliased || !msaaSupported) {
		return fb;
	}
	imageInfo.samples = msaaSampleCount;
	imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	VK_CHECK(vkCreateImage(device, &imageInfo, nullptr, &fb->msaaImage), "vkCreateImage");
	vkGetImageMemoryRequirements(device, fb->msaaImage, &memReq);
	allocInfo.allocationSize = memReq.size;
	allocInfo.memoryTypeIndex =
	    findMemoryType(memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	VK_CHECK(vkAllocateMemory(device, &allocInfo, nullptr, &fb->msaaMemory), "vkAllocateMemory");
	VK_CHECK(vkBindImageMemory(device, fb->msaaImage, fb->msaaMemory, 0), "vkBindImageMemory");

	// Clear it like the color image and move it to COLOR_ATTACHMENT_OPTIMAL, which it stays in
	recordUpload([&](VkCommandBuffer c) {
		auto toDst = vkStruct<VkImageMemoryBarrier>(VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER);
		toDst.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		toDst.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		toDst.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		toDst.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		toDst.image = fb->msaaImage;
		toDst.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		toDst.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		vkCmdPipelineBarrier(c, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
		                     0, 0, nullptr, 0, nullptr, 1, &toDst);
		VkClearColorValue clearColor{ { 1.f, 1.f, 1.f, 0.f } };
		VkImageSubresourceRange range{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		vkCmdClearColorImage(c, fb->msaaImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearColor, 1,
		                     &range);
		auto toAttachment = vkStruct<VkImageMemoryBarrier>(VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER);
		toAttachment.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		toAttachment.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		toAttachment.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		toAttachment.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		toAttachment.image = fb->msaaImage;
		toAttachment.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		toAttachment.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		toAttachment.dstAccessMask =
		    VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		vkCmdPipelineBarrier(c, VK_PIPELINE_STAGE_TRANSFER_BIT,
		                     VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, nullptr, 0,
		                     nullptr, 1, &toAttachment);
	});

	viewInfo.image = fb->msaaImage;
	VK_CHECK(vkCreateImageView(device, &viewInfo, nullptr, &fb->msaaView), "vkCreateImageView");

	const std::array msaaAttachments = { fb->msaaView, fb->color.view };
	fbInfo.renderPass = passes.msaa;
	fbInfo.attachmentCount = static_cast<uint32_t>(msaaAttachments.size());
	fbInfo.pAttachments = msaaAttachments.data();
	VK_CHECK(vkCreateFramebuffer(device, &fbInfo, nullptr, &fb->msaaFramebuffer),
	         "vkCreateFramebuffer");
	fbInfo.renderPass = passes.expand;
	fbInfo.attachmentCount = 1;
	fbInfo.pAttachments = &fb->msaaView;
	VK_CHECK(vkCreateFramebuffer(device, &fbInfo, nullptr, &fb->expandFramebuffer),
	         "vkCreateFramebuffer");
	return fb;
}

void VulkanRenderer::expandIntoSamples(VulkanFramebuffer& fb) {
	const VkCommandBuffer cmd = commandBuffers[currentFrame];
	const VkExtent2D extent{ static_cast<uint32_t>(fb.width), static_cast<uint32_t>(fb.height) };
	auto rpInfo = vkStruct<VkRenderPassBeginInfo>(VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO);
	rpInfo.renderPass = offscreenRenderPasses[fb.hdr ? 1 : 0].expand;
	rpInfo.framebuffer = fb.expandFramebuffer;
	rpInfo.renderArea.extent = extent;
	vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);
	const VkViewport viewport{
		0.f, 0.f, static_cast<float>(extent.width), static_cast<float>(extent.height), 0.f, 1.f
	};
	vkCmdSetViewport(cmd, 0, 1, &viewport);
	const VkRect2D scissor{ { 0, 0 }, extent };
	vkCmdSetScissor(cmd, 0, 1, &scissor);

	// A quad covering all of it in clip space, which samples each texel at its center into all of
	// its pixel's samples, as it's drawn without blending
	const std::array<float, 24> xyuv = {
		-1.f, 1.f,  0.f, 1.f, // x, y, u, v
		1.f,  1.f,  1.f, 1.f, //
		-1.f, -1.f, 0.f, 0.f, //
		1.f,  1.f,  1.f, 1.f, //
		1.f,  -1.f, 1.f, 0.f, //
		-1.f, -1.f, 0.f, 0.f, //
	};
	const auto [vertexBuffer, offset] = uploadVertices(xyuv.data(), sizeof(xyuv));
	// The identity as modelview and projection, and no mixing towards lerpColor. Not in
	// drawUniforms, as the draw that's started the render pass might have written those already.
	std::vector<uint8_t> uniforms(texturedUniforms.size);
	writeUniform(uniforms, texturedUniforms.offsets.modelview, Mat3());
	writeUniform(uniforms, texturedUniforms.offsets.projection, Mat4());
	const PassKind kind = passKind(fb.hdr, true);
	ensurePipelines(kind, BlendVariant::Replace);
	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
	                  texturedPipelines[static_cast<size_t>(kind)][pipelineIndex(
	                      BlendVariant::Replace, PrimitiveType::Triangles)]);
	vkCmdBindVertexBuffers(cmd, 0, 1, &vertexBuffer, &offset);
	bindUniforms(cmd, texturedUniforms, uniforms, fb.color.descriptorSet);
	vkCmdDraw(cmd, 6, 1, 0, 0);
	vkCmdEndRenderPass(cmd);
	fb.samplesStale = false;
}

void VulkanRenderer::resolveFramebuffer(VulkanFramebuffer& fb) {
	auto rpInfo = vkStruct<VkRenderPassBeginInfo>(VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO);
	rpInfo.renderPass = offscreenRenderPasses[fb.hdr ? 1 : 0].resolve;
	rpInfo.framebuffer = fb.msaaFramebuffer;
	rpInfo.renderArea.extent = { static_cast<uint32_t>(fb.width),
		                         static_cast<uint32_t>(fb.height) };
	const VkCommandBuffer cmd = commandBuffers[currentFrame];
	vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdEndRenderPass(cmd);
	fb.resolvePending = false;
}

const VulkanTexture& VulkanRenderer::framebufferTexture(VulkanFramebuffer& fb,
                                                        const TextureFilter textureFilter) {
	const VkFilter filter = toVkFilter(textureFilter);
	if (filter == fb.color.minFilter) {
		return fb.color;
	}
	if (!fb.alternateFilter.descriptorSet) {
		auto& texture = fb.alternateFilter;
		texture.minFilter = filter;
		texture.magFilter = filter;
		createTextureSampler(texture);
		// Only the descriptor references the shared view; alternateFilter must not own it.
		VulkanTexture binding = texture;
		binding.view = fb.color.view;
		allocateTextureDescriptor(binding);
		texture.descriptorSet = binding.descriptorSet;
		texture.descriptorPool = binding.descriptorPool;
	}
	return fb.alternateFilter;
}

void VulkanRenderer::freeFramebufferResources(VulkanFramebuffer& fb) {
	for (VkFramebuffer* framebuffer :
	     { &fb.framebuffer, &fb.msaaFramebuffer, &fb.expandFramebuffer }) {
		if (*framebuffer) {
			vkDestroyFramebuffer(device, *framebuffer, nullptr);
			*framebuffer = VK_NULL_HANDLE;
		}
	}
	if (fb.msaaView) {
		vkDestroyImageView(device, fb.msaaView, nullptr);
		fb.msaaView = VK_NULL_HANDLE;
	}
	if (fb.msaaImage) {
		vkDestroyImage(device, fb.msaaImage, nullptr);
		fb.msaaImage = VK_NULL_HANDLE;
	}
	if (fb.msaaMemory) {
		vkFreeMemory(device, fb.msaaMemory, nullptr);
		fb.msaaMemory = VK_NULL_HANDLE;
	}
	freeTextureResources(fb.color);
	freeTextureResources(fb.alternateFilter);
	fb.color = {};
	fb.alternateFilter = {};
}

void VulkanRenderer::destroyFramebuffer(VulkanFramebuffer& fb) {
	if (!device) {
		return;
	}
	// The framebuffer may still be referenced by the current (not yet submitted) or an in-flight
	// command buffer - jngl::FrameBuffers can be destroyed mid-frame.
	destroyLater([this, fb]() mutable { freeFramebufferResources(fb); });
	fb = {}; // the copy owns the handles now
}

void VulkanRenderer::pushRenderTarget(VulkanFramebuffer& framebuffer) {
	if (!frameActive) {
		return;
	}
	endCurrentRenderPass();
	renderTargetStack.push_back(&framebuffer);
	beginCurrentRenderPass(VK_ATTACHMENT_LOAD_OP_LOAD);
}

void VulkanRenderer::popRenderTarget() {
	if (!frameActive || renderTargetStack.empty()) {
		return;
	}
	endCurrentRenderPass(true);
	VulkanFramebuffer& popped = *renderTargetStack.back();
	renderTargetStack.pop_back();
	if (popped.resolvePending) {
		resolveFramebuffer(popped); // as it's done being drawn into, like on OpenGL
	}
	beginCurrentRenderPass(VK_ATTACHMENT_LOAD_OP_LOAD);
}

void VulkanRenderer::clearCurrentRenderTarget(const Rgba color) {
	if (!frameActive) {
		return;
	}
	const VkExtent2D extent = currentRenderTargetExtent();
	const VkRect2D scissor = effectiveScissor(extent);
	if (!renderPassActive && !renderTargetStack.empty() && scissor.offset.x == 0 &&
	    scissor.offset.y == 0 && scissor.extent.width == extent.width &&
	    scissor.extent.height == extent.height) {
		// Clear on tile load instead of loading the previous frame and clearing it afterwards.
		pendingLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		pendingClearColor = color;
		return;
	}
	ensureRenderPass();
	VkClearAttachment attachment{};
	attachment.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	attachment.colorAttachment = 0;
	attachment.clearValue.color = { { color.getRed(), color.getGreen(), color.getBlue(),
		                              color.getAlpha() } };
	VkClearRect rect{};
	rect.rect = scissor;
	rect.baseArrayLayer = 0;
	rect.layerCount = 1;
	vkCmdClearAttachments(renderCommands, 1, &attachment, 1, &rect);
}

} // namespace jngl
