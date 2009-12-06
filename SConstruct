#!/usr/bin/python

import os

version = "0.8.5"

#Replace @VERSION@ in certain files
files = ["jngl.pc.in", "jnal.pc.in", "autopackage/default.apspec.in", "installer/mingw.nsi.in", 'installer/msvc.nsi.in', 'installer/python.nsi.in']
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
tess.cpp
texture.cpp
window.cpp
windowptr.cpp
callbacks.c
ConvertUTF.c
""")

if env['PLATFORM'] == 'win32': # Windows
	jnalLibs = Split("alut vorbisfile OpenAL32")
	jnglLibs = Split("freetype png opengl32 glu32 user32 shell32 gdi32 z jpeg")
	env.Append(CPPPATH="./include")
	lib = env.Library(target="jngl", source=source_files + Glob('win32/*.cpp'), LIBS=jnglLibs)
	env.Library(target="jnal", source = "jnal.cpp", LIBS = jnalLibs)
	linkflags = "-mwindows"
	if int(debug) or int(msvc):
		linkflags = ""
	env.Program("test.cpp",
	            CPPPATH=".",
				LIBPATH=Split(". ./lib"),
				LIBS=Split("jngl jnal") + jnglLibs + jnalLibs,
				LINKFLAGS=linkflags)
	if int(python):
		env = env.Clone()
		env.Append(CPPPATH=Split("C:\Python26\include"),
		           LIBPATH=Split(". ./lib ./python C:\Python26\libs"),
		           LIBS=libs + Split("python26 liblibboost_python-mgw34-1_39"),
		           LINKFLAGS=linkflags)
		env.SharedLibrary(target="python/jngl.dll",
		                  source="python/main.cpp")
		env.SharedLibrary(target="python/jnal.dll",
		                  source="python/jnal.cpp")

if env['PLATFORM'] == 'posix': # Linux
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
		env.Append(LIBS=Split("jpeg wizGLES opengles_lite z png dl") + lib)
		env.Program(source = "wiz/test.cpp", target = "wiz/test.gpe")
	else:
		env.ParseConfig("pkg-config --cflags --libs jngl.pc jnal.pc")
		env.Program("test.cpp")
		env.Library(target="jnal", source = "jnal.cpp")
	if int(python):
		env.SharedLibrary(target="python/jngl.so",
		                  source="python/main.cpp",
		                  LINKFLAGS='`export CFLAGS="-fPIC" && relaytool --relay jpeg -ljpeg`',
						  CPPPATH="/usr/include/python2.6",
						  LIBPATH=Split(". ./lib ./python"),
						  LIBS=Split("python2.6 boost_python-py26 jngl GL GLU png12 glib-2.0 Xxf86vm freetype fontconfig"))

if int(autopackage):
	t = Command('jngl Library ' + version + '.package', [lib, 'autopackage/default.apspec.in'], "makepackage")
	Clean(t, ['jngl Library ' + version + '.package', 'jngl Library ' + version + '.package.meta', 'jngl.xml', 'jngl.xml.old'])

if int(installer):
	nsiFile = 'installer/mingw.nsi'
	name = 'MinGW'
	if int(msvc):
		nsiFile = 'installer/msvc.nsi'
		name = 'MS Visual C++'
	if python:
		nsiFile = 'installer/python.nsi'
		name = 'Python 2.6'
	import os
	t = Command('jngl Library ' + version + '.exe', lib, '"' + os.path.expandvars("%programfiles%") + '\NSIS\makensis.exe " ' + nsiFile)
	if python:
		Depends(t, ['python/jngl.dll', 'python/jnal.dll'])
	Clean(t, 'installer/JNGL ' + version + ' (' + name + ').exe')
