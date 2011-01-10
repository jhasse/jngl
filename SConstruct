#!/usr/bin/python

import os

version = "0.9.2"

#Replace @VERSION@ in certain files
files = ["jngl.pc.in", "autopackage/default.apspec.in", "installer/mingw.nsi.in", 'installer/msvc.nsi.in', 'installer/python.nsi.in']
for filename in files:
	newfilename = filename.replace(".in", "")
	datei = open(filename,"r").readlines()
	open(newfilename,"w").writelines([line.replace("@VERSION@",version) for line in datei])
	Clean('.', newfilename) # Make sure scons -c does clean up tidily

env = Environment()

if env['PLATFORM'] == 'win32':
	msvc = ARGUMENTS.get('msvc', 0)
	if int(msvc):
		env.Append(CCFLAGS = '/EHsc /MD')
	else:
		env = Environment(tools=['mingw'])

debug = ARGUMENTS.get('debug', 0)
profile = ARGUMENTS.get('profile', 0)
autopackage = ARGUMENTS.get('autopackage', 0)
installer = ARGUMENTS.get('installer', 0)
python = int(ARGUMENTS.get('python', 0))
m32 = ARGUMENTS.get('m32', 0)
wiz = ARGUMENTS.get('wiz', 0)
if int(debug):
	env.Append(CCFLAGS = '-g -Wall')
else:
	env.Append(CCFLAGS = '-O2 -DNDEBUG')
if int(profile):
	env.Append(CCFLAGS = '-pg', _LIBFLAGS = ' -pg')
if int(m32):
	env.Append(CCFLAGS = '-m32', LINKFLAGS = ' -m32')
if int(wiz):
	env['CXX'] = "/toolchain/bin/arm-openwiz-linux-gnu-g++"
	env['CC'] = "/toolchain/bin/arm-openwiz-linux-gnu-gcc"
	env.Append(CPPPATH=["/toolchain/include", "wiz"], LIBPATH=["/toolchain/lib", "wiz"])

source_files = Split("""
finally.cpp
freetype.cpp
main.cpp
opengl.cpp
sprite.cpp
tess.cpp
texture.cpp
window.cpp
windowptr.cpp
callbacks.c
ConvertUTF.c
""")

if env['PLATFORM'] == 'win32': # Windows
	jnglLibs = Split("freetype png opengl32 glu32 user32 shell32 gdi32 z jpeg dl")
	env.Append(CPPPATH="./include")
	audioFile = env.Object("audio.cpp", CPPFLAGS="-std=c++0x")
	source_files += audioFile
	lib = env.Library(target="jngl", source=source_files + Glob('win32/*.cpp'), LIBS=jnglLibs)
	linkflags = "-mwindows"
	if int(debug) or int(msvc):
		linkflags = ""
	libs = Split("jngl") + jnglLibs
	env.Program("test.cpp",
	            CPPPATH=".",
				LIBPATH=Split(". ./lib"),
				LIBS=libs,
				LINKFLAGS=linkflags)
	if int(python):
		env = env.Clone()
		env.Append(CPPPATH=Split("C:\Python27\include"),
		           LIBPATH=Split(". ./lib ./python C:\Python27\libs"),
		           LIBS=libs + Split("python27 libboost_python-mgw44-1_41"),
		           LINKFLAGS=linkflags)
		env.SharedLibrary(target="python/jngl.dll",
		                  source="python/main.cpp")

if env['PLATFORM'] == 'posix': # Linux
	source_files += ["audio.cpp"]
	if int(python):
		env.Append(CCFLAGS = '-DNOJPEG')
	if int(wiz):
		env.Append(CCFLAGS = '-DWIZ -DOPENGLES')
		source_files += Glob('wiz/*.cpp') + Glob('wiz/*.c')
	else:
		source_files += Glob('linux/*.cpp')
		env.ParseConfig('pkg-config --cflags --libs fontconfig glib-2.0')
	env.ParseConfig('pkg-config --cflags --libs freetype2')
	env.Append(CCFLAGS="-fPIC")
	lib = env.Library(target="jngl", source=source_files)
	env.Append(LIBPATH=".", CPPPATH='.')
	if wiz:
		env.Append(LIBS=Split("jpeg glport opengles_lite z png dl openal vorbisfile vorbis ogg") + lib)
		env.Program(source = "wiz/test.cpp", target = "wiz/test.gpe")
	else:
		testEnv = env.Clone()
		testEnv.ParseConfig("pkg-config --cflags --libs jngl.pc")
		testEnv.Program("test.cpp")
	if int(python):
		env = env.Clone()
		env.ParseConfig("pkg-config --cflags --libs jngl.pc")
		env.Append(CPPPATH="/usr/include/python2.6",
		           LIBPATH=Split(". ./lib ./python"),
		           LIBS=Split("python2.6 boost_python-py26"))
		env.SharedLibrary(target="python/jngl.so",
		                  source="python/main.cpp")

if int(autopackage):
	t = Command('JNGL ' + version + '.package', [lib, 'autopackage/default.apspec.in'], "makepackage")
	Clean(t, ['JNGL ' + version + '.package', 'JNGL Library ' + version + '.package.meta', 'jngl.xml', 'jngl.xml.old'])

if int(installer):
	nsiFile = 'installer/mingw.nsi'
	name = 'MinGW'
	if int(msvc):
		nsiFile = 'installer/msvc.nsi'
		name = 'MS Visual C++'
	if python:
		nsiFile = 'installer/python.nsi'
		name = 'Python 2.7'
	import os
	t = Command('JNGL ' + version + '.exe', lib, '"' + os.path.expandvars("%programfiles%") + '\NSIS\makensis.exe " ' + nsiFile)
	if python:
		Depends(t, ['python/jngl.dll'])
	Clean(t, 'installer/JNGL ' + version + ' (' + name + ').exe')
