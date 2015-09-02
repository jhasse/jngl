/*
Copyright 2015 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/
#pragma once

#ifndef JNGL_MAIN_BEGIN
    #ifdef ANDROID
        #include <android_native_app_glue.h>

        namespace jngl {
            extern android_app* androidApp;
        }
        #define JNGL_MAIN_BEGIN void android_main(android_app* __androidApp) { \
            jngl::androidApp = __androidApp; \
            app_dummy();
        #define JNGL_MAIN_END }
    #else
        #define JNGL_MAIN_BEGIN void main() {
        #define JNGL_MAIN_END }
    #endif
#endif
