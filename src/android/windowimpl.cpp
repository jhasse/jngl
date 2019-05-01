// Copyright 2015-2019 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "windowimpl.hpp"

#include "../jngl/other.hpp"
#include "../jngl/debug.hpp"
#include "../audio.hpp"
#include "../windowptr.hpp"
#include "../main.hpp"
#include "fopen.hpp"

#include <android_native_app_glue.h>
#include <android/storage_manager.h>

namespace jngl {

android_app* androidApp;

static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
	jngl::debug("Received event "); jngl::debugLn(cmd);
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
			// TODO: The window is being hidden or closed, clean it up.
			break;
		case APP_CMD_GAINED_FOCUS:
			impl.makeCurrent();
			break;
		case APP_CMD_PAUSE:
			impl.pause();
			break;
	}
}

static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) {
	WindowImpl& impl = *reinterpret_cast<WindowImpl*>(app->userData);
	switch (AInputEvent_getType(event)) {
		case AINPUT_EVENT_TYPE_KEY:
			if (AKeyEvent_getAction(event) == AKEY_EVENT_ACTION_DOWN) {
				return impl.handleKeyEvent(event);
			}
			break;
		case AINPUT_EVENT_TYPE_MOTION: {
			const auto action = AMotionEvent_getAction(event);
			switch (action & AMOTION_EVENT_ACTION_MASK) {
				case AMOTION_EVENT_ACTION_POINTER_DOWN:
				case AMOTION_EVENT_ACTION_DOWN:
				case AMOTION_EVENT_ACTION_MOVE: {
					const int32_t index = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >>
					                      AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
					const auto x = AMotionEvent_getX(event, index);
					const auto y = AMotionEvent_getY(event, index);
					auto& touch = impl.touches[AMotionEvent_getPointerId(event, index)];
					impl.mouseX = touch.x = x;
					impl.mouseY = touch.y = y;
					return 1;
				}
				case AMOTION_EVENT_ACTION_POINTER_UP:
				case AMOTION_EVENT_ACTION_UP: {
					const int32_t index = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >>
					                      AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
					const auto it = impl.touches.find(AMotionEvent_getPointerId(event, index));
					if (it != impl.touches.end()) {
						impl.touches.erase(it);
					}
					return 1;
				}
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
	jngl::debugLn("Handler set.");

	android_asset_manager = app->activity->assetManager;
	assert(android_asset_manager);
	app->activity->vm->AttachCurrentThread(&env, nullptr);
}

WindowImpl::~WindowImpl() {
	// We need to destroy the surface before finishing our activity, otherwise the app won't start
	// again.
	pause();
	// jngl::quit() has been called. We need to gracefully quit the activity, too, and handle any
	// pending events (this will result in destroyRequested != 0):
	ANativeActivity_finish(app->activity);
	updateInput();
	app->activity->vm->DetachCurrentThread();
}

void WindowImpl::init() {
	if (initialized) {
		return;
	}
	// Here specify the attributes of the desired configuration. Below, we select an EGLConfig with
	// at least 8 bits per color component compatible with on-screen windows
	const EGLint attribs[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_BLUE_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_RED_SIZE, 8,
		EGL_NONE
	};
	EGLint format;
	EGLint numConfigs;

	display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	eglInitialize(display, 0, 0);

	// Here, the application chooses the configuration it desires. In this sample, we have a very
	// simplified selection process, where we pick the first EGLConfig that matches our criteria
	eglChooseConfig(display, attribs, &config, 1, &numConfigs);

	// EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is guaranteed to be accepted by
	// ANativeWindow_setBuffersGeometry(). As soon as we picked a EGLConfig, we can safely
	// reconfigure the ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID.
	eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

	if (!app) {
		throw std::runtime_error("android_app struct not set. "
		                         "Use JNGL_MAIN_BEGIN and JNGL_MAIN_END.");
	}
	ANativeWindow_setBuffersGeometry(app->window, 0, 0, format);

	const EGLint contextAttribList[] = {
			EGL_CONTEXT_CLIENT_VERSION, 3,
			EGL_NONE
	};
	context = eglCreateContext(display, config, NULL, contextAttribList);

	makeCurrent();

	EGLint w, h;
	eglQuerySurface(display, surface, EGL_WIDTH, &w);
	eglQuerySurface(display, surface, EGL_HEIGHT, &h);
	window->width_ = w;
	window->height_ = h;
	window->calculateCanvasSize(minAspectRatio, maxAspectRatio);

	// Initialize GL state.
	Init(window->width_, window->height_, window->canvasWidth, window->canvasHeight);

	initialized = true;
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
	if (!surface) { return; }
	if (eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT) ==
		EGL_FALSE) {
		debugLn("Couldn't unbind surfaces!");
	}
	if (eglDestroySurface(display, surface) == EGL_FALSE) {
		debugLn("Couldn't destroy surface!");
	}
	surface = nullptr;
	pauseAudioDevice();
}

void WindowImpl::makeCurrent() {
	if (surface) { return; }
	surface = eglCreateWindowSurface(display, config, app->window, nullptr);
	if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
		throw std::runtime_error("Unable to eglMakeCurrent");
	}
	resumeAudioDevice();
}

int WindowImpl::handleKeyEvent(AInputEvent* const event) {
	const auto key = AKeyEvent_getKeyCode(event);
	if (key == AKEYCODE_DEL) {
		jngl::setKeyPressed(jngl::key::BackSpace, true);
		return 1;
	} else if (key == AKEYCODE_BACK) {
		jngl::quit();
		return 1;
	}
	const auto metaState = AKeyEvent_getMetaState(event);

	const jclass keyEventClass = env->FindClass("android/view/KeyEvent");

	const jmethodID getUnicodeCharMethod =
	    env->GetMethodID(keyEventClass, "getUnicodeChar", (metaState == 0) ? "()I" : "(I)I");

	const jmethodID eventConstructor = env->GetMethodID(keyEventClass, "<init>", "(II)V");

	const jobject eventObj =
	    env->NewObject(keyEventClass, eventConstructor, AKEY_EVENT_ACTION_DOWN, key);

	const int unicode = (metaState == 0)
	                        ? env->CallIntMethod(eventObj, getUnicodeCharMethod)
	                        : env->CallIntMethod(eventObj, getUnicodeCharMethod, metaState);

	if (unicode == 0) { // For example the back button
		return 0;
	}

	char bytes[5];
	std::memcpy(bytes, &unicode, 4);
	if (bytes[0] == '\n') {
		setKeyPressed(jngl::key::Return, true);
		return 1;
	}
	bytes[4] = '\0';
	window->textInput += std::string(bytes);
	return 1;
}

void WindowImpl::updateInput() {
	// Read all pending events.
	int ident;
	int events;
	android_poll_source* source;

	while ((ident = ALooper_pollAll(
	            surface ? 0 : 1e9, // This is the timeout. When we're in the background, we don't
	                               // want to busy-wait for events.
	            nullptr, &events, (void**)&source)) >= 0 ||
	       !initialized /* wait for WindowImpl::init to get called by engine_handle_cmd */ ||
	       !surface /* we're in the background, don't leave this event loop */) {

		// Process this event.
		if (source != NULL) {
			source->process(app, source);
		}

		// Check if we are exiting.
		if (app->destroyRequested != 0) {
			if (eglDestroyContext(display, context) == EGL_FALSE) {
				debugLn("Couldn't destroy context!");
			}
			if (eglTerminate(display) == EGL_FALSE) {
				debugLn("Couldn't terminate display!");
			}
			jngl::quit();
			return; // surface == nullptr, we need to exit this loop
		}
	}

	if (!window->mouseDown_[0]) {
		window->mousePressed_[0] = !touches.empty();
	}
	window->mouseDown_[0] = !touches.empty();
	window->multitouch = touches.size() > 1;
	window->mousex_ = mouseX - relativeX;
	window->mousey_ = mouseY - relativeY;
	if (window->relativeMouseMode) {
		relativeX = mouseX;
		relativeY = mouseY;
	}
}

void WindowImpl::swapBuffers() {
	if (surface) {
		eglSwapBuffers(display, surface);
	}
}

WindowImpl* Window::getImpl() const { return impl; }

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

	const jmethodID toggleSoftInputMethod =
			env->GetMethodID(inputMethodManagerClass, "toggleSoftInput", "(II)V");

	int showFlags;
	if (visible) {
		const jfieldID SHOW_FORCED_FIELD =
				env->GetStaticFieldID(inputMethodManagerClass, "SHOW_FORCED", "I");

		showFlags = env->GetStaticIntField(inputMethodManagerClass, SHOW_FORCED_FIELD);
	} else {
		const jfieldID HIDE_IMPLICIT_ONLY_FIELD =
				env->GetStaticFieldID(inputMethodManagerClass, "HIDE_IMPLICIT_ONLY", "I");

		showFlags = env->GetStaticIntField(inputMethodManagerClass, HIDE_IMPLICIT_ONLY_FIELD);
	}

	env->CallVoidMethod(inputMethodManager, toggleSoftInputMethod, showFlags, 0);
}

std::vector<Vec2> Window::getTouchPositions() const {
	std::vector<Vec2> positions;
	for (auto [id, pos] : impl->touches) {
		positions.emplace_back(pos.x - (width_ - canvasWidth) / 2,
		                       pos.y - (height_ - canvasHeight) / 2);
	}
	return positions;
}

} // namespace jngl
