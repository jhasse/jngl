// Copyright 2024 Jan Niklas Hasse <jhasse@gmail.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Logging functions
/// @file
#pragma once

#if __has_include(<format>)
#include <format>
#endif

namespace jngl {

void trace(const std::string&);

#if __has_include(<format>)
template <class... Args> void trace(std::format_string<Args...> format, Args&&... args) {
	return trace(std::format(std::move(format), std::forward<Args>(args)...));
}
#else
template <class... Args> void trace(Args&&... args) {}
#endif

void warn(const std::string&);

#if __has_include(<format>)
template <class... Args> void warn(std::format_string<Args...> format, Args&&... args) {
	return warn(std::format(std::move(format), std::forward<Args>(args)...));
}
#else
template <class... Args> void warn(Args&&... args) {}
#endif

} // namespace jngl
