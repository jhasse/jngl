#pragma once

#include "constants.hpp"

#include <cstdint>
#include <cmath>

namespace psemek::audio {

struct duration {
	duration() : samples_{ 0 } {
	}

	/* NOLINT */ duration(std::int64_t samples) : samples_{ samples } {
	}

	/* NOLINT */ duration(std::size_t samples) : samples_{ static_cast<int64_t>(samples) } {
	}

	/* NOLINT */ duration(float seconds) : samples_{ seconds_to_samples(seconds) } {
	}

	duration(duration const& other) = default;

	float seconds() const {
		return samples_to_seconds(samples_);
	}

	std::int64_t samples() const {
		return samples_;
	}

	duration& operator+=(duration const& other) {
		samples_ += other.samples_;
		return *this;
	}

	duration& operator-=(duration const& other) {
		samples_ -= other.samples_;
		return *this;
	}

	friend duration operator+(duration const& d1, duration const& d2) {
		return duration{ d1.samples() + d2.samples() };
	}

	friend duration operator-(duration const& d1, duration const& d2) {
		return duration{ d1.samples() - d2.samples() };
	}

private:
	std::int64_t samples_;
};

} // namespace psemek::audio
