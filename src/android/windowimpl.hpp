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
#include <optional>
#include <string>
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
	/// Handles key down and key up events from physical keyboards and the IME
	[[nodiscard]] int handleKeyEvent(const AInputEvent*);
	[[nodiscard]] int32_t handleJoystickEvent(const AInputEvent*);
	void setKeyboardVisible(bool);
	std::vector<std::shared_ptr<Controller>> getConnectedControllers() const;
	/// Forgets all touches and marks all keys as released, e.g. when we lose focus and would miss
	/// the up events
	void resetInputState();

	/// Whether the input device is a full keyboard (as opposed to a gamepad or a TV remote).
	/// Physical keyboards report the DPAD source because they have arrow keys, so we can't use the
	/// source alone to distinguish them from controllers.
	[[nodiscard]] bool isAlphabeticKeyboard(int32_t deviceId);

	/// See Window::toWindowCoordinates
	Vec2 toWindowCoordinates(float x, float y) const;

	bool focused = true; // see Window::hasFocus
	int mouseX = 0;
	int mouseY = 0;
	bool touchPressedThisUpdate = false;
	std::map<int32_t, Vec2> touches;
	int relativeX = 0;
	int relativeY = 0;
	JNIEnv* env = nullptr;

private:
	std::pair<EGLint, EGLint> getSurfaceSize() const;

	/// Asks android.view.KeyEvent which character the key produces with the event's modifiers
	[[nodiscard]] int32_t getUnicodeChar(const AInputEvent*) const;

	const std::pair<int, int> minAspectRatio;
	const std::pair<int, int> maxAspectRatio;
	android_app* app;
	Window* window;
	std::optional<Finally> pauseAudio;

	bool firstFrame = true;

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

	/// Which entry of Window::characterDown_ a currently held key (by Android key code) set, so
	/// that we can clear it again on key up even if the modifier state changed in the meantime
	std::map<int32_t, std::string> keyCodeToCharacter;

	/// Cache for isAlphabeticKeyboard, by device id
	std::map<int32_t, bool> alphabeticKeyboards;
};

} // namespace jngl
