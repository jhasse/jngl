// Copyright 2015-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include <EGL/egl.h>
#include <utility>

struct android_app;

namespace jngl {

class Window;

class WindowImpl {
public:
	WindowImpl(Window*, std::pair<int, int> minAspectRatio, std::pair<int, int> maxAspectRatio);

	void updateInput();
	void swapBuffers();
	void init();
	void setRelativeMouseMode(bool);
	void addTextInput(const char*);

	int mouseX = 0;
	int mouseY = 0;
	unsigned int numberOfTouches = 0;
	int relativeX = 0;
	int relativeY = 0;

private:
	const std::pair<int, int> minAspectRatio;
	const std::pair<int, int> maxAspectRatio;
	android_app* app;
	Window* window;

	bool initialized = false;
	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
};

} // namespace jngl
