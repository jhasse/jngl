// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

/// Small helpers shared by the Vulkan backend, see VulkanRenderer.hpp.
/// @file
#pragma once

#include <stdexcept>
#include <string>
#include <vulkan/vulkan.h>

namespace jngl {

/// Returns a human-readable name for a VkResult.
const char* vkResultToString(VkResult);

/// Throws a std::runtime_error if \a result isn't VK_SUCCESS, mentioning \a what.
inline void vkCheck(const VkResult result, const char* const what) {
	if (result != VK_SUCCESS) {
		throw std::runtime_error(std::string(what) + " failed: " + vkResultToString(result));
	}
}

} // namespace jngl

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage): keeps call sites short and passes the call name.
#define VK_CHECK(call, what) ::jngl::vkCheck((call), (what))
