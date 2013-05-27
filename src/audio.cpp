/*
Copyright 2009-2013 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "jngl.hpp"
#include "audio.hpp"
#include "main.hpp"

#include <cstdio>
#include <stdexcept>
#include <boost/unordered_map.hpp>
#include <algorithm>

namespace jngl {
	float Sound::masterVolume = 1.0f;

	Sound::Sound(ALenum format, std::vector<char>& bufferData, ALsizei freq) : source_(0) {
		alGenBuffers(1, &buffer_);
		alGenSources(1, &source_);
		alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
		alSource3f(source_, AL_POSITION, 0.0f, 0.0f, 0.0f);
		alBufferData(buffer_, format, &bufferData[0], static_cast<ALsizei>(bufferData.size()), freq);
		alSourcei(source_, AL_BUFFER, buffer_);
		alSourcePlay(source_);
		setVolume(masterVolume);
	}
	Sound::~Sound() {
		debug("freeing sound buffer ... ");
		alSourceStop(source_);
		alSourceUnqueueBuffers(source_, 1, &buffer_);
		alDeleteSources(1, &source_);
		alDeleteBuffers(1, &buffer_);
		debug("OK\n");
	}
	bool Sound::IsPlaying() {
		ALint state;
		alGetSourcei(source_, AL_SOURCE_STATE, &state);
		return state == AL_PLAYING;
	}
	bool Sound::Stopped() {
		ALint state;
		alGetSourcei(source_, AL_SOURCE_STATE, &state);
		return state == AL_STOPPED;
	}
	void Sound::SetPitch(float p) {
		alSourcef(source_, AL_PITCH, p);
	}
	void Sound::setVolume(float v) {
		alSourcef(source_, AL_GAIN, v);
	}

	boost::unordered_map<std::string, std::shared_ptr<SoundFile>> sounds;

	class Audio : boost::noncopyable {
	public:
		Audio() : device_(0), context_(0) {
			device_ = alcOpenDevice(0);
			if (!device_) {
				throw std::runtime_error("Could not open audio device.");
			}
			context_ = alcCreateContext(device_, 0);
			if (context_) {
				alcMakeContextCurrent(context_);
			} else {
				throw std::runtime_error("Could not create audio context.");
			}
		}
		~Audio() {
			sounds_.clear();
			sounds.clear();
			alcMakeContextCurrent(0);
			alcDestroyContext(context_);
			alcCloseDevice(device_);
		}
		static bool IsStopped(std::shared_ptr<Sound>& s) {
			return s->Stopped();
		}
		void Play(std::shared_ptr<Sound>& sound) {
			sounds_.erase(std::remove_if(sounds_.begin(), sounds_.end(), IsStopped), sounds_.end());
			sounds_.push_back(sound);
		}
		void Stop(std::shared_ptr<Sound>& sound) {
			std::vector<std::shared_ptr<Sound>>::iterator i;
			if ((i = std::find(sounds_.begin(), sounds_.end(), sound)) != sounds_.end()) {
				sounds_.erase(i);
			}
		}
	private:
		std::vector<std::shared_ptr<Sound>> sounds_;
		ALCdevice* device_;
		ALCcontext* context_;
	};

	Audio& GetAudio();

	SoundFile::SoundFile(const std::string& filename) : sound_(nullptr) {
		debug("Decoding "); debug(filename); debug(" ... ");
		// based on http://www.gamedev.net/reference/articles/article2031.asp
		FILE* f = fopen(filename.c_str(), "rb");
		if (!f) {
			throw std::runtime_error("File not found (" + filename + ").");
		}

		OggVorbis_File oggFile;
		if (ov_open(f, &oggFile, 0, 0) != 0) {
			throw std::runtime_error("Could not open OGG file (" + filename + ").");
		}

		vorbis_info* pInfo;
		pInfo = ov_info(&oggFile, -1);
		if (pInfo->channels == 1) {
			format = AL_FORMAT_MONO16;
		} else {
			format = AL_FORMAT_STEREO16;
		}
		freq = static_cast<ALsizei>(pInfo->rate);

		const int bufferSize = 32768;
		char array[bufferSize]; // 32 KB buffers
		const int endian = 0; // 0 for Little-Endian, 1 for Big-Endian
		int bitStream;
		long bytes;
		do {
			bytes = ov_read(&oggFile, array, bufferSize, endian, 2, 1, &bitStream);

			if (bytes < 0) {
				ov_clear(&oggFile);
				throw std::runtime_error("Error decoding OGG file (" + filename + ").");
			}

			buffer_.insert(buffer_.end(), array, array + bytes);
		} while(bytes > 0);

		ov_clear(&oggFile);
		debug("OK\n");
	}
	void SoundFile::Play() {
		sound_.reset(new Sound(format, buffer_, freq));
		GetAudio().Play(sound_);
	}
	void SoundFile::Stop() {
		if (sound_) {
			GetAudio().Stop(sound_);
			sound_.reset((Sound*)0);
		}
	}
	bool SoundFile::IsPlaying() {
		if (sound_) {
			return sound_->IsPlaying();
		}
		return false;
	}
	void SoundFile::SetPitch(float p) {
		if (sound_) {
			sound_->SetPitch(p);
		}
	}
	void SoundFile::setVolume(float v) {
		if (sound_) {
			sound_->setVolume(v);
		}
	}

	bool isOpenALInstalled() {
#ifdef WEAK_LINKING_OPENAL
		try {
			GetAudio();
		} catch(WeakLinkingError& e) {
			debug(e.what()); debug("\n");
			return false;
		}
#endif
		return true;
	}

	SoundFile& GetSoundFile(const std::string& filename) {
		GetAudio();
		auto i = sounds.find(filename);
		if (i == sounds.end()) { // sound hasn't been loaded yet?
			sounds[filename].reset(new SoundFile(pathPrefix + filename));
			return *(sounds[filename]);
		}
		return *(i->second);
	}

	void play(const std::string& filename) {
		GetSoundFile(filename).Play();
	}

	void stop(const std::string& filename) {
		GetSoundFile(filename).Stop();
	}

	void loadSound(const std::string& filename) {
		GetSoundFile(filename);
	}

	bool isPlaying(const std::string& filename)	{
		return GetSoundFile(filename).IsPlaying();
	}

	void setPlaybackSpeed(float speed) {
		auto end = sounds.end();
		for (auto i = sounds.begin(); i != end; ++i) {
			i->second->SetPitch(speed);
		}
	}

	void setVolume(float volume) {
		auto end = sounds.end();
		for (auto i = sounds.begin(); i != end; ++i) {
			i->second->setVolume(volume);
		}
		Sound::masterVolume = volume;
	}

	float getVolume() {
		return Sound::masterVolume;
	}

	Audio& GetAudio() {
		static Audio audio_;
		return audio_;
	}
};
