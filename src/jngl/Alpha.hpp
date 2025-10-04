// Copyright 2024-2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::Alpha class
/// @file
#pragma once

#include <cstdint>

namespace jngl {
/// Object representing only the Alpha Channel in an RGBA color, 0 meaning invisible, 1 fully
/// visible.
class Alpha {
public:
	/// \param alpha 0.0f ... 1.0f
	///
	/// Values over 1.0f will result in 1.0f and negatives will result in 0.0f.
	explicit Alpha(float alpha);

	/// \param alpha 0...255
	static Alpha u8(uint8_t alpha);

	/// 0...255
	[[nodiscard]] uint8_t u8() const;

	/// 0.0f ... 1.0f
	float getAlpha() const;
	/// 0.0f ... 1.0f
	void setAlpha(float);

private:
	float alpha;
};

} // namespace jngl
