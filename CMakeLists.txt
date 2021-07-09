cmake_minimum_required(VERSION 3.13)

project(jngl LANGUAGES C CXX)

if(NOT MSVC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror=implicit-function-declaration")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
	string(APPEND CMAKE_C_FLAGS " -Wno-shorten-64-to-32") # ogg, vorbis, etc. are full of these
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

file(GLOB SRC src/*.cpp src/jngl/*.cpp)
add_library(jngl ${SRC})
file(GLOB HEADERS src/*.hpp src/jngl/*.hpp)
target_sources(jngl PRIVATE ${HEADERS})

target_compile_features(jngl PUBLIC cxx_std_17)

set_target_properties(jngl PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/src/
)

include(FetchContent)
if(IOS)
	set(Boost_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include/ios")
	# find_package is broken on iOS, see https://gitlab.kitware.com/cmake/cmake/issues/19808
	set(CMAKE_DISABLE_FIND_PACKAGE_ZLIB TRUE)
	set(CMAKE_DISABLE_FIND_PACKAGE_BZip2 TRUE)
endif()
find_package(Boost)
if(Boost_FOUND)
	target_include_directories(jngl PUBLIC ${Boost_INCLUDE_DIRS})
else()
	FetchContent_Declare(boost
		URL https://boostorg.jfrog.io/artifactory/main/release/1.71.0/source/boost_1_71_0.7z
		URL_HASH SHA256=1b3949ffddf56129141032542523920034f2e69775fb9332a52ba37c548dbf35)
	FetchContent_MakeAvailable(boost)
	FetchContent_GetProperties(boost SOURCE_DIR BOOST_SRC_DIR)
	target_include_directories(jngl PUBLIC ${BOOST_SRC_DIR})
endif()

option(JNGL_VIDEO "Enable Theora video playback" ON)
option(JNGL_JPEG "Enable JPEG support if libjpeg is available" ON)

set(ALSOFT_TESTS OFF CACHE BOOL "Build and install test programs" FORCE)
set(ALSOFT_EXAMPLES OFF CACHE BOOL "Build and install example programs" FORCE)
set(ALSOFT_UTILS OFF CACHE BOOL "Build and install utility programs" FORCE)

if (ANDROID)
	file(GLOB ANDROID_SOURCES src/android/*.cpp)
	target_sources(jngl PRIVATE ${ANDROID_SOURCES})
elseif(IOS)
	file(GLOB SRC src/ios/*.cpp src/ios/*.mm src/ios/*.h)
	target_sources(jngl PRIVATE ${SRC} src/mac/message.cpp)
elseif (UNIX)
	file(GLOB SRC src/sdl/*.cpp)
	target_sources(jngl PRIVATE ${SRC})
	if(APPLE)
		file(GLOB SRC src/mac/*.cpp src/mac/*.mm)
		enable_language(OBJCXX)
		target_sources(jngl PRIVATE ${SRC})
		find_library(MAC_FOUNDATION Foundation)
		find_library(MAC_CORETEXT CoreText)
		target_link_libraries(jngl PRIVATE ${MAC_FOUNDATION} ${MAC_CORETEXT})
	elseif(EMSCRIPTEN)
		file(GLOB SRC src/emscripten/*.cpp)
		target_sources(jngl PRIVATE src/mac/message.cpp ${SRC})
	else()
		file(GLOB SRC src/linux/*.cpp src/linux/*.c)
		target_sources(jngl PRIVATE ${SRC})
    endif()

	target_link_libraries(jngl PRIVATE $<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,9.0>>:stdc++fs>)
else() # Windows
	set(JNGL_SDL2_DEFAULT OFF)
	if(WINDOWS_STORE)
		set(JNGL_SDL2_DEFAULT ON)

		FetchContent_Declare(
			angle
			GIT_REPOSITORY https://github.com/jhasse/angle.git
			GIT_TAG        cmake
		)
		if(NOT angle_POPULATED)
			FetchContent_Populate(angle)

			set(BUILD_SHARED_LIBS_OLD ${BUILD_SHARED_LIBS})
			set(BUILD_SHARED_LIBS ON CACHE INTERNAL "")
			add_subdirectory(${angle_SOURCE_DIR} ${angle_BINARY_DIR})
			set(BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS_OLD} CACHE BOOL "" FORCE)
		endif()
	endif()
	option(JNGL_SDL2 "Use SDL2 instead of WinAPI" ${JNGL_SDL2_DEFAULT})

	target_compile_definitions(jngl PRIVATE _WIN32_WINNT=0x602 _USE_MATH_DEFINES UNICODE)
	if(JNGL_SDL2)
		file(GLOB SRC src/sdl/*.cpp)
		target_sources(jngl PRIVATE src/win32/fontfile.cpp src/win32/message.cpp src/win32/other.cpp
		                            src/win32/unicode.cpp)
	else()
		file(GLOB SRC src/win32/*.cpp)
	endif()
	target_sources(jngl PRIVATE ${SRC})
	target_link_libraries(jngl PUBLIC winmm)
	if(WINDOWS_STORE)
		target_link_libraries(jngl PUBLIC libEGL libGLESv2)
	endif()
	if(MSVC)
		string(APPEND CMAKE_C_FLAGS " /D_CRT_SECURE_NO_WARNINGS")
		string(APPEND CMAKE_CXX_FLAGS " /D_CRT_SECURE_NO_WARNINGS /D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING")
		if(CMAKE_SIZEOF_VOID_P EQUAL 8)
			set(JNGL_ARCH "x86_64")
		else()
			set(JNGL_ARCH "x86")
		endif()
		target_link_libraries(jngl PUBLIC Xinput)
	else()
		target_link_libraries(jngl PUBLIC xinput1_4)
	endif()
endif()

# fontconfig
if (UNIX AND NOT EMSCRIPTEN)
	if(NOT APPLE AND NOT ANDROID)
        find_package(Fontconfig REQUIRED)
		target_include_directories(jngl PUBLIC ${FONTCONFIG_INCLUDE_DIR})
		target_link_libraries(jngl PRIVATE ${FONTCONFIG_LIBRARIES})
    endif()
endif()

if(ANDROID OR MSVC OR IOS)
	include(cmake/CPM.cmake)
	CPMAddPackage(NAME libogg
		URL http://downloads.xiph.org/releases/ogg/libogg-1.3.5.tar.xz
		URL_HASH SHA256=c4d91be36fc8e54deae7575241e03f4211eb102afb3fc0775fbbc1b740016705
		OPTIONS "BUILD_TESTING OFF")
	set(OGG_INCLUDE_DIR ${libogg_SOURCE_DIR}/include)
	set(OGG_INCLUDE_DIRS ${OGG_INCLUDE_DIR} ${libogg_BINARY_DIR}/include)
	set(OGG_LIBRARY $<TARGET_FILE:ogg>)

	FetchContent_Declare(vorbis
		URL https://github.com/xiph/vorbis/archive/v1.3.7.zip
		URL_HASH SHA1=fb943e21c34a1a2035009c06cfbfd9235af406d0)
	FetchContent_MakeAvailable(vorbis)
	target_include_directories(jngl PRIVATE ${vorbis_SOURCE_DIR}/include)
	target_link_libraries(jngl PUBLIC vorbis vorbisfile ogg)
endif()

get_directory_property(hasParent PARENT_DIRECTORY)
if(ANDROID)
	target_compile_definitions(jngl PRIVATE NOJPEG NOPNG OPENGLES)

	set(ALSOFT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android/openal-soft)
	if ((NOT EXISTS ${ALSOFT_SRC_DIR}) OR (NOT EXISTS ${ALSOFT_SRC_DIR}/CMakeLists.txt))
		execute_process(COMMAND git clone --depth=1 -b v1.18
		                https://github.com/kcat/openal-soft
		                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/android/)
	endif()
	set(ALSOFT_CONFIG OFF CACHE BOOL "Install alsoft.conf sample configuration file" FORCE)
	set(ALSOFT_HRTF_DEFS OFF CACHE BOOL "Install HRTF definition files" FORCE)
	set(ALSOFT_AMBDEC_PRESETS OFF CACHE BOOL "Install AmbDec preset files" FORCE)
	set(ALSOFT_BACKEND_WAVE OFF CACHE BOOL "Enable Wave Writer backend" FORCE)
	set(LIBTYPE "STATIC" CACHE STRING "" FORCE)
	add_subdirectory(${ALSOFT_SRC_DIR} ${CMAKE_CURRENT_BINARY_DIR}/openal-soft)
	set(OPENAL_LIBRARY OpenAL)
	target_include_directories(jngl PRIVATE ${ALSOFT_SRC_DIR}/include)

	set(FREETYPE_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android/freetype2)
	if ((NOT EXISTS ${FREETYPE_SRC_DIR}) OR (NOT EXISTS ${FREETYPE_SRC_DIR}/CMakeLists.txt))
		execute_process(COMMAND git clone --depth=1 -b VER-2-9-1
		                https://git.savannah.gnu.org/git/freetype/freetype2.git
		                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/android/)
	endif()
	add_subdirectory(${FREETYPE_SRC_DIR} ${CMAKE_CURRENT_BINARY_DIR}/freetype2)
	if(hasParent)
		set(FREETYPE_INCLUDE_DIRS ${FREETYPE_SRC_DIR}/include PARENT_SCOPE)
		set(FREETYPE_LIBRARY freetype PARENT_SCOPE)
	endif()
	set(FREETYPE_INCLUDE_DIRS ${FREETYPE_SRC_DIR}/include)
	set(FREETYPE_LIBRARIES freetype)

	target_include_directories(jngl PUBLIC ${ANDROID_NDK}/sources/android/native_app_glue)
	target_include_directories(jngl PRIVATE ${OGG_INCLUDE_DIRS} ${VORBIS_SRC_DIR}/include)
else()
	if(UNIX AND NOT IOS AND NOT EMSCRIPTEN)
		find_package(SDL2 REQUIRED)
		if(SDL2_INCLUDE_DIRS)
			include_directories(${SDL2_INCLUDE_DIRS})
			target_link_libraries(jngl PUBLIC ${SDL2_LIBRARIES})
		else()
			target_link_libraries(jngl PUBLIC SDL2::SDL2)
		endif()
	endif()

	if(JNGL_JPEG)
		find_package(JPEG)
	endif()
	if(JPEG_FOUND)
		target_include_directories(jngl PUBLIC ${JPEG_INCLUDE_DIR})
		target_link_libraries(jngl PRIVATE ${JPEG_LIBRARIES})
	else()
		target_compile_definitions(jngl PRIVATE NOJPEG)
	endif()

	if(NOT IOS AND NOT EMSCRIPTEN)
		find_package(Threads REQUIRED)
		target_link_libraries(jngl PUBLIC Threads::Threads)
	endif()

	if(NOT EMSCRIPTEN)
		find_package(PNG)
		if(PNG_FOUND)
			target_link_libraries(jngl PUBLIC ${PNG_LIBRARIES})
		else()
			target_compile_definitions(jngl PRIVATE NOPNG)
		endif()
	endif()

	if(MSVC)
		if(JNGL_SDL2)
			set(SDL_SENSOR OFF CACHE BOOL "" FORCE) # doesn't work with UWP
			FetchContent_Declare(sdl2
				URL https://www.libsdl.org/release/SDL2-2.0.14.zip
				URL_HASH SHA1=94c60f9c34fe5aaad443df0051e0202bfd4d583b
			)
			FetchContent_MakeAvailable(sdl2)
			target_link_libraries(jngl PRIVATE SDL2-static)
		endif()
		if(WINDOWS_STORE)
			target_compile_definitions(jngl PUBLIC JNGL_UWP)
			target_include_directories(jngl PUBLIC ${sdl2_SOURCE_DIR}/include ${sdl2_BINARY_DIR}/include) # TODO: This should be PRIVATE
			FetchContent_Declare(openal
				GIT_REPOSITORY https://github.com/jhasse/openal-soft-winphone
				GIT_TAG e4f3681316b1e8d8d68dfc31b1fc952d9bba085d)
			target_link_options(jngl PUBLIC $<IF:$<CONFIG:Debug>,/defaultlib:vccorlibd.lib /defaultlib:msvcrtd.lib,/defaultlib:vccorlib.lib /defaultlib:msvcrt.lib>)
		else()
			FetchContent_Declare(openal
				URL https://github.com/kcat/openal-soft/archive/openal-soft-1.20.1.zip
				URL_HASH SHA1=d86a714e0e7900f4f7197738cb35432772bfe2e0)
			set(ALSOFT_NO_CONFIG_UTIL ON CACHE BOOL "Disable building the alsoft-config utility" FORCE)
			set(LIBTYPE "STATIC" CACHE STRING "" FORCE)
			target_compile_definitions(jngl PRIVATE AL_LIBTYPE_STATIC)

			target_include_directories(jngl PRIVATE ${PROJECT_SOURCE_DIR}/include/msvc)
			target_link_directories(jngl PUBLIC ${PROJECT_SOURCE_DIR}/lib/msvc/${JNGL_ARCH})
			add_custom_command(TARGET jngl COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/lib/msvc/${JNGL_ARCH}/epoxy-0.dll "$<TARGET_FILE_DIR:jngl>")
			target_link_libraries(jngl PUBLIC epoxy)
		endif()
		FetchContent_MakeAvailable(openal)
		set(OPENAL_LIBRARY OpenAL)

	elseif(IOS)
		target_compile_options(jngl PRIVATE -Wno-deprecated-declarations) # OpenAL
		target_compile_definitions(jngl PRIVATE IOS)
		target_compile_definitions(jngl PUBLIC GLES_SILENCE_DEPRECATION)
		target_include_directories(jngl PUBLIC include/ios)
		find_library(IOS_OPENGLES OpenGLES)
		find_library(IOS_OPENAL OpenAL)
		find_library(IOS_QUARTZCORE QuartzCore)
		find_library(IOS_UIKIT UIKit)
		find_library(IOS_AVFOUNDATION AVFoundation)
		find_library(IOS_GAMECONTROLLER GameController)
		target_link_libraries(jngl PRIVATE
			${IOS_OPENGLES} ${IOS_OPENAL} ${IOS_QUARTZCORE} ${IOS_UIKIT} ${IOS_AVFOUNDATION}
			${IOS_GAMECONTROLLER}
		)

		if(NOT hasParent)
			set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.bixense.jngl-test")
			set(MACOSX_BUNDLE_BUNDLE_VERSION "1.0")
			set(MACOSX_BUNDLE_LONG_VERSION_STRING "1.0")
			set(MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0")
			set_source_files_properties(data/Arial.ttf PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
			set_source_files_properties(data/jngl.webp PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
			set_source_files_properties(data/test.ogg PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
			add_executable(jngl-test src/test/test.cpp src/ios/test/main.mm data/Arial.ttf
				data/jngl.webp data/test.ogg)
			set_target_properties(jngl-test PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${PROJECT_SOURCE_DIR}/data/ios/jngl-test.in.plist")
		endif()
	else()
		find_package(OpenAL REQUIRED)

		find_package(WebP)
		if(WebP_FOUND)
			target_link_libraries(jngl PRIVATE ${WEBP_LIBRARIES})
		else()
			target_compile_definitions(jngl PRIVATE NOWEBP)
		endif()

		find_package(PkgConfig)
		if(PkgConfig_FOUND)
			if(JNGL_SDL2)
				pkg_check_modules(SDL2 REQUIRED sdl2)
				target_include_directories(jngl PRIVATE ${SDL2_INCLUDE_DIRS})
				target_link_libraries(jngl PRIVATE ${SDL2_LINK_LIBRARIES})
			endif()
			if(JNGL_VIDEO)
				if(APPLE)
					# On macOS we'll build theora from source, since Homebrew's version crashes due
					# to a Xcode bug in 10.15: https://forums.developer.apple.com/thread/121887
					pkg_check_modules(OGG ogg REQUIRED)
					target_link_libraries(jngl PRIVATE ${OGG_LINK_LIBRARIES})
				else()
					pkg_check_modules(THEORA_DEC theoradec)
				endif()
				if(APPLE OR THEORA_DEC_FOUND)
					pkg_check_modules(VORBIS vorbis)
					if(VORBIS_FOUND)
						target_compile_definitions(jngl PRIVATE JNGL_VIDEO)
						target_sources(jngl PRIVATE subprojects/theoraplay/theoraplay.cpp)
						target_include_directories(jngl PRIVATE ${THEORA_DEC_INCLUDE_DIRS})
						target_link_libraries(jngl PRIVATE ${VORBIS_LINK_LIBRARIES} ${THEORA_DEC_LINK_LIBRARIES})

						add_executable(videoplayer examples/videoplayer.cpp)
						target_link_libraries(videoplayer PRIVATE jngl)
						add_custom_command(TARGET videoplayer COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/data/verysmall.ogv "$<TARGET_FILE_DIR:videoplayer>")
					endif()
				endif()
			endif()
			if(NOT APPLE AND NOT EMSCRIPTEN)
				pkg_check_modules(EPOXY REQUIRED epoxy)
				target_link_libraries(jngl PUBLIC ${EPOXY_LINK_LIBRARIES})
			endif()
			if(NOT EMSCRIPTEN)
				pkg_check_modules(VorbisFile REQUIRED vorbisfile)
				pkg_check_modules(FREETYPE REQUIRED freetype2)
			endif()
		else()
			if(NOT EMSCRIPTEN)
				find_package(VorbisFile REQUIRED)
				find_package(Freetype REQUIRED)
			endif()
		endif()
		target_include_directories(jngl PUBLIC ${VorbisFile_INCLUDE_DIRS})
		if(NOT EMSCRIPTEN)
			target_link_libraries(jngl PUBLIC ${VorbisFile_LIBRARIES})
		endif()
		target_link_directories(jngl PUBLIC ${VorbisFile_LIBRARY_DIRS})
		if(NOT PkgConfig_FOUND OR APPLE AND NOT EMSCRIPTEN)
			find_package(epoxy REQUIRED)
			target_include_directories(jngl PRIVATE ${epoxy_INCLUDE_DIR})
			target_link_libraries(jngl PUBLIC ${epoxy_LIBRARY})
		endif()
	endif()

	if(MSVC OR IOS)
		FetchContent_Declare(freetype
			URL https://download.savannah.gnu.org/releases/freetype/freetype-2.10.2.tar.xz
			URL_HASH SHA1=b074d5c34dc0e3cc150be6e7aa6b07c9ec4ed875)
		FetchContent_MakeAvailable(freetype)
		FetchContent_GetProperties(freetype SOURCE_DIR FREETYPE_SRC_DIR)
		if(hasParent)
			set(FREETYPE_INCLUDE_DIRS ${FREETYPE_SRC_DIR}/include PARENT_SCOPE)
			set(FREETYPE_LIBRARY freetype PARENT_SCOPE)
		endif()
		set(FREETYPE_LIBRARIES freetype)
	endif()

	if(NOT hasParent)
		if(WINDOWS_STORE)
			# WIN32 is needed to build for the Windows subsystem, not Console
			add_executable(jngl-test WIN32 src/test/test.cpp src/uwp/SDL_winrt_main_NonXAML.cpp
				data/Arial.ttf data/jngl.webp data/test.ogg)
			set_source_files_properties(data/Arial.ttf data/jngl.webp data/test.ogg
				PROPERTIES VS_DEPLOYMENT_CONTENT 1)

			# Otherwise linking will fail with
			#   error LNK2038: mismatch detected for
			#   'vccorlib_lib_should_be_specified_before_msvcrt_lib_to_linker': value '1' doesn't
			#   match value '0' in msvcrtd.lib(app_appinit.obj)
			set_source_files_properties(src/uwp/SDL_winrt_main_NonXAML.cpp
				PROPERTIES COMPILE_FLAGS "/ZW /Zc:twoPhase-")
		else()
			if(NOT IOS)
				add_executable(jngl-test src/test/test.cpp)

				add_executable(example-bike examples/bike/base.cpp examples/bike/bike.cpp
					examples/bike/line.cpp examples/bike/main.cpp examples/bike/wheel.cpp)
				target_link_libraries(example-bike jngl)
				add_executable(example-throw examples/throw/main.cpp)
				target_link_libraries(example-throw jngl)
				add_executable(example-shader examples/shader/main.cpp)
				target_link_libraries(example-shader jngl)
			endif()
		endif()
		target_link_libraries(jngl-test jngl)
		add_custom_command(TARGET jngl-test COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/data/jngl.webp "$<TARGET_FILE_DIR:jngl-test>")
		add_custom_command(TARGET jngl-test COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/data/test.ogg "$<TARGET_FILE_DIR:jngl-test>")
		add_custom_command(TARGET jngl-test COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/jngl.png "$<TARGET_FILE_DIR:jngl-test>")
	endif()
endif()

if(MSVC OR IOS OR ANDROID)
	FetchContent_Declare(webp
		GIT_REPOSITORY https://chromium.googlesource.com/webm/libwebp
		GIT_TAG 25ae67b3de2ba941ff19ee0e90655b591faf048e)
	# Unfortunately disabling this breaks ARM builds:
	# set(WEBP_ENABLE_SIMD OFF CACHE BOOL "Enable any SIMD optimization." FORCE)
	set(WEBP_BUILD_ANIM_UTILS OFF CACHE BOOL "Build animation utilities." FORCE)
	set(WEBP_BUILD_CWEBP OFF CACHE BOOL "Build the cwebp command line tool." FORCE)
	set(WEBP_BUILD_DWEBP OFF CACHE BOOL "Build the dwebp command line tool." FORCE)
	set(WEBP_BUILD_GIF2WEBP OFF CACHE BOOL "Build the gif2webp conversion tool." FORCE)
	set(WEBP_BUILD_IMG2WEBP OFF CACHE BOOL "Build the img2webp animation tool." FORCE)
	set(WEBP_BUILD_VWEBP OFF CACHE BOOL "Build the vwebp viewer tool." FORCE)
	set(WEBP_BUILD_WEBPINFO OFF CACHE BOOL "Build the webpinfo command line tool." FORCE)
	set(WEBP_BUILD_WEBPMUX OFF CACHE BOOL "Build the webpmux command line tool." FORCE)
	set(WEBP_BUILD_EXTRAS OFF CACHE BOOL "Build extras." FORCE)
	if(NOT ANDROID) # error: undefined reference to 'WebPInitSharpYUVNEON'
		set(WEBP_ENABLE_SIMD OFF CACHE BOOL "Enable any SIMD optimization." FORCE)
	endif()
	FetchContent_MakeAvailable(webp)
	FetchContent_GetProperties(webp SOURCE_DIR WEBP_SRC_DIR)
	target_include_directories(jngl PRIVATE ${WEBP_SRC_DIR}/src)
	target_link_libraries(jngl PRIVATE webpdecode webpdspdecode webputilsdecode)
	# Note: For some reason it's important to add webp last for MSVC. Otherwise other CMake
	# projects linking JNGL will get:
	#   MSVCRTD.lib(exe_winmain.obj) : error LNK2019: unresolved external symbol WinMain
	#   referenced in function "int __cdecl invoke_main(void)" (?invoke_main@@YAHXZ)
endif()

if(ANDROID OR IOS OR APPLE OR WINDOWS_STORE)
	if(JNGL_VIDEO)
		FetchContent_Declare(theora
			URL http://downloads.xiph.org/releases/theora/libtheora-1.1.1.tar.bz2
			URL_HASH SHA1=8dcaa8e61cd86eb1244467c0b64b9ddac04ae262)
		FetchContent_Populate(theora)
		add_library(theoradec OBJECT ${LIBTHEORA_DEC} "${theora_SOURCE_DIR}/lib/apiwrapper.c"
			"${theora_SOURCE_DIR}/lib/bitpack.c" "${theora_SOURCE_DIR}/lib/dequant.c"
			"${theora_SOURCE_DIR}/lib/fragment.c" "${theora_SOURCE_DIR}/lib/idct.c"
			"${theora_SOURCE_DIR}/lib/info.c" "${theora_SOURCE_DIR}/lib/internal.c"
			"${theora_SOURCE_DIR}/lib/state.c" "${theora_SOURCE_DIR}/lib/quant.c"
			"${theora_SOURCE_DIR}/lib/decapiwrapper.c" "${theora_SOURCE_DIR}/lib/decinfo.c"
			"${theora_SOURCE_DIR}/lib/decode.c" "${theora_SOURCE_DIR}/lib/huffdec.c")
		target_include_directories(theoradec PRIVATE ${OGG_INCLUDE_DIRS}
			"${theora_SOURCE_DIR}/include")
		if(NOT MSVC)
			target_compile_options(theoradec PRIVATE -Wno-shift-negative-value
				-Wno-shift-op-parentheses -Wno-logical-op-parentheses -Wno-parentheses)
		endif()
		target_link_libraries(jngl PRIVATE theoradec)
		target_compile_definitions(jngl PRIVATE JNGL_VIDEO)
		target_include_directories(jngl PRIVATE "${theora_SOURCE_DIR}/include")
		target_sources(jngl PRIVATE subprojects/theoraplay/theoraplay.cpp)
	endif()
endif()

target_include_directories(jngl PRIVATE ${FREETYPE_INCLUDE_DIRS})
target_link_directories(jngl PUBLIC ${FREETYPE_LIBRARY_DIRS})
target_link_libraries(jngl PUBLIC ${FREETYPE_LIBRARIES} ${OPENAL_LIBRARY})
