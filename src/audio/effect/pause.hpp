#pragma once

#include "../stream.hpp"
#include "../duration.hpp"

namespace psemek::audio {

struct pause_control : stream {
	virtual bool paused() const = 0;
	virtual bool paused(bool value) = 0;

	virtual void pause() {
		paused(true);
	}
	virtual void resume() {
		paused(false);
	}
};

std::shared_ptr<pause_control> pause(stream_ptr stream, bool paused = false,
                                     duration length = 0.01f);

} // namespace psemek::audio
