# JNGL - Just a Neat Game Library

[🌐 Website](https://bixense.com/jngl/)
[🏃 Quickstart](https://github.com/jhasse/jngl/wiki/JNGL-Quickstart)
[🎓 Tutorial](https://github.com/pinguin999/my-jngl-starter)
[📚 Documentation](https://bixense.com/jngl/annotated.html)

[![Chat](https://img.shields.io/badge/chat-on%20discord-7289da.svg)](https://discord.gg/254P43HvWJ)

An easy to use C++ game library for Linux, Windows, macOS, Android, iOS, Xbox, the Nintendo Switch,
LG webOS and the Web.

[![Portal Dogs running on the Switch](https://user-images.githubusercontent.com/80071/105062511-f651d480-5a7a-11eb-8cd3-260c7929353a.gif)](https://portaldogs.com/)

### Building from Source
```bash
cmake -Bbuild
cmake --build build
./build/jngl-test
```

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
Set up [MSYS2](https://www.msys2.org/) and install the following in a MinGW-w64 Win64 Shell:
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
Use [Homebrew](http://brew.sh/) to install the build dependencies:
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
#include <jngl/init.hpp>

class MyGame : public jngl::Scene {
    void step() override {
        // game logic
    }
    void draw() const override {
        text.draw(jngl::modelview().translate({ -100, 0 }));
    }
    jngl::Font font{ "Arial.ttf", 12 };
    jngl::TextLine text{ font, "Hello World!" };
};

jngl::AppParameters jnglInit() {
    jngl::AppParameters params;
    params.start = []() {
        return std::make_shared<MyGame>();
    };
    return params;
}
```

For a more complete starting point (i.e. project structure, etc.) check out the
[JNGL project template](https://github.com/jhasse/jngl-starter).

