// Copyright 2015-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "windowimpl.hpp"

#include "AndroidController.hpp"
#include "../jngl/input.hpp"
#include "../jngl/other.hpp"
#include "../log.hpp"
#include "../jngl/screen.hpp"
#include "../jngl/sound.hpp"
#include "../jngl/window.hpp"
#include "../jngl/work.hpp"
#include "../App.hpp"
#include "../windowptr.hpp"
#include "../main.hpp"
#include "fopen.hpp"

#include <android_native_app_glue.h>
#include <android/storage_manager.h>
#include <android/window.h>

namespace jngl {

android_app* androidApp;

static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
	internal::debug("Received event {}", cmd);
	WindowImpl& impl = *reinterpret_cast<WindowImpl*>(app->userData);
	switch (cmd) {
	case APP_CMD_SAVE_STATE:
		// TODO: The system has asked us to save our current state.  Do so.
		break;
	case APP_CMD_INIT_WINDOW:
		// The window is being shown, get it ready.
		assert(androidApp == app);
		if (androidApp->window) {
			impl.init();
		}
		break;
	case APP_CMD_TERM_WINDOW:
		impl.terminate();
		break;
	case APP_CMD_GAINED_FOCUS:
		impl.focused = true;
		impl.makeCurrent();
		break;
	case APP_CMD_LOST_FOCUS:
		impl.focused = false;
		impl.resetInputState();
		break;
	case APP_CMD_PAUSE:
		impl.pause();
		break;
	}
}

static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) {
	if (!app || !app->userData) {
		return 0; // shouldn't happen but I seen a crash in this function in the Play Console
	}
	WindowImpl& impl = *reinterpret_cast<WindowImpl*>(app->userData);
	const auto source = AInputEvent_getSource(event);
	if ((source & AINPUT_SOURCE_JOYSTICK) == AINPUT_SOURCE_JOYSTICK ||
	    (source & AINPUT_SOURCE_GAMEPAD) == AINPUT_SOURCE_GAMEPAD) {
		return impl.handleJoystickEvent(event);
	}
	if ((source & AINPUT_SOURCE_DPAD) == AINPUT_SOURCE_DPAD &&
	    !impl.isAlphabeticKeyboard(AInputEvent_getDeviceId(event))) {
		// A TV remote or similar: Treat its directional pad as a controller. Physical keyboards
		// also have the DPAD source though (because of their arrow keys), those are handled as
		// keyboards.
		return impl.handleJoystickEvent(event);
	}
	switch (AInputEvent_getType(event)) {
	case AINPUT_EVENT_TYPE_KEY:
		return impl.handleKeyEvent(event);
	case AINPUT_EVENT_TYPE_MOTION: {
		const auto action = AMotionEvent_getAction(event);
		switch (action & AMOTION_EVENT_ACTION_MASK) {
		case AMOTION_EVENT_ACTION_DOWN: {
			const int32_t index = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >>
			                      AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
			const auto id = AMotionEvent_getPointerId(event, index);
			auto [touch, created] = impl.touches.insert(
			    { id, impl.toWindowCoordinates(AMotionEvent_getX(event, index),
			                                   AMotionEvent_getY(event, index)) });
			assert(created);
			impl.mouseX = touch->second.x;
			impl.mouseY = touch->second.y;
			impl.touchPressedThisUpdate = true;
			return 1;
		}
		case AMOTION_EVENT_ACTION_POINTER_DOWN: {
			const int32_t index = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >>
			                      AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
			const auto id = AMotionEvent_getPointerId(event, index);
			[[maybe_unused]] auto [_, created] = impl.touches.insert(
			    { id, impl.toWindowCoordinates(AMotionEvent_getX(event, index),
			                                   AMotionEvent_getY(event, index)) });
			assert(created);
			return 1;
		}
		case AMOTION_EVENT_ACTION_POINTER_UP: {
			const int32_t index = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >>
			                      AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
			const auto id = AMotionEvent_getPointerId(event, index);
			const auto it = impl.touches.find(id);
			assert(it != impl.touches.end());
			impl.touches.erase(it);
			assert(!impl.touches.empty());
			// For the case that the mouse position pointed to this pointer we need to update it so
			// that it still makes sense. This is very important when going from 2 pointer to 1
			// since getTouchPositions() will switch to getMousePos() then.
			impl.mouseX = impl.touches.begin()->second.x;
			impl.mouseY = impl.touches.begin()->second.y;
			return 1;
		}
		case AMOTION_EVENT_ACTION_MOVE: {
			for (size_t index = 0; index < AMotionEvent_getPointerCount(event); ++index) {
				const auto [x, y] = impl.toWindowCoordinates(AMotionEvent_getX(event, index),
				                                             AMotionEvent_getY(event, index));
				const auto id = AMotionEvent_getPointerId(event, index);
				const auto it = impl.touches.find(id);
				if (it == impl.touches.end()) {
					internal::debug("Unknown touch id: {}", id);
					return 0;
				}
				it->second.x = x;
				it->second.y = y;
				if (index == 0) {
					impl.mouseX = x;
					impl.mouseY = y;
				}
			}
			return 1;
		}
		case AMOTION_EVENT_ACTION_UP:
			impl.touches.clear();
			return 1;
		case AMOTION_EVENT_ACTION_CANCEL:
			impl.touches.clear();
			return 1;
		case AMOTION_EVENT_ACTION_HOVER_MOVE:
			if (AMotionEvent_getPointerCount(event) >= 1) {
				const auto [x, y] = impl.toWindowCoordinates(
				    AMotionEvent_getX(event, 0 /* JNGL supports only one mouse */),
				    AMotionEvent_getY(event, 0));
				impl.mouseX = x;
				impl.mouseY = y;
			}
			return 1;
		}
	}
	}
	return 0;
}

