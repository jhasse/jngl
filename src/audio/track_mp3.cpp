#include "track.hpp"
#include "resampler.hpp"
#include "constants.hpp"

#include "minimp3/options.h"
#include "minimp3/minimp3.h"

#include <atomic>

namespace psemek::audio
{

	namespace
	{

		struct mp3_data_holder
		{
			std::vector<char> storage;
			std::span<char const> data;

			mp3_data_holder(std::vector<char> storage)
				: storage(std::move(storage))
				, data(this->storage)
			{}

			mp3_data_holder(std::span<char const> data)
				: data(data)
			{}
		};

		struct mp3_stream_impl
			: stream
		{
			mp3_stream_impl(std::shared_ptr<mp3_data_holder> data)
				: data_(std::move(data))
				, source_buffer_(MINIMP3_MAX_SAMPLES_PER_FRAME)
			{
				mp3dec_init(&decoder_);
			}

			std::optional<std::size_t> length() const override
			{
				return std::nullopt;
			}

			std::size_t read(float * data, std::size_t sample_count) override
			{
				auto input = reinterpret_cast<std::uint8_t const *>(data_->data.data());

				std::size_t result = 0;
				while (result < sample_count)
				{
					if (resampler_pos_ < resampler_.result().size())
					{
						std::size_t size = std::min(resampler_.result().size() - resampler_pos_, sample_count - result);
						std::copy(resampler_.result().data() + resampler_pos_, resampler_.result().data() + resampler_pos_ + size, data + result);
						resampler_pos_ += size;
						result += size;
					}
					else
					{
						mp3dec_frame_info_t frame_info;
						auto source_buffer_size = mp3dec_decode_frame(&decoder_, input + read_bytes_, data_->data.size() - read_bytes_, source_buffer_.data(), &frame_info);
						resampler_pos_ = 0;
						read_bytes_ += frame_info.frame_bytes;

						if (frame_info.frame_bytes == 0)
							break;

						if (frame_info.channels == 1)
						{
							for (std::size_t i = source_buffer_size * 2; i > 0;)
							{
								i -= 2;
								source_buffer_[i + 0] = source_buffer_[i / 2];
								source_buffer_[i + 1] = source_buffer_[i / 2];
							}
						}

						source_buffer_size *= 2;

						resampler_.ratio(static_cast<float>(frequency) / frame_info.hz);
						resampler_.feed({source_buffer_.data(), source_buffer_.data() + source_buffer_size});
					}
				}

				played_.fetch_add(result);

				return result;
			}

			std::size_t played() const override
			{
				return played_.load();
			}

		private:
			std::shared_ptr<mp3_data_holder> data_;
			std::size_t read_bytes_{0};

			mp3dec_t decoder_;
			std::vector<float> source_buffer_;

			resampler resampler_;
			std::size_t resampler_pos_{0};

			std::atomic<std::size_t> played_{0};
		};

		struct mp3_track_impl
			: track
		{
			mp3_track_impl(std::shared_ptr<mp3_data_holder> data)
				: data_(std::move(data))
			{}

			stream_ptr stream() const override
			{
				return std::make_shared<mp3_stream_impl>(data_);
			}

			std::optional<std::size_t> length() const override
			{
				return std::nullopt;
			}

		private:
			std::shared_ptr<mp3_data_holder> data_;
		};

	}

	track_ptr load_mp3(std::span<char const> data)
	{
		return std::make_shared<mp3_track_impl>(std::make_shared<mp3_data_holder>(data));
	}

	track_ptr load_mp3(std::vector<char> data)
	{
		return std::make_shared<mp3_track_impl>(std::make_shared<mp3_data_holder>(std::move(data)));
	}

}
