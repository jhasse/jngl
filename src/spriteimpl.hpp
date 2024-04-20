// Copyright 2009-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "jngl/Rgba.hpp"
#include "jngl/sprite.hpp"

namespace jngl {

Finally loadSprite(const std::string&);
Sprite& GetSprite(std::string_view filename, Sprite::LoadType loadType = Sprite::LoadType::NORMAL);

extern unsigned char spriteColorRed, spriteColorGreen, spriteColorBlue, spriteColorAlpha;
extern Rgba gShapeColor;

} // namespace jngl
