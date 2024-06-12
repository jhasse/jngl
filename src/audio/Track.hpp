// Copyright 2023-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
// Based on the audio implementation of the psemek engine, see
// https://lisyarus.github.io/blog/programming/2022/10/15/audio-mixing.html
#pragma once

#include "Stream.hpp"

#include <atomic>
#include <gsl/span>
#include <optional>
#include <vector>

namespace jngl {

namespace internal {
struct data_holder;
} // namespace internal

class PlayingTrack : public Stream {
public:
	explicit PlayingTrack(std::shared_ptr<internal::data_holder>);

	/// percentage of samples that have been returned by read, resets to 0 after calling rewind()
	float progress() const;

private:
	size_t read(float* data, std::size_t sample_count) override;
	void rewind() override;
	bool isPlaying() const override;

	std::shared_ptr<internal::data_holder> data_holder_;
	std::atomic<std::size_t> played_{ 0 };
};

struct Track {
	virtual std::shared_ptr<PlayingTrack> play() const = 0;
	virtual std::optional<std::size_t> length() const = 0;

	virtual ~Track() = default;
};

std::shared_ptr<Track> load_raw(gsl::span<float const> samples);
std::shared_ptr<Track> load_raw(std::vector<float> samples);

} // namespace jngl
