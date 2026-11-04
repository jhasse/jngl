# Finds shaderc, which compiles GLSL to SPIR-V: SHADERC_INCLUDE_DIR, SHADERC_LIBRARY and
# JNGL_SHADERC_DEFINITIONS for what links it. It's in the Vulkan SDK, which is also what's needed
# for building the Direct3D 12 backend, see JNGL_VULKAN_SDK_LIBDIR.

if(WIN32)
	set(JNGL_VULKAN_SDK_LIBDIR "$ENV{VULKAN_SDK}/Lib")
	if(CMAKE_SIZEOF_VOID_P EQUAL 4)
		set(JNGL_VULKAN_SDK_LIBDIR "$ENV{VULKAN_SDK}/Lib32")
	endif()
endif()

find_path(SHADERC_INCLUDE_DIR shaderc/shaderc.hpp
	HINTS "$ENV{VULKAN_SDK}/Include" "$ENV{VULKAN_SDK}/include")
if(WIN32)
	# The SDK's static shaderc uses its own CRT settings. The C API in its DLL works with
	# both Debug and Release builds, including applications using a static CRT.
	find_library(SHADERC_LIBRARY NAMES shaderc_shared HINTS "${JNGL_VULKAN_SDK_LIBDIR}")
	set(JNGL_SHADERC_DEFINITIONS SHADERC_SHAREDLIB)
else()
	# The shared library before Ubuntu's libshaderc_combined.a, which lacks glslang
	find_library(SHADERC_LIBRARY NAMES shaderc_shared shaderc shaderc_combined
		HINTS "$ENV{VULKAN_SDK}/lib")
	set(JNGL_SHADERC_DEFINITIONS "")
endif()
if(NOT SHADERC_INCLUDE_DIR OR NOT SHADERC_LIBRARY)
	message(FATAL_ERROR "JNGL_VULKAN, JNGL_D3D12 and JNGL_METAL require shaderc (install the "
		"Vulkan SDK, shaderc via Homebrew on macOS, or libshaderc-devel / libshaderc-dev on "
		"Linux). "
		"include=${SHADERC_INCLUDE_DIR} lib=${SHADERC_LIBRARY}")
endif()
