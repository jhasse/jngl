all: gradle

.PHONY: gradle
gradle:
	cd test && ./gradlew build

.PHONY: run
run: gradle
	${ANDROID_SDK_ROOT}/platform-tools/adb install -r test/app/build/outputs/apk/debug/app-debug.apk
	${ANDROID_SDK_ROOT}/platform-tools/adb shell am start -a android.intent.action.MAIN -n com.bixense.jngl_test/android.app.NativeActivity
