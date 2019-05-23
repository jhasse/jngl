// Copyright 2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include <android/native_activity.h>
#include <android/log.h>
#include <dlfcn.h>
#include <stdexcept>
#include <string>

void* load_lib(const std::string& l) {
	auto handle = dlopen(l.c_str(), RTLD_NOW | RTLD_GLOBAL);
	if (!handle) {
		throw std::runtime_error(std::string("dlopen(") + l + "): " + dlerror());
	}
	__android_log_print(ANDROID_LOG_INFO, "bootstrap", "Loaded %s", l.c_str());
	return handle;
}

void ANativeActivity_onCreate(ANativeActivity* app, void* ud, size_t udsize) {
	const jclass contextClass = app->env->GetObjectClass(app->clazz);
	const jmethodID getApplicationContextMethod =
	    app->env->GetMethodID(contextClass, "getApplicationContext", "()Landroid/content/Context;");
	const jobject contextObject =
	    app->env->CallObjectMethod(app->clazz, getApplicationContextMethod);
	const jmethodID getApplicationInfoMethod = app->env->GetMethodID(
	    contextClass, "getApplicationInfo", "()Landroid/content/pm/ApplicationInfo;");
	const jobject applicationInfoObject =
	    app->env->CallObjectMethod(contextObject, getApplicationInfoMethod);
	const jfieldID nativeLibraryDirField = app->env->GetFieldID(
	    app->env->GetObjectClass(applicationInfoObject), "nativeLibraryDir", "Ljava/lang/String;");
	const jobject nativeLibraryDirObject =
	    app->env->GetObjectField(applicationInfoObject, nativeLibraryDirField);
	const jmethodID getBytesMethod = app->env->GetMethodID(
	    app->env->GetObjectClass(nativeLibraryDirObject), "getBytes", "(Ljava/lang/String;)[B");
	const auto bytesObject = static_cast<jbyteArray>(app->env->CallObjectMethod(
	    nativeLibraryDirObject, getBytesMethod, app->env->NewStringUTF("UTF-8")));
	const size_t length = app->env->GetArrayLength(bytesObject);
	const jbyte* const bytes = app->env->GetByteArrayElements(bytesObject, nullptr);
	const std::string libDir(reinterpret_cast<const char*>(bytes), length);
	try {
		auto main = reinterpret_cast<void (*)(ANativeActivity*, void*, size_t)>(
		    dlsym(load_lib(libDir + "/libjngl-test.so"), "ANativeActivity_onCreate"));
		if (!main) {
			throw std::runtime_error("undefined symbol ANativeActivity_onCreate");
		}
		main(app, ud, udsize);
	} catch (std::exception& e) {
		__android_log_print(ANDROID_LOG_ERROR, "bootstrap", "%s", e.what());
		ANativeActivity_finish(app);
	}
}
