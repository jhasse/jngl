// Copyright 2023-2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
// Based on the audio implementation of the psemek engine, see
// https://lisyarus.github.io/blog/programming/2022/10/15/audio-mixing.html
#include "volume_base.hpp"

#include "../smooth.hpp"

#include <cmath>

namespace jngl::audio {

volume_base::volume_base(float gain_left, float gain_right, float smoothness)
: gain_{ gain_left, gain_right }, smoothness_multiplier_{ smoothness_to_multiplier(smoothness) },
  real_gain_{ gain_left, gain_right } {
}

float volume_base::smoothness() const {
	return multiplier_to_smoothness(smoothness_multiplier_.load());
}

float volume_base::smoothness(float value) {
	auto old = smoothness_multiplier_.exchange(smoothness_to_multiplier(value));
	return multiplier_to_smoothness(old);
}

void volume_base::apply(float* data, std::size_t sample_count) {
	float gain[2] = { gain_[0].load(), gain_[1].load() };
	float smoothness_multiplier = smoothness_multiplier_.load();

	auto end = data + sample_count;
	for (auto p = data; p < end;) {
		*p++ *= real_gain_[0];
		*p++ *= real_gain_[1];

		smooth_update(real_gain_[0], gain[0], smoothness_multiplier);
		smooth_update(real_gain_[1], gain[1], smoothness_multiplier);
	}
}

} // namespace jngl::audio
