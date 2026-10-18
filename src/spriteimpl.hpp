// Copyright 2009-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#pragma once

#include "jngl/Rgba.hpp"
#include "jngl/Sprite.hpp"

#include <optional>

namespace jngl {

Finally loadSprite(const std::string&);
Sprite& GetSprite(const std::string& filename, Sprite::LoadType loadType = Sprite::LoadType::NORMAL);

extern Rgba gSpriteColor;
extern Rgba gShapeColor;

/// What setBlendMode() set, or none for the default, which differs between the window and a
/// FrameBuffer in use
extern std::optional<BlendMode> gBlendMode;

/// Sets OpenGL's blend function to gBlendMode, or to the default of the window or of a FrameBuffer,
/// whichever is drawn on right now
void applyBlendMode();

} // namespace jngl
