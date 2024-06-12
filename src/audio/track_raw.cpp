// Copyright 2023-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
// Based on the audio implementation of the psemek engine, see
// https://lisyarus.github.io/blog/programming/2022/10/15/audio-mixing.html
#include "Track.hpp"

#include <stdexcept>

namespace jngl {

namespace internal {

struct data_holder {
	std::vector<float> storage;
	gsl::span<float const> samples;

	explicit data_holder(gsl::span<float const> samples) : samples(samples) {
	}

	explicit data_holder(std::vector<float> storage)
	: storage(std::move(storage)), samples(this->storage) {
	}
};

} // namespace internal

PlayingTrack::PlayingTrack(std::shared_ptr<internal::data_holder> data_holder)
: data_holder_(std::move(data_holder)) {
}

float PlayingTrack::progress() const {
	return static_cast<float>(played_) / static_cast<float>(data_holder_->samples.size());
}

std::size_t PlayingTrack::read(float* data, std::size_t sample_count) {
	auto played = played_.load();

	auto count = std::min(data_holder_->samples.size() - played, sample_count);
	std::copy(data_holder_->samples.begin() + played,
	          data_holder_->samples.begin() + played + count, data);
	played_.fetch_add(count);
	return count;
}

void PlayingTrack::rewind() {
	played_ = 0;
}

bool PlayingTrack::isPlaying() const {
	return data_holder_->samples.size() - played_ > 0;
}

struct raw_track_impl : Track {
	explicit raw_track_impl(std::shared_ptr<internal::data_holder> data_holder)
	: data_holder_(std::move(data_holder)) {
	}

	std::shared_ptr<PlayingTrack> play() const override {
		return std::make_shared<PlayingTrack>(data_holder_);
	}

	std::optional<std::size_t> length() const override {
		return data_holder_->samples.size();
	}

private:
	std::shared_ptr<internal::data_holder> data_holder_;
};

std::shared_ptr<Track> load_raw(gsl::span<float const> samples) {
	if ((samples.size() % 2) != 0) {
		throw std::runtime_error("bad sample count");
	}
	return std::make_shared<raw_track_impl>(std::make_shared<internal::data_holder>(samples));
}

std::shared_ptr<Track> load_raw(std::vector<float> samples) {
	if ((samples.size() % 2) != 0) {
		throw std::runtime_error("bad sample count");
	}
	return std::make_shared<raw_track_impl>(
	    std::make_shared<internal::data_holder>(std::move(samples)));
}

} // namespace jngl
