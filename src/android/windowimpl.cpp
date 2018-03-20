// Copyright 2015-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "windowimpl.hpp"

#include "../opengl.hpp"
#include "../jngl/other.hpp"
#include "../jngl/debug.hpp"
#include "../window.hpp"
#include "../main.hpp"

#include <android_native_app_glue.h>
#include <android/storage_manager.h>
#include <android/obb.h>
#include <stdexcept>
#include <cassert>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <fstream>
#include <sstream>

namespace jngl {
    android_app* androidApp;

    static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
        jngl::debugLn("Received event");
        WindowImpl& impl = *reinterpret_cast<WindowImpl*>(app->userData);
        switch (cmd) {
            case APP_CMD_SAVE_STATE:
                // TODO: The system has asked us to save our current state.  Do so.
                break;
            case APP_CMD_INIT_WINDOW:
                // The window is being shown, get it ready.
                if (androidApp->window) {
                    impl.init();
                }
                break;
            case APP_CMD_TERM_WINDOW:
                // TODO: The window is being hidden or closed, clean it up.
                break;
            case APP_CMD_GAINED_FOCUS:
                // TODO
                break;
            case APP_CMD_LOST_FOCUS:
                // TODO
                break;
        }
    }

    static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) {
        WindowImpl& impl = *reinterpret_cast<WindowImpl*>(app->userData);
        if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
            impl.mouseX = AMotionEvent_getY(event, 0);
            impl.mouseY = AMotionEvent_getX(event, 0);
            return 1;
        }
        return 0;
    }

    WindowImpl::WindowImpl(Window* window) : app(androidApp), window(window) {
        app->userData = this;
        app->onAppCmd = engine_handle_cmd;
        app->onInputEvent = engine_handle_input;
        jngl::debugLn("Handler set.");
    }

    void callback(const char* filename, const int32_t state, void* data) {
        debug("ist "); debug(state); debug(" und sollte "); debugLn(AOBB_STATE_MOUNTED);
    }

    void WindowImpl::init() {
        /*
         * Here specify the attributes of the desired configuration.
         * Below, we select an EGLConfig with at least 8 bits per color
         * component compatible with on-screen windows
         */
        const EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_NONE
        };
        EGLint dummy, format;
        EGLint numConfigs;
        EGLConfig config;

        display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

        eglInitialize(display, 0, 0);

        /* Here, the application chooses the configuration it desires. In this
         * sample, we have a very simplified selection process, where we pick
         * the first EGLConfig that matches our criteria */
        eglChooseConfig(display, attribs, &config, 1, &numConfigs);

        /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
         * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
         * As soon as we picked a EGLConfig, we can safely reconfigure the
         * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
        eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

        if (!app) {
            throw std::runtime_error("android_app struct not set. "
                                     "Use JNGL_MAIN_BEGIN and JNGL_MAIN_END.");
        }
        ANativeWindow_setBuffersGeometry(app->window, 0, 0, format);

        surface = eglCreateWindowSurface(display, config, app->window, NULL);
        context = eglCreateContext(display, config, NULL, NULL);

        if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
            throw std::runtime_error("Unable to eglMakeCurrent");
        }

        EGLint w, h;
        eglQuerySurface(display, surface, EGL_WIDTH, &w);
        width = w;
        eglQuerySurface(display, surface, EGL_HEIGHT, &h);
        height = h;
        debug("Surface size "); debug(width); debug("x"); debugLn(height);

        // Initialize GL state.
        Init(width, height, width, height);

        ANativeActivity* activity = app->activity;

        JNIEnv* env = nullptr;
        activity->vm->AttachCurrentThread(&env, nullptr);
        jclass clazz = env->GetObjectClass(activity->clazz);

        jmethodID methodID = env->GetMethodID(clazz, "getPackageName", "()Ljava/lang/String;");
        jobject result = env->CallObjectMethod(activity->clazz, methodID);
        std::string packageName = env->GetStringUTFChars((jstring)result, nullptr);

        jmethodID mid = env->GetMethodID(clazz, "getObbDir", "()Ljava/io/File;");
        jobject obj_File = env->CallObjectMethod(activity->clazz, mid, nullptr);
        jclass cls_File = env->FindClass("java/io/File");
        jmethodID mid_getPath = env->GetMethodID(cls_File, "getPath", "()Ljava/lang/String;");
        jstring obj_Path = (jstring) env->CallObjectMethod(obj_File, mid_getPath);

        std::string obbDir = env->GetStringUTFChars((jstring)obj_Path, nullptr);

        std::stringstream sstream;
        sstream << obbDir << "/main.1."
                << packageName << ".obb";
        std::string obbFile = sstream.str();

        std::fstream test(obbFile);
        debug("obbFile: "); debugLn(obbFile);
        if (test) {
            debugLn("exists");
            test.close();
        }
        auto storageManager = AStorageManager_new();
        auto info = AObbScanner_getObbInfo(obbFile.c_str());
        if (!info) {
            debugLn("info failed");
        } else {
            debugLn("info success");
        }
        debugLn(AStorageManager_isObbMounted(storageManager, obbFile.c_str()));
        // FIXME: crashes:
        // AStorageManager_mountObb(storageManager, obbFile.c_str(), nullptr, callback, nullptr);
        debugLn(AStorageManager_isObbMounted(storageManager, obbFile.c_str()));

        activity->vm->DetachCurrentThread();

        initialized = true;
    }

    void WindowImpl::updateInput() {
        // Read all pending events.
        int ident;
        int events;
        android_poll_source* source;

        while ((ident = ALooper_pollAll(0, NULL, &events, (void**)&source)) >= 0) {

            // Process this event.
            if (source != NULL) {
                source->process(app, source);
            }

            // Check if we are exiting.
            if (app->destroyRequested != 0) {
                // TODO: engine_term_display(&engine);
                jngl::quit();
            }
        }

        window->mousex_ = mouseX - relativeX;
        window->mousey_ = mouseY - relativeY;
        if (window->relativeMouseMode) {
            relativeX = mouseX;
            relativeY = mouseY;
        }
    }

    void WindowImpl::swapBuffers() {
        eglSwapBuffers(display, surface);
    }
}
