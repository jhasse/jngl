// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// The tests' ShaderPrograms: Sprite::vertexShader() with one of data/unittest/*.frag
/// @file
#pragma once

#include <jngl/ShaderProgram.hpp>

#ifdef JNGL_D3D12
// Direct3D 12 can't compile them at runtime, see jngl_compile_shaders() in CMakeLists.txt
#include "TestShaderBytecode.hpp" // IWYU pragma: export
#define JNGL_TEST_SHADER_PROGRAM(name) jngl::ShaderProgram(test_shaders::name)
#else
/// Compiles data/unittest/<name>.frag
jngl::ShaderProgram testShaderProgram(const char* name);
#define JNGL_TEST_SHADER_PROGRAM(name) testShaderProgram(#name)
#endif
