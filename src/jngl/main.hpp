// Copyright 2015-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#if !defined(JNGL_MAIN_BEGIN)
	#if defined(ANDROID)
		#include <android_native_app_glue.h>

		namespace jngl {
			extern android_app* androidApp;
		}
		#define JNGL_MAIN_BEGIN void android_main(android_app* __androidApp) { \
			jngl::androidApp = __androidApp; \
			app_dummy();
		#define JNGL_MAIN_END }
	#else
		#define JNGL_MAIN_BEGIN int main() {
		#define JNGL_MAIN_END }
	#endif
#endif
