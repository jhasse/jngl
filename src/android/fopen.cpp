// See https://github.com/googlesamples/android-ndk/blob/master/native-media/app/src/main/cpp/android_fopen.c

#include "fopen.hpp"

#include "../jngl/main.hpp"

#include <errno.h>
#include <android/asset_manager.h>
#include <boost/algorithm/string/replace.hpp>

static int android_read(void* cookie, char* buf, int size) {
	return AAsset_read((AAsset*)cookie, buf, size);
}

static int android_write(void* cookie, const char* buf, int size) {
	return EACCES; // can't provide write access to the apk
}

static fpos_t android_seek(void* cookie, fpos_t offset, int whence) {
	return AAsset_seek((AAsset*)cookie, offset, whence);
}

static int android_close(void* cookie) {
	AAsset_close((AAsset*)cookie);
	return 0;
}

FILE* android_fopen(const char* fname, const char* mode) {
	if (mode[0] == 'w') return NULL;

	if (fname[0] != '\0' && fname[0] == '.' && fname[1] == '/') {
		fname += 2;
	}

	std::string tmp(fname);
	boost::replace_all(tmp, "/./", "/");

	AAsset* asset = AAssetManager_open(jngl::androidApp->activity->assetManager, tmp.c_str(), 0);
	if (!asset) return NULL;

	return funopen(asset, android_read, android_write, android_seek, android_close);
}
