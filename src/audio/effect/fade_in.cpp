#include "fade_in.hpp"
#include "../constants.hpp"

#include <algorithm>
#include <cmath>

namespace psemek::audio
{

	namespace
	{

		struct fade_in_impl
			: stream
		{
			fade_in_impl(stream_ptr stream, duration length, duration start)
				: stream_(std::move(stream))
				, length_(length)
				, start_(start)
			{}

			std::optional<std::size_t> length() const override
			{
				return stream_->length();
			}

			std::size_t played() const override
			{
				return stream_->played();
			}

			std::size_t read(float * data, std::size_t sample_count) override
			{
				auto const result = stream_->read(data, sample_count);
				std::fill(data, data + std::min<std::size_t>(result, start_.samples()), 0.f);

				if (result <= start_.samples())
				{
					start_ -= result;
				}
				else
				{
					for (std::size_t i = start_.samples(); i < result; i += 2)
					{
						float m = static_cast<float>(std::min<std::size_t>(current_, length_.samples())) / length_.samples();

						data[i + 0] *= m;
						data[i + 1] *= m;

						current_ += 2;
					}
					start_ = duration{};
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

	stream_ptr fade_in(stream_ptr stream, duration length, duration start)
	{
		return std::make_shared<fade_in_impl>(std::move(stream), length, start);
	}

}
