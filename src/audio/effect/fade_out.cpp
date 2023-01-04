#include "fade_out.hpp"
#include "../constants.hpp"

#include <algorithm>
#include <cmath>

namespace psemek::audio
{

	namespace
	{

		struct fade_out_impl
			: stream
		{
			fade_out_impl(stream_ptr stream, duration length, duration start)
				: stream_(std::move(stream))
				, length_(length)
				, start_(start)
			{}

			std::optional<std::size_t> length() const override
			{
				// TODO: compute fade_out length using the number of samples already played
				return stream_->length();
			}

			std::size_t played() const override
			{
				return stream_->played();
			}

			std::size_t read(float * data, std::size_t sample_count) override
			{
				if (current_ >= length_.samples())
					return 0;

				auto result = stream_->read(data, sample_count);

				if (result <= start_.samples())
				{
					start_ -= result;
				}
				else
				{
					for (std::size_t i = start_.samples(); i < result; i += 2)
					{
						float m = static_cast<float>(length_.samples() - std::min<std::size_t>(current_, length_.samples())) / length_.samples();

						data[i + 0] *= m;
						data[i + 1] *= m;

						current_ += 2;
					}
					start_ = {};
				}

				return result;
			}

		private:
			stream_ptr stream_;
			duration length_;
			duration start_;
			std::size_t current_ = 0;
		};

	}

	stream_ptr fade_out(stream_ptr stream, duration length, duration start)
	{
		return std::make_shared<fade_out_impl>(std::move(stream), length, start);
	}

}
