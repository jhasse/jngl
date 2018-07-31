// See https://github.com/googlesamples/android-ndk/blob/master/native-media/app/src/main/cpp/android_fopen.c

#include "fopen.hpp"

#include <errno.h>
#include <android/asset_manager.h>

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

AAssetManager* android_asset_manager = NULL;

FILE* android_fopen(const char* fname, const char* mode) {
	if (mode[0] == 'w') return NULL;

	AAsset* asset = AAssetManager_open(android_asset_manager, fname, 0);
	if (!asset) return NULL;

	return funopen(asset, android_read, android_write, android_seek, android_close);
}
