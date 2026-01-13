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

unittest-headless config='Debug':
	cd build && {{config}}/jngl-unittest Color
	cd build && {{config}}/jngl-unittest Rgb
	cd build && {{config}}/jngl-unittest Drawable
	cd build && {{config}}/jngl-unittest ImageData
	cd build && {{config}}/jngl-unittest FinallyTest
	cd build && {{config}}/jngl-unittest halfLoadTest
	cd build && {{config}}/jngl-unittest getBinaryPath
	cd build && {{config}}/jngl-unittest getConfigPath
	cd build && {{config}}/jngl-unittest readAsset
	cd build && {{config}}/jngl-unittest Vec2

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

webos:
	cp build-web/jngl-test.data webOS/
	cp build-web/jngl-test.js webOS/
	cp build-web/jngl-test.wasm webOS/
	ares-package --no-minify webOS
	ares-install com.bixense.jngl-test_1.0.0_all.ipk
	ares-launch com.bixense.jngl-test

[windows]
build-web version='3.1.61':
	powershell -Command "podman run -v$(cygpath -w $PWD):/w emscripten/emsdk:{{version}} /bin/bash -c \"cd /w && rm -rf build-web && emcmake cmake -Bbuild-web -DCMAKE_BUILD_TYPE=Release && cp -r data build-web/ && make -j8 -Cbuild-web\""
