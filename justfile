debug:
	cd build && ninja

release:
	cd build && ninja -f build-Release.ninja

rebuild: && debug
	rm -rf build && cmake -Bbuild -G"Ninja Multi-Config"

android:
	cd android/test && ./gradlew build
