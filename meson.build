project('jngl', ['c', 'cpp'], default_options : ['cpp_std=c++17', 'warning_level=3'])

conf_data = configuration_data()
conf_data.set('VERSION', '1.4.1')
configure_file(input : 'jngl.pc.in', output : 'jngl.pc', configuration : conf_data,
               install : true, install_dir : 'lib/pkgconfig')

if get_option('buildtype') == 'release'
	add_project_arguments('-DNDEBUG', language : 'cpp')
endif

src = [
	'src/audio.cpp',
	'src/freetype.cpp',
	'src/helper.cpp',
	'src/jngl/App.cpp',
	'src/jngl/Color.cpp',
	'src/jngl/Container.cpp',
	'src/jngl/Controller.cpp',
	'src/jngl/Drawable.cpp',
	'src/jngl/effects.cpp',
	'src/jngl/Finally.cpp',
	'src/jngl/font.cpp',
	'src/jngl/framebuffer.cpp',
	'src/jngl/input.cpp',
	'src/jngl/job.cpp',
	'src/jngl/Label.cpp',
	'src/jngl/Mat3.cpp',
	'src/jngl/Mat4.cpp',
	'src/jngl/Pixels.cpp',
	'src/jngl/ScaleablePixels.cpp',
	'src/jngl/screen.cpp',
	'src/jngl/Shader.cpp',
	'src/jngl/ShaderProgram.cpp',
	'src/jngl/shapes.cpp',
	'src/jngl/SoundFile.cpp',
	'src/jngl/sprite.cpp',
	'src/jngl/text.cpp',
	'src/jngl/TextLine.cpp',
	'src/jngl/time.cpp',
	'src/jngl/Vec2.cpp',
	'src/jngl/Widget.cpp',
	'src/jngl/window.cpp',
	'src/jngl/work.cpp',
	'src/main.cpp',
	'src/opengl.cpp',
	'src/Sound.cpp',
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
	'-Wfloat-equal',
	'-Wdelete-non-virtual-dtor',
	'-Wlogical-op',
	'-Wduplicated-cond',
	'-Wduplicated-branches',
	'-Wnewline-eof',
]), language : 'cpp')

if target_machine.system() != 'windows'
	opengl_dep = [
		dependency('SDL2'),
	]
	if target_machine.system() == 'darwin'
		add_languages(['objcpp'])
		add_project_arguments('-std=c++17', language : 'objcpp')
		opengl_dep += [
			dependency('appleframeworks', modules : [
				'Foundation', 'AppKit', 'IOKIT', 'QuartzCore', 'OpenGL', 'GLUT', 'OpenAL'
			], required : true),
		]
		src += [
			'src/mac/other.cpp',
			'src/mac/paths.mm',
			'src/mac/window.mm',
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
		'src/sdl/other.cpp',
		'src/sdl/sdl.cpp',
		'src/sdl/window.cpp',
		'src/sdl/SdlController.cpp',
		'src/linux/message.cpp',
		'src/linux/other.cpp',
		'src/linux/binreloc.c',
	]
else
	cpp_args += ['-D_WIN32_WINNT=0x602', '-D_USE_MATH_DEFINES', '-DUNICODE']
	if cxx.get_argument_syntax() == 'msvc'
		xinput_lib = cxx.find_library('xinput')
	else
		xinput_lib = cxx.find_library('xinput1_4')
	endif
	opengl_dep = [
		cxx.find_library('opengl32'),
		cxx.find_library('glu32'),
		xinput_lib,
		cxx.find_library('winmm'),
	]
	src += [
		'src/win32/ConvertUTF.cpp',
		'src/win32/fontfile.cpp',
		'src/win32/inputimpl.cpp',
		'src/win32/message.cpp',
		'src/win32/other.cpp',
		'src/win32/unicode.cpp',
		'src/win32/windowimpl.cpp',
		'src/win32/XinputController.cpp',
	]
endif

jpeg_dep = dependency('libjpeg', required : false)
if not jpeg_dep.found()
	cpp_args += ['-DNOJPEG']
endif

thread_dep = dependency('threads')
boost_dep = dependency('boost')

lib = static_library('jngl', sources : src, cpp_args : cpp_args, install : true, dependencies : [
	dependency('freetype2'),
	dependency('epoxy'),
	dependency('libwebp'),
	dependency('libpng'),
	dependency('openal'),
	dependency('vorbisfile'),
	boost_dep,
	jpeg_dep,
	opengl_dep,
])
include = include_directories('src')

jngl_dep = declare_dependency(include_directories : include, link_with : lib,
                              dependencies : [boost_dep, thread_dep])

if not meson.is_subproject()
	executable('jngl-test', sources : ['src/test/test.cpp'], link_with : lib, dependencies : [
		boost_dep, thread_dep,
	])
	boost_test_dep = dependency('boost', modules : [ 'unit_test_framework' ], required : false)
	if boost_test_dep.found()
		test('Unit Test', executable('unittest', [
			'src/unittest/main.cpp', # needs to come first for unity build
			'src/unittest/ColorTest.cpp',
			'src/unittest/DrawableTest.cpp',
			'src/unittest/Fixture.cpp',
			'src/unittest/FrameBufferTest.cpp',
			'src/unittest/misc.cpp',
			'src/unittest/SpriteTest.cpp',
			'src/unittest/TextTest.cpp',
		], dependencies : [boost_test_dep, thread_dep], link_with : lib,
		include_directories : include))
	endif
	executable('example-bike', ['examples/bike/base.cpp', 'examples/bike/bike.cpp',
		'examples/bike/line.cpp', 'examples/bike/main.cpp', 'examples/bike/wheel.cpp'],
		dependencies : jngl_dep)
	executable('example-throw', ['examples/throw/main.cpp'], dependencies : jngl_dep)
	executable('example-shader', ['examples/shader/main.cpp'], dependencies : jngl_dep)
endif

install_headers('src/jngl.hpp')
install_headers([
	'src/jngl/all.hpp',
	'src/jngl/Color.hpp',
	'src/jngl/Controller.hpp',
	'src/jngl/debug.hpp',
	'src/jngl/Drawable.hpp',
	'src/jngl/Finally.hpp',
	'src/jngl/font.hpp',
	'src/jngl/framebuffer.hpp',
	'src/jngl/input.hpp',
	'src/jngl/job.hpp',
	'src/jngl/main.hpp',
	'src/jngl/matrix.hpp',
	'src/jngl/message.hpp',
	'src/jngl/other.hpp',
	'src/jngl/Pixels.hpp',
	'src/jngl/ScaleablePixels.hpp',
	'src/jngl/screen.hpp',
	'src/jngl/shapes.hpp',
	'src/jngl/sound.hpp',
	'src/jngl/sprite.hpp',
	'src/jngl/text.hpp',
	'src/jngl/time.hpp',
	'src/jngl/window.hpp',
	'src/jngl/work.hpp',
], subdir : 'jngl')