WindowImpl::WindowImpl(Window* window, const std::pair<int, int> minAspectRatio,
                       const std::pair<int, int> maxAspectRatio)
: minAspectRatio(minAspectRatio), maxAspectRatio(maxAspectRatio), app(androidApp), window(window) {
	app->userData = this;
	app->onAppCmd = engine_handle_cmd;
	app->onInputEvent = engine_handle_input;
	internal::debug("Handler set.");

	app->activity->vm->AttachCurrentThread(&env, nullptr);

	ANativeActivity_setWindowFlags(app->activity, AWINDOW_FLAG_KEEP_SCREEN_ON, 0);


	jclass inputDeviceClass = env->FindClass("android/view/InputDevice");
	if (!inputDeviceClass) { return; }
	Finally cleanInputDeviceClass([&]() { env->DeleteLocalRef(inputDeviceClass); });

	jmethodID getInputDeviceMethod =
	    env->GetStaticMethodID(inputDeviceClass, "getDevice", "(I)Landroid/view/InputDevice;");
	if (!getInputDeviceMethod) { return; }
	jmethodID getSourcesMethod = env->GetMethodID(inputDeviceClass, "getSources", "()I");
	if (!getSourcesMethod) { return; }

	jmethodID getDeviceIdsMethod = env->GetStaticMethodID(inputDeviceClass, "getDeviceIds", "()[I");
	if (!getDeviceIdsMethod) { return; }
	auto deviceIds =
	    static_cast<jintArray>(env->CallStaticObjectMethod(inputDeviceClass, getDeviceIdsMethod));
	if (!deviceIds) { return; }
	Finally cleanDeviceIds([&]() { env->DeleteLocalRef(deviceIds); });

	jint* elements = env->GetIntArrayElements(deviceIds, nullptr);
	if (!elements) { return; }
	Finally cleanElements([&]() { env->ReleaseIntArrayElements(deviceIds, elements, 0); });

	jsize length = env->GetArrayLength(deviceIds);
	for (jsize i = 0; i < length; ++i) {
		jobject inputDeviceObj =
		    env->CallStaticObjectMethod(inputDeviceClass, getInputDeviceMethod, elements[i]);
		if (!inputDeviceObj) { continue; }
		Finally cleanInputDeviceObj([&]() { env->DeleteLocalRef(inputDeviceObj); });

		jint sources = env->CallIntMethod(inputDeviceObj, getSourcesMethod);
		if ((sources & AINPUT_SOURCE_JOYSTICK) == AINPUT_SOURCE_JOYSTICK ||
		    (sources & AINPUT_SOURCE_GAMEPAD) == AINPUT_SOURCE_GAMEPAD) {
            if (auto& controller = controllers[elements[i]]; !controller) {
                controller = std::make_shared<AndroidController>();
            }
		}
	}
}

WindowImpl::~WindowImpl() {
	// We need to destroy the surface before finishing our activity, otherwise the app won't start
	// again.
	pause();
	if (app->destroyRequested == 0) {
		// jngl::quit() has been called. We need to gracefully quit the activity, too, and handle
		// any pending events (this will result in destroyRequested != 0):
		ANativeActivity_finish(app->activity);
		updateInput();
	}
	app->activity->vm->DetachCurrentThread();
}

static void handleEglError() {
	EGLenum error = eglGetError();
	const char* errorString = eglQueryString(EGL_NO_DISPLAY, error);
	if (errorString != nullptr) {
		throw std::runtime_error(errorString);
	}
	throw std::runtime_error("EGL error: " + std::to_string(error));
}

