// Copyright 2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
// Based on the audio implementation of the psemek engine, see
// https://lisyarus.github.io/blog/programming/2022/10/15/audio-mixing.html
#pragma once

#include "constants.hpp"

#include <cmath>

namespace jngl::audio {

inline float smoothness_to_multiplier(float smoothness) {
	if (smoothness == 0.f) return 1.f;
	return -std::expm1(-inv_frequency / smoothness);
}

inline float multiplier_to_smoothness(float multiplier) {
	if (multiplier == 1.f) return 0.f;

	return -inv_frequency / std::log1p(-multiplier);
}

inline void smooth_update(float& value, float target_value, float smoothness_multiplier) {
	value += (target_value - value) * smoothness_multiplier;
}

} // namespace jngl::audio
