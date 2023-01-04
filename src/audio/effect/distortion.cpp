#include "distortion.hpp"

#include <cmath>
#include <atomic>

namespace psemek::audio
{

	namespace
	{

		struct distortion_impl
			: distortion_control
		{
			distortion_impl(stream_ptr stream, float strength)
				: stream_(std::move(stream))
				, strength_(strength)
			{}

			float strength() const override
			{
				return strength_.load();
			}

			float strength(float value) override
			{
				return strength_.exchange(value);
			}

			std::optional<std::size_t> length() const override
			{
				return stream_->length();
			}

			std::size_t read(float * data, std::size_t sample_count) override
			{
				auto result = stream_->read(data, sample_count);
				float strength = strength_.load();

				for (std::size_t i = 0; i < result; ++i)
				{
					data[i] = std::tanh(strength * data[i]);
				}

				return result;
			}

			std::size_t played() const override
			{
				return stream_->played();
			}

		private:
			stream_ptr stream_;
			std::atomic<float> strength_;
		};

	}

	stream_ptr distortion(stream_ptr stream, float strength)
	{
		return std::make_shared<distortion_impl>(std::move(stream), strength);
	}

}
