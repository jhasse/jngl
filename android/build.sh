#!/usr/bin/env bash

source ~/.bashrc

colormake && gnome-terminal -e "bash -c '(adb install -r test/bin/JNGLTest-debug.apk && adb push test/main.obb /mnt/shell/emulated/obb/com.bixense.jngl_test/main.1.com.bixense.jngl_test.obb && adb shell am start -a android.intent.action.MAIN -n com.bixense.jngl_test/android.app.NativeActivity && adb logcat | egrep libjngl\\|DEBUG\\|bootstrap); read'"
