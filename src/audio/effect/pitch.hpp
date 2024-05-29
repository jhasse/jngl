// Copyright 2023-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
// Based on the audio implementation of the psemek engine, see
// https://lisyarus.github.io/blog/programming/2022/10/15/audio-mixing.html
#pragma once

#include "../Stream.hpp"

#include <memory>

namespace jngl::audio {

struct pitch_control : Stream {
	virtual float pitch() const = 0;
	virtual float pitch(float ratio) = 0;
};

std::shared_ptr<pitch_control> pitch(std::shared_ptr<Stream> stream, float ratio = 1.f);

} // namespace jngl::audio
