#include <android/native_activity.h>
#include <android/log.h>
#include <dlfcn.h>
#include <errno.h>
#include <stdexcept>

void* load_lib(const std::string& l) {
	auto handle = dlopen(std::string("/data/data/com.bixense.jngl_test/lib/" + l).c_str(),
	                     RTLD_NOW | RTLD_GLOBAL);
	if (!handle) {
		throw std::runtime_error(std::string("dlopen(") + l + "): " + dlerror());
	}
	__android_log_print(ANDROID_LOG_INFO, "bootstrap", "Loaded %s", l.c_str());
	return handle;
}

void ANativeActivity_onCreate(ANativeActivity* app, void* ud, size_t udsize) {
	try {
		load_lib("libogg.so");
		load_lib("libvorbis.so");
		auto main = reinterpret_cast<void (*)(ANativeActivity*, void*, size_t)>(
			dlsym(load_lib("libjngl-test.so"), "ANativeActivity_onCreate")
		);
		if (!main) {
			throw std::runtime_error("undefined symbol ANativeActivity_onCreate");
		}
		main(app, ud, udsize);
	} catch(std::exception& e) {
		__android_log_print(ANDROID_LOG_ERROR, "bootstrap", "%s", e.what());
		ANativeActivity_finish(app);
	}
}
