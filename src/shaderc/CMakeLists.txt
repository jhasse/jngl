# jngl-shaderc, see main.cpp. Part of JNGL's build, or built on its own for the machine that's
# building when JNGL is cross-compiled (e.g. for Android), as it runs while building.
cmake_minimum_required(VERSION 3.16)
project(jngl-shaderc LANGUAGES CXX)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/shaderc.cmake)

add_executable(jngl-shaderc main.cpp ../spirv/ShaderCompiler.cpp)
target_compile_features(jngl-shaderc PRIVATE cxx_std_20)
target_include_directories(jngl-shaderc PRIVATE ${SHADERC_INCLUDE_DIR})
target_link_libraries(jngl-shaderc PRIVATE ${SHADERC_LIBRARY})
target_compile_definitions(jngl-shaderc PRIVATE ${JNGL_SHADERC_DEFINITIONS})

# MSL for the Metal renderer, see ../metal/MslCompiler.hpp. SPIRV-Cross translates the SPIR-V to it,
# only its core and its MSL backend, with its C API like the Vulkan SDK's that the Direct3D 12
# backend uses, built from their sources.
option(JNGL_SHADERC_MSL "Support translating the shaders to MSL for the Metal renderer" OFF)
if(JNGL_SHADERC_MSL)
	include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/CPM.cmake)
	CPMAddPackage(
		NAME spirv_cross
		URL https://github.com/KhronosGroup/SPIRV-Cross/archive/refs/tags/vulkan-sdk-1.4.357.0.tar.gz
		URL_HASH SHA1=05389fa399187083dd6fb1f5935a9f6de4ab9567
		DOWNLOAD_ONLY YES
	)
	add_library(jngl-spirv-cross STATIC
		${spirv_cross_SOURCE_DIR}/spirv_cfg.cpp
		${spirv_cross_SOURCE_DIR}/spirv_cross.cpp
		${spirv_cross_SOURCE_DIR}/spirv_cross_c.cpp
		${spirv_cross_SOURCE_DIR}/spirv_cross_parsed_ir.cpp
		${spirv_cross_SOURCE_DIR}/spirv_glsl.cpp
		${spirv_cross_SOURCE_DIR}/spirv_msl.cpp
		${spirv_cross_SOURCE_DIR}/spirv_parser.cpp)
	target_include_directories(jngl-spirv-cross PUBLIC ${spirv_cross_SOURCE_DIR})
	target_compile_definitions(jngl-spirv-cross PUBLIC SPIRV_CROSS_C_API_GLSL=1
	                                                   SPIRV_CROSS_C_API_MSL=1)
	target_compile_features(jngl-spirv-cross PRIVATE cxx_std_11)
	target_sources(jngl-shaderc PRIVATE ../metal/MslCompiler.cpp)
	target_link_libraries(jngl-shaderc PRIVATE jngl-spirv-cross)
	target_compile_definitions(jngl-shaderc PRIVATE JNGL_SHADERC_MSL)
endif()
