#pragma once

#include <gsl/span>
#include <vector>
#include <atomic>

namespace psemek::audio {

struct resampler {
	explicit resampler(float ratio = 1.f, float smoothness = 0.f);

	float ratio() const;
	float ratio(float value);

	float smoothness() const;
	float smoothness(float value);

	// ratio is target frequency / source frequency
	void feed(gsl::span<float const> samples);

	gsl::span<float const> result() const {
		return result_;
	}

private:
	std::vector<float> result_;
	int position_{ 0 };
	float position_frac_{ 0.f };

	float last_sample_[2]{ 0.f, 0.f };

	std::atomic<float> ratio_{ 1.f };
	std::atomic<float> smoothness_multiplier_{ 1.f };
	float real_ratio_{ 1.f };

	void advance(float delta);
};

} // namespace psemek::audio
