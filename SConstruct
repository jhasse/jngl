#!/usr/bin/python

import os

version = "1.3.0"

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

env = Environment(variables = vars, ENV = os.environ)
env['ENV']['TERM'] = os.getenv('TERM')
env['CC'] = os.getenv('CC', env['CC'])
env['CXX'] = os.getenv('CXX', env['CXX'])
if 'clang' in env['CXX']:
	env.Append(CXXFLAGS = '-stdlib=libc++', LINKFLAGS='-stdlib=libc++')
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

if env['debug']:
	buildDir = 'build/debug/'
	env.Append(CCFLAGS = '-g -Wall -Wextra -pedantic')
else:
	buildDir = 'build/release/'
	env.Append(CCFLAGS = '-O2 -DNDEBUG')

VariantDir(buildDir, 'src', duplicate=0)

if env['profile']:
	env.Append(CCFLAGS = '-pg', _LIBFLAGS = ' -pg')
if not env['verbose']:
	env['CCCOMSTR'] = env['CXXCOMSTR'] = env['SHCXXCOMSTR'] = "compiling: $TARGET"
	env['LINKCOMSTR'] = env['SHLINKCOMSTR'] = "linking: $TARGET"
	env['ARCOMSTR'] = "archiving: $TARGET"

env.Append(CXXFLAGS = '-std=c++1y')

if not env['msvc']:
	source_files = Glob(buildDir + "*.cpp") + Glob(buildDir + "jngl/*.cpp")

testSrc = buildDir + "test/test.cpp"

if env['PLATFORM'] in ['win32', 'msys'] and not env['msvc']: # Windows
	jnglLibs = Split("glew32 freetype png opengl32 glu32 user32 shell32 gdi32 z jpeg dl webp Xinput vorbisfile winmm")
	if env['python']:
		jnglLibs += Split("openal ogg")
	if env['msvc']:
		jnglLibs += Split("openal32 ogg")
	if not env['msvc']:
		env.Append(CPPDEFINES={'WEAK_LINKING_OPENAL': '1', 'WINVER': '0x0602'})
	env.Append(CPPPATH = ['/mingw64/include/freetype2'])
	lib = env.Library(target="jngl",
	                  source=source_files +
	                         Glob(buildDir + 'win32/*.cpp'),
	                  LIBS=jnglLibs)
	linkflags = "-mwindows"
	if env['debug'] or env['msvc']:
		linkflags = ""
	libs = Split("jngl") + jnglLibs
	env.Program("test", testSrc,
	            LIBPATH = ["."],
	            LIBS = libs,
	            LINKFLAGS = linkflags)
	if env['python']:
		env = env.Clone()
		env.Append(CPPPATH=["/mingw64/include/python3.5m"],
		           LIBPATH=Split(". lib ./boost-libs/lib/win ./python"),
		           LIBS=libs + Split("python3.5 boost_python3-mt"),
		           LINKFLAGS=linkflags)
		env.SharedLibrary(source="python/main.cpp")

if env['PLATFORM'] == 'posix': # Linux
	if env['python']:
		env.Append(CCFLAGS = '-DNOJPEG')
	source_files += Glob(buildDir + 'linux/*.cpp') + Glob(buildDir + 'sdl/*.cpp')
	env.ParseConfig('pkg-config --cflags --libs fontconfig freetype2 sdl2')
	env.Append(CCFLAGS="-fPIC -DNO_WEAK_LINKING_OPENAL")
	lib = env.Library(target="jngl", source=source_files)
	env.Append(LIBPATH='.', CPPPATH='src')
	testEnv = env.Clone()
	testEnv.ParseConfig("pkg-config --cflags --libs jngl.pc")
	testEnv.Program('test', testSrc)
	if env['python']:
		env = env.Clone()
		env.ParseConfig("pkg-config --cflags --libs jngl.pc python3")
		env.Append(LIBPATH = ["src", "./python"])
		if conf.CheckLib(["boost_python3"]):
			env.Append(LIBS=["boost_python3"])
		else:
			import re
			pythonVersion = ''.join(re.search('.*(\d)\.(\d).*', env['LIBS'][-1]).groups())
			env.Append(LIBS = ["boost_python-py" + pythonVersion])
		env.SharedLibrary(target="python/libjngl.so",
		                  source="python/main.cpp")

if env['PLATFORM'] == 'darwin': # Mac
	env.Append(LIBS=Split('jpeg ogg vorbisfile webp'),
	           LIBPATH=['.'],
	           CPPPATH=['/usr/local/include/'],
	           LINKFLAGS='-framework OpenAL -framework OpenGL')
	env.ParseConfig('pkg-config --cflags --libs sdl2 freetype2 libpng')
	lib = env.Library(target="jngl",
	                  source=source_files + Glob(buildDir + 'sdl/*.cpp') +
	                         Glob(buildDir + 'mac/*.cpp'))
	testEnv = env.Clone()
	testEnv.Append(CPPPATH='.')
	testEnv.Append(LIBS=lib)
	testEnv.Program("test", testSrc)
	if env['python']:
		env = env.Clone()
		env.Append(CPPPATH='/System/Library/Frameworks/Python.framework/Headers/',
		           LINKFLAGS='-framework Python',
		           LIBPATH=Split(". ./lib ./python"),
		           LIBS=Split("boost_python"))
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
		name = 'Python 3.4'
	exefile = 'installer/JNGL ' + version + ' (' + name + ').exe'
	if env['msvc']:
		t = Command(exefile, None,
		            '"C:\\Program Files (x86)\\NSIS\\Bin\\makensis" ' + nsiFile)
	else:
		t = Command(exefile, None,
		            '/c/Program\ Files\ \(x86\)/NSIS/Bin/makensis ' + nsiFile)
	if env['python'] and not env['msvc']:
		Depends(t, ['python/jngl.dll'])
	Clean(t, exefile)
