run: (debug "jngl-test")
	build/Debug/jngl-test

debug target:
	cd build && ninja {{target}}

release target:
	cd build && ninja -f build-Release.ninja {{target}}

run-release: (release "jngl-test")
	build/Release/jngl-test

unittest test='': (debug "jngl-unittest")
	cd build && Debug/jngl-unittest {{test}}

[working-directory: 'build']
unittest-headless config='Debug':
	{{config}}/jngl-unittest Color
	{{config}}/jngl-unittest Rgb
	{{config}}/jngl-unittest Drawable
	{{config}}/jngl-unittest ImageData
	{{config}}/jngl-unittest FinallyTest
	{{config}}/jngl-unittest halfLoadTest
	{{config}}/jngl-unittest getBinaryPath
	{{config}}/jngl-unittest readAsset
	{{config}}/jngl-unittest Vec2

unittest-release test='': (release "jngl-unittest")
	cd build && Release/jngl-unittest {{test}}

cmake:
	cmake -Bbuild -G"Ninja Multi-Config"

[unix]
cmake-clang:
	cmake -Bbuild-clang -G"Ninja Multi-Config" -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_CXX_FLAGS="-stdlib=libc++" -DCMAKE_EXPORT_COMPILE_COMMANDS=1

clean:
	rm -rf build

rebuild: && clean cmake (debug "")


[unix]
android:
	cd android/test && ./gradlew build

[windows]
rebuild-msvc: && msvc
	rm -rf build-msvc && cmake -Bbuild-msvc

[windows]
msvc:
	explorer build-msvc\jngl.sln

set windows-shell := ["C:/Program Files/Git/bin/bash", "-c"]

[linux]
ubuntu:
	sudo apt install libgl1-mesa-dev libfreetype6-dev libfontconfig1-dev libpng-dev libxxf86vm-dev \
	                 libvorbis-dev cmake g++ libwebp-dev git libsdl2-dev
