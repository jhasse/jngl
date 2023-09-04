#include "pause.hpp"

#include <algorithm>
#include <atomic>

namespace jngl::audio {

namespace {

struct pause_control_impl : pause_control {
	pause_control_impl(std::shared_ptr<Stream> stream, bool paused, duration length)
	: stream_(std::move(stream)), paused_{ paused }, length_(length),
	  level_(paused ? 0 : length_.samples()) {
	}

	bool paused() const override {
		return paused_.load();
	}

	bool paused(bool value) override {
		return paused_.exchange(value);
	}

	std::size_t read(float* data, std::size_t sample_count) override {
		bool const paused = paused_.load();

		if (paused) {
			auto result = stream_->read(data, std::min(sample_count, level_));

			for (std::size_t i = 0; i < result; i += 2) {
				float gain = static_cast<float>(level_) / length_.samples();
				data[i + 0] *= gain;
				data[i + 1] *= gain;
				level_ -= 2;
			}

			std::fill(data + result, data + sample_count, 0.f);
			return sample_count;
		}
		auto result = stream_->read(data, sample_count);

		auto const max_level = static_cast<std::size_t>(length_.samples());

		for (std::size_t i = 0; i < result; i += 2) {
			float gain = static_cast<float>(level_) / max_level;
			data[i + 0] *= gain;
			data[i + 1] *= gain;

			if (level_ < max_level) {
				level_ += 2;
			}
		}

		return result;
	}

	void rewind() override {
		return stream_->rewind();
	}

private:
	std::shared_ptr<Stream> stream_;
	std::atomic<bool> paused_;
	duration length_;
	std::size_t level_;
};

} // namespace

std::shared_ptr<pause_control> pause(std::shared_ptr<Stream> stream, bool paused, duration length) {
	return std::make_shared<pause_control_impl>(std::move(stream), paused, length);
}

} // namespace jngl::audio
