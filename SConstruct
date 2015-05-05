#!/usr/bin/python

import os

version = "1.2.0"

#Replace @VERSION@ in certain files
files = ["jngl.pc.in", "installer/mingw.nsi.in", 'installer/msvc.nsi.in', 'installer/python.nsi.in']
for filename in files:
	newfilename = filename.replace(".in", "")
	datei = open(filename,"r").readlines()
	open(newfilename,"w").writelines([line.replace("@VERSION@",version) for line in datei])
	Clean('.', newfilename) # Make sure scons -c does clean up tidily

vars = Variables()
vars.Add(BoolVariable('debug', 'Enable debug build', 0))
vars.Add(BoolVariable('profile', 'Enable profile build', 0))
vars.Add(BoolVariable('python', 'Build python bindings', 0))
vars.Add(BoolVariable('installer', 'Create a Windows installer using NSIS', 0))
vars.Add(BoolVariable('verbose', 'Show verbose compiling output', 0))
vars.Add(BoolVariable('msvc', "Build installer using Visual C++'s output", 0))
vars.Add(BoolVariable('clang', "Compile with Clang", 0))

env = Environment(variables = vars)
env['ENV']['TERM'] = os.environ['TERM']
Help(vars.GenerateHelpText(env))
try:
	import multiprocessing
	env.SetOption('num_jobs', multiprocessing.cpu_count())
except NotImplementedError:
	pass

if env['PLATFORM'] == 'win32':
	if env['msvc']:
		env.Append(CCFLAGS = '/EHsc /MD')
	else:
		env = Environment(tools = ['mingw'], variables = vars)

conf = Configure(env)

if env['PLATFORM'] == 'darwin':
	env['CC']  = 'clang'
	env['CXX'] = 'clang++'

if env['debug']:
	buildDir = 'build/debug/'
	env.Append(CCFLAGS = '-g -Wall -Wextra')
else:
	buildDir = 'build/release/'
	env.Append(CCFLAGS = '-O2 -DNDEBUG')

if env['clang']:
    env["CC"] = os.getenv("CC") or "clang"
    env["CXX"] = os.getenv("CXX") or "clang++"

VariantDir(buildDir, 'src', duplicate=0)

if env['profile']:
	env.Append(CCFLAGS = '-pg', _LIBFLAGS = ' -pg')
if not env['verbose']:
	env['CCCOMSTR'] = env['CXXCOMSTR'] = env['SHCXXCOMSTR'] = "compiling: $TARGET"
	env['LINKCOMSTR'] = env['SHLINKCOMSTR'] = "linking: $TARGET"
	env['ARCOMSTR'] = "archiving: $TARGET"

source_files = [buildDir + 'ConvertUTF.c']

if not env['msvc']:
	source_files += env.Object(Glob(buildDir + "*.cpp") + Glob(buildDir + "jngl/*.cpp"), CPPFLAGS="-std=c++0x")

testSrc = buildDir + "test/test.cpp"

if env['PLATFORM'] in ['win32', 'msys'] and not env['msvc']: # Windows
	jnglLibs = Split("glew32 freetype png opengl32 glu32 user32 shell32 gdi32 z jpeg dl webp Xinput vorbisfile")
	if env['python'] or env['msvc']:
		jnglLibs += Split("openal32 ogg")
	else:
		env.Append(CPPDEFINES={'WEAK_LINKING_OPENAL': '1', 'WINVER': '0x0602'})
	env.Append(CPPPATH = ['/mingw64/include/freetype2'])
	lib = env.Library(target="jngl",
	                  source=source_files +
	                         env.Object(Glob(buildDir + 'win32/*.cpp'),
	                                         CPPFLAGS="-std=c++11"),
	                  LIBS=jnglLibs)
	linkflags = "-mwindows"
	if env['debug'] or env['msvc']:
		linkflags = ""
	libs = Split("jngl") + jnglLibs
	if conf.CheckDeclaration("__i386__"):
		env.Append(LIBPATH = 'lib/win/x86')
	else:
		env.Append(LIBPATH = 'lib/win/x86_64')
	env.Program("test", testSrc,
	            LIBPATH = ["."],
	            CPPFLAGS = "-std=c++11",
	            LIBS = libs,
	            LINKFLAGS = linkflags)
	if env['python']:
		env = env.Clone()
		env.Append(CPPPATH=Split("C:\Python33\include ./boost-libs/include"),
		             LIBPATH=Split(". lib ./boost-libs/lib/win ./python C:\Python33\libs"),
		             LIBS=libs + Split("python33 libboost_python-mgw47-mt-1_51.dll"),
		             LINKFLAGS=linkflags,
		             CPPFLAGS="-std=c++11")
		env.SharedLibrary(target="python/jngl.dll",
		                  source="python/main.cpp")

