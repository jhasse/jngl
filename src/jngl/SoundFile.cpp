// Copyright 2019-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "SoundFile.hpp"

#include "../Sound.hpp"
#include "../audio.hpp"
#include "../audio/constants.hpp"
#include "../audio/effect/pitch.hpp"
#include "../audio/effect/volume.hpp"
#include "../audio/engine.hpp"
#include "../audio/mixer.hpp"
#include "../log.hpp"
#include "../main.hpp"
#include "Channel.hpp"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <stdexcept>
#include <unordered_map>

#define OV_EXCLUDE_STATIC_CALLBACKS
#include <vorbis/vorbisfile.h>

#ifdef ANDROID
#include "../android/fopen.hpp"
#endif

namespace jngl {

Audio::Audio()
: mixer(std::make_shared<Mixer>()), pitchControl(audio::pitch(mixer)),
  volumeControl(volume(pitchControl)), engine(volumeControl) {
}
Audio::~Audio() = default;

void Audio::play(Channel& channel, std::shared_ptr<Sound> sound) {
	channel.add(sound->getStream());
	sounds_.erase(std::remove_if(sounds_.begin(), sounds_.end(),
	                             [](const auto& s) { return !s->isPlaying(); }),
	              sounds_.end());
	sounds_.emplace_back(std::move(sound));
}

void Audio::stop(Channel& channel, std::shared_ptr<Sound>& sound) {
	channel.remove(sound->getStream().get());
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

Channel& Audio::getMainChannel() {
	if (!mainChannel) {
		mainChannel = std::make_unique<Channel>();
	}
	return *mainChannel;
}

void Audio::registerChannel(std::shared_ptr<Stream> stream) {
	this->mixer->add(std::move(stream));
}

void Audio::unregisterChannel(const Stream& stream) {
	mixer->remove(&stream);
}

void Audio::step() {
	engine.step();
}

SoundFile::SoundFile(const std::string& filename, std::launch) {
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
	if (pInfo->rate != jngl::audio::frequency) {
		float resampleFactor =
		    static_cast<float>(jngl::audio::frequency) / static_cast<float>(pInfo->rate);
		auto newSize = static_cast<size_t>(static_cast<float>(buffer_.size()) * resampleFactor);
		if (newSize % 2 != 0) {
			++newSize;
		}
		std::vector<float> resampledData(newSize);
		for (size_t i = 0; i < newSize / 2; ++i) {
			float originalIndex = static_cast<float>(i) / resampleFactor;
			auto index = static_cast<size_t>(originalIndex);
			float fraction = originalIndex - static_cast<float>(index);
			{
				size_t originalLeftIndex = index * 2;
				const float b =
				    (originalLeftIndex + 2 < buffer_.size()) ? buffer_[originalLeftIndex + 2] : 0;
				resampledData[i * 2] =
				    buffer_[originalLeftIndex] * (1.0f - fraction) + b * fraction;
			}
			{
				const size_t originalRightIndex = index * 2 + 1;
				const float b =
				    (originalRightIndex + 2 < buffer_.size()) ? buffer_[originalRightIndex + 2] : 0;
				resampledData[i * 2 + 1] =
				    buffer_[originalRightIndex] * (1.0f - fraction) + b * fraction;
			}
		}
		buffer_ = std::move(resampledData);
	}

	internal::debug("Decoded {} ({:.2f} MB, {})", filename,
	                buffer_.size() * sizeof(float) / 1024. / 1024.,
#if !defined(__APPLE__) /* FIXME: Remove when AppleClang's libc++ supports this C++20 feature */   \
    && defined(__GNUC__) && __GNUC__ > 13 // Ubuntu 22.04's GCC doesn't fully support C++20
	                std::chrono::duration_cast<std::chrono::seconds>(length())
#else
	                "unknown length"
#endif
	);
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
	play(Channel::main());
}

void SoundFile::play(Channel& channel) {
	sound_ = std::make_shared<Sound>(buffer_);
	Audio::handle().play(channel, sound_);
}

void SoundFile::stop() {
	stop(Channel::main());
}

void SoundFile::stop(Channel& channel) {
	if (sound_) {
		Audio::handle().stop(channel, sound_);
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
	loop(Channel::main());
}

void SoundFile::loop(Channel& channel) {
	if (sound_ && sound_->isLooping()) {
		return;
	}
	sound_ = std::make_shared<Sound>(buffer_);
	sound_->loop();
	Audio::handle().play(channel, sound_);
}

void SoundFile::setVolume(float v) {
	if (sound_) {
		sound_->setVolume(v);
	}
}

void SoundFile::load() {
}

std::chrono::milliseconds SoundFile::length() const {
	return std::chrono::milliseconds{ buffer_.size() * 1000 / jngl::audio::frequency /
		                              2 /* stereo */ };
}

float SoundFile::progress() const {
	return sound_ ? sound_->progress() : 0;
}

std::shared_ptr<SoundFile> Audio::getSoundFile(const std::string& filename, std::launch policy) {
	auto i = soundFiles.find(filename);
	if (i == soundFiles.end()) { // sound hasn't been loaded yet?
		soundFiles[filename] = std::make_shared<SoundFile>(pathPrefix + filename, policy);
		return soundFiles[filename];
	}
	return i->second;
}

void play(const std::string& filename) {
	Audio::handle().getSoundFile(filename, std::launch::deferred)->play();
}

void stop(const std::string& filename) {
	Audio::handle().getSoundFile(filename, std::launch::async)->stop();
}

Finally loadSound(const std::string& filename) {
	auto soundFile = Audio::handle().getSoundFile(filename, std::launch::async);
	return Finally([soundFile = std::move(soundFile)]() { soundFile->load(); });
}

bool isPlaying(const std::string& filename) {
	return Audio::handle().getSoundFile(filename, std::launch::async)->isPlaying();
}

std::shared_ptr<SoundFile> loop(const std::string& filename) {
	auto tmp = Audio::handle().getSoundFile(filename, std::launch::deferred);
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

} // namespace jngl
