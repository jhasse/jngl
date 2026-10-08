// Copyright 2015-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "../jngl/Finally.hpp"
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
	void terminate();
	void setRelativeMouseMode(bool);
	void pause();
	void makeCurrent();
	[[nodiscard]] int handleKeyEvent(AInputEvent*);
	[[nodiscard]] int32_t handleJoystickEvent(const AInputEvent*);
	void setKeyboardVisible(bool);
	std::vector<std::shared_ptr<Controller>> getConnectedControllers() const;
	void resetTouchState();

	/// Converts from actual pixels of the (maybe resized) window to the coordinates of the window
	/// as it was originally created
	Vec2 toWindowCoordinates(float x, float y) const;
	float getResizedWindowScalingX() const;
	float getResizedWindowScalingY() const;

	int mouseX = 0;
	int mouseY = 0;
	bool touchPressedThisUpdate = false;
	std::map<int32_t, Vec2> touches;
	int relativeX = 0;
	int relativeY = 0;
	JNIEnv* env = nullptr;

private:
	std::pair<EGLint, EGLint> getSurfaceSize() const;
	void resize(int width, int height);

	const std::pair<int, int> minAspectRatio;
	const std::pair<int, int> maxAspectRatio;
	android_app* app;
	Window* window;
	std::optional<Finally> pauseAudio;

	bool firstFrame = true;

	/// Size of the EGL surface and the letterboxed area inside of it. These differ from
	/// Window::width_ etc. after the window has been resized (e.g. by unfolding a foldable).
	int actualWidth = 0;
	int actualHeight = 0;
	int actualCanvasWidth = 0;
	int actualCanvasHeight = 0;

	struct DisplayWrapper {
		DisplayWrapper();
		~DisplayWrapper();
		EGLDisplay display;
		EGLConfig config;
		EGLContext context;

		struct SurfaceWrapper {
			SurfaceWrapper(const DisplayWrapper& parent);
			~SurfaceWrapper();
			const DisplayWrapper& parent;
			EGLSurface surface;
		};
		std::optional<SurfaceWrapper> surface;
	};
	std::optional<DisplayWrapper> display;

	std::map<int32_t, std::shared_ptr<AndroidController>> controllers;
};

} // namespace jngl
