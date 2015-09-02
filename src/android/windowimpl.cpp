/*
Copyright 2015 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/
#include "windowimpl.hpp"

#include "../opengl.hpp"

#include <android_native_app_glue.h>
#include <stdexcept>

namespace jngl {
    android_app* androidApp;

    static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
        WindowImpl& impl = *reinterpret_cast<WindowImpl*>(app->userData);
        switch (cmd) {
            case APP_CMD_SAVE_STATE:
                // TODO: The system has asked us to save our current state.  Do so.
                break;
            case APP_CMD_INIT_WINDOW:
                // TODO: The window is being shown, get it ready.
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

    WindowImpl::WindowImpl() {
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

        // Initialize GL state.
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
        glEnable(GL_CULL_FACE);
        glShadeModel(GL_SMOOTH);
        glDisable(GL_DEPTH_TEST);

        app->userData = this;
    }

    void WindowImpl::swapBuffers() {
        eglSwapBuffers(display, surface);
    }
}
