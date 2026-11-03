// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// The tests' ShaderPrograms: Sprite::vertexShader() or data/unittest/*.vert with one of
/// data/unittest/*.frag
/// @file
#pragma once

#include <jngl/ShaderProgram.hpp>

#ifdef JNGL_D3D12
// Direct3D 12 can't compile them at runtime, see jngl_compile_shaders() in CMakeLists.txt
#include "TestShaderBytecode.hpp" // IWYU pragma: export
#define JNGL_TEST_SHADER_PROGRAM(name) jngl::ShaderProgram(test_shaders::name)
#define JNGL_TEST_SHADER_PROGRAM_WITH_VERTEX(name) jngl::ShaderProgram(test_shaders::name)
#else
/// Compiles data/unittest/<name>.frag, and data/unittest/<name>.vert if \a vertexShader is true
jngl::ShaderProgram testShaderProgram(const char* name, bool vertexShader = false);
#define JNGL_TEST_SHADER_PROGRAM(name) testShaderProgram(#name)
#define JNGL_TEST_SHADER_PROGRAM_WITH_VERTEX(name) testShaderProgram(#name, true)
#endif
