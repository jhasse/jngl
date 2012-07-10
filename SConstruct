#!/usr/bin/python

import os

version = "1.0.1"

#Replace @VERSION@ in certain files
files = ["jngl.pc.in", "installer/mingw.nsi.in", 'installer/msvc.nsi.in', 'installer/python.nsi.in']
for filename in files:
	newfilename = filename.replace(".in", "")
	datei = open(filename,"r").readlines()
	open(newfilename,"w").writelines([line.replace("@VERSION@",version) for line in datei])
	Clean('.', newfilename) # Make sure scons -c does clean up tidily

env = Environment()
try:
	import multiprocessing
	env.SetOption('num_jobs', multiprocessing.cpu_count())
except NotImplementedError:
	pass

msvc = bool(ARGUMENTS.get('msvc', 0))
if env['PLATFORM'] == 'win32':
	if msvc:
		env.Append(CCFLAGS = '/EHsc /MD')
	else:
		env = Environment(tools=['mingw'])

if env['PLATFORM'] == 'darwin':
	env = Environment(CXX='clang++', CC='clang')

debug = int(ARGUMENTS.get('debug', 0))
profile = ARGUMENTS.get('profile', 0)
installer = ARGUMENTS.get('installer', 0)
python = int(ARGUMENTS.get('python', 0))
m32 = ARGUMENTS.get('m32', 0)
if debug:
	env.Append(CCFLAGS = '-g -Wall')
else:
	env.Append(CCFLAGS = '-O2 -DNDEBUG')
if int(profile):
	env.Append(CCFLAGS = '-pg', _LIBFLAGS = ' -pg')
if int(m32):
	env.Append(CCFLAGS = '-m32', LINKFLAGS = ' -m32')
if ARGUMENTS.get('VERBOSE') != "1":
	env['CCCOMSTR'] = env['CXXCOMSTR'] = "compiling: $TARGET"
	env['LINKCOMSTR'] = "linking: $TARGET"
	env['ARCOMSTR'] = "archiving: $TARGET"

if not msvc:
	source_files = env.Object(Split("""
	src/audio.cpp
	src/finally.cpp
	src/freetype.cpp
	src/main.cpp
	src/opengl.cpp
	src/sprite.cpp
	src/tess.cpp
	src/texture.cpp
	src/window.cpp
	src/windowptr.cpp
	src/work.cpp
	src/framebuffer.cpp
	src/framebufferimpl.cpp
	"""), CPPFLAGS="-std=gnu++0x")
	source_files += Split("""
	src/callbacks.c
	src/ConvertUTF.c
	""")

testSrc = "src/test.cpp"

if env['PLATFORM'] == 'win32' and not msvc: # Windows
	jnglLibs = Split("glew32 freetype png opengl32 glu32 user32 shell32 gdi32 z jpeg dl")
	if int(python) or int(msvc):
		jnglLibs += Split("openal32 ogg vorbisfile")
	else:
		env.Append(CPPDEFINES='WEAK_LINKING_OPENAL')
	env.Append(CPPPATH="./include")
	lib = env.Library(target="jngl", source=source_files + Glob('win32/*.cpp'), LIBS=jnglLibs)
	linkflags = "-mwindows"
	if debug or int(msvc):
		linkflags = ""
	libs = Split("jngl") + jnglLibs
	env.Program(testSrc,
	            CPPFLAGS="-std=gnu++0x",
	            CPPPATH=".",
				LIBPATH=Split("src ./lib"),
				LIBS=libs,
				LINKFLAGS=linkflags)
	if int(python):
		env = env.Clone()
		env.Append(CPPPATH=Split("C:\Python27\include"),
		           LIBPATH=Split(". ./lib ./python C:\Python27\libs"),
		           LIBS=libs + Split("python27 libboost_python-mgw44-1_45"),
		           LINKFLAGS=linkflags)
		env.SharedLibrary(target="python/jngl.dll",
		                  source="python/main.cpp")

if env['PLATFORM'] == 'posix': # Linux
	if int(python):
		env.Append(CCFLAGS = '-DNOJPEG')
	source_files += Glob('src/linux/*.cpp')
	env.ParseConfig('pkg-config --cflags --libs fontconfig glib-2.0')
	env.ParseConfig('pkg-config --cflags --libs freetype2')
	env.Append(CCFLAGS="-fPIC -DNO_WEAK_LINKING_OPENAL")
	lib = env.Library(target="jngl", source=source_files)
	env.Append(LIBPATH='.', CPPPATH='src')
	testEnv = env.Clone()
	testEnv.ParseConfig("pkg-config --cflags --libs jngl.pc")
	testEnv.Program('test', testSrc, CPPFLAGS="-std=c++0x")
	if int(python):
		env = env.Clone()
		env.ParseConfig("pkg-config --cflags --libs jngl.pc")
		env.Append(CPPPATH="/usr/include/python2.7",
		           LIBPATH=Split("src ./python"),
		           LIBS=Split("python2.7 boost_python-py27"))
		env.SharedLibrary(target="python/jngl.so",
		                  source="python/main.cpp")

if env['PLATFORM'] == 'darwin': # Mac
	env.Append(LIBS=Split('jngl GLEW jpeg ogg vorbisfile'),
	           LIBPATH=Split('/opt/local/lib .'),
	           CPPPATH='/opt/local/include/',
	           LINKFLAGS='-framework OpenAL -framework OpenGL')
	env.ParseConfig('/opt/local/bin/pkg-config --cflags --libs freetype2 libpng')
	env.ParseConfig('/opt/local/bin/sdl-config --cflags --libs')
	env.Library(target="jngl", source=source_files + Glob('src/sdl/*.cpp'))
	testEnv = env.Clone()
	testEnv.Append(CPPPATH='.')
	testEnv.Program(testSrc, CPPFLAGS='-std=c++0x')
	if int(python):
		env = env.Clone()
		env.Append(CPPPATH='/System/Library/Frameworks/Python.framework/Headers/',
		           LINKFLAGS='-framework Python',
		           LIBPATH=Split(". ./lib ./python"),
		           LIBS=Split("boost_python"))
		env.SharedLibrary(target="python/jngl.so",
		                  source="python/main.cpp")

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
	if msvc:
		lib = None
	t = Command('JNGL ' + version + '.exe', lib, '"' + os.path.expandvars("%programfiles%") + '\NSIS\makensis.exe " ' + nsiFile)
	if python:
		Depends(t, ['python/jngl.dll'])
	Clean(t, 'installer/JNGL ' + version + ' (' + name + ').exe')