WindowImpl::DisplayWrapper::DisplayWrapper() {
	// Here specify the attributes of the desired configuration. Below, we select an EGLConfig with
	// at least 8 bits per color component compatible with on-screen windows
	EGLint attribs[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
		EGL_BLUE_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_RED_SIZE, 8,
		EGL_NONE
	};
	EGLint format;
	EGLint numConfigs;

	display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (display == EGL_NO_DISPLAY) {
		handleEglError();
	}
	if (eglInitialize(display, nullptr, nullptr) == EGL_FALSE) {
		handleEglError();
	}
	// Here, the application chooses the configuration it desires. In this sample, we have a very
	// simplified selection process, where we pick the first EGLConfig that matches our criteria
	if (eglChooseConfig(display, attribs, &config, 1, &numConfigs) == EGL_FALSE) {
		assert(attribs[3] == EGL_OPENGL_ES3_BIT);
		attribs[3] = EGL_OPENGL_ES2_BIT;
		if (eglChooseConfig(display, attribs, &config, 1, &numConfigs) == EGL_FALSE) {
			handleEglError();
		}
	}

	// EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is guaranteed to be accepted by
	// ANativeWindow_setBuffersGeometry(). As soon as we picked a EGLConfig, we can safely
	// reconfigure the ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID.
	if (eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format) == EGL_FALSE) {
		handleEglError();
	}

	if (!androidApp) {
		throw std::runtime_error("android_app struct not set. "
		                         "Use JNGL_MAIN_BEGIN and JNGL_MAIN_END.");
	}
	ANativeWindow_setBuffersGeometry(androidApp->window, 0, 0, format);

	const EGLint contextAttribList[] = {
			EGL_CONTEXT_CLIENT_VERSION, (attribs[3] & EGL_OPENGL_ES3_BIT) ? 3 : 2,
			EGL_NONE
	};
	context = eglCreateContext(display, config, NULL, contextAttribList);
	if (context == EGL_NO_CONTEXT) {
		handleEglError();
	}
}

WindowImpl::DisplayWrapper::~DisplayWrapper() {
	surface = std::nullopt;
	assert(context != EGL_NO_CONTEXT);
	if (eglDestroyContext(display, context) == EGL_FALSE) {
		handleEglError();
	}
	if (eglTerminate(display) == EGL_FALSE) {
		handleEglError();
	}
}

WindowImpl::DisplayWrapper::SurfaceWrapper::SurfaceWrapper(const DisplayWrapper& parent)
: parent(parent) {
	surface = eglCreateWindowSurface(parent.display, parent.config, androidApp->window, nullptr);
	if (surface == EGL_NO_SURFACE) {
		handleEglError();
	}
	if (eglMakeCurrent(parent.display, surface, surface, parent.context) == EGL_FALSE) {
		handleEglError();
	}
}

WindowImpl::DisplayWrapper::SurfaceWrapper::~SurfaceWrapper() {
	assert(parent.display != EGL_NO_DISPLAY);
	if (eglMakeCurrent(parent.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT) ==
	    EGL_FALSE) {
		handleEglError();
	}
	assert(surface != EGL_NO_SURFACE);
	if (eglDestroySurface(parent.display, surface) == EGL_FALSE) {
		handleEglError();
	}
}

std::pair<EGLint, EGLint> WindowImpl::getSurfaceSize() const {
	EGLint w, h;
	if (eglQuerySurface(display->display, display->surface->surface, EGL_WIDTH, &w) == EGL_FALSE) {
		handleEglError();
	}
	if (eglQuerySurface(display->display, display->surface->surface, EGL_HEIGHT, &h) == EGL_FALSE) {
		handleEglError();
	}
	return { w, h };
}

void WindowImpl::init() {
	makeCurrent();

	const auto [w, h] = getSurfaceSize();
	// APP_CMD_INIT_WINDOW isn't only called on first start, but also when the app was sent to the
	// background and brought to foreground again. In that case the scale factor is already set, but
	// we still need to initialize OpenGL again.
	if (!pWindow) {
		if (window->width_ > 0) {
			assert(window->height_ > 0);
			// when AppParameters::screenSize was set, width_ and height_ contain it. Otherwise they
			// are set to -1.
			setScaleFactor(std::min(static_cast<double>(w) / window->width_,
			                        static_cast<double>(h) / window->height_));
		}
		window->width_ = w;
		window->height_ = h;
		window->calculateCanvasSize(minAspectRatio, maxAspectRatio);
	}
	// The window might have been resized while we were in the background:
	if (w != window->actualWidth || h != window->actualHeight) {
		window->actualWidth = w;
		window->actualHeight = h;
		window->canvasCandidates.clear();
	}
	window->initGl();
}

Vec2 WindowImpl::toWindowCoordinates(const float x, const float y) const {
	return window->toWindowCoordinates(x, y);
}

void WindowImpl::resetInputState() {
	touches.clear();
	window->mouseDown_[0] = false;
	for (auto& entry : window->keyDown_) {
		entry.second = false;
	}
	for (auto& entry : window->characterDown_) {
		entry.second = false;
	}
	keyCodeToCharacter.clear();
}

void WindowImpl::terminate() {
	if (display) {
		display->surface = std::nullopt;
	}
	resetInputState(); // It seems when we get terminated via the home swipe gesture that we don't
	                   // receive the AMOTION_EVENT_ACTION_UP which would leave a touch in the map
	                   // and result in an assertion failure the next time the user returns to the
	                   // app and touches the screen.
}

