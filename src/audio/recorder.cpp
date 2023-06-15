#include "recorder.hpp"

#include <stdexcept>
#include <vector>

namespace psemek::audio {

namespace {

struct recorder_impl : recorder {
	explicit recorder_impl(stream_ptr stream) : stream_(std::move(stream)) {
	}

	explicit recorder_impl(std::vector<float> samples)
	: storage_(std::move(samples)), samples_(storage_) {
	}

	explicit recorder_impl(gsl::span<float const> samples) : samples_(samples) {
	}

	std::optional<std::size_t> length() const override {
		return stream_ ? stream_->length() : storage_.size();
	}

	std::size_t request(std::size_t samples) override {
		if (!stream_) return 0;

		if (storage_.size() < samples_.size() + samples) {
			storage_.resize(std::max<std::size_t>(samples_.size() + samples, storage_.size() * 2));
			samples_ = { storage_.data(), storage_.data() + samples_.size() };
		}

		auto result = stream_->read(storage_.data() + samples_.size(), samples);
		samples_ = { storage_.data(), storage_.data() + samples_.size() + result };
		return result;
	}

	gsl::span<float const> buffer() const override {
		return samples_;
	}

private:
	stream_ptr stream_;
	std::vector<float> storage_;
	gsl::span<float const> samples_;
};

} // namespace

std::shared_ptr<recorder> make_recorder(stream_ptr stream) {
	return std::make_shared<recorder_impl>(std::move(stream));
}

std::shared_ptr<recorder> make_recorder(std::vector<float> samples) {
	if ((samples.size() % 2) != 0) {
		throw std::runtime_error("bad sample count");
	}
	return std::make_shared<recorder_impl>(std::move(samples));
}

std::shared_ptr<recorder> make_recorder(gsl::span<float const> samples) {
	if ((samples.size() % 2) != 0) {
		throw std::runtime_error("bad sample count");
	}
	return std::make_shared<recorder_impl>(samples);
}

} // namespace psemek::audio
