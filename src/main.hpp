// Copyright 2012-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "jngl/Finally.hpp"

#include <memory>
#include <string>
#include <vector>

namespace jngl {

class ShaderProgram;

void clearBackgroundColor();

bool Init(int width, int height, int canvasWidth, int canvasHeight);

extern std::string pathPrefix;
extern std::string configPath;
extern std::vector<std::string> args;
extern std::unique_ptr<jngl::ShaderProgram> simpleShaderProgram;
extern int simpleModelviewUniform;
extern int simpleColorUniform;

Finally useSimpleShaderProgram();

} // namespace jngl
