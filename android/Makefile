all: test/bin/JNGLTest-debug.apk

jngl/obj/local/armeabi/libjngl.a:
	cd jngl && ndk-build -j4

test/build.xml:
	android update project --path test --name JNGLTest

test/bin/JNGLTest-debug.apk: test/build.xml jngl/obj/local/armeabi/libjngl.a
	cd test && ndk-build && ant debug