void WindowImpl::setRelativeMouseMode(const bool relativeMouseMode) {
	if (relativeMouseMode) {
		relativeX = mouseX;
		relativeY = mouseY;
		window->mousex_ = window->mousey_ = 0;
	} else {
		relativeX = relativeY = 0;
	}
}

void WindowImpl::pause() {
	if (display) {
		display->surface = std::nullopt;
	}
	resetInputState();
	if (!pauseAudio) {
		pauseAudio = jngl::pauseAudio();
	}
}

void WindowImpl::makeCurrent() {
	if (!display) {
		display.emplace();
	}
	if (!display->surface) {
		display->surface.emplace(*display);
	}
	pauseAudio = {};
}

namespace {

std::optional<key::KeyType> toJnglKey(const int32_t androidKeyCode) {
	switch (androidKeyCode) {
	case AKEYCODE_DPAD_LEFT:
		return key::Left;
	case AKEYCODE_DPAD_UP:
		return key::Up;
	case AKEYCODE_DPAD_RIGHT:
		return key::Right;
	case AKEYCODE_DPAD_DOWN:
		return key::Down;
	case AKEYCODE_PAGE_UP:
		return key::PageUp;
	case AKEYCODE_PAGE_DOWN:
		return key::PageDown;
	case AKEYCODE_MOVE_HOME:
		return key::Home;
	case AKEYCODE_MOVE_END:
		return key::End;
	case AKEYCODE_DEL:
		return key::BackSpace;
	case AKEYCODE_TAB:
		return key::Tab;
	case AKEYCODE_CLEAR:
		return key::Clear;
	case AKEYCODE_ENTER:
	case AKEYCODE_NUMPAD_ENTER:
		return key::Return;
	case AKEYCODE_BREAK:
		return key::Pause;
	case AKEYCODE_ESCAPE:
		return key::Escape;
	case AKEYCODE_FORWARD_DEL:
		return key::Delete;
	case AKEYCODE_CTRL_LEFT:
		return key::ControlL;
	case AKEYCODE_CTRL_RIGHT:
		return key::ControlR;
	case AKEYCODE_CAPS_LOCK:
		return key::CapsLock;
	case AKEYCODE_ALT_LEFT:
		return key::AltL;
	case AKEYCODE_ALT_RIGHT:
		return key::AltR;
	case AKEYCODE_META_LEFT:
		return key::SuperL;
	case AKEYCODE_META_RIGHT:
		return key::SuperR;
	case AKEYCODE_SPACE:
		return key::Space;
	case AKEYCODE_SHIFT_LEFT:
		return key::ShiftL;
	case AKEYCODE_SHIFT_RIGHT:
		return key::ShiftR;
	case AKEYCODE_F1:
		return key::F1;
	case AKEYCODE_F2:
		return key::F2;
	case AKEYCODE_F3:
		return key::F3;
	case AKEYCODE_F4:
		return key::F4;
	case AKEYCODE_F5:
		return key::F5;
	case AKEYCODE_F6:
		return key::F6;
	case AKEYCODE_F7:
		return key::F7;
	case AKEYCODE_F8:
		return key::F8;
	case AKEYCODE_F9:
		return key::F9;
	case AKEYCODE_F10:
		return key::F10;
	case AKEYCODE_F11:
		return key::F11;
	case AKEYCODE_F12:
		return key::F12;
	default:
		return std::nullopt;
	}
}

std::string toUtf8(const uint32_t codepoint) {
	std::string result;
	if (codepoint < 0x80) {
		result += static_cast<char>(codepoint);
	} else if (codepoint < 0x800) {
		result += static_cast<char>(0xc0 | (codepoint >> 6));
		result += static_cast<char>(0x80 | (codepoint & 0x3f));
	} else if (codepoint < 0x10000) {
		result += static_cast<char>(0xe0 | (codepoint >> 12));
		result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f));
		result += static_cast<char>(0x80 | (codepoint & 0x3f));
	} else {
		result += static_cast<char>(0xf0 | (codepoint >> 18));
		result += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3f));
		result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f));
		result += static_cast<char>(0x80 | (codepoint & 0x3f));
	}
	return result;
}

} // namespace

