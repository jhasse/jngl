# JNGL - Just a Neat Game Library

[🌐 Website](https://bixense.com/jngl/)
[🏃 Quickstart](https://github.com/jhasse/jngl/wiki/JNGL-Quickstart)
[🎓 Tutorial](https://github.com/pinguin999/my-jngl-starter)
[📚 Documentation](https://bixense.com/jngl/annotated.html)

[![Chat](https://img.shields.io/badge/chat-on%20discord-7289da.svg)](https://discord.gg/254P43HvWJ)

An easy to use C++ game library for Linux, Windows, macOS, Android, iOS, Xbox, the Nintendo Switch,
LG webOS and the Web.

[![Portal Dogs running on the Switch](https://user-images.githubusercontent.com/80071/105062511-f651d480-5a7a-11eb-8cd3-260c7929353a.gif)](https://portaldogs.com/)

## Building

```
cmake -Bbuild
cmake --build build
./build/jngl-test
```

## Linux

### Ubuntu

```
sudo apt-get install libgl1-mesa-dev libfreetype6-dev libfontconfig1-dev libpng-dev \
libxxf86vm-dev libvorbis-dev cmake g++ libwebp-dev git libsdl2-dev
```

### Fedora

```
sudo dnf install fontconfig-devel freetype-devel libvorbis-devel libwebp-devel \
cmake SDL3-devel gcc-c++ libatomic
```

### Arch Linux

```
pacman -Syu --needed cmake gcc sdl2 pkg-config fontconfig libwebp libvorbis
```

## Windows

### MSYS2 / MinGW-w64

Set up [MSYS2](https://www.msys2.org/) and install the following in a MinGW-w64 Win64 Shell:

```
pacman -Syu --needed mingw-w64-x86_64-gcc \
mingw-w64-x86_64-freetype mingw-w64-x86_64-libvorbis mingw-w64-x86_64-libwebp \
mingw-w64-x86_64-dlfcn mingw-w64-x86_64-cmake make mingw-w64-x86_64-gdb \
mingw-w64-x86_64-libtheora mingw-w64-x86_64-SDL3
```

### Visual Studio 2017 or newer

```
cmake -Bbuild -DFETCHCONTENT_QUIET=0
```

and then open `build/jngl.sln`.

## Mac

Use [Homebrew](http://brew.sh/) to install the build dependencies:

```
brew install sdl2 freetype libvorbis webp pkg-config cmake
```

## Android

Install the Android SDK and let `ANDROID_HOME` point to it. In the Android SDK Manager install the
NDK build tools. Type the following to run the test app via ADB:

```
make -C android run
```

You can also open the `android/test` folder in Android Studio and build from there.

## iOS

Generate a Xcode project using CMake:

```
cmake -Bbuild-ios -GXcode -DCMAKE_TOOLCHAIN_FILE=cmake/iOS.toolchain.cmake -DIOS_PLATFORM=SIMULATOR
```

Open and build `build-ios/jngl.xcodeproj` in Xcode.

## Xbox

```
cmake -Bbuild-uwp -DCMAKE_SYSTEM_NAME=WindowsStore "-DCMAKE_SYSTEM_VERSION=10.0"
```

and then open `build-uwp/jngl.sln`.
