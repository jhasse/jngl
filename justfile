debug: env
	cd build && ninja

release: env
	cd build && ninja -f build-Release.ninja

[unix]
rebuild: env && debug
	rm -rf build && cmake -Bbuild -G"Ninja Multi-Config"

[windows]
rebuild:
	if exist build rmdir /q /s build
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