int32_t WindowImpl::getUnicodeChar(const AInputEvent* const event) const {
	const jclass keyEventClass = env->FindClass("android/view/KeyEvent");
	if (!keyEventClass) {
		env->ExceptionClear();
		return 0;
	}
	Finally releaseClass([&]() { env->DeleteLocalRef(keyEventClass); });
	// Use the constructor which takes the device id so that KeyEvent.getUnicodeChar uses the key
	// character map (i.e. the keyboard layout) of the keyboard the event came from:
	const jmethodID constructor = env->GetMethodID(keyEventClass, "<init>", "(JJIIIIII)V");
	const jmethodID getUnicodeCharMethod =
	    env->GetMethodID(keyEventClass, "getUnicodeChar", "(I)I");
	if (!constructor || !getUnicodeCharMethod) {
		env->ExceptionClear();
		return 0;
	}
	const int32_t metaState = AKeyEvent_getMetaState(event);
	const jobject keyEvent = env->NewObject(
	    keyEventClass, constructor,
	    static_cast<jlong>(AKeyEvent_getDownTime(event) / 1000000), // ns -> ms
	    static_cast<jlong>(AKeyEvent_getEventTime(event) / 1000000), // ns -> ms
	    static_cast<jint>(AKEY_EVENT_ACTION_DOWN), static_cast<jint>(AKeyEvent_getKeyCode(event)),
	    static_cast<jint>(AKeyEvent_getRepeatCount(event)), static_cast<jint>(metaState),
	    static_cast<jint>(AInputEvent_getDeviceId(event)),
	    static_cast<jint>(AKeyEvent_getScanCode(event)));
	if (!keyEvent) {
		env->ExceptionClear();
		return 0;
	}
	Finally releaseEvent([&]() { env->DeleteLocalRef(keyEvent); });
	const int32_t unicode = env->CallIntMethod(keyEvent, getUnicodeCharMethod, metaState);
	if (env->ExceptionCheck()) {
		env->ExceptionClear();
		return 0;
	}
	return unicode;
}

bool WindowImpl::isAlphabeticKeyboard(const int32_t deviceId) {
	if (const auto it = alphabeticKeyboards.find(deviceId); it != alphabeticKeyboards.end()) {
		return it->second;
	}
	const jclass inputDeviceClass = env->FindClass("android/view/InputDevice");
	if (!inputDeviceClass) {
		env->ExceptionClear();
		return false;
	}
	Finally releaseClass([&]() { env->DeleteLocalRef(inputDeviceClass); });
	const jmethodID getDevice =
	    env->GetStaticMethodID(inputDeviceClass, "getDevice", "(I)Landroid/view/InputDevice;");
	const jmethodID getKeyboardType = env->GetMethodID(inputDeviceClass, "getKeyboardType", "()I");
	if (!getDevice || !getKeyboardType) {
		env->ExceptionClear();
		return false;
	}
	const jobject device = env->CallStaticObjectMethod(inputDeviceClass, getDevice, deviceId);
	if (env->ExceptionCheck() || !device) {
		env->ExceptionClear();
		return false; // the device might already be gone, don't cache this
	}
	Finally releaseDevice([&]() { env->DeleteLocalRef(device); });
	const jint keyboardType = env->CallIntMethod(device, getKeyboardType);
	if (env->ExceptionCheck()) {
		env->ExceptionClear();
		return false;
	}
	const bool result = keyboardType == 2; // InputDevice.KEYBOARD_TYPE_ALPHABETIC
	alphabeticKeyboards[deviceId] = result; // device ids are never reused, so this is safe to cache
	return result;
}

int WindowImpl::handleKeyEvent(const AInputEvent* const event) {
	const int32_t keyCode = AKeyEvent_getKeyCode(event);
	const int32_t action = AKeyEvent_getAction(event);
	if (keyCode == AKEYCODE_BACK) {
		if (action == AKEY_EVENT_ACTION_DOWN) {
			if (const auto& scene = window->getScene()) {
				scene->onBackEvent();
			}
		}
		return 1;
	}
	const std::optional<key::KeyType> jnglKey = toJnglKey(keyCode);
	bool handled = false;
	if (action == AKEY_EVENT_ACTION_UP) {
		if (jnglKey) {
			// Don't touch keyPressed_ here: The IME sends key down and key up back to back (e.g.
			// for Backspace), so clearing it now would hide the press from the game.
			window->keyDown_[*jnglKey] = false;
			handled = true;
		}
		if (const auto it = keyCodeToCharacter.find(keyCode); it != keyCodeToCharacter.end()) {
			window->characterDown_[it->second] = false;
			keyCodeToCharacter.erase(it);
			handled = true;
		}
		return handled ? 1 : 0;
	}
	if (action != AKEY_EVENT_ACTION_DOWN) {
		return 0; // AKEY_EVENT_ACTION_MULTIPLE is deprecated and the NDK can't read its characters
	}
	// Android sends repeated key down events while a key is held. Like SDL we treat those like a
	// fresh press, that's what makes holding Backspace in a text field delete repeatedly.
	if (jnglKey) {
		window->keyDown_[*jnglKey] = true;
		window->keyPressed_[*jnglKey] = true;
		window->needToBeSetFalse_.push(&window->keyPressed_[*jnglKey]);
		window->addKeyPressed(*jnglKey);
		handled = true;
	}
	const int32_t unicode = getUnicodeChar(event);
	// Dead keys (e.g. ^ on a German layout) have KeyCharacterMap.COMBINING_ACCENT set, there's no
	// character to type yet. Control characters (Enter, Tab, ...) are only reported as keys.
	const bool deadKey = (unicode & 0x80000000) != 0;
	if (!deadKey && unicode >= 0x20 && unicode != 0x7f) {
		const std::string character = toUtf8(static_cast<uint32_t>(unicode));
		if (const auto it = keyCodeToCharacter.find(keyCode);
		    it != keyCodeToCharacter.end() && it->second != character) {
			// A key repeat where a modifier changed in the meantime, e.g. Shift got pressed while
			// holding A: "a" is no longer down.
			window->characterDown_[it->second] = false;
		}
		keyCodeToCharacter[keyCode] = character;
		window->characterDown_[character] = true;
		window->characterPressed_[character] = true;
		window->needToBeSetFalse_.push(&window->characterPressed_[character]);
		window->addKeyPressed(Key(character));
		const int32_t metaState = AKeyEvent_getMetaState(event);
		if ((metaState & (AMETA_CTRL_ON | AMETA_META_ON)) == 0) { // Ctrl+S shouldn't type an "s"
			window->textInput += character;
			internal::feedTextInput(character);
		}
		handled = true;
	}
	if (handled) {
		window->anyKeyPressed_ = true;
	}
	return handled ? 1 : 0;
}

