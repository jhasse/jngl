#!/usr/bin/python

version = "1.0.3"

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

env = Environment(variables = vars)
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

if env['PLATFORM'] == 'darwin':
	env['CC']  = 'clang'
	env['CXX'] = 'clang++'

if env['debug']:
	env.Append(CCFLAGS = '-g -Wall')
else:
	env.Append(CCFLAGS = '-O2 -DNDEBUG')
if env['profile']:
	env.Append(CCFLAGS = '-pg', _LIBFLAGS = ' -pg')
if not env['verbose']:
	env['CCCOMSTR'] = env['CXXCOMSTR'] = "compiling: $TARGET"
	env['LINKCOMSTR'] = "linking: $TARGET"
	env['ARCOMSTR'] = "archiving: $TARGET"

if not env['msvc']:
	source_files = env.Object(Glob("src/*.cpp") + Glob("src/jngl/*.cpp"), CPPFLAGS="-std=c++0x")
	source_files += Split("""
	src/callbacks.c
	src/ConvertUTF.c
	""")

testSrc = "src/test/test.cpp"

if env['PLATFORM'] == 'win32' and not env['msvc']: # Windows
	jnglLibs = Split("glew32 freetype png opengl32 glu32 user32 shell32 gdi32 z jpeg dl webp")
	if env['python'] or env['msvc']:
		jnglLibs += Split("openal32 ogg vorbisfile")
	else:
		env.Append(CPPDEFINES='WEAK_LINKING_OPENAL')
	env.Append(CPPPATH="./include")
	lib = env.Library(target="jngl",
	                  source=source_files +
	                         env.Object(Glob('src/win32/*.cpp'),
	                                         CPPFLAGS="-std=c++11"),
	                  LIBS=jnglLibs)
	linkflags = "-mwindows"
	if env['debug'] or env['msvc']:
		linkflags = ""
	libs = Split("jngl") + jnglLibs
	env.Program("test", testSrc,
	            CPPFLAGS="-std=gnu++0x",
	            CPPPATH=".",
				LIBPATH=Split("lib ."),
				LIBS=libs,
				LINKFLAGS=linkflags)
	if env['python']:
		env = env.Clone()
		env.Append(CPPPATH=Split("C:\Python27\include"),
		           LIBPATH=Split(". ./lib ./python C:\Python32\libs"),
		           LIBS=libs + Split("python32 libboost_python-mgw47-mt-1_51"),
		           LINKFLAGS=linkflags)
		env.SharedLibrary(target="python/jngl.dll",
		                  source="python/main.cpp")

if env['PLATFORM'] == 'posix': # Linux
	if env['python']:
		env.Append(CCFLAGS = '-DNOJPEG')
	source_files += env.Object(Glob('src/linux/*.cpp'), CPPFLAGS='-std=c++0x')
	env.ParseConfig('pkg-config --cflags --libs fontconfig glib-2.0')
	env.ParseConfig('pkg-config --cflags --libs freetype2')
	env.Append(CCFLAGS="-fPIC -DNO_WEAK_LINKING_OPENAL")
	lib = env.Library(target="jngl", source=source_files)
	env.Append(LIBPATH='.', CPPPATH='src')
	testEnv = env.Clone()
	testEnv.ParseConfig("pkg-config --cflags --libs jngl.pc")
	testEnv.Program('test', testSrc, CPPFLAGS="-std=c++0x")
	if env['python']:
		env = env.Clone()
		env.ParseConfig("pkg-config --cflags --libs jngl.pc")
		env.Append(CPPPATH="/usr/include/python3.2",
		           LIBPATH=Split("src ./python"),
		           LIBS=Split("python3.2mu boost_python-py32"))
		env.SharedLibrary(target="python/libjngl.so",
		                  source="python/main.cpp")

if env['PLATFORM'] == 'darwin': # Mac
	env.Append(LIBS=Split('GLEW jpeg ogg vorbisfile webp'),
	           LIBPATH=Split('/opt/local/lib .'),
	           CPPPATH=['/opt/local/include/', '../boost-libs/include'],
	           LINKFLAGS='-framework OpenAL -framework OpenGL')
	env.ParseConfig('/opt/local/bin/pkg-config --cflags --libs freetype2 libpng')
	env.ParseConfig('/opt/local/bin/sdl-config --cflags --libs')
	lib = env.Library(target="jngl", source=source_files + env.Object(Glob('src/sdl/*.cpp'), CPPFLAGS='-std=c++11'))
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
	import os
	t = Command('JNGL ' + version + '.exe', lib, '"' + os.path.expandvars("%programfiles%") + '\NSIS\makensis.exe " ' + nsiFile)
	if env['python']:
		Depends(t, ['python/jngl.dll'])
	Clean(t, 'installer/JNGL ' + version + ' (' + name + ').exe')
