# JNGL - Just a Neat Game Library

*A lightweight, cross-platform 2D game library for modern C++ development.*

[![Portal Dogs running on the Switch](https://user-images.githubusercontent.com/80071/105062511-f651d480-5a7a-11eb-8cd3-260c7929353a.gif)](https://portaldogs.com/)

## Overview
JNGL (Just a Neat Game Library) is an easy-to-use C++ game development library that runs on multiple platforms, including:

- **Linux**
- **Windows**
- **macOS**
- **Android**
- **iOS**
- **Xbox**
- **Nintendo Switch**
- **LG webOS**
- **Web (via Emscripten)**

### Features
- Simple API for fast game development  
- Cross-platform support with minimal configuration  
- Hardware-accelerated rendering  
- Built-in support for audio, fonts, and textures  
- Open-source and lightweight  

## Getting Started
To get started, follow these build instructions for your platform.

### Building from Source
```bash
cmake -Bbuild
cmake --build build
./build/jngl-test
```

## Installation by Platform
## Linux
### Ubuntu
```bash
sudo apt-get install libgl1-mesa-dev libfreetype6-dev libfontconfig1-dev libpng-dev \
libxxf86vm-dev libvorbis-dev cmake g++ libwebp-dev git libsdl2-dev
```
### Fedora
```bash
sudo dnf install fontconfig-devel freetype-devel libvorbis-devel libwebp-devel \
cmake SDL2-devel gcc-c++ libatomic
```
### Arch Linux
```bash
pacman -Syu --needed cmake gcc sdl2 pkg-config fontconfig libwebp libvorbis
```

## Windows
### MSYS2 / MinGW-w64
Set up MSYS2 and install the required dependencies:
```bash
pacman -Syu --needed mingw-w64-x86_64-gcc \
mingw-w64-x86_64-freetype mingw-w64-x86_64-libvorbis mingw-w64-x86_64-libwebp \
mingw-w64-x86_64-dlfcn mingw-w64-x86_64-cmake make mingw-w64-x86_64-gdb \
mingw-w64-x86_64-libtheora mingw-w64-x86_64-SDL2
```
### Visual Studio 2017 or newer
```bash
cmake -Bbuild -DFETCHCONTENT_QUIET=0
```
Then open `build/jngl.sln` in Visual Studio.

## macOS
Install dependencies using Homebrew:
```bash
brew install sdl2 freetype libvorbis webp pkg-config cmake
```

## Android
1. Install the Android SDK and set `ANDROID_HOME` to point to it.
2. In the Android SDK Manager, install the NDK build tools.
3. Run the following command to test via ADB:
```bash
make -C android run
```
Alternatively, open `android/test` in Android Studio and build from there.

## iOS
Generate an Xcode project using CMake:
```bash
cmake -Bbuild-ios -GXcode -DCMAKE_TOOLCHAIN_FILE=cmake/iOS.toolchain.cmake -DIOS_PLATFORM=SIMULATOR
```
Then open and build `build-ios/jngl.xcodeproj` in Xcode.

## Xbox
```bash
cmake -Bbuild-uwp -DCMAKE_SYSTEM_NAME=WindowsStore "-DCMAKE_SYSTEM_VERSION=10.0"
```
Then open `build-uwp/jngl.sln` in Visual Studio.

## Example Usage
Here's a simple "Hello, World!" example in JNGL:
```cpp
#include <jngl.hpp>

void step() {
    jngl::setColor(255, 255, 255);
    jngl::drawText("Hello, JNGL!", jngl::Vec2(100, 100));
}

int main() {
    jngl::setStepCallback(step);
    jngl::run();
    return 0;
}
```

## Contributing
If you'd like to contribute:
- Fork the repository and create a new branch.
- Follow the coding guidelines.
- Submit a pull request with a clear description of your changes.

## License
JNGL is licensed under the **zlib license**. See [LICENSE](https://en.wikipedia.org/wiki/Zlib_License) for details.

---
For more details, visit the [official documentation](https://bixense.com/jngl/annotated.html).