void WindowImpl::updateInput() {
	touchPressedThisUpdate = false;
	if (app->destroyRequested != 0) {
		// The activity is being destroyed and is waiting for android_main to return (blocking the
		// UI thread). There won't be any new window, so we must not wait for one.
		return;
	}
	// Read all pending events.
	int ident;
	int events;
	android_poll_source* source;

	while ((ident = ALooper_pollOnce(
	            // This is the timeout. When we're in the background, we don't want to busy-wait for
	            // events.
	            (display && display->surface) ? 0 : 1e9, nullptr, &events, (void**)&source)) >= 0 ||
	       !display /* wait for WindowImpl::init to get called by engine_handle_cmd */ ||
	       !display->surface /* we're in the background, don't leave this event loop */) {

		// Process this event.
		if (source != NULL) {
			source->process(app, source);
		}

		// Check if we are exiting.
		if (app->destroyRequested != 0) {
			display = std::nullopt;
			// Not jngl::quit(), because the game might call jngl::cancelQuit() in onQuitEvent()
			// (e.g. to show a pause menu). We can't continue though.
			if (!window->forceExitCode) {
				window->forceExitCode = 0;
			}
			return; // surface == nullptr, we need to exit this loop
		}
	}

	window->mouseDown_[0] = !touches.empty();
	if (touchPressedThisUpdate) {
		window->mousePressed_[0] = true;
		window->needToBeSetFalse_.push(&window->mousePressed_[0]);
	}
	window->multitouch = touches.size() > 1;
	window->mousex_ = mouseX - relativeX;
	window->mousey_ = mouseY - relativeY;
	if (window->relativeMouseMode) {
		relativeX = mouseX;
		relativeY = mouseY;
	}
}

void WindowImpl::swapBuffers() {
	if (display && display->surface) {
		if (eglSwapBuffers(display->display, display->surface->surface) == EGL_FALSE) {
			handleEglError();
		}
		// We get APP_CMD_WINDOW_RESIZED e.g. when a foldable gets unfolded, but the EGL surface
		// only picks up the new size of the native window after eglSwapBuffers:
		const auto [w, h] = getSurfaceSize();
		window->setActualSize(w, h);
		if (firstFrame) {
			firstFrame = false;
			JNIEnv* const jni = env;
			jclass clazz = jni->GetObjectClass(app->activity->clazz);
			jmethodID methodID = jni->GetMethodID(clazz, "markNativeCodeReady", "()V");
			if (methodID) {
				jni->CallVoidMethod(app->activity->clazz, methodID);
			} else {
				jni->ExceptionClear();
			}
		}
	}
}

WindowImpl* Window::getImpl() const { return impl.get(); }

void setKeyboardVisible(const bool visible) {
	pWindow->getImpl()->setKeyboardVisible(visible);
}

