cmake_minimum_required(VERSION 3.13)

project(jngl LANGUAGES C CXX)

if(MSVC)
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus /wd4244 /wd4305")
else()
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror=implicit-function-declaration")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fdiagnostics-color")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
	string(APPEND CMAKE_C_FLAGS " -Wno-shorten-64-to-32") # ogg, vorbis, etc. are full of these
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

file(GLOB SRC src/*.cpp src/jngl/*.cpp
	src/audio/*.cpp
	src/audio/effect/*.cpp
)
add_library(jngl ${SRC})
file(GLOB HEADERS src/*.hpp src/jngl/*.hpp)
target_sources(jngl PRIVATE ${HEADERS})

target_compile_features(jngl PUBLIC cxx_std_20)

if(JNGL_STEAMWORKS)
	message(STATUS "Using Steam SDK: ${JNGL_STEAMWORKS}")
	target_include_directories(jngl PRIVATE ${JNGL_STEAMWORKS}/public/steam)
	if(APPLE)
		target_link_directories(jngl PUBLIC ${JNGL_STEAMWORKS}/redistributable_bin/osx)
		target_link_libraries(jngl PRIVATE steam_api)
	elseif(WIN32)
		target_link_directories(jngl PUBLIC ${JNGL_STEAMWORKS}/redistributable_bin/win64)
		target_link_libraries(jngl PRIVATE steam_api64)
	else()
		target_link_directories(jngl PUBLIC ${JNGL_STEAMWORKS}/redistributable_bin/linux64)
		target_link_libraries(jngl PRIVATE steam_api)
	endif()
	target_sources(jngl PRIVATE src/steam/SteamAchievements.cpp)
else()
	target_sources(jngl PRIVATE src/steam/disabled/initSteam.cpp)
endif()

set_target_properties(jngl PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/src/
)

include(FetchContent)
if(IOS)
	# find_package is broken on iOS, see https://gitlab.kitware.com/cmake/cmake/issues/19808
	set(CMAKE_DISABLE_FIND_PACKAGE_ZLIB TRUE)
	set(CMAKE_DISABLE_FIND_PACKAGE_BZip2 TRUE)
endif()
find_package(Boost 1.76.0) # first version with boost/qvm_lite.hpp
if(Boost_FOUND)
	target_include_directories(jngl PUBLIC ${Boost_INCLUDE_DIRS})
else()
	target_include_directories(jngl PUBLIC include/boost-qvm)
endif()
target_include_directories(jngl PUBLIC include/public)

option(JNGL_VIDEO "Enable Theora video playback" ON)
option(JNGL_STEAMWORKS "Enable Steamsworks SDK support for achievements" OFF)

set(JNGL_BUILD_WEBP_FROM_SOURCE_DEFAULT OFF)
if(MSVC OR IOS OR ANDROID OR EMSCRIPTEN)
	set(JNGL_BUILD_WEBP_FROM_SOURCE_DEFAULT ON)
endif()
option(JNGL_BUILD_WEBP_FROM_SOURCE "Build libwebp from source instead of using the system version" ${JNGL_BUILD_WEBP_FROM_SOURCE_DEFAULT})

if (ANDROID)
	find_package(oboe REQUIRED CONFIG)
	target_link_libraries(jngl PRIVATE oboe::oboe)
	file(GLOB ANDROID_SOURCES src/android/*.cpp)
	target_sources(jngl PRIVATE ${ANDROID_SOURCES} src/audio/android/engine.cpp)
elseif(IOS)
	file(GLOB SRC src/ios/*.cpp src/ios/*.mm src/audio/ios/Engine.mm src/ios/*.h)
	target_sources(jngl PRIVATE ${SRC} src/mac/message.cpp)
elseif (UNIX)
	file(GLOB SRC src/sdl/*.cpp src/sdl/controller/SdlController.cpp)
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
		target_link_libraries(jngl PRIVATE atomic)
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

	target_compile_definitions(jngl PRIVATE _WIN32_WINNT=0x602 _USE_MATH_DEFINES _SILENCE_ALL_CXX20_DEPRECATION_WARNINGS UNICODE)
	if(JNGL_SDL2)
		file(GLOB SRC src/sdl/*.cpp)
		if(WINDOWS_STORE)
			target_sources(jngl PRIVATE src/xinput/XinputController.cpp)
		else()
			target_sources(jngl PRIVATE src/sdl/controller/SdlController.cpp)
		endif()
		target_sources(jngl PRIVATE src/win32/fontfile.cpp src/win32/message.cpp src/win32/other.cpp
		                            src/win32/unicode.cpp)
	else()
		file(GLOB SRC src/win32/*.cpp src/xinput/XinputController.cpp)
		target_link_libraries(jngl PRIVATE opengl32)
	endif()
	target_sources(jngl PRIVATE ${SRC})
	target_link_libraries(jngl PUBLIC winmm)
	if(WINDOWS_STORE)
		target_link_libraries(jngl PUBLIC libEGL libGLESv2)
	else()
		target_sources(jngl PRIVATE glad/src/wgl.c)
	endif()
	if(MSVC)
		string(APPEND CMAKE_C_FLAGS " /D_CRT_SECURE_NO_WARNINGS")
		string(APPEND CMAKE_CXX_FLAGS " /D_CRT_SECURE_NO_WARNINGS /D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING")
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

include(cmake/CPM.cmake)
if(ANDROID OR MSVC OR IOS)
	CPMAddPackage(NAME libogg
		URL http://downloads.xiph.org/releases/ogg/libogg-1.3.5.tar.xz
		URL_HASH SHA256=c4d91be36fc8e54deae7575241e03f4211eb102afb3fc0775fbbc1b740016705
		OPTIONS "BUILD_TESTING OFF")
	set(OGG_INCLUDE_DIR ${libogg_SOURCE_DIR}/include)
	set(OGG_INCLUDE_DIRS ${OGG_INCLUDE_DIR} ${libogg_BINARY_DIR}/include)
	set(OGG_LIBRARY $<TARGET_FILE:ogg>)

	CPMAddPackage(NAME vorbis
		URL https://github.com/xiph/vorbis/archive/v1.3.7.zip
		URL_HASH SHA1=fb943e21c34a1a2035009c06cfbfd9235af406d0)
	target_include_directories(jngl PRIVATE ${vorbis_SOURCE_DIR}/include)
	target_link_libraries(jngl PUBLIC vorbis vorbisfile ogg)
endif()

get_directory_property(hasParent PARENT_DIRECTORY)
if(ANDROID OR MSVC OR IOS)
	CPMAddPackage(NAME freetype
		URL https://sourceforge.net/projects/freetype/files/freetype2/2.11.1/freetype-2.11.1.tar.xz/download
		URL_HASH SHA256=3333ae7cfda88429c97a7ae63b7d01ab398076c3b67182e960e5684050f2c5c8
		OPTIONS "FT_DISABLE_HARFBUZZ 1"
	)
	if(hasParent)
		set(FREETYPE_INCLUDE_DIRS ${freetype_SOURCE_DIR}/include PARENT_SCOPE)
		set(FREETYPE_LIBRARY freetype PARENT_SCOPE)
	endif()
	set(FREETYPE_LIBRARIES freetype)
endif()
if(ANDROID)
	target_compile_definitions(jngl PRIVATE NOPNG)

	target_include_directories(jngl PUBLIC ${ANDROID_NDK}/sources/android/native_app_glue)
	target_include_directories(jngl PRIVATE ${OGG_INCLUDE_DIRS} ${VORBIS_SRC_DIR}/include)
else()
	if(NOT IOS)
		target_sources(jngl PRIVATE src/audio/sdl/engine.cpp)
	endif()
	if(UNIX AND NOT IOS AND NOT EMSCRIPTEN)
		find_package(PkgConfig REQUIRED)
		pkg_check_modules(SDL2 REQUIRED sdl2)
		target_include_directories(jngl PRIVATE ${SDL2_INCLUDE_DIRS})
		target_link_libraries(jngl PRIVATE ${SDL2_LINK_LIBRARIES})
	endif()

	if(NOT IOS AND NOT EMSCRIPTEN)
		find_package(Threads REQUIRED)
		target_link_libraries(jngl PUBLIC Threads::Threads)
	endif()

	if(NOT EMSCRIPTEN)
		find_package(PNG)
		if(PNG_FOUND)
			target_sources(jngl PRIVATE src/png/ImageDataPNG.cpp)
			target_link_libraries(jngl PUBLIC ${PNG_LIBRARIES})
		else()
			target_compile_definitions(jngl PRIVATE NOPNG)
		endif()
	else()
		target_compile_definitions(jngl PRIVATE NOPNG)
	endif()

	if(MSVC)
		target_compile_definitions(jngl PUBLIC _USE_MATH_DEFINES)
		if(JNGL_SDL2)
			CPMAddPackage(NAME sdl2
				URL https://www.libsdl.org/release/SDL2-2.28.4.zip
				URL_HASH SHA1=0d9e0da0e935c4f0524cfd16d47d38b6045b9573
				OPTIONS
					"SDL_SENSOR OFF" # doesn't work with UWP
			)
		else()
			CPMAddPackage(NAME sdl2
				URL https://www.libsdl.org/release/SDL2-2.28.4.zip
				URL_HASH SHA1=0d9e0da0e935c4f0524cfd16d47d38b6045b9573
				OPTIONS
					"SDL_ATOMIC OFF"
					"SDL_CPUINFO ON"
					"SDL_DLOPEN OFF"
					"SDL_FILE OFF"
					"SDL_FILESYSTEM OFF"
					"SDL_HAPTIC OFF"
					"SDL_LOADSO ON"
					"SDL_LOCALE OFF"
					"SDL_POWER OFF"
					"SDL_RENDER OFF"
					"SDL_SENSOR OFF"
					"SDL_SHARED OFF"
					"SDL_TIMERS OFF"
			)
		endif()
		target_link_libraries(jngl PRIVATE SDL2-static)
		if(WINDOWS_STORE)
			target_compile_definitions(jngl PUBLIC JNGL_UWP)
			target_include_directories(jngl PUBLIC ${sdl2_SOURCE_DIR}/include ${sdl2_BINARY_DIR}/include) # TODO: This should be PRIVATE
			target_link_options(jngl PUBLIC $<IF:$<CONFIG:Debug>,/defaultlib:vccorlibd.lib /defaultlib:msvcrtd.lib,/defaultlib:vccorlib.lib /defaultlib:msvcrt.lib>)
		endif()
	elseif(IOS)
		target_compile_definitions(jngl PRIVATE IOS)
		target_compile_definitions(jngl PUBLIC GLES_SILENCE_DEPRECATION)
		target_include_directories(jngl PUBLIC include/ios)
		find_library(IOS_OPENGLES OpenGLES)
		find_library(IOS_QUARTZCORE QuartzCore)
		find_library(IOS_UIKIT UIKit)
		find_library(IOS_AVFOUNDATION AVFoundation)
		find_library(IOS_GAMECONTROLLER GameController)
		find_library(IOS_AudioToolbox AudioToolbox)
		target_link_libraries(jngl PRIVATE
			${IOS_OPENGLES} ${IOS_QUARTZCORE} ${IOS_UIKIT} ${IOS_AVFOUNDATION}
			${IOS_GAMECONTROLLER} ${IOS_AudioToolbox}
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
		if(NOT JNGL_BUILD_WEBP_FROM_SOURCE)
			find_package(WebP)
			if(WebP_FOUND)
				target_link_libraries(jngl PRIVATE ${WEBP_LIBRARIES})
			else()
				target_compile_definitions(jngl PRIVATE NOWEBP)
			endif()
		endif()

		find_package(PkgConfig)
		if(PkgConfig_FOUND)
			pkg_check_modules(SDL2 REQUIRED sdl2)
			target_include_directories(jngl PRIVATE ${SDL2_INCLUDE_DIRS})
			target_link_libraries(jngl PRIVATE ${SDL2_LINK_LIBRARIES})
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
						target_sources(jngl PRIVATE src/theoraplay/theoraplay.cpp)
						target_include_directories(jngl PRIVATE ${THEORA_DEC_INCLUDE_DIRS})
						target_link_libraries(jngl PRIVATE ${VORBIS_LINK_LIBRARIES} ${THEORA_DEC_LINK_LIBRARIES})

						add_executable(videoplayer examples/videoplayer.cpp)
						target_link_libraries(videoplayer PRIVATE jngl)
						add_custom_command(TARGET videoplayer COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/data/verysmall.ogv "$<TARGET_FILE_DIR:videoplayer>")
					endif()
				endif()
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
	endif()
	target_link_directories(jngl PUBLIC ${VorbisFile_LIBRARY_DIRS})
	if(NOT EMSCRIPTEN AND NOT IOS)
		target_include_directories(jngl PRIVATE glad/include)
		target_sources(jngl PRIVATE glad/src/gl.c)
	endif()

	if(NOT hasParent)
		if(WINDOWS_STORE)
			# WIN32 is needed to build for the Windows subsystem, not Console
			add_executable(jngl-test WIN32 src/test/test.cpp src/uwp/SDL_winrt_main_NonXAML.cpp
				data/Arial.ttf data/jngl.webp data/jngl-icon.webp data/test.ogg)
			set_source_files_properties(data/Arial.ttf data/jngl.webp data/jngl-icon.webp
				data/test.ogg
				PROPERTIES VS_DEPLOYMENT_CONTENT 1)

			# Otherwise linking will fail with
			#   error LNK2038: mismatch detected for
			#   'vccorlib_lib_should_be_specified_before_msvcrt_lib_to_linker': value '1' doesn't
			#   match value '0' in msvcrtd.lib(app_appinit.obj)
			set_source_files_properties(src/uwp/SDL_winrt_main_NonXAML.cpp
				PROPERTIES COMPILE_FLAGS "/ZW /Zc:twoPhase- /std:c++17")
		else()
			if(NOT IOS)
				add_executable(jngl-test src/test/test.cpp)
				if(WIN32)
					target_sources(jngl-test PRIVATE src/win32/jngl.manifest)
				endif()

				add_executable(example-bike examples/bike/base.cpp examples/bike/bike.cpp
					examples/bike/line.cpp examples/bike/main.cpp examples/bike/wheel.cpp)
				target_link_libraries(example-bike jngl)
				add_executable(example-throw examples/throw/main.cpp)
				target_link_libraries(example-throw jngl)
				add_executable(example-shader examples/shader/main.cpp)
				target_link_libraries(example-shader jngl)

				add_executable(audioplayer examples/audioplayer.cpp)
				target_link_libraries(audioplayer PRIVATE jngl)
				add_custom_command(TARGET audioplayer COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/data/test.ogg "$<TARGET_FILE_DIR:audioplayer>")
			endif()
		endif()
		target_link_libraries(jngl-test jngl)
	endif()
endif()

if(JNGL_BUILD_WEBP_FROM_SOURCE)
	CPMAddPackage(NAME webp
		URL https://storage.googleapis.com/downloads.webmproject.org/releases/webp/libwebp-1.2.4.tar.gz
		URL_HASH SHA1=24586e2a8e01a9cced102d68852661b964502429
		OPTIONS
			"WEBP_BUILD_ANIM_UTILS OFF"
			"WEBP_BUILD_CWEBP OFF"
			"WEBP_BUILD_DWEBP OFF"
			"WEBP_BUILD_GIF2WEBP OFF"
			"WEBP_BUILD_IMG2WEBP OFF"
			"WEBP_BUILD_VWEBP OFF"
			"WEBP_BUILD_WEBPINFO OFF"
			"WEBP_BUILD_WEBPMUX OFF"
			"WEBP_BUILD_EXTRAS OFF"
	)
	target_include_directories(jngl PRIVATE ${webp_SOURCE_DIR}/src)
	target_link_libraries(jngl PRIVATE webpdecode webpdspdecode webputilsdecode)
endif()

if(ANDROID OR IOS OR APPLE OR WINDOWS_STORE)
	if(JNGL_VIDEO)
		CPMAddPackage(NAME theora
			URL http://downloads.xiph.org/releases/theora/libtheora-1.1.1.tar.bz2
			URL_HASH SHA1=8dcaa8e61cd86eb1244467c0b64b9ddac04ae262
			DOWNLOAD_ONLY)
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
		target_sources(jngl PRIVATE src/theoraplay/theoraplay.cpp)
	endif()
endif()

target_include_directories(jngl PRIVATE ${FREETYPE_INCLUDE_DIRS})
target_link_directories(jngl PUBLIC ${FREETYPE_LIBRARY_DIRS})
target_link_libraries(jngl PUBLIC ${FREETYPE_LIBRARIES})

if(NOT hasParent AND NOT WINDOWS_STORE)
	include(CTest)
	if(BUILD_TESTING)
		file(GLOB UNITTEST_SRC src/unittest/*.cpp)
		add_executable(jngl-unittest ${UNITTEST_SRC})
		target_compile_features(jngl-unittest PRIVATE cxx_std_20)
		target_link_libraries(jngl-unittest PRIVATE jngl)
		target_compile_definitions(jngl-unittest PRIVATE BOOST_UT_DISABLE_MODULE)
		add_test(NAME JNGLUnitTest COMMAND jngl-unittest)
	endif()
endif()
