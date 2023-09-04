#pragma once

#include "../Stream.hpp"

namespace jngl::audio {

struct pitch_control : Stream {
	virtual float pitch() const = 0;
	virtual float pitch(float ratio) = 0;
};

std::shared_ptr<pitch_control> pitch(std::shared_ptr<Stream> stream, float ratio = 1.f);

} // namespace jngl::audio
