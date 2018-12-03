// Copyright 2015-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include <EGL/egl.h>
#include <jni.h>
#include <utility>
#include <android/input.h>

struct android_app;

namespace jngl {

class Window;

class WindowImpl {
public:
	WindowImpl(Window*, std::pair<int, int> minAspectRatio, std::pair<int, int> maxAspectRatio);
	~WindowImpl();

	void updateInput();
	void swapBuffers();
	void init();
	void setRelativeMouseMode(bool);
	void pause();
	void makeCurrent();
	void hideNavigationBar();
	int handleKeyEvent(AInputEvent*);
	void setKeyboardVisible(bool);

	int mouseX = 0;
	int mouseY = 0;
	unsigned int numberOfTouches = 0;
	int relativeX = 0;
	int relativeY = 0;

private:
	JNIEnv* env = nullptr;
	EGLConfig config;
	const std::pair<int, int> minAspectRatio;
	const std::pair<int, int> maxAspectRatio;
	android_app* app;
	Window* window;

	bool initialized = false;
	EGLDisplay display;
	EGLSurface surface = nullptr ;
	EGLContext context;
};

} // namespace jngl
