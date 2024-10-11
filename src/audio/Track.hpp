// Copyright 2023-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#pragma once

#include "Stream.hpp"

#include <atomic>
#include <gsl/span>

namespace jngl {

class PlayingTrack : public Stream {
public:
	/// doesn't copy the samples, so they must outlive this object
	explicit PlayingTrack(gsl::span<const float> samples);

	/// percentage of samples that have been returned by read, resets to 0 after calling rewind()
	float progress() const;

private:
	size_t read(float* data, std::size_t sample_count) override;
	void rewind() override;
	bool isPlaying() const override;

	gsl::span<const float> samples;
	std::atomic<std::size_t> played_{ 0 };
};

} // namespace jngl
