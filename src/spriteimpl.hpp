// Copyright 2009-2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#pragma once

#include "jngl/Rgba.hpp"
#include "jngl/Sprite.hpp"

namespace jngl {

Finally loadSprite(const std::string&);
Sprite& GetSprite(const std::string& filename, Sprite::LoadType loadType = Sprite::LoadType::NORMAL);

extern Rgba gSpriteColor;
extern Rgba gShapeColor;

} // namespace jngl
