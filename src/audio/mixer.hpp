#pragma once

#include "Stream.hpp"

#include <memory>

namespace jngl::audio {

struct mixer : Stream {
	virtual void add(std::shared_ptr<Stream> stream) = 0;
	virtual void remove(Stream*) = 0;
};

using mixer_ptr = std::shared_ptr<mixer>;

mixer_ptr make_mixer();

} // namespace jngl::audio
