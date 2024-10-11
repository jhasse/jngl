// Copyright 2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#pragma once

#include <functional>

namespace jngl {

extern std::function<void()> g_jnglMainLoop;

void jnglMainLoop();

} // namespace jngl
