all: gradle

${ANDROID_SDK_ROOT}/ndk/25.2.9519653/package.xml:
	yes | ${ANDROID_SDK_ROOT}/cmdline-tools/latest/bin/sdkmanager "ndk;25.2.9519653" >/dev/null

.PHONY: gradle
gradle:
	cd test && ./gradlew build

.PHONY: run
run: gradle
	${ANDROID_SDK_ROOT}/platform-tools/adb install -r test/app/build/outputs/apk/debug/app-debug.apk
	${ANDROID_SDK_ROOT}/platform-tools/adb shell am start -a android.intent.action.MAIN -n com.bixense.jngl_test/android.app.NativeActivity
