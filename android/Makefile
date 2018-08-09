all: test/app/build/outputs/apk/debug/app-debug.apk

include/boost:
	ln -s /usr/include/boost "$@"

test/app/src/main/assets:
	mkdir "$@"

test/app/src/main/assets/jngl.webp: test/app/src/main/assets ../jngl.webp
	cp ../jngl.webp "$@"

test/app/src/main/assets/jngl.png: test/app/src/main/assets ../jngl.png
	cp ../jngl.png "$@"

test/app/src/main/assets/test.ogg: test/app/src/main/assets ../test.ogg
	cp ../test.ogg "$@"

test/app/src/main/assets/Arial.ttf: test/app/src/main/assets ../Arial.ttf
	cp ../Arial.ttf "$@"

test/app/build/outputs/apk/debug/app-debug.apk: include/boost \
                                                test/app/src/main/jniLibs/armeabi-v7a/libopenal.so \
                                                test/app/src/main/assets/jngl.webp \
                                                test/app/src/main/assets/jngl.png \
                                                test/app/src/main/assets/test.ogg \
                                                test/app/src/main/assets/Arial.ttf
	cd test && ./gradlew build

.PHONE: run
run: test/app/build/outputs/apk/debug/app-debug.apk
	$$ANDROID_HOME/platform-tools/adb install -r test/app/build/outputs/apk/debug/app-debug.apk
	$$ANDROID_HOME/platform-tools/adb shell am start -a android.intent.action.MAIN -n com.bixense.jngl_test/android.app.NativeActivity

openal-soft/CMakeLists.txt:
	git clone https://github.com/kcat/openal-soft
	cd openal-soft && git checkout dc3fa3e51f91096a22ba53faec5bd1146936bee3

openal-soft/build/libopenal.so: openal-soft/CMakeLists.txt
	cd openal-soft && cd build && cmake -DCMAKE_SYSTEM_NAME=Android \
	-DANDROID_NDK=$$ANDROID_NDK_HOME -DCMAKE_ANDROID_API=23 -DCMAKE_ANDROID_ARCH_ABI=armeabi-v7a \
	-DALSOFT_BACKEND_OSS=OFF -DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_ANDROID_NDK_TOOLCHAIN_VERSION=clang -DALSOFT_BACKEND_WAVE=OFF .. && $(MAKE)

test/app/src/main/jniLibs/armeabi-v7a/libopenal.so: openal-soft/build/libopenal.so
	cp $< $@
