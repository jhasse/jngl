// Copyright 2019-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "SoundFile.hpp"

#include "../audio/engine.hpp"
#include "../audio/mixer.hpp"
#include "../Sound.hpp"
#include "../audio.hpp"
#include "../main.hpp"
#include "debug.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <stdexcept>
#include <unordered_map>

using namespace psemek; // FIXME

#ifdef ANDROID
#include "../android/fopen.hpp"

#define AL_ALEXT_PROTOTYPES 1
#include <AL/alext.h>
#endif

#ifdef __APPLE__
// OpenAL is deprecated in favor of AVAudioEngine
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

namespace jngl {

std::unordered_map<std::string, std::shared_ptr<SoundFile>> sounds;

class Audio {
public:
	Audio() : mixer(psemek::audio::make_mixer()) {
		engine.output()->stream(mixer);
	}
	Audio(const Audio&) = delete;
	Audio& operator=(const Audio&) = delete;
	Audio(Audio&&) = delete;
	Audio& operator=(Audio&&) = delete;
	~Audio() {
	}
	static bool IsStopped(std::shared_ptr<Sound>& s) {
		return s->isStopped();
	}

	void play(std::shared_ptr<Sound> sound) {
		mixer->add(sound->getStream());
		sounds_.erase(std::remove_if(sounds_.begin(), sounds_.end(), IsStopped), sounds_.end());
		sounds_.emplace_back(std::move(sound));
	}

	void Stop(std::shared_ptr<Sound>& sound) {
		// TODO: Actually stop playing
		std::vector<std::shared_ptr<Sound>>::iterator i;
		if ((i = std::find(sounds_.begin(), sounds_.end(), sound)) != sounds_.end()) {
			sounds_.erase(i);
		}
	}
#ifdef ALC_SOFT_pause_device
	void pauseDevice() {
		alcDevicePauseSOFT(device_);
	}
	void resumeDevice() {
		alcDeviceResumeSOFT(device_);
	}
#endif

private:
	std::vector<std::shared_ptr<Sound>> sounds_;
	audio::engine engine;
	std::shared_ptr<audio::mixer> mixer;
};

SoundFile::SoundFile(std::string filename, std::launch) {
	debug("Reading ");
	debug(filename);
	debug(" ... ");
#ifdef _WIN32
	FILE* const f = fopen(filename.c_str(), "rb");
#else
	FILE* const f = fopen(filename.c_str(), "rbe");
#endif
	if (f == nullptr) {
		throw std::runtime_error("File not found (" + filename + ").");
	}
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	buffer_.resize(fsize);
	int itemsRead = fread(buffer_.data(), fsize, 1, f);
	assert(itemsRead == 1);
	fclose(f);

	debug("OK (");
	debug(buffer_.size() / 1024. / 1024.);
	debugLn(" MB)");
}

SoundFile::~SoundFile() = default;
SoundFile::SoundFile(SoundFile&& other) noexcept {
	other.load();
	*this = std::move(other);
}
SoundFile& SoundFile::operator=(SoundFile&& other) noexcept {
	sound_ = std::move(other.sound_);
	buffer_ = std::move(other.buffer_);
	return *this;
}

void SoundFile::play() {
	sound_ = std::make_shared<Sound>(buffer_);
	GetAudio().play(sound_);
}
void SoundFile::stop() {
	if (sound_) {
		GetAudio().Stop(sound_);
		sound_.reset();
	}
}
bool SoundFile::isPlaying() {
	if (sound_) {
		return sound_->isPlaying();
	}
	return false;
}

void SoundFile::loop() {
	if (!isPlaying()) {
		play();
	}
	sound_->loop();
}

void SoundFile::setPitch(float p) {
	if (sound_) {
		sound_->SetPitch(p);
	}
}
void SoundFile::setVolume(float v) {
	if (sound_) {
		sound_->setVolume(v);
	}
}

void SoundFile::load() {
}

std::shared_ptr<SoundFile> getSoundFile(const std::string& filename, std::launch policy) {
	GetAudio();
	auto i = sounds.find(filename);
	if (i == sounds.end()) { // sound hasn't been loaded yet?
		sounds[filename] = std::make_shared<SoundFile>(pathPrefix + filename, policy);
		return sounds[filename];
	}
	return i->second;
}

void play(const std::string& filename) {
	getSoundFile(filename, std::launch::deferred)->play();
}

void stop(const std::string& filename) {
	getSoundFile(filename, std::launch::async)->stop();
}

Finally loadSound(const std::string& filename) {
	auto soundFile = getSoundFile(filename, std::launch::async);
	return Finally([soundFile = std::move(soundFile)]() { soundFile->load(); });
}

bool isPlaying(const std::string& filename) {
	return getSoundFile(filename, std::launch::async)->isPlaying();
}

std::shared_ptr<SoundFile> loop(const std::string& filename) {
	auto tmp = getSoundFile(filename, std::launch::deferred);
	tmp->loop();
	return tmp;
}

void setPlaybackSpeed(float speed) {
	auto end = sounds.end();
	for (auto i = sounds.begin(); i != end; ++i) {
		i->second->setPitch(speed);
	}
}

void setVolume(float volume) {
	auto end = sounds.end();
	for (auto i = sounds.begin(); i != end; ++i) {
		i->second->setVolume(volume);
	}
	Sound::masterVolume = volume;
}

#ifdef ALC_SOFT_pause_device
void pauseAudioDevice() {
	GetAudio().pauseDevice();
}

void resumeAudioDevice() {
	GetAudio().resumeDevice();
}
#endif

Audio& GetAudio() {
	static Audio audio;
	return audio;
}

} // namespace jngl
