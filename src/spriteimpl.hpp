// Copyright 2009-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "windowptr.hpp"
#include "jngl/sprite.hpp"

namespace jngl {

Finally loadSprite(const std::string&);
Sprite& GetSprite(const std::string& filename, Sprite::LoadType loadType = Sprite::LoadType::NORMAL);

extern unsigned char spriteColorRed, spriteColorGreen, spriteColorBlue, spriteColorAlpha,
                     colorRed,       colorGreen,       colorBlue,       colorAlpha;

} // namespace jngl
