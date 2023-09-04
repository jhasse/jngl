#include "loop.hpp"

#include "../Stream.hpp"

namespace jngl::audio {

namespace {

struct loop_impl : Stream {
	loop_impl(std::shared_ptr<Stream> stream, std::optional<std::size_t> count)
	: stream_(std::move(stream)), count_(count) {
	}

	std::size_t read(float* data, std::size_t sample_count) override {
		std::size_t result = 0;
		while (result < sample_count && (!count_ || repeated_ < *count_)) {
			auto need = sample_count - result;
			auto count = stream_->read(data + result, need);
			result += count;
			if (count == 0) {
				++repeated_;
				stream_->rewind();
			}
		}
		return result;
	}

	void rewind() override {
		repeated_ = 0;
	}

private:
	std::shared_ptr<Stream> stream_;
	std::optional<std::size_t> count_;
	std::size_t repeated_ = 0;
};

} // namespace

std::shared_ptr<Stream> loop(std::shared_ptr<Stream> stream, std::optional<std::size_t> count) {
	return std::make_shared<loop_impl>(std::move(stream), count);
}

} // namespace jngl::audio
