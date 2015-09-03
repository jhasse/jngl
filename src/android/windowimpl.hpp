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
    class Window;

    class WindowImpl {
    public:
        WindowImpl(Window*);

        void updateInput();
        void swapBuffers();
        void init();

        int mouseX = 0;
        int mouseY = 0;
        int width;
        int height;
        int relativeX = 0;
        int relativeY = 0;
    private:
        Window* window;
        android_app* app;

        ASensorManager* sensorManager;
        const ASensor* accelerometerSensor;
        ASensorEventQueue* sensorEventQueue;

        bool initialized = false;
        EGLDisplay display;
        EGLSurface surface;
        EGLContext context;
    };
}
