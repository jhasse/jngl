// Copyright 2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
// Based on the audio implementation of the psemek engine, see
// https://lisyarus.github.io/blog/programming/2022/10/15/audio-mixing.html
#pragma once

#include "../Stream.hpp"

#include <atomic>

namespace jngl::audio {

struct volume_base {
	volume_base(float gain_left, float gain_right, float smoothness);

	float gain_left() const {
		return gain_[0].load();
	}
	float gain_right() const {
		return gain_[1].load();
	}

	float gain_left(float value) {
		return gain_[0].exchange(value);
	}
	float gain_right(float value) {
		return gain_[1].exchange(value);
	}

	float smoothness() const;
	float smoothness(float value);

	void apply(float* data, std::size_t sample_count);

private:
	std::atomic<float> gain_[2];
	std::atomic<float> smoothness_multiplier_;
	float real_gain_[2];
};

} // namespace jngl::audio
