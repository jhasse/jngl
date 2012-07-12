# JNGL - Just a Neat Game Library

## Building using scons

Build jngl in debug mode:
```
scons debug=1
```

Build jngl in profile mode:
```
scons profile=1
```

Build python bindings:
```
scons python=1
```

Create a windows installer:
```
scons installer=1
scons installer=1 python=1
```

Install jngl to "/usr":
```
./install.sh /usr
```

Clean up:
```
scons installer=1 python=1 -c
```

## Libraries for Windows

* zlib for Windows: http://gnuwin32.sourceforge.net/packages/zlib.htm
* libpng for Windows: http://gnuwin32.sourceforge.net/packages/libpng.htm
* FreeType for Windows: http://gnuwin32.sourceforge.net/packages/freetype.htm

All three of them were built with:
```
./configure --enable-shared=no --prefix=/mingw --exec_prefix=/mingw && make && make install
```