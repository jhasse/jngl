// Copyright 2023-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
// Based on the audio implementation of the psemek engine, see
// https://lisyarus.github.io/blog/programming/2022/10/15/audio-mixing.html
#include "Track.hpp"

namespace jngl {

PlayingTrack::PlayingTrack(gsl::span<const float> samples) : samples(samples) {
}

float PlayingTrack::progress() const {
	return static_cast<float>(played_) / static_cast<float>(samples.size());
}

std::size_t PlayingTrack::read(float* data, std::size_t sample_count) {
	auto played = played_.load();

	auto count = std::min(samples.size() - played, sample_count);
	std::copy(samples.begin() + static_cast<decltype(samples)::difference_type>(played),
	          samples.begin() + static_cast<decltype(samples)::difference_type>(played) +
	              static_cast<decltype(samples)::difference_type>(count),
	          data);
	played_.fetch_add(count);
	return count;
}

void PlayingTrack::rewind() {
	played_ = 0;
}

bool PlayingTrack::isPlaying() const {
	return samples.size() - played_ > 0;
}

} // namespace jngl
