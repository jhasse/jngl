#pragma once

#include "../Stream.hpp"

#include <atomic>

namespace psemek::audio {

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

} // namespace psemek::audio
