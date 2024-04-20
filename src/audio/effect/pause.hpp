// Copyright 2023-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
// Based on the audio implementation of the psemek engine, see
// https://lisyarus.github.io/blog/programming/2022/10/15/audio-mixing.html
#pragma once

#include "../Stream.hpp"
#include "../duration.hpp"

#include <memory>

namespace jngl::audio {

struct pause_control : Stream {
	virtual bool paused() const = 0;
	virtual bool paused(bool value) = 0;

	virtual void pause() {
		paused(true);
	}
	virtual void resume() {
		paused(false);
	}
};

std::shared_ptr<pause_control> pause(std::shared_ptr<Stream> stream, bool paused = false,
                                     duration length = 0.01f);

} // namespace jngl::audio
