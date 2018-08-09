all: gradle

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

.PHONY: gradle
gradle: include/boost \
        test/app/src/main/assets/jngl.webp \
        test/app/src/main/assets/jngl.png \
        test/app/src/main/assets/test.ogg \
        test/app/src/main/assets/Arial.ttf
	cd test && ./gradlew build

.PHONY: run
run: gradle
	$$ANDROID_HOME/platform-tools/adb install -r test/app/build/outputs/apk/debug/app-debug.apk
	$$ANDROID_HOME/platform-tools/adb shell am start -a android.intent.action.MAIN -n com.bixense.jngl_test/android.app.NativeActivity
