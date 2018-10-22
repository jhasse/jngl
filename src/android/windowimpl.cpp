// Copyright 2015-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "windowimpl.hpp"

#include "../jngl/other.hpp"
#include "../jngl/debug.hpp"
#include "../window.hpp"
#include "../main.hpp"
#include "fopen.hpp"

#include <android_native_app_glue.h>
#include <android/storage_manager.h>

namespace jngl {

android_app* androidApp;

static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
	jngl::debugLn("Received event");
	WindowImpl& impl = *reinterpret_cast<WindowImpl*>(app->userData);
	switch (cmd) {
		case APP_CMD_SAVE_STATE:
			// TODO: The system has asked us to save our current state.  Do so.
			break;
		case APP_CMD_INIT_WINDOW:
			// The window is being shown, get it ready.
			if (androidApp->window) {
				impl.init();
			}
			break;
		case APP_CMD_TERM_WINDOW:
			// TODO: The window is being hidden or closed, clean it up.
			break;
		case APP_CMD_GAINED_FOCUS:
			// TODO
			break;
		case APP_CMD_LOST_FOCUS:
			// TODO
			break;
	}
}

static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) {
	WindowImpl& impl = *reinterpret_cast<WindowImpl*>(app->userData);
	switch (AInputEvent_getType(event)) {
		case AINPUT_EVENT_TYPE_KEY:
			switch (AKeyEvent_getAction(event)) {
				case AKEY_EVENT_ACTION_DOWN: {
					const auto key = AKeyEvent_getKeyCode(event);
					const auto metaState = AKeyEvent_getMetaState(event);

					JNIEnv* env;
					app->activity->vm->AttachCurrentThread(&env, nullptr);

					const jclass keyEventClass = env->FindClass("android/view/KeyEvent");

					const jmethodID getUnicodeCharMethod =
							env->GetMethodID(keyEventClass, "getUnicodeChar", "()I");

					const jmethodID eventConstructor =
							env->GetMethodID(keyEventClass, "<init>", "(II)V");

					const jobject eventObj = env->NewObject(
							keyEventClass, eventConstructor, AKEY_EVENT_ACTION_DOWN, key);

					const int unicode = (metaState == 0)
							? env->CallIntMethod(eventObj, getUnicodeCharMethod)
							: env->CallIntMethod(eventObj, getUnicodeCharMethod, metaState);

					app->activity->vm->DetachCurrentThread();

					if (unicode == 0) { // For example the back button
						return 0;
					}

					char bytes[5];
					std::memcpy(bytes, &unicode, 4);
					bytes[4] = '\0';
					impl.addTextInput(bytes);
					return 1;
				}
			}
			break;
		case AINPUT_EVENT_TYPE_MOTION:
			switch (AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK) {
				case AMOTION_EVENT_ACTION_POINTER_DOWN:
					++impl.numberOfTouches;
					return 1;
				case AMOTION_EVENT_ACTION_DOWN:
					++impl.numberOfTouches;
					[[fallthrough]];
				case AMOTION_EVENT_ACTION_MOVE:
					impl.mouseX = AMotionEvent_getX(event, 0);
					impl.mouseY = AMotionEvent_getY(event, 0);
					return 1;
				case AMOTION_EVENT_ACTION_POINTER_UP:
				case AMOTION_EVENT_ACTION_UP:
					--impl.numberOfTouches;
					return 1;
			};
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

	JNIEnv* env = nullptr;
	app->activity->vm->AttachCurrentThread(&env, nullptr);

	jclass activityClass = env->FindClass("android/app/NativeActivity");
	jmethodID getWindow = env->GetMethodID(activityClass, "getWindow", "()Landroid/view/Window;");

	jclass windowClass = env->FindClass("android/view/Window");
	jmethodID getDecorView = env->GetMethodID(windowClass, "getDecorView", "()Landroid/view/View;");

	jclass viewClass = env->FindClass("android/view/View");
	jmethodID setSystemUiVisibility = env->GetMethodID(viewClass, "setSystemUiVisibility", "(I)V");

	jobject androidWindow = env->CallObjectMethod(app->activity->clazz, getWindow);

	jobject decorView = env->CallObjectMethod(androidWindow, getDecorView);

	jfieldID flagFullscreenID = env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_FULLSCREEN", "I");
	jfieldID flagHideNavigationID =
	    env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_HIDE_NAVIGATION", "I");
	jfieldID flagImmersiveStickyID =
	    env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_IMMERSIVE_STICKY", "I");

	const int flagFullscreen = env->GetStaticIntField(viewClass, flagFullscreenID);
	const int flagHideNavigation = env->GetStaticIntField(viewClass, flagHideNavigationID);
	const int flagImmersiveSticky = env->GetStaticIntField(viewClass, flagImmersiveStickyID);
	const int flag = flagFullscreen | flagHideNavigation | flagImmersiveSticky;

	env->CallVoidMethod(decorView, setSystemUiVisibility, flag);

	app->activity->vm->DetachCurrentThread();
}

void WindowImpl::init() {
	/*
	 * Here specify the attributes of the desired configuration.
	 * Below, we select an EGLConfig with at least 8 bits per color
	 * component compatible with on-screen windows
	 */
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
	EGLConfig config;

	display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	eglInitialize(display, 0, 0);

	/* Here, the application chooses the configuration it desires. In this
	 * sample, we have a very simplified selection process, where we pick
	 * the first EGLConfig that matches our criteria */
	eglChooseConfig(display, attribs, &config, 1, &numConfigs);

	/* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
	 * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
	 * As soon as we picked a EGLConfig, we can safely reconfigure the
	 * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
	eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

	if (!app) {
		throw std::runtime_error("android_app struct not set. "
		                         "Use JNGL_MAIN_BEGIN and JNGL_MAIN_END.");
	}
	ANativeWindow_setBuffersGeometry(app->window, 0, 0, format);

	surface = eglCreateWindowSurface(display, config, app->window, NULL);
	const EGLint contextAttribList[] = {
			EGL_CONTEXT_CLIENT_VERSION, 3,
			EGL_NONE
	};
	context = eglCreateContext(display, config, NULL, contextAttribList);

	if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
		throw std::runtime_error("Unable to eglMakeCurrent");
	}

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
	} else {
		relativeX = relativeY = 0;
	}
}

void WindowImpl::addTextInput(const char* const character) {
	window->textInput += std::string(character);
}

void WindowImpl::updateInput() {
	// Read all pending events.
	int ident;
	int events;
	android_poll_source* source;

	while ((ident = ALooper_pollAll(0, NULL, &events, (void**)&source)) >= 0 ||
	       !initialized /* wait for WindowImpl::init to get called by engine_handle_cmd */) {

		// Process this event.
		if (source != NULL) {
			source->process(app, source);
		}

		// Check if we are exiting.
		if (app->destroyRequested != 0) {
			if (eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT) ==
			    EGL_FALSE) {
				debugLn("Couldn't unbind surfaces!");
			}
			if (eglDestroyContext(display, context) == EGL_FALSE) {
				debugLn("Couldn't destroy context!");
			}
			if (eglDestroySurface(display, surface) == EGL_FALSE) {
				debugLn("Couldn't destroy surface!");
			}
			if (eglTerminate(display) == EGL_FALSE) {
				debugLn("Couldn't terminate display!");
			}
			jngl::quit();
		}
	}

	if (!window->mouseDown_[0]) {
		window->mousePressed_[0] = numberOfTouches > 0;
	}
	window->mouseDown_[0] = numberOfTouches > 0;
	window->multitouch = numberOfTouches > 1;
	window->mousex_ = mouseX - relativeX;
	window->mousey_ = mouseY - relativeY;
	if (window->relativeMouseMode) {
		relativeX = mouseX;
		relativeY = mouseY;
	}
}

void WindowImpl::swapBuffers() {
	eglSwapBuffers(display, surface);
}

void setKeyboardVisible(const bool visible) {
	const auto app = androidApp->activity;

	JNIEnv* env = nullptr;
	app->vm->AttachCurrentThread(&env, nullptr);

	const jclass nativeActivityClass = env->GetObjectClass(app->clazz);

	const jmethodID getApplicationContextMethod = env->GetMethodID(
			nativeActivityClass, "getApplicationContext", "()Landroid/content/Context;");

	const jobject contextObject = env->CallObjectMethod(app->clazz, getApplicationContextMethod);

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

	app->vm->DetachCurrentThread();
}

} // namespace jngl
