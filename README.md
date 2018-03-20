# JNGL - Just a Neat Game Library [![Build Status](https://travis-ci.org/jhasse/jngl.svg?branch=master)](https://travis-ci.org/jhasse/jngl) [![Build status](https://ci.appveyor.com/api/projects/status/8gbj2tkxie9uyern?svg=true)](https://ci.appveyor.com/project/jhasse/jngl) [![pipeline status](https://gitlab.com/jhasse/jngl/badges/master/pipeline.svg)](https://gitlab.com/jhasse/jngl/commits/master)

An easy to use C++/Python game library for Linux, Windows, macOS, Android, iOS and the Nintendo
Switch.

## Linux

### Ubuntu

```
sudo apt-get install libgl1-mesa-dev libfreetype6-dev libfontconfig1-dev libxxf86vm-dev \
libjpeg-dev libpng12-dev libvorbis-dev libopenal-dev meson libepoxy-dev libboost-dev g++ \
libwebp-dev git libsdl2-dev
```

### Fedora

```
sudo dnf install fontconfig-devel freetype-devel libvorbis-devel libepoxy-devel libwebp-devel \
libjpeg-turbo-devel boost-python3-devel python3-devel meson SDL2-devel openal-soft-devel gcc-c++
```

### Arch Linux

```
pacman -Syu --needed meson gcc sdl2 pkg-config fontconfig libepoxy libwebp openal libvorbis boost
```

## Windows

### MSYS2 / MinGW-w64

Set up [MSYS2](http://sourceforge.net/p/msys2/wiki/MSYS2%20installation/) and install the following
in a mingw64 shell:

```
pacman -Syu --needed mingw-w64-x86_64-meson mingw-w64-x86_64-gcc mingw-w64-x86_64-boost \
mingw-w64-x86_64-openal mingw-w64-x86_64-freetype mingw-w64-x86_64-libvorbis \
mingw-w64-x86_64-libwebp mingw-w64-x86_64-dlfcn mingw-w64-x86_64-libepoxy mingw-w64-x86_64-python3
```

### Visual C++ 2015 or newer

Open `visualcpp/JNGL.sln` and press <kbd>Ctrl</kbd>+<kbd>F5</kbd> to build JNGL and run the example
application.

## Mac

Use [Homebrew](http://brew.sh/) to install the build dependencies:

```
brew install sdl2 freetype libvorbis glew jpeg webp meson pkg-config boost
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig
```

## Building

```
meson build
ninja -C build
./build/jngl-test
```

## Android

Install the [CrystaX NDK 10.3](https://www.crystax.net), add the folder to your `PATH` and run
```
ndk-build
```
inside the `android/jngl` folder.

## iOS

Open `xcode/JNGL.xcodeproj` and run the "Test iOS" target.
