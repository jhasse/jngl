#pragma once

#include <cstddef>
#include <memory>
#include <optional>

namespace psemek::audio {

struct stream {
	// Return value less than sample count means end of stream
	// Must be called from mixing thread
	virtual std::size_t read(float* data, std::size_t sample_count) = 0;

	virtual void rewind() = 0;

	virtual ~stream() = default;
};

using stream_ptr = std::shared_ptr<stream>;

} // namespace psemek::audio
