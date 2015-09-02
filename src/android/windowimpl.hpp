/*
Copyright 2015 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/
#pragma once

#include <EGL/egl.h>

struct android_app;
struct ASensorManager;
struct ASensor;
struct ASensorEventQueue;

namespace jngl {
    class WindowImpl {
    public:
        WindowImpl();

        void updateInput();
        void swapBuffers();
        void init();

        int mouseX;
        int mouseY;
    private:
        android_app* app;

        ASensorManager* sensorManager;
        const ASensor* accelerometerSensor;
        ASensorEventQueue* sensorEventQueue;

        bool initialized = false;
        EGLDisplay display;
        EGLSurface surface;
        EGLContext context;
        int32_t width;
        int32_t height;
    };
}
