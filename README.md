# JNGL - Just a Neat Game Library

Easy to use game library for C++ and Python

## Linux

On a Debian-based distribution:

```
sudo apt-get install libgl1-mesa-dev libfreetype6-dev libfontconfig1-dev libxxf86vm-dev \
libjpeg62-dev libpng12-dev libvorbis-dev libopenal-dev scons libglew1.5-dev libboost-dev g++ \
libwebp-dev git
```

## Windows

### Using MSYS2 / MinGW-w64

Set up [MSYS2](http://sourceforge.net/p/msys2/wiki/MSYS2%20installation/) and install the following
in a mingw64 shell:

```
pacman -S scons mingw-w64-x86_64-gcc mingw-w64-x86_64-boost mingw-w64-x86_64-openal \
mingw-w64-x86_64-freetype mingw-w64-x86_64-libvorbis mingw-w64-x86_64-libwebp \
mingw-w64-x86_64-dlfcn mingw-w64-x86_64-glew
```

### Using Visual C++ 2013

Open visualcpp/JNGL.sln and choose 'TOOLS' -> Library Package Manager -> Package Manager Console.
Type:

```
Install-Package boost -Project Library
Install-Package boost -Project Python
Install-Package libpng -Project Library
```

## Build using scons

Build JNGL in debug mode:
```
scons debug=1
```

Build python bindings:
```
scons python=1
```

Create a Windows installer:
```
scons installer=1
scons installer=1 python=1
```

Install JNGL to "/usr/local":
```
./install.sh /usr/local
```

Clean up:
```
scons installer=1 python=1 -c
```

A list of all variables:
```
scons -h
```

## Android

Install the Android SDK and NDK. Add the NDK's folder to your PATH and run
```
ndk-build
```
inside the android/jngl folder.
