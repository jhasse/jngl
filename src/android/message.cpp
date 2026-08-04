// Copyright 2013-2026 Jan Niklas Hasse <jhasse@gmail.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/message.hpp"

#include "../jngl/Finally.hpp"
#include "../jngl/log.hpp"

#include <android/log.h>
#include <android_native_app_glue.h>
#include <jni.h>

namespace jngl {

extern android_app* androidApp;

namespace {

/// Loads one of the app's own classes, rather than a framework class
///
/// FindClass() wouldn't do: on the game loop thread, which JNGL attached to the VM itself, it only
/// searches the system ClassLoader, which doesn't know about the classes in the APK.
jclass loadAppClass(JNIEnv* env, jobject activity, const char* name) {
	const jclass activityClass = env->GetObjectClass(activity);
	Finally releaseActivityClass([&]() { env->DeleteLocalRef(activityClass); });
	const jobject classLoader = env->CallObjectMethod(
	    activity, env->GetMethodID(activityClass, "getClassLoader", "()Ljava/lang/ClassLoader;"));
	if (!classLoader) {
		return nullptr;
	}
	Finally releaseClassLoader([&]() { env->DeleteLocalRef(classLoader); });
	const jclass classLoaderClass = env->GetObjectClass(classLoader);
	Finally releaseClassLoaderClass([&]() { env->DeleteLocalRef(classLoaderClass); });
	const jmethodID loadClass =
	    env->GetMethodID(classLoaderClass, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	const jstring nameString = env->NewStringUTF(name);
	Finally releaseNameString([&]() { env->DeleteLocalRef(nameString); });
	return static_cast<jclass>(env->CallObjectMethod(classLoader, loadClass, nameString));
}

/// Shows \a text in an AlertDialog, does nothing if that isn't possible
///
/// Creating the dialog is left to android/java/com/bixense/jngl/Message.java, as it can only happen
/// on Android's UI thread and getting there needs a Java Runnable. Apps which don't compile that
/// helper into their APK only get the logcat output of errorMessage().
void showDialog(const std::string& text) {
	if (!androidApp || !androidApp->activity || !androidApp->activity->vm) {
		return;
	}
	JNIEnv* env = nullptr;
	// The game loop thread is attached already, in which case this just hands back its JNIEnv.
	if (androidApp->activity->vm->AttachCurrentThread(&env, nullptr) != JNI_OK || !env) {
		return;
	}
	// Never leave a Java exception pending, it would break the next JNI call someone makes.
	Finally clearException([env]() {
		if (env->ExceptionCheck()) {
			env->ExceptionClear();
		}
	});

	jobject activity = androidApp->activity->clazz;
	const jclass messageClass = loadAppClass(env, activity, "com.bixense.jngl.Message");
	if (env->ExceptionCheck() || !messageClass) {
		warn("Not showing a dialog for the message above: com.bixense.jngl.Message is missing. Add "
		     "JNGL's android/java directory to your app's Java sourceSets to get one.");
		return;
	}
	Finally releaseMessageClass([&]() { env->DeleteLocalRef(messageClass); });
	const jmethodID error = env->GetStaticMethodID(messageClass, "error",
	                                               "(Landroid/app/Activity;Ljava/lang/String;)V");
	if (!error) {
		return;
	}
	const jstring textString = env->NewStringUTF(text.c_str());
	Finally releaseTextString([&]() { env->DeleteLocalRef(textString); });
	env->CallStaticVoidMethod(messageClass, error, activity, textString);
}

} // namespace

void errorMessage(const std::string& text) {
	// Print it as well, the dialog may not show up (e.g. when called before the activity exists).
	printMessage(text + "\n");
	showDialog(text);
}

void printMessage(const std::string& text) {
	static std::string cache;
	const char* startLine = text.c_str();
	const char* c;
	for (c = text.c_str(); *c; ++c) {
		if (*c == '\n') {
			for (auto* n = startLine; n < c; ++n) {
				cache.append(1, *n);
			}
			startLine = c + 1;
			__android_log_print(ANDROID_LOG_ERROR, "libjngl", "%s", cache.c_str());
			cache = "";
		}
	}
	if (startLine) {
		cache += startLine;
	}
}
} // namespace jngl
