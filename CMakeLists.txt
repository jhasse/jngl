cmake_minimum_required (VERSION 3.6 FATAL_ERROR)

project(jngl LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

file(GLOB SRC src/*.cpp src/jngl/*.cpp)
add_library(jngl ${SRC})

if (ANDROID)
	set(CMAKE_CXX_STANDARD 14)
else()
	target_compile_features(jngl PUBLIC cxx_std_17)
endif()

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
	)
	if(APPLE)
		file(GLOB SRC src/mac/*.cpp)
		target_sources(jngl PRIVATE ${SRC})
	else()
		file(GLOB SRC src/linux/*.cpp src/linux/*.c)
		target_sources(jngl PRIVATE ${SRC})
    endif()
else() # Windows
	target_compile_definitions(jngl PRIVATE WINVER=0x0602 _USE_MATH_DEFINES)
	file(GLOB SRC src/win32/*.cpp)
	target_sources(jngl PRIVATE ${SRC})
	target_link_libraries(jngl PUBLIC xinput1_4 winmm)
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
	if(UNIX)
		find_package(SDL2 REQUIRED)
		include_directories(${SDL2_INCLUDE_DIRS})
		target_link_libraries(jngl PUBLIC ${SDL2_LIBRARIES})
	endif()

	find_package(JPEG)
	if(JPEG_FOUND)
		target_include_directories(jngl PUBLIC ${JPEG_INCLUDE_DIR})
		target_link_libraries(jngl PRIVATE ${JPEG_LIBRARIES})
	else()
		target_compile_definitions(jngl PRIVATE NOJPEG)
	endif()

	# Threads
	find_package(Threads REQUIRED)
	target_link_libraries(jngl PUBLIC Threads::Threads)

	# PNG
	find_package(PNG REQUIRED)
	target_link_libraries(jngl PUBLIC ${PNG_LIBRARIES})

	# VorbisFile
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
	if(PkgConfig_FOUND AND NOT APPLE)
		pkg_check_modules(THEORA_DEC theoradec)
		if(THEORA_DEC_FOUND)
			pkg_check_modules(VORBIS vorbis)
			if(VORBIS_FOUND)
				target_compile_definitions(jngl PRIVATE JNGL_VIDEO)
				target_sources(jngl PRIVATE subprojects/theoraplay/theoraplay.cpp)
				target_include_directories(jngl PRIVATE ${THEORA_DEC_INCLUDE_DIRS})
				target_link_libraries(jngl PRIVATE ${VORBIS_LIBRARIES} ${THEORA_DEC_LIBRARIES})

				add_executable(videoplayer examples/videoplayer.cpp)
				target_link_libraries(videoplayer PRIVATE jngl)
			endif()
		endif()
		pkg_check_modules(EPOXY REQUIRED epoxy)
		target_link_libraries(jngl PUBLIC ${EPOXY_LIBRARIES})
	else()
		find_package(LibEpoxy REQUIRED)
		target_include_directories(jngl PRIVATE ${epoxy_INCLUDE_DIR})
		target_link_libraries(jngl PUBLIC ${epoxy_LIBRARY})
	endif()
endif()

target_include_directories(jngl PRIVATE ${FREETYPE_INCLUDE_DIRS})
target_link_libraries(jngl PUBLIC ${FREETYPE_LIBRARIES} ${OPENAL_LIBRARY})
