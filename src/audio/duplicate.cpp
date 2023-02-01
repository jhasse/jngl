#include "duplicate.hpp"
#include "recorder.hpp"

#include <atomic>

namespace psemek::audio {

namespace {

struct duplicate_stream_impl : stream {
	explicit duplicate_stream_impl(std::shared_ptr<recorder> recorder)
	: recorder_(std::move(recorder)) {
	}

	std::optional<std::size_t> length() const override {
		return recorder_->length();
	}

	std::size_t played() const override {
		return played_.load();
	}

	std::size_t read(float* data, std::size_t sample_count) override {
		auto buffer = recorder_->buffer();
		auto played = played_.load();
		if (buffer.size() < played + sample_count) {
			recorder_->request(sample_count);
			buffer = recorder_->buffer();
		}

		auto count = std::min<std::size_t>(sample_count, buffer.size() - played);
		std::copy(buffer.data() + played, buffer.data() + played + count, data);
		played_.fetch_add(count);
		return count;
	}

private:
	std::shared_ptr<recorder> recorder_;
	std::atomic<std::size_t> played_{ 0 };
};

struct duplicate_track_impl : track {
	explicit duplicate_track_impl(std::shared_ptr<recorder> recorder)
	: recorder_(std::move(recorder)) {
	}

	stream_ptr stream() const override {
		return std::make_shared<duplicate_stream_impl>(recorder_);
	}

	std::optional<std::size_t> length() const override {
		return recorder_->length();
	}

private:
	std::shared_ptr<recorder> recorder_;
};

} // namespace

track_ptr make_duplicator(std::shared_ptr<recorder> recorder) {
	return std::make_shared<duplicate_track_impl>(std::move(recorder));
}

} // namespace psemek::audio
