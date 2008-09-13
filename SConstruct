import os

version = "0.7"

#Replace @VERSION@ in certain files
files = ["jngl.pc.in", "autopackage/default.apspec.in", "jngl.nsi.in"]
for filename in files:
	newfilename = filename.replace(".in", "")
	datei = open(filename,"r").readlines()
	open(newfilename,"w").writelines([line.replace("@VERSION@",version) for line in datei])
	Clean('.', newfilename) # Make sure scons -c does clean up tidily


env = Environment()
debug = ARGUMENTS.get('debug', 0)
profile = ARGUMENTS.get('profile', 0)
autopackage = ARGUMENTS.get('autopackage', 0)
installer = ARGUMENTS.get('installer', 0)
if int(debug):
	env.Append(CCFLAGS = '-g -Wall')
else:
	env.Append(CCFLAGS = '-O2 -DNDEBUG')
if int(profile):
	env.Append(CCFLAGS = '-pg', _LIBFLAGS = ' -pg')

if env['PLATFORM'] == 'win32': # Windows
	lib = env.Library(target="jngl", source=Split("main.cpp glu.c texture.cpp freetype.cpp ConvertUTF.c win32/window.cpp win32/time.cpp window.cpp finally.cpp"))
	linkflags = "-mwindows"
	if int(debug):
		linkflags = ""
	env.Program("test.cpp", CPPPATH=".", LIBPATH=".", LIBS=Split("jngl opengl32 glu32 gdi32 png freetype"), LINKFLAGS=linkflags)

if env['PLATFORM'] == 'posix': # Linux
	env.Append(CCFLAGS = '`pkg-config glib-2.0 freetype2 fontconfig --cflags`', LINKFLAGS = '`pkg-config glib-2.0 freetype2 fontconfig --libs`')
	lib = env.Library(target="jngl", source=Split("main.cpp glu.c texture.cpp freetype.cpp ConvertUTF.c linux/window.cpp linux/time.cpp window.cpp finally.cpp"))
	env.Program("test.cpp", CPPPATH=".", LIBPATH=Split("/usr/X11R6/lib ."), LIBS=Split("png jngl GL GLU Xxf86vm"));

if int(autopackage):
	t = Command('jngl Library ' + version + '.package', [lib, 'autopackage/default.apspec.in'], "makepackage")
	Clean(t, ['jngl Library ' + version + '.package', 'jngl Library ' + version + '.package.meta', 'jngl.xml', 'jngl.xml.old'])

if int(installer):
	t = Command('jngl Library ' + version + '.exe', lib, "C:/Programme/NSIS/makensis jngl.nsi")
	Clean(t, 'jngl Library ' + version + '.exe')
