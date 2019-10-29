// Copyright 2015 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "dll.hpp"

#include <string>

namespace jngl {

void JNGLDLL_API errorMessage(const std::string& text);

void JNGLDLL_API printMessage(const std::string& text);

} // namespace jngl
