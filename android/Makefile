UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	ANDROID_PLATFORM := linux
	BOOST_PATH := /usr/include/boost
endif
ifeq ($(UNAME_S),Darwin)
	ANDROID_PLATFORM := darwin
	BOOST_PATH := /usr/local/include/boost
endif

all: gradle

${ANDROID_HOME}/ndk/21.1.6352462/package.xml:
	yes | ${ANDROID_HOME}/tools/bin/sdkmanager "ndk;21.1.6352462" >/dev/null

${ANDROID_HOME}/ndk/21.1.6352462/toolchains/llvm/prebuilt/$(ANDROID_PLATFORM)-x86_64/sysroot/usr/include/boost/version.hpp: | ${ANDROID_HOME}/ndk/21.1.6352462/package.xml
	ln -s $(BOOST_PATH) `dirname "$@"`

test/app/src/main/assets:
	mkdir "$@"

test/app/src/main/assets/jngl.webp: test/app/src/main/assets ../jngl.webp
	cp ../jngl.webp "$@"

test/app/src/main/assets/test.ogg: test/app/src/main/assets ../test.ogg
	cp ../test.ogg "$@"

test/app/src/main/assets/Arial.ttf: test/app/src/main/assets ../Arial.ttf
	cp ../Arial.ttf "$@"

test/app/src/main/assets/verysmall.ogv: test/app/src/main/assets ../data/verysmall.ogv
	cp ../data/verysmall.ogv "$@"

.PHONY: gradle
gradle: ${ANDROID_HOME}/ndk/21.1.6352462/toolchains/llvm/prebuilt/$(ANDROID_PLATFORM)-x86_64/sysroot/usr/include/boost/version.hpp \
        test/app/src/main/assets/jngl.webp \
        test/app/src/main/assets/test.ogg \
        test/app/src/main/assets/Arial.ttf \
        test/app/src/main/assets/verysmall.ogv
	cd test && ./gradlew build

.PHONY: run
run: gradle
	${ANDROID_HOME}/platform-tools/adb install -r test/app/build/outputs/apk/debug/app-debug.apk
	${ANDROID_HOME}/platform-tools/adb shell am start -a android.intent.action.MAIN -n com.bixense.jngl_test/android.app.NativeActivity
