// Copyright 2020-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Include this file only once, as it defines the main function
/// \file

#pragma once

#include "AppParameters.hpp"
#include "main.hpp"
#include "message.hpp"

#include <cmath>

#if defined(__has_include) && __has_include("filesystem")
#include <filesystem>
#endif

namespace jngl::internal {

void mainLoop(AppParameters);

} // namespace jngl::internal

/// Implement this function and set AppParameters::start
///
/// Usually you'd want to do this in a file called e.g. `main.cpp`:
/// \code
/// #include "MyGame.hpp" // class that derives from jngl::Work
///
/// #include <jngl/init.hpp>
///
/// jngl::AppParameters jnglInit() {
///     jngl::AppParameters params;
///     params.displayName = "My Game";
///     params.screenSize = { 1920, 1080 };
///
///     params.start = []() {
///         return std::make_shared<MyGame>();
///     };
///     return params;
/// }
/// \endcode
jngl::AppParameters jnglInit();

#if !defined(__APPLE__) || !TARGET_OS_IPHONE // iOS
JNGL_MAIN_BEGIN {                            // NOLINT
#if !defined(ANDROID) && defined(__has_include) && __has_include(<filesystem>) && \
    (!defined(TARGET_OS_IOS) || TARGET_OS_IOS == 0)
	std::error_code err;
	std::filesystem::current_path("data", err);
	if (err) {
		std::filesystem::current_path("../data", err); // move out of build/bin folder
		if (err) {
			std::filesystem::current_path("../../data", err); // move out of build/Debug folder
			if (err) {
				std::filesystem::current_path("../../../data",
				                              err); // move out of out\build\x64-Debug
				if (err) {
					std::filesystem::current_path(jngl::getBinaryPath() + "data", err);
				}
			}
		}
	}
#endif
	jngl::internal::mainLoop(jnglInit());
}
JNGL_MAIN_END
#endif

#if defined(_MSC_VER) && WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

INT WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	return main(1, nullptr);
}
#endif
