all: test/bin/JNGLTest-debug.apk

include/boost:
	ln -s /usr/include/boost include/boost

.PHONY: libjngl
libjngl: include/boost test/build.xml ../src/* ../src/android/*
	cd .. && NDK_PROJECT_PATH=android/jngl ndk-build NDK_DEBUG=1 -j4 -f android/jngl/jni/Application.mk

test/build.xml: test/project.properties
	android update project --path test --name JNGLTest

.PHONY: jngl-test
jngl-test: test/build.xml libjngl
	cd .. && NDK_PROJECT_PATH=android/test ndk-build NDK_DEBUG=1 -j4 -f android/test/jni/Application.mk

test/bin/JNGLTest-debug.apk: jngl-test
	cd test && ant debug
