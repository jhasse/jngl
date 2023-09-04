#include "pitch.hpp"

#include <atomic>
#include <cassert>
#include <cmath>

#if __cplusplus < 202002L
float lerp(float a, float b, float t) {
	return a + t * (b - a);
}
#else
using std::lerp;
#endif

namespace jngl::audio {

namespace {

struct pitch_control_impl : pitch_control {
	pitch_control_impl(std::shared_ptr<Stream> stream, float ratio)
	: stream_(std::move(stream)), ratio(1.f / ratio) {
	}

	// N.B. resampler ratio is in terms of sampling frequency, while
	// pitch control ratio is in terms of sound frequency, which
	// is the opposite

	float pitch() const override {
		return 1.f / ratio;
	}

	float pitch(float ratio) override {
		return this->ratio = 1.f / ratio;
	}

	std::size_t read(float* data, std::size_t sample_count) override {
		assert(sample_count % 2 == 0);
		std::size_t result = 0;

		if (sourceBufferSize == 0) {
			sourceBufferSize = stream_->read(sourceBuffer, MAX_SOURCE_BUFFER_SIZE);
			if (sourceBufferSize == 0) { return 0; }
			assert(sourceBufferSize % 2 == 0);
		}

		while (result < sample_count) {
			// assert(2 * (position + 1) <= sourceBufferSize);
			if (2 * (position + 1) >= sourceBufferSize) {
				static_assert(MAX_SOURCE_BUFFER_SIZE % 2 == 0);
				sourceBuffer[0] = sourceBuffer[sourceBufferSize - 2];
				sourceBuffer[1] = sourceBuffer[sourceBufferSize - 1];
				position = 0;
				sourceBufferSize = stream_->read(sourceBuffer + 2, MAX_SOURCE_BUFFER_SIZE - 2) + 2;
				if (sourceBufferSize == 2) {
					break;
				}
				assert(sourceBufferSize % 2 == 0);
			}
			data[result] =
			    lerp(sourceBuffer[2 * position], sourceBuffer[2 * (position + 1)], positionFrac);
			++result;
			data[result] = lerp(sourceBuffer[2 * position + 1],
			                    sourceBuffer[2 * (position + 1) + 1], positionFrac);
			++result;

			positionFrac += 1.f / ratio;
			while (positionFrac > 1.f) {
				++position;
				positionFrac -= 1.f;
			}
		}
		played_ += result;
		return result;
	}

	void rewind() override {
		stream_->rewind();
		position = 0;
		sourceBufferSize = 0;
	}

private:
	std::shared_ptr<Stream> stream_;

	constexpr static size_t MAX_SOURCE_BUFFER_SIZE = 996;
	size_t position = 0;
	size_t sourceBufferSize = 0;
	float sourceBuffer[MAX_SOURCE_BUFFER_SIZE] = { 0 };

	float positionFrac = 0; //< 0 = left sample, 1 = right sample

	float ratio; // 1. / ratio

	std::atomic<std::size_t> played_{ 0 };
};

} // namespace

std::shared_ptr<pitch_control> pitch(std::shared_ptr<Stream> stream, float ratio) {
	return std::make_shared<pitch_control_impl>(std::move(stream), ratio);
}

} // namespace jngl::audio
