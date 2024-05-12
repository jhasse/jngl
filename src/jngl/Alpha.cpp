// Copyright 2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "Alpha.hpp"

#include <algorithm>
#include <cmath>

namespace jngl {

Alpha::Alpha(float alpha) : alpha(alpha) {
}

Alpha Alpha::u8(uint8_t alpha) {
	return Alpha{ static_cast<float>(alpha) / 255.f };
}

uint8_t Alpha::u8() const {
	return std::clamp(std::lround(alpha * 255), 0L, 255L);
}

float Alpha::getAlpha() const {
	return alpha;
}

void Alpha::setAlpha(const float alpha) {
	this->alpha = alpha;
}

} // namespace jngl
