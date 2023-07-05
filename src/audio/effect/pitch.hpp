#pragma once

#include "../stream.hpp"

namespace psemek::audio {

struct pitch_control : stream {
	virtual float pitch() const = 0;
	virtual float pitch(float ratio) = 0;
};

std::shared_ptr<pitch_control> pitch(stream_ptr stream, float ratio = 1.f);

} // namespace psemek::audio
