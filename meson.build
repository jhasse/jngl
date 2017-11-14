project('jngl', ['c', 'cpp'])

if get_option('buildtype') == 'release' and not meson.is_subproject()
	add_global_arguments('-DNDEBUG', language : 'cpp')
endif

src = [
	'src/audio.cpp',
	'src/ConvertUTF.c',
	'src/jngl/Finally.cpp',
	'src/framebufferimpl.cpp',
	'src/freetype.cpp',
	'src/helper.cpp',
	'src/main.cpp',
	'src/opengl.cpp',
	'src/spriteimpl.cpp',
	'src/texture.cpp',
	'src/window.cpp',
	'src/windowptr.cpp',
	'src/jngl/color.cpp',
	'src/jngl/drawable.cpp',
	'src/jngl/font.cpp',
	'src/jngl/framebuffer.cpp',
	'src/jngl/input.cpp',
	'src/jngl/job.cpp',
	'src/jngl/screen.cpp',
	'src/jngl/shapes.cpp',
	'src/jngl/sprite.cpp',
	'src/jngl/text.cpp',
	'src/jngl/time.cpp',
	'src/jngl/window.cpp',
	'src/jngl/work.cpp',
]
cpp_args = []

cxx = meson.get_compiler('cpp')
if target_machine.system() == 'linux'
	opengl_dep = [
		cxx.find_library('GL'),
		dependency('SDL2'),
		dependency('fontconfig'),
	]
	src += [
		'src/sdl/input.cpp',
		'src/sdl/sdl.cpp',
		'src/sdl/window.cpp',
		'src/linux/message.cpp',
		'src/linux/window.cpp',
	]
else
	cpp_args += ['-DWINVER=0x0602']
	opengl_dep = [
		cxx.find_library('opengl32'),
		cxx.find_library('glu32'),
		cxx.find_library('xinput'),
	]
	src += [
		'src/win32/inputimpl.cpp',
		'src/win32/message.cpp',
		'src/win32/time.cpp',
		'src/win32/windowimpl.cpp',
	]
endif

jpeg_dep = dependency('libjpeg', required : false)
if not jpeg_dep.found()
	cpp_args += ['-DNOJPEG']
endif


lib = static_library('jngl', sources : src, cpp_args : cpp_args, dependencies : [
	dependency('freetype2'),
	dependency('glew'),
	dependency('libwebp'),
	dependency('libpng'),
	dependency('openal'),
	dependency('vorbisfile'),
	jpeg_dep,
	opengl_dep,
])
include = include_directories('src')

executable('jngl-test', sources : ['src/test/test.cpp'], link_with : lib, dependencies : [
	dependency('threads')
])