#include "pitch.hpp"
#include "../resampler.hpp"

#include <cmath>

namespace psemek::audio
{

	namespace
	{

		struct pitch_control_impl
			: pitch_control
		{
			pitch_control_impl(stream_ptr stream, float ratio, float smoothness)
				: stream_(std::move(stream))
				, resampler_(1.f / ratio, smoothness)
			{}

			// N.B. resampler ratio is in terms of sampling frequency, while
			// pitch control ratio is in terms of sound frequency, which
			// is the opposite

			float pitch() const override
			{
				return 1.f / resampler_.ratio();
			}

			float pitch(float ratio) override
			{
				return 1.f / resampler_.ratio(1.f / ratio);
			}

			float smoothness() const override
			{
				return resampler_.smoothness();
			}

			float smoothness(float value) override
			{
				return resampler_.smoothness(value);
			}

			std::optional<std::size_t> length() const override
			{
				return std::nullopt;
			}

			std::size_t read(float * data, std::size_t sample_count) override
			{
				std::size_t result = 0;

				while (result < sample_count)
				{
					if (resampler_pos_ < resampler_.result().size())
					{
						std::size_t size = std::min(sample_count - result, resampler_.result().size() - resampler_pos_);
						std::copy(resampler_.result().data() + resampler_pos_, resampler_.result().data() + resampler_pos_ + size, data + result);
						result += size;
						resampler_pos_ += size;
						played_ += size;
					}
					else
					{
						resampler_pos_ = 0;

						std::size_t request_size = std::max<std::size_t>(sample_count, std::ceil(resampler_.ratio() * sample_count / 2.f) * 2);
						source_buffer_.resize(request_size);
						auto count = stream_->read(source_buffer_.data(), request_size);

						if (count == 0)
							break;

						resampler_.feed({source_buffer_.data(), source_buffer_.data() + count});
					}
				}

				return result;
			}

			std::size_t played() const override
			{
				return played_.load();
			}

		private:
			stream_ptr stream_;

			std::vector<float> source_buffer_;

			resampler resampler_;
			std::size_t resampler_pos_{0};

			std::atomic<std::size_t> played_{0};
		};

	}

	std::shared_ptr<pitch_control> pitch(stream_ptr stream, float ratio, float smoothness)
	{
		return std::make_shared<pitch_control_impl>(std::move(stream), ratio, smoothness);
	}

}
