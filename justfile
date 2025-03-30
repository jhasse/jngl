default: run

debug target: env
	cd build && ninja {{target}}

release target: env
	cd build && ninja -f build-Release.ninja {{target}}

run: (debug "jngl-test")
	{{absolute_path("build/Debug/jngl-test")}}

run-release: (release "jngl-test")
	{{absolute_path("build/Release/jngl-test")}}

unittest test='': (debug "jngl-unittest")
	cd build && {{absolute_path("build/Debug/jngl-unittest")}} {{test}}

unittest-release test='': (release "jngl-unittest")
	cd build && {{absolute_path("build/Release/jngl-unittest")}} {{test}}

[unix] # https://github.com/casey/just/issues/1639
cmake:
	cmake -Bbuild -G"Ninja Multi-Config"

[unix]
cmake-clang:
	cmake -Bbuild-clang -G"Ninja Multi-Config" -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_CXX_FLAGS="-stdlib=libc++" -DCMAKE_EXPORT_COMPILE_COMMANDS=1

[unix]
clean:
	rm -rf build

[unix]
rebuild: && clean cmake (debug "")

[windows]
clean:
	if exist build rmdir /q /s build

[windows]
rebuild: clean
	.vscode\build.bat

[unix]
android:
	cd android/test && ./gradlew build

[windows]
rebuild-msvc: && msvc
	rm -rf build-msvc && cmake -Bbuild-msvc

[windows]
msvc:
	explorer build-msvc\jngl.sln

set windows-shell := ["cmd.exe", "/c"]

[windows]
env:
	call .vscode\env.bat

[unix]
env:

[linux]
ubuntu:
	sudo apt install libgl1-mesa-dev libfreetype6-dev libfontconfig1-dev libpng-dev libxxf86vm-dev \
	                 libvorbis-dev cmake g++ libwebp-dev git libsdl2-dev
