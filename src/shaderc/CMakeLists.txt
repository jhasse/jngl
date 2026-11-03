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
