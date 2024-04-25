// Copyright 2015-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Defines macros for the main entry point
/// @file
#pragma once

#include "Finally.hpp"
#include "other.hpp"
#include "window.hpp"

#ifdef JNGL_UWP
#include <SDL.h>

#define JNGL_MAIN_BEGIN /* NOLINT */ int SDL_main(int argc, char** argv) { \
	jngl::Finally _ZtzNg47T5XSjogv(jngl::hideWindow);
#define JNGL_MAIN_END return 0; } // NOLINT

#undef main // SDL defines this to SDL_main
#else

#ifdef __APPLE__
#include "TargetConditionals.h"
#endif

#if !defined(JNGL_MAIN_BEGIN)
	#if defined(ANDROID)
		#include <android_native_app_glue.h>

		namespace jngl {
			extern android_app* androidApp;
		}
		#define JNGL_MAIN_BEGIN void android_main(android_app* __androidApp) { \
			jngl::androidApp = __androidApp;
		#define JNGL_MAIN_END }
	#elif defined(__APPLE__) && TARGET_OS_IPHONE // iOS
		#define JNGL_MAIN_BEGIN void shouldNotBeCalled() {
		#define JNGL_MAIN_END }
	#else
		#ifdef NDEBUG
			#define JNGL_CATCH_EXCEPTION_TO_ERROR_MESSAGE try {
		#else
			#define JNGL_CATCH_EXCEPTION_TO_ERROR_MESSAGE
		#endif
		#define JNGL_MAIN_BEGIN /* NOLINT */ int main(int argc, char** argv) { \
			JNGL_CATCH_EXCEPTION_TO_ERROR_MESSAGE \
				{ \
					std::vector<std::string> tmp(argc - 1); \
					for (int i = 1; i < argc; ++i) { \
						tmp[i - 1] = argv[i]; \
					} \
					jngl::setArgs(tmp); \
				} \
				jngl::Finally _ZtzNg47T5XSjogv(jngl::hideWindow);
		#ifdef NDEBUG
			#define JNGL_MAIN_END /* NOLINT */ } catch (std::exception& e) { \
					jngl::errorMessage(e.what()); \
					return EXIT_FAILURE; \
				} \
			}
		#else
			#define JNGL_MAIN_END }
		#endif
	#endif
#endif

#endif
