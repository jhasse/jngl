# JNGL - Just a Neat Game Library [![Build Status](https://travis-ci.org/jhasse/jngl.svg?branch=master)](https://travis-ci.org/jhasse/jngl) [![pipeline status](https://gitlab.com/jhasse/jngl/badges/master/pipeline.svg)](https://gitlab.com/jhasse/jngl/commits/master)

An easy to use C++/Python game library for Linux, Windows, macOS, Android, iOS and the Nintendo
Switch.

## Linux

### Ubuntu

```
sudo apt-get install libgl1-mesa-dev libfreetype6-dev libfontconfig1-dev libxxf86vm-dev \
libjpeg-dev libpng-dev libvorbis-dev libopenal-dev meson libepoxy-dev libboost-dev g++ \
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

Set up [MSYS2](https://www.msys2.org/) and install the following in a MinGW-w64 Win64 Shell:

```
pacman -Syu --needed mingw-w64-x86_64-meson mingw-w64-x86_64-gcc mingw-w64-x86_64-boost \
mingw-w64-x86_64-openal mingw-w64-x86_64-freetype mingw-w64-x86_64-libvorbis \
mingw-w64-x86_64-libwebp mingw-w64-x86_64-dlfcn mingw-w64-x86_64-libepoxy mingw-w64-x86_64-python3 \
mingw-w64-x86_64-cmake make mingw-w64-x86_64-gdb
```

### Visual Studio 2017 or newer

```
cmake -Bbuild -H. -DFETCHCONTENT_QUIET=0
```

and then open `build/jngl.sln`.

### Visual Studio Code

Change the terminal to MINGW64 in `settings.json`:

```
"terminal.integrated.shell.windows": "C:\\msys64\\usr\\bin\\bash.exe",
"terminal.integrated.shellArgs.windows": [
    "--login",
],
"terminal.integrated.env.windows": {
    "CHERE_INVOKING": "1",
    "MSYSTEM": "MINGW64",
},
```

In the terminal run:

```
cmake -H. -Bbuild -G"MSYS Makefiles"
make -Cbuild
```

## Mac

Use [Homebrew](http://brew.sh/) to install the build dependencies:

```
brew install sdl2 freetype libvorbis libepoxy jpeg webp meson pkg-config boost theora cmake
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig
```

## Building

```
meson build
ninja -C build
./build/jngl-test
```

## Android

Install the Android SDK and let `ANDROID_HOME` point to it. In the Android SDK Manager install the
NDK build tools. Type the following to run the test app via ADB:

```
make -C android run
```

You can also open the `android/test` folder in Android Studio and build from there.

## iOS

Install Boost using `brew install boost` and run `ln -s /usr/local/include/boost include/ios/boost`.
Open `xcode/JNGL.xcodeproj` and run the "Test iOS" target.
