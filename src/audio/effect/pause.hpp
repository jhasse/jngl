#pragma once

#include "../Stream.hpp"
#include "../duration.hpp"

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