void WindowImpl::setKeyboardVisible(const bool visible) {
	const jclass nativeActivityClass = env->GetObjectClass(app->activity->clazz);

	const jmethodID getApplicationContextMethod = env->GetMethodID(
			nativeActivityClass, "getApplicationContext", "()Landroid/content/Context;");

	const jobject contextObject =
	    env->CallObjectMethod(app->activity->clazz, getApplicationContextMethod);

	const jclass contextClass = env->FindClass("android/content/Context");

	const jmethodID getSystemServiceMethod = env->GetMethodID(
			contextClass, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");

	const jfieldID INPUT_METHOD_SERVICE_FIELD =
			env->GetStaticFieldID(contextClass, "INPUT_METHOD_SERVICE", "Ljava/lang/String;");

	const jobject INPUT_METHOD_SERVICE =
			env->GetStaticObjectField(contextClass, INPUT_METHOD_SERVICE_FIELD);

	const jobject inputMethodManager =
			env->CallObjectMethod(contextObject, getSystemServiceMethod, INPUT_METHOD_SERVICE);

	const jclass inputMethodManagerClass = env->GetObjectClass(inputMethodManager);

	const jmethodID getWindowMethod =
	    env->GetMethodID(nativeActivityClass, "getWindow", "()Landroid/view/Window;");

	const jobject window = env->CallObjectMethod(app->activity->clazz, getWindowMethod);

	const jclass windowClass = env->FindClass("android/view/Window");

	const jmethodID getDecorViewMethod =
	    env->GetMethodID(windowClass, "getDecorView", "()Landroid/view/View;");

	const jobject decorView = env->CallObjectMethod(window, getDecorViewMethod);

	if (visible) {
		const jmethodID showSoftInputMethod =
		    env->GetMethodID(inputMethodManagerClass, "showSoftInput", "(Landroid/view/View;I)Z");

		const jfieldID SHOW_FORCED_FIELD =
		    env->GetStaticFieldID(inputMethodManagerClass, "SHOW_FORCED", "I");

		const int showFlags = env->GetStaticIntField(inputMethodManagerClass, SHOW_FORCED_FIELD);

		env->CallBooleanMethod(inputMethodManager, showSoftInputMethod, decorView, showFlags);
	} else {
		const jclass viewClass = env->FindClass("android/view/View");

		const jmethodID getWindowTokenMethod =
		    env->GetMethodID(viewClass, "getWindowToken", "()Landroid/os/IBinder;");

		const jobject windowToken = env->CallObjectMethod(decorView, getWindowTokenMethod);

		const jmethodID hideSoftInputFromWindowMethod = env->GetMethodID(
		    inputMethodManagerClass, "hideSoftInputFromWindow", "(Landroid/os/IBinder;I)Z");

		env->CallBooleanMethod(inputMethodManager, hideSoftInputFromWindowMethod, windowToken, 0);
	}
}

std::vector<std::shared_ptr<Controller>> WindowImpl::getConnectedControllers() const {
	std::vector<std::shared_ptr<Controller>> rtn;
	for (auto [id, controller] : controllers) {
		rtn.emplace_back(controller);
	}
	return rtn;
}

int32_t WindowImpl::handleJoystickEvent(const AInputEvent* const event) {
	const int32_t deviceId = AInputEvent_getDeviceId(event);
	auto& controller = controllers[deviceId];
	if (!controller) {
		controller = std::make_shared<AndroidController>();
		if (window->controllerChangedCallback) {
			window->controllerChangedCallback();
		}
		for (const auto& job : window->jobs) {
			job->onControllersChanged();
		}
		if (window->currentWork_) {
			window->currentWork_->onControllersChanged();
		}
	}
	switch (AInputEvent_getType(event)) {
	case AINPUT_EVENT_TYPE_KEY: {
		const bool down = AKeyEvent_getAction(event) == AKEY_EVENT_ACTION_DOWN;
		switch (AKeyEvent_getKeyCode(event)) {
		case AKEYCODE_BUTTON_A:
		case AKEYCODE_DPAD_CENTER:
			controller->buttonA = down;
			break;
		case AKEYCODE_BUTTON_B:
			controller->buttonB = down;
			break;
		case AKEYCODE_BUTTON_START:
			controller->buttonStart = down;
			break;
		case AKEYCODE_BACK:
			controller->buttonBack = down;
			if (down && pWindow) {
				if (const auto scene = pWindow->getScene()) {
					scene->onControllerBack();
				}
			}
			break;
		case AKEYCODE_DPAD_LEFT:
			controller->dpadX = down ? -1 : 0;
			break;
		case AKEYCODE_DPAD_RIGHT:
			controller->dpadX = down ? 1 : 0;
			break;
		case AKEYCODE_DPAD_UP:
			controller->dpadY = down ? -1 : 0;
			break;
		case AKEYCODE_DPAD_DOWN:
			controller->dpadY = down ? 1 : 0;
			break;
		}
		break;
	}
	case AINPUT_EVENT_TYPE_MOTION:
		assert(AMotionEvent_getPointerCount(event) == 1);
		controller->leftStickX = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_X, 0);
		controller->leftStickY = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_Y, 0);
		controller->rightStickX = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_Z, 0);
		controller->rightStickY = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_RZ, 0);
		controller->dpadX = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_HAT_X, 0);
		controller->dpadY = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_HAT_Y, 0);
		break;
	default:
		internal::warn("Unknown joystick event!");
	}
	return 1;
}

std::vector<Vec2> Window::getTouchPositions() const {
	std::vector<Vec2> positions;
	for (auto [id, pos] : impl->touches) {
		positions.emplace_back(
		    (pos.x - (width_ - canvasWidth) / 2) / getScaleFactor() - getScreenWidth() / 2,
		    (pos.y - (height_ - canvasHeight) / 2) / getScaleFactor() - getScreenHeight() / 2);
	}
	return positions;
}

