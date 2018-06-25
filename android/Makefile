all: test/app/build/outputs/apk/debug/app-debug.apk test/main.obb

include/boost:
	ln -s /usr/include/boost include/boost

.PHONY: libjngl
libjngl: include/boost ../src/* ../src/android/*
	$$ANDROID_NDK_HOME/ndk-build NDK_DEBUG=1 -j$(nproc) -Cjngl

test/assets:
	mkdir test/assets

test/assets/jngl.webp: test/assets
	cp ../jngl.webp "$@"

test/assets/test.ogg: test/assets
	cp ../test.ogg "$@"

test/obb:
	mkdir test/obb

test/main.obb: test/assets/jngl.webp test/assets/test.ogg test/obb
	$$ANDROID_HOME/tools/bin/jobb -d test/assets/ -o test/main.obb -pn com.bixense.jngl_test -pv 11 -k secret

test/app/build/outputs/apk/debug/app-debug.apk: libjngl test/app/src/main/jniLibs/armeabi-v7a/libopenal.so
	cd test && ./gradlew build

.PHONE: run
run: test/app/build/outputs/apk/debug/app-debug.apk test/main.obb
	$$ANDROID_HOME/platform-tools/adb install -r test/app/build/outputs/apk/debug/app-debug.apk
	$$ANDROID_HOME/platform-tools/adb push test/main.obb /mnt/sdcard/Android/obb/com.bixense.jngl_test/main.1.com.bixense.jngl_test.obb
	$$ANDROID_HOME/platform-tools/adb shell am start -a android.intent.action.MAIN -n com.bixense.jngl_test/android.app.NativeActivity

openal-soft/CMakeLists.txt:
	git clone https://github.com/kcat/openal-soft
	cd openal-soft && git checkout dc3fa3e51f91096a22ba53faec5bd1146936bee3

openal-soft/build/libopenal.so: openal-soft/CMakeLists.txt
	cd openal-soft && sed -i 's/aligned_alloc/not_working_on_android/g' CMakeLists.txt && \
	cd build && cmake -DCMAKE_SYSTEM_NAME=Android -DANDROID_NDK=$$ANDROID_NDK_HOME \
	-DCMAKE_ANDROID_ARCH_ABI=armeabi-v7a -DALSOFT_BACKEND_OSS=OFF -DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_ANDROID_NDK_TOOLCHAIN_VERSION=clang -DALSOFT_BACKEND_WAVE=OFF .. && $(MAKE)

test/app/src/main/jniLibs/armeabi-v7a/libopenal.so: openal-soft/build/libopenal.so
	cp $< $@
