// Copyright 2010-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "audio/engine.hpp"
#include "jngl/Singleton.hpp"

#include <memory>

namespace jngl {

class Mixer;
class Sound;

namespace audio {
struct pitch_control;
struct volume_control;
} // namespace audio

void checkAlError();

class Audio : public jngl::Singleton<Audio> {
public:
	Audio();
	Audio(const Audio&) = delete;
	Audio& operator=(const Audio&) = delete;
	Audio(Audio&&) = delete;
	Audio& operator=(Audio&&) = delete;
	~Audio();

	void play(std::shared_ptr<Sound> sound);
	void stop(std::shared_ptr<Sound>& sound);
	void increasePauseDeviceCount();
	void decreasePauseDeviceCount();
	void setPitch(float pitch);
	float getVolume() const;
	void setVolume(float volume);
	std::shared_ptr<Mixer> getMixer();
    void step();

private:
	std::vector<std::shared_ptr<Sound>> sounds_;
	std::shared_ptr<Mixer> mixer;
	std::shared_ptr<audio::pitch_control> pitchControl;
	std::shared_ptr<audio::volume_control> volumeControl;
	audio::engine engine;
	uint8_t pauseDeviceCount = 0; //< if >0 audio device is paused
};

} // namespace jngl