void Window::setFullscreen(bool fullscreen) {
	if (!fullscreen) {
		internal::warn("Can't unset fullscreen on Android!");
	}
}

void Window::startTextInputSession(TextInputType) {
	setKeyboardVisible(true);
}

void Window::stopTextInputSession() {
	setKeyboardVisible(false);
}

void Window::setTextInputArea(Rect, double) {
	// Android positions the IME itself, there's nothing to tell it here.
}

std::string getSystemConfigPath() {
	return jngl::androidApp->activity->internalDataPath;
}

namespace {
namespace jni {
class Class {
	const jclass javaClass;

public:
	explicit Class(const char* name)
	: javaClass(reinterpret_cast<WindowImpl*>(androidApp->userData)->env->FindClass(name)) {
		if (!javaClass) {
			throw std::runtime_error("Couldn't find class " + std::string(name));
		}
	}
	explicit Class(jobject object)
	: javaClass(reinterpret_cast<WindowImpl*>(androidApp->userData)->env->GetObjectClass(object)) {
		assert(javaClass);
	}
	~Class() {
		reinterpret_cast<WindowImpl*>(androidApp->userData)->env->DeleteLocalRef(javaClass);
	}
	Class(const Class&) = delete;
	Class& operator=(const Class&) = delete;

	operator jclass() const {
		return javaClass;
	}
};

class String {
	const jstring javaString;

public:
	explicit String(const char* str)
	: javaString(reinterpret_cast<WindowImpl*>(androidApp->userData)->env->NewStringUTF(str)) {
		assert(javaString);
	}
	~String() {
		reinterpret_cast<WindowImpl*>(androidApp->userData)->env->DeleteLocalRef(javaString);
	}
	String(const String&) = delete;
	String& operator=(const String&) = delete;

	operator jstring() const {
		return javaString;
	}
};
} // namespace jni
} // namespace

std::string getPreferredLanguage() {
	const auto env = reinterpret_cast<WindowImpl*>(androidApp->userData)->env;
	const jni::Class localeClass("java/util/Locale");
	const jobject defaultLocale = env->CallStaticObjectMethod(
	    localeClass, env->GetStaticMethodID(localeClass, "getDefault", "()Ljava/util/Locale;"));

	const jobject language = env->CallObjectMethod(
	    defaultLocale, env->GetMethodID(localeClass, "getLanguage", "()Ljava/lang/String;"));
	env->DeleteLocalRef(defaultLocale);

	jni::Class stringClass(language);
	const jmethodID getBytesMethod =
	    env->GetMethodID(stringClass, "getBytes", "(Ljava/lang/String;)[B");

	jni::String utf8("UTF-8");
	const auto bytesObject = static_cast<jbyteArray>(
	    env->CallObjectMethod(language, getBytesMethod, static_cast<jstring>(utf8)));
	Finally releaseBytesObject([&]() { env->DeleteLocalRef(bytesObject); });
	env->DeleteLocalRef(language);
	const size_t length = env->GetArrayLength(bytesObject);
	if (length != 2) {
		internal::error("Couldn't get preferred language. Falling back to \"en\".");
		return "en";
	}
	auto byteArray = env->GetByteArrayElements(bytesObject, nullptr);
	std::string result(reinterpret_cast<const char*>(byteArray), length);
	env->ReleaseByteArrayElements(bytesObject, reinterpret_cast<jbyte*>(byteArray), JNI_ABORT);
	return result;
}

void openURL(const std::string& url) {
	const auto env = reinterpret_cast<WindowImpl*>(androidApp->userData)->env;
	jni::String urlString(url.c_str());
	jni::Class uriClass("android/net/Uri");
	jmethodID uriParse =
	    env->GetStaticMethodID(uriClass, "parse", "(Ljava/lang/String;)Landroid/net/Uri;");
	jobject uri = env->CallStaticObjectMethod(uriClass, uriParse, static_cast<jstring>(urlString));
	Finally releaseUri([&]() { env->DeleteLocalRef(uri); });

	jni::Class intentClass("android/content/Intent");
	jobject actionView = env->GetStaticObjectField(
	    intentClass, env->GetStaticFieldID(intentClass, "ACTION_VIEW", "Ljava/lang/String;"));
	Finally releaseActionView([&]() { env->DeleteLocalRef(actionView); });

	jmethodID newIntent =
	    env->GetMethodID(intentClass, "<init>", "(Ljava/lang/String;Landroid/net/Uri;)V");
	jobject intent = env->AllocObject(intentClass);
	Finally releaseIntent([&]() { env->DeleteLocalRef(intent); });

	env->CallVoidMethod(intent, newIntent, actionView, uri);

	jni::Class activity_class("android/app/Activity");
	jmethodID start_activity =
	    env->GetMethodID(activity_class, "startActivity", "(Landroid/content/Intent;)V");
	env->CallVoidMethod(androidApp->activity->clazz, start_activity, intent);
}

void setCursor(jngl::Cursor) {}

} // namespace jngl
