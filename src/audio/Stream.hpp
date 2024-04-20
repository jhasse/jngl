// Copyright 2023-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
// Based on the audio implementation of the psemek engine, see
// https://lisyarus.github.io/blog/programming/2022/10/15/audio-mixing.html
#pragma once

#include <cstddef>

namespace jngl {

struct Stream {
	// Return value less than sample count means end of stream
	// Must be called from mixing thread
	virtual std::size_t read(float* data, std::size_t sample_count) = 0;

	virtual void rewind() = 0;
	virtual bool isPlaying() const = 0;

	virtual ~Stream() = default;
};

} // namespace jngl
