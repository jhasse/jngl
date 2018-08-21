cmake_minimum_required (VERSION 3.6 FATAL_ERROR)

project(jngl LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_library(jngl
	src/audio.cpp
	src/framebufferimpl.cpp
	src/freetype.cpp
	src/helper.cpp
	src/jngl/color.cpp
	src/jngl/Controller.cpp
	src/jngl/drawable.cpp
	src/jngl/Finally.cpp
	src/jngl/font.cpp
	src/jngl/framebuffer.cpp
	src/jngl/input.cpp
	src/jngl/job.cpp
	src/jngl/screen.cpp
	src/jngl/shapes.cpp
	src/jngl/sprite.cpp
	src/jngl/text.cpp
	src/jngl/time.cpp
	src/jngl/Vec2.cpp
	src/jngl/window.cpp
	src/jngl/work.cpp
	src/main.cpp
	src/opengl.cpp
	src/spriteimpl.cpp
	src/texture.cpp
	src/window.cpp
	src/windowptr.cpp
    src/jngl.hpp)

set_target_properties(jngl PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/src/
)

if (ANDROID)
	target_include_directories(jngl PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/android/include)
	file(GLOB ANDROID_SOURCES src/android/*.cpp)
	target_sources(jngl PRIVATE ${ANDROID_SOURCES})
elseif (UNIX)
	target_sources(jngl PRIVATE
        src/sdl/input.cpp
        src/sdl/sdl.cpp
        src/sdl/window.cpp
        src/sdl/SdlController.cpp
    	src/linux/message.cpp)
    if(NOT APPLE)
		target_sources(jngl PRIVATE src/linux/window.cpp)
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

if(ANDROID)
	target_compile_definitions(jngl PRIVATE NOJPEG NOPNG OPENGLES)

	set(WEBP_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android/libwebp)
	if ((NOT EXISTS ${WEBP_SRC_DIR}) OR (NOT EXISTS ${WEBP_SRC_DIR}/CMakeLists.txt))
		execute_process(COMMAND git clone --depth=1 -b 1.0.0
		                https://chromium.googlesource.com/webm/libwebp
		                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/android/)
	endif()
	add_subdirectory(${WEBP_SRC_DIR} ${CMAKE_CURRENT_BINARY_DIR}/libwebp)
	set(WEBP_INCLUDE_DIRS ${WEBP_SRC_DIR}/src)
	set(WEBP_LIBRARIES webp)

	set(ALSOFT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android/openal-soft)
	if ((NOT EXISTS ${ALSOFT_SRC_DIR}) OR (NOT EXISTS ${ALSOFT_SRC_DIR}/CMakeLists.txt))
		execute_process(COMMAND git clone --depth=1 -b v1.18
		                https://github.com/kcat/openal-soft
		                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/android/)
	endif()
	set(ALSOFT_BACKEND_WAVE OFF CACHE BOOL "Enable Wave Writer backend" FORCE)
	add_subdirectory(${ALSOFT_SRC_DIR} ${CMAKE_CURRENT_BINARY_DIR}/openal-soft)
	set(OPENAL_LIBRARY OpenAL)

	set(FREETYPE_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android/freetype2)
	if ((NOT EXISTS ${FREETYPE_SRC_DIR}) OR (NOT EXISTS ${FREETYPE_SRC_DIR}/CMakeLists.txt))
		execute_process(COMMAND git clone --depth=1 -b VER-2-9-1
		                https://git.savannah.gnu.org/git/freetype/freetype2.git
		                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/android/)
	endif()
	add_subdirectory(${FREETYPE_SRC_DIR} ${CMAKE_CURRENT_BINARY_DIR}/freetype2)
	set(FREETYPE_INCLUDE_DIRS ${FREETYPE_SRC_DIR}/include)
	set(FREETYPE_LIBRARIES freetype)

	set(OGG_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android/ogg)
	if ((NOT EXISTS ${OGG_SRC_DIR}) OR (NOT EXISTS ${OGG_SRC_DIR}/CMakeLists.txt))
		execute_process(COMMAND git clone -b v1.3.3
		                https://git.xiph.org/ogg.git
		                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/android/)
	endif()
	add_subdirectory(${OGG_SRC_DIR} ${CMAKE_CURRENT_BINARY_DIR}/ogg)
	set(OGG_INCLUDE_DIRS ${OGG_SRC_DIR}/include ${CMAKE_CURRENT_BINARY_DIR}/ogg/include)
	set(OGG_LIBRARIES ogg)

	set(VORBIS_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android/vorbis)
	if ((NOT EXISTS ${VORBIS_SRC_DIR}) OR (NOT EXISTS ${VORBIS_SRC_DIR}/CMakeLists.txt))
		execute_process(COMMAND git clone -b v1.3.6
		                https://git.xiph.org/vorbis.git
		                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/android/)
	endif()
	add_subdirectory(${VORBIS_SRC_DIR} ${CMAKE_CURRENT_BINARY_DIR}/vorbis)
	target_link_libraries(jngl PUBLIC vorbisfile)

	target_include_directories(jngl PRIVATE ${ANDROID_NDK}/sources/android/native_app_glue
	                           ${OGG_INCLUDE_DIRS} ${VORBIS_SRC_DIR}/include)
else()
	# SDL2
	find_package(SDL2 REQUIRED)
	include_directories(${SDL2_INCLUDE_DIRS})
	target_link_libraries(jngl PUBLIC ${SDL2_LIBRARIES})

	# libjpeg
	find_package(JPEG REQUIRED)
	target_include_directories(jngl PUBLIC ${JPEG_INCLUDE_DIR})
	target_link_libraries(jngl PRIVATE ${JPEG_LIBRARIES})

	# Threads
	find_package(Threads REQUIRED)
	target_link_libraries(jngl PUBLIC Threads::Threads)

	# PNG
	find_package(PNG REQUIRED)
	target_link_libraries(jngl PUBLIC ${PNG_LIBRARIES})

	# epoxy
	find_package(LibEpoxy REQUIRED)
	target_include_directories(jngl PRIVATE ${epoxy_INCLUDE_DIR})
	target_link_libraries(jngl PUBLIC ${epoxy_LIBRARY})

	# VorbisFile
	find_package(VorbisFile REQUIRED)
	target_include_directories(jngl PUBLIC ${VorbisFile_INCLUDE_DIRS})
	target_link_libraries(jngl PUBLIC ${VorbisFile_LIBRARIES})

	find_package(Freetype REQUIRED)
	find_package(OpenAL REQUIRED)
	find_package(WebP REQUIRED)
endif()

target_include_directories(jngl PRIVATE ${FREETYPE_INCLUDE_DIRS} ${WEBP_INCLUDE_DIRS})
target_link_libraries(jngl PUBLIC ${FREETYPE_LIBRARIES} ${OPENAL_LIBRARY} ${WEBP_LIBRARIES})
