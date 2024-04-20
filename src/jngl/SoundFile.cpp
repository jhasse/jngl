// Copyright 2019-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "SoundFile.hpp"

#include "../Sound.hpp"
#include "../audio.hpp"
#include "../audio/effect/pitch.hpp"
#include "../audio/effect/volume.hpp"
#include "../audio/engine.hpp"
#include "../audio/mixer.hpp"
#include "../main.hpp"
#include "debug.hpp"

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <unordered_map>

#define OV_EXCLUDE_STATIC_CALLBACKS
#include <vorbis/vorbisfile.h>

#ifdef ANDROID
#include "../android/fopen.hpp"
#endif

namespace jngl {

std::unordered_map<std::string, std::shared_ptr<SoundFile>> sounds;

Audio::Audio()
: mixer(std::make_shared<Mixer>()), pitchControl(audio::pitch(mixer)),
  volumeControl(volume(pitchControl)), engine(volumeControl) {
}
Audio::~Audio() = default;

void Audio::play(std::shared_ptr<Sound> sound) {
	mixer->add(sound->getStream());
	sounds_.erase(std::remove_if(sounds_.begin(), sounds_.end(),
	                             [](const auto& s) { return !s->isPlaying(); }),
	              sounds_.end());
	sounds_.emplace_back(std::move(sound));
}

void Audio::stop(std::shared_ptr<Sound>& sound) {
	mixer->remove(sound->getStream().get());
	if (auto i = std::find(sounds_.begin(), sounds_.end(), sound); i != sounds_.end()) {
		sounds_.erase(i);
	}
}

void Audio::increasePauseDeviceCount() {
	if (pauseDeviceCount == 0) {
		engine.setPause(true);
	}
	++pauseDeviceCount;
}

void Audio::decreasePauseDeviceCount() {
	assert(pauseDeviceCount > 0);
	--pauseDeviceCount;
	if (pauseDeviceCount == 0) {
		engine.setPause(false);
	}
}

void Audio::setPitch(float pitch) {
	pitchControl->pitch(pitch);
}
float Audio::getVolume() const {
	return volumeControl->gain();
}
void Audio::setVolume(float volume) {
	volumeControl->gain(volume);
}
std::shared_ptr<Mixer> Audio::getMixer() {
	return mixer;
}
void Audio::step() {
	engine.step();
}

SoundFile::SoundFile(const std::string& filename, std::launch) {
	debug("Decoding ");
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

	OggVorbis_File oggFile;
	if (ov_open(f, &oggFile, nullptr, 0) != 0) {
		fclose(f); // If [and only if] an ov_open() call fails, the application must explicitly
		           // fclose() the FILE * pointer itself.
		throw std::runtime_error("Could not open OGG file (" + filename + ").");
	}
	Finally cleanup(
	    [&oggFile]()
	    {
		ov_clear(&oggFile); /* calls fclose */
	});

	const vorbis_info* const pInfo = ov_info(&oggFile, -1);
	frequency = pInfo->rate;

	int bitStream;
	while (true) {
		float** buffer = nullptr;
		auto samples_read = ov_read_float(&oggFile, &buffer, 1024, &bitStream);
		if (samples_read == 0) {
			break;
		}
		if (samples_read < 0) {
			throw std::runtime_error("Error decoding OGG file (" + filename + ").");
		}

		size_t start = buffer_.size();
		buffer_.resize(start + samples_read * 2);
		for (std::size_t i = samples_read; i > 0;) {
			i -= 1;
			auto tmp = buffer[0][i];
			buffer_[start + i * 2 + 0] = tmp;
			buffer_[start + i * 2 + 1] = buffer[pInfo->channels == 1 ? 0 : 1][i];
		}
	}

	debug("OK (");
	debug(buffer_.size() * sizeof(float) / 1024. / 1024.);
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
	frequency = other.frequency;
	return *this;
}

void SoundFile::play() {
	sound_ = std::make_shared<Sound>(buffer_, frequency);
	Audio::handle().play(sound_);
}
void SoundFile::stop() {
	if (sound_) {
		Audio::handle().stop(sound_);
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
	if (sound_ && sound_->isLooping()) {
		return;
	}
	sound_ = std::make_shared<Sound>(buffer_, frequency);
	sound_->loop();
	Audio::handle().play(sound_);
}

void SoundFile::setVolume(float v) {
	if (sound_) {
		sound_->setVolume(v);
	}
}

void SoundFile::load() {
}

std::shared_ptr<SoundFile> getSoundFile(const std::string& filename, std::launch policy) {
	Audio::handle();
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
	Audio::handle().setPitch(speed);
}

float getVolume() {
	return Audio::handle().getVolume();
}

void setVolume(float volume) {
	Audio::handle().setVolume(volume);
}

Finally pauseAudio() {
	if (auto audio = Audio::handleIfAlive()) {
		audio->increasePauseDeviceCount();
		return Finally([audio]() { audio->decreasePauseDeviceCount(); });
	}
	return Finally(nullptr);
}

Audio& GetAudio() {
	return Audio::handle();
}

std::shared_ptr<Mixer> getMixer() {
	return Audio::handle().getMixer();
}

} // namespace jngl
