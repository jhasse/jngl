// Copyright 2012-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "jngl/Rgba.hpp"
#include "jngl/ShaderProgram.hpp"

#include <memory>
#include <string>
#include <vector>
#if defined(__has_include) && __has_include(<optional>)
#include <optional>
using std::optional;
#else
#include <experimental/optional>
using std::experimental::optional;
#endif

namespace jngl {

class Mat3;
class ShaderProgram;

void clearBackgroundColor();

bool Init(int width, int height, int canvasWidth, int canvasHeight);
void updateViewportAndLetterboxing(int width, int height, int canvasWidth, int canvasHeight);
void updateProjection(int windowWidth, int windowHeight, int, int);

extern std::string pathPrefix;
extern optional<std::string> configPath;
extern std::vector<std::string> args;
extern std::unique_ptr<jngl::ShaderProgram> simpleShaderProgram;
extern int simpleModelviewUniform;
extern int simpleColorUniform;

ShaderProgram::Context useSimpleShaderProgram();
ShaderProgram::Context useSimpleShaderProgram(const Mat3& modelview, Rgba color);

} // namespace jngl
