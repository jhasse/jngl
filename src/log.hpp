// Copyright 2024 Jan Niklas Hasse <jhasse@gmail.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#pragma once

#if __has_include(<format>) && (!defined(_LIBCPP_VERSION) || _LIBCPP_VERSION >= 170000)
#include <format>
#endif
#include <string>

namespace jngl::internal {

void trace(const std::string&);

#if __has_include(<format>) && (!defined(_LIBCPP_VERSION) || _LIBCPP_VERSION >= 170000)
template <class... Args> void trace(std::format_string<Args...> format, Args&&... args) {
	return trace(std::format(std::move(format), std::forward<Args>(args)...));
}
#else
template <class... Args> void trace(Args&&...) {
}
#endif

void debug(const std::string&);

#if __has_include(<format>) && (!defined(_LIBCPP_VERSION) || _LIBCPP_VERSION >= 170000)
template <class... Args> void debug(std::format_string<Args...> format, Args&&... args) {
	return debug(std::format(std::move(format), std::forward<Args>(args)...));
}
#else
template <class... Args> void debug(Args&&...) {
}
#endif

void info(const std::string&);

#if __has_include(<format>) && (!defined(_LIBCPP_VERSION) || _LIBCPP_VERSION >= 170000)
template <class... Args> void info(std::format_string<Args...> format, Args&&... args) {
	return info(std::format(std::move(format), std::forward<Args>(args)...));
}
#else
template <class... Args> void info(Args&&...) {
}
#endif

void warn(const std::string&);

#if __has_include(<format>) && (!defined(_LIBCPP_VERSION) || _LIBCPP_VERSION >= 170000)
template <class... Args> void warn(std::format_string<Args...> format, Args&&... args) {
	return warn(std::format(std::move(format), std::forward<Args>(args)...));
}
#else
template <class... Args> void warn(Args&&...) {
}
#endif

void error(const std::string&);

#if __has_include(<format>) && (!defined(_LIBCPP_VERSION) || _LIBCPP_VERSION >= 170000)
template <class... Args> void error(std::format_string<Args...> format, Args&&... args) {
	return error(std::format(std::move(format), std::forward<Args>(args)...));
}
#else
template <class... Args> void error(Args&&...) {
}
#endif

void log(const std::string& appName, const std::string& level, const std::string& message);

} // namespace jngl::internal
