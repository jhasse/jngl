#!/usr/bin/python

import os

version = "0.8.1"

#Replace @VERSION@ in certain files
files = ["jngl.pc.in", "autopackage/default.apspec.in", "jngl.nsi.in"]
for filename in files:
	newfilename = filename.replace(".in", "")
	datei = open(filename,"r").readlines()
	open(newfilename,"w").writelines([line.replace("@VERSION@",version) for line in datei])
	Clean('.', newfilename) # Make sure scons -c does clean up tidily

env = Environment(tools=['mingw'])
debug = ARGUMENTS.get('debug', 0)
profile = ARGUMENTS.get('profile', 0)
autopackage = ARGUMENTS.get('autopackage', 0)
installer = ARGUMENTS.get('installer', 0)
opengles = ARGUMENTS.get('opengles', 0)
m32 = ARGUMENTS.get('m32', 0)
if int(debug):
	env.Append(CCFLAGS = '-g -Wall')
else:
	env.Append(CCFLAGS = '-O2 -DNDEBUG')
if int(profile):
	env.Append(CCFLAGS = '-pg', _LIBFLAGS = ' -pg')
if int(opengles):
	env.Append(CCFLAGS = '-DOPENGLES')
if int(m32):
	env.Append(CCFLAGS = '-m32', LINKFLAGS = ' -m32')

source_files = Split("""
finally.cpp
freetype.cpp
main.cpp
opengl.cpp
tess.cpp
texture.cpp
window.cpp
windowptr.cpp
""") + Glob('*.c')

if env['PLATFORM'] == 'win32': # Windows
	env.Append(CPPPATH="C:/MinGW/include/freetype2")
	lib = env.Library(target="jngl", source=source_files + Glob('win32/*.cpp'))
	linkflags = "-mwindows"
	if int(debug):
		linkflags = ""
	env.Program("test.cpp", CPPPATH=".", LIBPATH=".", LIBS=Split("jngl opengl32 glu32 gdi32 png freetype"), LINKFLAGS=linkflags)

if env['PLATFORM'] == 'posix': # Linux
	env.ParseConfig('pkg-config --cflags --libs freetype2 fontconfig glib-2.0')
	lib = env.Library(target="jngl", source=source_files + Glob('linux/*.cpp'))
	env.Append(LIBPATH=".", CPPPATH='.')
	env.ParseConfig("pkg-config --cflags --libs jngl.pc")
	env.Program("test.cpp")

if int(autopackage):
	t = Command('jngl Library ' + version + '.package', [lib, 'autopackage/default.apspec.in'], "makepackage")
	Clean(t, ['jngl Library ' + version + '.package', 'jngl Library ' + version + '.package.meta', 'jngl.xml', 'jngl.xml.old'])

if int(installer):
	t = Command('jngl Library ' + version + '.exe', lib, "C:/Programme/NSIS/makensis jngl.nsi")
	Clean(t, 'jngl Library ' + version + '.exe')
