#include "loop.hpp"
#include "../duplicate.hpp"

namespace psemek::audio {

namespace {

struct loop_impl : stream {
	loop_impl(track_ptr dup, std::optional<std::size_t> count)
	: dup_(std::move(dup)), stream_(dup_->stream()), count_(count) {
	}

	std::optional<std::size_t> length() const override {
		if (count_) {
			return (*stream_->length()) * (*count_);
		}
		return std::nullopt;
	}

	std::size_t read(float* data, std::size_t sample_count) override {
		std::size_t result = 0;
		while (result < sample_count && (!count_ || repeated_ < *count_)) {
			auto need = sample_count - result;
			auto count = stream_->read(data + result, need);
			result += count;
			if (count < need) {
				++repeated_;
				stream_ = dup_->stream();
			}
		}
		return result;
	}

	std::size_t played() const override {
		return stream_->played() + (*stream_->length()) * repeated_;
	}

private:
	track_ptr dup_;
	stream_ptr stream_;
	std::optional<std::size_t> count_;
	std::size_t repeated_ = 0;
};

} // namespace

stream_ptr loop(stream_ptr stream, std::optional<std::size_t> count) {
	if (!stream->length()) {
		return stream;
	}
	return std::make_shared<loop_impl>(make_duplicator(std::move(stream)), count);
}

} // namespace psemek::audio
