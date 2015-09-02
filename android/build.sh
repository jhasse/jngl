#!/usr/bin/env bash

colormake && gnome-terminal -e "bash -c '(adb install -r test/bin/JNGLTest-debug.apk && adb shell am start -a android.intent.action.MAIN -n com.bixense.jngl_test/android.app.NativeActivity && adb logcat | egrep libjngl\\|DEBUG); read'"
