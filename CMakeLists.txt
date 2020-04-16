cmake_minimum_required(VERSION 3.14)

project(jngl LANGUAGES C CXX)

if(NOT MSVC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror=implicit-function-declaration")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

file(GLOB SRC src/*.cpp src/jngl/*.cpp)
add_library(jngl ${SRC})

target_compile_features(jngl PUBLIC cxx_std_17)

set_target_properties(jngl PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/src/
)

include(FetchContent)
if(IOS)
	set(Boost_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include/ios")
endif()
find_package(Boost)
if(Boost_FOUND)
	target_link_libraries(jngl PUBLIC Boost::headers)
else()
	FetchContent_Declare(boost
		URL https://dl.bintray.com/boostorg/release/1.71.0/source/boost_1_71_0.7z
		URL_HASH SHA256=1b3949ffddf56129141032542523920034f2e69775fb9332a52ba37c548dbf35)
	FetchContent_MakeAvailable(boost)
	FetchContent_GetProperties(boost SOURCE_DIR BOOST_SRC_DIR)
	target_include_directories(jngl PUBLIC ${BOOST_SRC_DIR})
endif()

option(JNGL_VIDEO "Enable Theora video playback" ON)
option(JNGL_JPEG "Enable JPEG support if libjpeg is available" ON)

set(BUILD_TESTING OFF CACHE INTERNAL "") # for libogg

if (ANDROID)
	file(GLOB ANDROID_SOURCES src/android/*.cpp)
	target_sources(jngl PRIVATE ${ANDROID_SOURCES})
elseif(IOS)
	file(GLOB SRC src/ios/*.cpp src/ios/*.mm src/ios/*.h)
	target_sources(jngl PRIVATE ${SRC} src/mac/message.cpp)
elseif (UNIX)
	target_sources(jngl PRIVATE
        src/sdl/input.cpp
        src/sdl/sdl.cpp
        src/sdl/window.cpp
        src/sdl/SdlController.cpp
	)
	if(APPLE)
		file(GLOB SRC src/mac/*.cpp src/mac/*.mm)
		enable_language(OBJCXX)
		target_sources(jngl PRIVATE ${SRC})
		find_library(MAC_FOUNDATION Foundation)
		find_library(MAC_CORETEXT CoreText)
		target_link_libraries(jngl PRIVATE ${MAC_FOUNDATION} ${MAC_CORETEXT})
	else()
		file(GLOB SRC src/linux/*.cpp src/linux/*.c)
		target_sources(jngl PRIVATE ${SRC})
    endif()
else() # Windows
	option(JNGL_SDL2 "Use SDL2 instead of WinAPI" OFF)

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
	if(MSVC)
		target_compile_definitions(jngl PRIVATE _CRT_SECURE_NO_WARNINGS)
		if(CMAKE_SIZEOF_VOID_P EQUAL 8)
			set(JNGL_ARCH "x86_64")
		else()
			set(JNGL_ARCH "x86")
		endif()
		target_link_directories(jngl PUBLIC ${PROJECT_SOURCE_DIR}/lib/win/${JNGL_ARCH})
		target_link_libraries(jngl PUBLIC Xinput)
	else()
		target_link_libraries(jngl PUBLIC xinput1_4)
	endif()
endif()

# fontconfig
if (UNIX)
	if(NOT APPLE AND NOT ANDROID)
        find_package(Fontconfig REQUIRED)
		target_include_directories(jngl PUBLIC ${FONTCONFIG_INCLUDE_DIR})
		target_link_libraries(jngl PRIVATE ${FONTCONFIG_LIBRARIES})
    endif()
endif()

if(ANDROID OR MSVC)
	FetchContent_Declare(ogg
		URL http://downloads.xiph.org/releases/ogg/libogg-1.3.4.tar.xz
		URL_HASH SHA256=c163bc12bc300c401b6aa35907ac682671ea376f13ae0969a220f7ddf71893fe)
	FetchContent_MakeAvailable(ogg)
	set(OGG_INCLUDE_DIRS "${ogg_SOURCE_DIR}/include" "${ogg_BINARY_DIR}/include")
	set(OGG_LIBRARIES ogg)

	FetchContent_Declare(vorbis
		URL https://github.com/xiph/vorbis/archive/v1.3.6.zip
		URL_HASH SHA1=694c8b3739f66ef38c08092d0df2ef42197fc480)
	FetchContent_MakeAvailable(vorbis)
	target_include_directories(jngl PRIVATE ${vorbis_SOURCE_DIR}/include)
	target_link_libraries(jngl PUBLIC vorbis vorbisfile ogg)
endif()

get_directory_property(hasParent PARENT_DIRECTORY)
if(ANDROID)
	target_compile_definitions(jngl PRIVATE NOJPEG NOPNG OPENGLES)

	set(WEBP_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android/libwebp)
	if ((NOT EXISTS ${WEBP_SRC_DIR}) OR (NOT EXISTS ${WEBP_SRC_DIR}/CMakeLists.txt))
		execute_process(COMMAND git clone --depth=1 -b 1.1.0
		                https://chromium.googlesource.com/webm/libwebp
		                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/android/)
	endif()
	add_subdirectory(${WEBP_SRC_DIR} ${CMAKE_CURRENT_BINARY_DIR}/libwebp)
	target_include_directories(jngl PRIVATE ${WEBP_SRC_DIR}/src)
	target_link_libraries(jngl PUBLIC webp)

	set(ALSOFT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android/openal-soft)
	if ((NOT EXISTS ${ALSOFT_SRC_DIR}) OR (NOT EXISTS ${ALSOFT_SRC_DIR}/CMakeLists.txt))
		execute_process(COMMAND git clone --depth=1 -b v1.18
		                https://github.com/kcat/openal-soft
		                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/android/)
	endif()
	set(ALSOFT_UTILS OFF CACHE BOOL "Build and install utility programs" FORCE)
	set(ALSOFT_EXAMPLES OFF CACHE BOOL "Build and install example programs" FORCE)
	set(ALSOFT_TESTS OFF CACHE BOOL "Build and install test programs" FORCE)
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
	if(UNIX AND NOT IOS)
		find_package(SDL2 REQUIRED)
		if(SDL2_LIBRARIES)
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

	if(NOT IOS)
		find_package(Threads REQUIRED)
		target_link_libraries(jngl PUBLIC Threads::Threads)
	endif()

	# PNG
	find_package(PNG)
	if(PNG_FOUND)
		target_link_libraries(jngl PUBLIC ${PNG_LIBRARIES})
	else()
		target_compile_definitions(jngl PRIVATE NOPNG)
	endif()

	if(MSVC)
		FetchContent_Declare(freetype
			URL https://download.savannah.gnu.org/releases/freetype/freetype-2.10.1.tar.xz
			URL_HASH SHA1=79874ef4eaa52025126b71d836453b8279bdd331)
		FetchContent_MakeAvailable(freetype)
		FetchContent_GetProperties(freetype SOURCE_DIR FREETYPE_SRC_DIR)
		if(hasParent)
		set(FREETYPE_INCLUDE_DIRS ${FREETYPE_SRC_DIR}/include PARENT_SCOPE)
		set(FREETYPE_LIBRARY freetype PARENT_SCOPE)
		endif()
		set(FREETYPE_LIBRARIES freetype)

		FetchContent_Declare(openal
			URL https://github.com/kcat/openal-soft/archive/openal-soft-1.20.1.zip
			URL_HASH SHA1=d86a714e0e7900f4f7197738cb35432772bfe2e0)
		set(ALSOFT_NO_CONFIG_UTIL ON CACHE BOOL "Disable building the alsoft-config utility" FORCE)
		set(ALSOFT_UTILS OFF CACHE BOOL "Build and install utility programs" FORCE)
		set(ALSOFT_EXAMPLES OFF CACHE BOOL "Build and install example programs" FORCE)
		set(ALSOFT_TESTS OFF CACHE BOOL "Build and install test programs" FORCE)
		FetchContent_MakeAvailable(openal)
		FetchContent_GetProperties(openal SOURCE_DIR OPENAL_SRC_DIR)
		target_include_directories(jngl PRIVATE ${OPENAL_SRC_DIR}/include)
		set(OPENAL_LIBRARY OpenAL)

		FetchContent_Declare(dlfcn
			URL https://github.com/dlfcn-win32/dlfcn-win32/archive/v1.2.0.zip
			URL_HASH SHA1=83c37a7d7d152c951e9b8cb673aec47d1776c881)
		FetchContent_MakeAvailable(dlfcn)
		FetchContent_GetProperties(dlfcn SOURCE_DIR DLFCN_SRC_DIR)
		target_include_directories(jngl PRIVATE ${DLFCN_SRC_DIR})
		target_link_libraries(jngl PRIVATE dl)

		target_include_directories(jngl PRIVATE ${PROJECT_SOURCE_DIR}/include/msvc)
		target_link_directories(jngl PUBLIC ${PROJECT_SOURCE_DIR}/lib/msvc/${JNGL_ARCH})
		add_custom_command(TARGET jngl COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/lib/msvc/${JNGL_ARCH}/epoxy-0.dll "$<TARGET_FILE_DIR:jngl>")
		target_link_libraries(jngl PUBLIC epoxy)

		FetchContent_Declare(webp
			GIT_REPOSITORY https://chromium.googlesource.com/webm/libwebp
			GIT_TAG 3d97bb75144147e47db278ec76e5e70c6b2243db)
		FetchContent_MakeAvailable(webp)
		target_link_libraries(jngl PRIVATE webp)
	elseif(IOS)
		include(FetchContent)
		FetchContent_Declare(ogg
			GIT_REPOSITORY https://github.com/xiph/ogg
			GIT_TAG c8fca6b4a02d695b1ceea39b330d4406001c03ed)
		FetchContent_MakeAvailable(ogg)
		FetchContent_GetProperties(ogg SOURCE_DIR OGG_SOURCE_DIR)
		set(OGG_INCLUDE_DIRS ${OGG_SOURCE_DIR}/include)
		set(OGG_LIBRARIES ogg)
		FetchContent_Declare(vorbis
			URL https://github.com/xiph/vorbis/archive/v1.3.6.zip
			URL_HASH SHA1=694c8b3739f66ef38c08092d0df2ef42197fc480)
		FetchContent_MakeAvailable(vorbis)
		FetchContent_GetProperties(vorbis SOURCE_DIR VORBIS_SRC_DIR)
		target_include_directories(jngl PRIVATE ${VORBIS_SRC_DIR}/include)
		target_link_libraries(jngl PUBLIC vorbis vorbisfile ogg)
		target_compile_definitions(jngl PRIVATE IOS)
		target_compile_definitions(jngl PUBLIC GLES_SILENCE_DEPRECATION)
		target_include_directories(jngl PUBLIC include/ios)
		find_library(IOS_OPENGLES OpenGLES)
		find_library(IOS_OPENAL OpenAL)
		find_library(IOS_QUARTZCORE QuartzCore)
		find_library(IOS_UIKIT UIKit)
		find_library(IOS_AVFOUNDATION AVFoundation)
		target_link_libraries(jngl PRIVATE
			"${PROJECT_SOURCE_DIR}/lib/ios/libfreetype.a"
			"${PROJECT_SOURCE_DIR}/lib/ios/WebPDecoder.framework"
			${IOS_OPENGLES} ${IOS_OPENAL} ${IOS_QUARTZCORE} ${IOS_UIKIT} ${IOS_AVFOUNDATION}
		)
		if(hasParent)
			set(FREETYPE_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include/ios PARENT_SCOPE)
			set(FREETYPE_LIBRARY "${PROJECT_SOURCE_DIR}/lib/ios/libfreetype.a" PARENT_SCOPE)
		endif()

		set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.bixense.jngl-test")
		set(MACOSX_BUNDLE_BUNDLE_VERSION "1.0")
		set(MACOSX_BUNDLE_LONG_VERSION_STRING "1.0")
		set(MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0")
		set_source_files_properties(Arial.ttf PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
		set_source_files_properties(jngl.webp PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
		set_source_files_properties(test.ogg PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
		add_executable(jngl-test "xcode/Test iOS/AppDelegate.mm" "xcode/Test iOS/main.mm" Arial.ttf jngl.webp test.ogg)
		set_target_properties(jngl-test PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${PROJECT_SOURCE_DIR}/data/ios/jngl-test.in.plist")
		target_include_directories(jngl-test PRIVATE include/ios src/ios)
	else()
		find_package(VorbisFile REQUIRED)
		target_include_directories(jngl PUBLIC ${VorbisFile_INCLUDE_DIRS})
		target_link_libraries(jngl PUBLIC ${VorbisFile_LIBRARIES})

		find_package(Freetype REQUIRED)
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
				pkg_check_modules(THEORA_DEC theoradec)
				if(THEORA_DEC_FOUND)
					pkg_check_modules(VORBIS vorbis)
					if(VORBIS_FOUND)
						target_compile_definitions(jngl PRIVATE JNGL_VIDEO)
						target_sources(jngl PRIVATE subprojects/theoraplay/theoraplay.cpp)
						target_include_directories(jngl PRIVATE ${THEORA_DEC_INCLUDE_DIRS})
						target_link_libraries(jngl PRIVATE ${VORBIS_LINK_LIBRARIES} ${THEORA_DEC_LINK_LIBRARIES})

						add_executable(videoplayer examples/videoplayer.cpp)
						target_link_libraries(videoplayer PRIVATE jngl)
					endif()
				endif()
			endif()
			if(NOT APPLE)
				pkg_check_modules(EPOXY REQUIRED epoxy)
				target_link_libraries(jngl PUBLIC ${EPOXY_LINK_LIBRARIES})
			endif()
		endif()
		if(NOT PkgConfig_FOUND OR APPLE)
			find_package(epoxy REQUIRED)
			target_include_directories(jngl PRIVATE ${epoxy_INCLUDE_DIR})
			target_link_libraries(jngl PUBLIC ${epoxy_LIBRARY})
		endif()
	endif()

	if(NOT IOS)
		add_executable(jngl-test src/test/test.cpp)
	endif()
	target_link_libraries(jngl-test jngl)
	add_custom_command(TARGET jngl-test COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/jngl.webp "$<TARGET_FILE_DIR:jngl-test>")
endif()

if(ANDROID OR IOS)
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
		target_compile_options(theoradec PRIVATE -Wno-shift-negative-value
			-Wno-shift-op-parentheses)
		target_link_libraries(jngl PRIVATE theoradec)
		target_compile_definitions(jngl PRIVATE JNGL_VIDEO)
		target_include_directories(jngl PRIVATE "${theora_SOURCE_DIR}/include")
		target_sources(jngl PRIVATE subprojects/theoraplay/theoraplay.cpp)
	endif()
endif()

target_include_directories(jngl PRIVATE ${FREETYPE_INCLUDE_DIRS})
target_link_libraries(jngl PUBLIC ${FREETYPE_LIBRARIES} ${OPENAL_LIBRARY})