if env['PLATFORM'] == 'posix': # Linux
	if env['python']:
		env.Append(CCFLAGS = '-DNOJPEG')
	source_files += env.Object(Glob(buildDir + 'linux/*.cpp') +
		                       Glob(buildDir + 'sdl/*.cpp'), CPPFLAGS='-std=c++11')
	env.ParseConfig('pkg-config --cflags --libs fontconfig freetype2 sdl2')
	env.Append(CCFLAGS="-fPIC -DNO_WEAK_LINKING_OPENAL")
	lib = env.Library(target="jngl", source=source_files)
	env.Append(LIBPATH='.', CPPPATH='src')
	testEnv = env.Clone()
	testEnv.ParseConfig("pkg-config --cflags --libs jngl.pc")
	testEnv.Program('test', testSrc, CPPFLAGS="-std=c++0x")
	if env['python']:
		env = env.Clone()
		env.ParseConfig("pkg-config --cflags --libs jngl.pc")
		env.Append(CPPPATH="/usr/include/python3.4",
		           LIBPATH=Split("src ./python"),
		           LIBS=Split("python3.4m boost_python-py34"),
		           CPPFLAGS="-std=c++11")
		env.SharedLibrary(target="python/libjngl.so",
		                  source="python/main.cpp")

if env['PLATFORM'] == 'darwin': # Mac
	env.Append(LIBS=Split('GLEW jpeg ogg vorbisfile webp'),
	           LIBPATH=Split('/opt/local/lib .'),
	           CPPPATH=['/opt/local/include/', './boost-libs/include'],
	           LINKFLAGS='-framework OpenAL -framework OpenGL')
	env.ParseConfig('/opt/local/bin/pkg-config --cflags --libs freetype2 libpng')
	env.ParseConfig('/opt/local/bin/sdl-config --cflags --libs')
	lib = env.Library(target="jngl", source=source_files + env.Object(Glob(buildDir + 'sdl/*.cpp'), CPPFLAGS='-std=c++11'))
	testEnv = env.Clone()
	testEnv.Append(CPPPATH='.')
	testEnv.Append(LIBS=lib)
	testEnv.Program("test", testSrc, CPPFLAGS='-std=c++0x')
	if env['python']:
		env = env.Clone()
		env.Append(CPPPATH='/System/Library/Frameworks/Python.framework/Headers/',
		           LINKFLAGS='-framework Python',
		           LIBPATH=Split(". ./lib ./python"),
		           LIBS=Split("boost_python"),
		           CPPFLAGS='-std=c++11')
		env.SharedLibrary(target="python/jngl.so",
		                  source="python/main.cpp")

if env['installer']:
	nsiFile = 'installer/mingw.nsi'
	name = 'MinGW'
	if env['msvc']:
		nsiFile = 'installer/msvc.nsi'
		name = 'MS Visual C++'
	if env['python']:
		nsiFile = 'installer/python.nsi'
		name = 'Python 2.7'
	import os
	if env['msvc']:
		lib = None
	t = Command('JNGL ' + version + '.exe', lib, '/c/Program\ Files\ \(x86\)/NSIS/Bin/makensis ' + nsiFile)
	if env['python']:
		Depends(t, ['python/jngl.dll'])
	Clean(t, 'installer/JNGL ' + version + ' (' + name + ').exe')
