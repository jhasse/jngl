// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::TextureFilter
/// @file
#pragma once

#include <cstdint>

namespace jngl {

/// How a texture is sampled when it's drawn bigger or smaller than its pixel size
enum class TextureFilter : uint8_t {
	/// Bilinear Filtering is used when jngl::AppParameters::pixelArt is false. It smooths the
	/// image when it is scaled up.
	Bilinear,

	/// Nearest Neighbor Filtering is used when jngl::AppParameters::pixelArt is true. It
	/// preserves the pixelated look when the image is scaled up.
	NearestNeighbor,
};

} // namespace jngl
