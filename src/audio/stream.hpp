#pragma once

#include <cstddef>
#include <memory>
#include <optional>

namespace psemek::audio
{

	struct stream
	{
		// The length of the stream in samples, or nullopt if the stream is infinite
		virtual std::optional<std::size_t> length() const = 0;

		// Return value less than sample count means end of stream
		// Must be called from mixing thread
		virtual std::size_t read(float * data, std::size_t sample_count) = 0;

		// The number of samples already played from this stream
		virtual std::size_t played() const = 0;

		virtual ~stream() {}
	};

	using stream_ptr = std::shared_ptr<stream>;

}
