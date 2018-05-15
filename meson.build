project('jngl', ['cpp'], default_options : ['cpp_std=c++1z'])

conf_data = configuration_data()
conf_data.set('VERSION', '1.3.0')
configure_file(input : 'jngl.pc.in', output : 'jngl.pc', configuration : conf_data,
               install : true, install_dir : 'lib/pkgconfig')

if get_option('buildtype') == 'release'
	add_project_arguments('-DNDEBUG', language : 'cpp')
endif

src = [
	'src/audio.cpp',
	'src/framebufferimpl.cpp',
	'src/freetype.cpp',
	'src/helper.cpp',
	'src/jngl/color.cpp',
	'src/jngl/Controller.cpp',
	'src/jngl/drawable.cpp',
	'src/jngl/Finally.cpp',
	'src/jngl/font.cpp',
	'src/jngl/framebuffer.cpp',
	'src/jngl/input.cpp',
	'src/jngl/job.cpp',
	'src/jngl/screen.cpp',
	'src/jngl/shapes.cpp',
	'src/jngl/sprite.cpp',
	'src/jngl/text.cpp',
	'src/jngl/time.cpp',
	'src/jngl/Vec2.cpp',
	'src/jngl/window.cpp',
	'src/jngl/work.cpp',
	'src/main.cpp',
	'src/opengl.cpp',
	'src/spriteimpl.cpp',
	'src/texture.cpp',
	'src/window.cpp',
	'src/windowptr.cpp',
]
cpp_args = []

cxx = meson.get_compiler('cpp')
add_project_arguments(cxx.get_supported_arguments([
	'-Winit-self',
	'-Winconsistent-missing-override',
	'-Wimplicit-fallthrough',
	'-Wpedantic',
]), language : 'cpp')

if target_machine.system() != 'windows'
	opengl_dep = [
		dependency('SDL2'),
	]
	if target_machine.system() == 'darwin'
		opengl_dep += [
			dependency('appleframeworks', modules : [
				'Foundation', 'AppKit', 'IOKIT', 'QuartzCore', 'OpenGL', 'GLUT', 'OpenAL'
			], required : true),
		]
	else
		opengl_dep += [
			dependency('fontconfig'),
		]
		src += [
			'src/linux/window.cpp',
		]
	endif
	src += [
		'src/sdl/input.cpp',
		'src/sdl/sdl.cpp',
		'src/sdl/window.cpp',
		'src/sdl/SdlController.cpp',
		'src/linux/message.cpp',
	]
else
	cpp_args += ['-DWINVER=0x0602']
	opengl_dep = [
		cxx.find_library('opengl32'),
		cxx.find_library('glu32'),
		cxx.find_library('xinput1_4'),
		cxx.find_library('winmm'),
	]
	src += [
		'src/win32/ConvertUTF.cpp',
		'src/win32/inputimpl.cpp',
		'src/win32/message.cpp',
		'src/win32/windowimpl.cpp',
		'src/win32/XinputController.cpp',
	]
endif

jpeg_dep = dependency('libjpeg', required : false)
if not jpeg_dep.found()
	cpp_args += ['-DNOJPEG']
endif

thread_dep = dependency('threads')

lib = static_library('jngl', sources : src, cpp_args : cpp_args, install : true, dependencies : [
	dependency('freetype2'),
	dependency('epoxy'),
	dependency('libwebp'),
	dependency('libpng'),
	dependency('openal'),
	dependency('vorbisfile'),
	jpeg_dep,
	opengl_dep,
])
include = include_directories('src')

executable('jngl-test', sources : ['src/test/test.cpp'], link_with : lib, dependencies : [
	thread_dep,
])

if target_machine.system() == 'linux'
	boost_python_dep = dependency('boost', modules : [ 'python3' ], required : false)
	python3_dep = dependency('python3', required : false)
	if boost_python_dep.found() and python3_dep.found()
		shared_library('jngl', link_with : lib, name_prefix : '', sources : [
			'python/main.cpp',
		], dependencies : [
			boost_python_dep,
			thread_dep,
			python3_dep,
		],
		install : true, install_dir : import('python3').sysconfig_path('platlib'))
	endif
endif

boost_test_dep = dependency('boost', modules : [ 'unit_test_framework' ], required : false)
if boost_test_dep.found()
	test('Unit Test', executable('unittest', [
		'src/unittest/main.cpp',
		'src/unittest/misc.cpp',
		'src/unittest/TextTest.cpp',
		'src/unittest/Fixture.cpp',
	], dependencies : [boost_test_dep, thread_dep], link_with : lib, include_directories : include))
endif

install_headers('src/jngl.hpp')
install_headers([
	'src/jngl/all.hpp',
	'src/jngl/color.hpp',
	'src/jngl/Controller.hpp',
	'src/jngl/debug.hpp',
	'src/jngl/dll.hpp',
	'src/jngl/drawable.hpp',
	'src/jngl/Finally.hpp',
	'src/jngl/font.hpp',
	'src/jngl/framebuffer.hpp',
	'src/jngl/input.hpp',
	'src/jngl/job.hpp',
	'src/jngl/main.hpp',
	'src/jngl/matrix.hpp',
	'src/jngl/message.hpp',
	'src/jngl/other.hpp',
	'src/jngl/screen.hpp',
	'src/jngl/shapes.hpp',
	'src/jngl/sound.hpp',
	'src/jngl/sprite.hpp',
	'src/jngl/text.hpp',
	'src/jngl/time.hpp',
	'src/jngl/types.hpp',
	'src/jngl/window.hpp',
	'src/jngl/work.hpp',
], subdir : 'jngl')
