#include "truncate.hpp"

#include <algorithm>

namespace psemek::audio
{

	namespace
	{

		struct truncate_impl
			: stream
		{
			truncate_impl(stream_ptr stream, duration length)
				: stream_(std::move(stream))
				, length_(length)
			{}

			std::optional<std::size_t> length() const override
			{
				return length_.samples();
			}

			std::size_t read(float * data, std::size_t sample_count) override
			{
				auto played = stream_->played();
				auto result = std::min<std::size_t>(length_.samples() - played, sample_count);
				result = stream_->read(data, result);
				return result;
			}

			std::size_t played() const override
			{
				return stream_->played();
			}

		private:
			stream_ptr stream_;
			duration length_;
		};

	}

	stream_ptr truncate(stream_ptr stream, duration length)
	{
		return std::make_shared<truncate_impl>(std::move(stream), length);
	}

}
