// Copyright 2010-2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "audio/engine.hpp"
#include "helper.hpp"
#include "jngl/Singleton.hpp"

#include <future>
#include <memory>

namespace jngl {

class Channel;
class Mixer;
class Sound;
class SoundFile;

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

	void play(Channel&, std::shared_ptr<Sound> sound);
	void stop(Channel&, std::shared_ptr<Sound>& sound);
	void increasePauseDeviceCount();
	void decreasePauseDeviceCount();
	void setPitch(float pitch);
	float getVolume() const;
	void setVolume(float volume);
	Channel& getMainChannel();
	void registerChannel(std::shared_ptr<Stream>);
	void unregisterChannel(const Stream&);
	void step();

	/// Returns nullptr if the sound hasn't been loaded yet (or if the file doesn't exist)
	std::shared_ptr<SoundFile> getSoundFileIfLoaded(std::string_view filename);

	std::shared_ptr<SoundFile> getSoundFile(std::string_view filename, std::launch policy);

private:
	std::vector<std::shared_ptr<Sound>> sounds_;
	std::shared_ptr<Mixer> mixer;
	std::unique_ptr<Channel> mainChannel;
	std::shared_ptr<audio::pitch_control> pitchControl;
	std::shared_ptr<audio::volume_control> volumeControl;
	uint8_t pauseDeviceCount = 0; //< if >0 audio device is paused
	std::unordered_map<std::string, std::shared_ptr<SoundFile>, string_hash, std::equal_to<>>
	    soundFiles;
	// engine has to be declared after soundFiles so that the mixer thread gets joined and closed
	// before any SoundFile is destroyed:
	audio::engine engine;
};

} // namespace jngl
