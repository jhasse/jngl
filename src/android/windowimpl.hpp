// Copyright 2015-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "../jngl/Vec2.hpp"

#include <EGL/egl.h>
#include <jni.h>
#include <utility>
#include <android/input.h>
#include <map>
#include <vector>

struct android_app;

namespace jngl {

class AndroidController;
class Controller;
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
	[[nodiscard]] int handleKeyEvent(AInputEvent*);
	[[nodiscard]] int32_t handleJoystickEvent(const AInputEvent*);
	void setKeyboardVisible(bool);
	std::vector<std::shared_ptr<Controller>> getConnectedControllers() const;

	int mouseX = 0;
	int mouseY = 0;
	std::map<int32_t, Vec2> touches;
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
	std::map<int32_t, std::shared_ptr<AndroidController>> controllers;
};

} // namespace jngl
