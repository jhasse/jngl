// Copyright 2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
// Based on the audio implementation of the psemek engine, see
// https://lisyarus.github.io/blog/programming/2022/10/15/audio-mixing.html
#pragma once

#include "../Stream.hpp"

namespace jngl::audio {

struct volume_control : Stream {
	virtual float gain() const = 0;
	virtual float gain(float value) = 0;

	virtual float smoothness() const = 0;
	virtual float smoothness(float value) = 0;
};

struct volume_control_stereo : Stream {
	virtual float gain_left() const = 0;
	virtual float gain_right() const = 0;
	virtual float gain_left(float value) = 0;
	virtual float gain_right(float value) = 0;

	virtual float smoothness() const = 0;
	virtual float smoothness(float value) = 0;
};

std::shared_ptr<volume_control> volume(std::shared_ptr<Stream> stream, float gain = 1.f, float smoothness = 0.f);
std::shared_ptr<volume_control_stereo> volume_stereo(std::shared_ptr<Stream> stream, float gain_left = 1.f,
                                                     float gain_right = 1.f,
                                                     float smoothness = 0.f);

} // namespace jngl::audio
