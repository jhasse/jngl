// Copyright 2015-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include <EGL/egl.h>

struct android_app;

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
	unsigned int numberOfTouches = 0;
	int width;
	int height;
	int relativeX = 0;
	int relativeY = 0;

private:
	android_app* app;
	Window* window;

	bool initialized = false;
	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
};

} // namespace jngl
