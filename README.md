# JNGL - Just a Neat Game Library

Easy to use game library for C++ and Python

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

Install JNGL to "/usr":
```
./install.sh /usr
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

## Libraries for Windows

* [zlib for Windows](http://gnuwin32.sourceforge.net/packages/zlib.htm)
* [libpng for Windows](http://gnuwin32.sourceforge.net/packages/libpng.htm)
* [FreeType for Windows](http://gnuwin32.sourceforge.net/packages/freetype.htm)

All three of them were built with:
```
./configure --enable-shared=no --prefix=/mingw --exec_prefix=/mingw && make && make install
```