UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	ANDROID_PLATFORM := linux
	BOOST_PATH := /usr/include/boost
endif
ifeq ($(UNAME_S),Darwin)
	ANDROID_PLATFORM := darwin
	BOOST_PATH := /opt/homebrew/include/boost
endif

all: gradle

${ANDROID_SDK_ROOT}/ndk/25.0.8775105/package.xml:
	yes | ${ANDROID_SDK_ROOT}/cmdline-tools/latest/bin/sdkmanager "ndk;25.0.8775105" >/dev/null

${ANDROID_SDK_ROOT}/ndk/25.0.8775105/toolchains/llvm/prebuilt/$(ANDROID_PLATFORM)-x86_64/sysroot/usr/include/boost/version.hpp: | ${ANDROID_SDK_ROOT}/ndk/25.0.8775105/package.xml
	ln -s $(BOOST_PATH) `dirname "$@"`

.PHONY: gradle
gradle: ${ANDROID_SDK_ROOT}/ndk/25.0.8775105/toolchains/llvm/prebuilt/$(ANDROID_PLATFORM)-x86_64/sysroot/usr/include/boost/version.hpp
	cd test && ./gradlew build

.PHONY: run
run: gradle
	${ANDROID_SDK_ROOT}/platform-tools/adb install -r test/app/build/outputs/apk/debug/app-debug.apk
	${ANDROID_SDK_ROOT}/platform-tools/adb shell am start -a android.intent.action.MAIN -n com.bixense.jngl_test/android.app.NativeActivity
