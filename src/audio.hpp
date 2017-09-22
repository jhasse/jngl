// Copyright 2010-2017 Jan Niklas Hasse <jhasse@gmail.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#ifdef WEAK_LINKING_OPENAL
 #include "win32/openal.hpp"
#else
 #ifdef __APPLE__
  #include <OpenAL/al.h>
  #include <OpenAL/alc.h>
 #else
  #include <AL/al.h>
  #include <AL/alc.h>
 #endif
 #define OV_EXCLUDE_STATIC_CALLBACKS
 #include <vorbis/vorbisfile.h>
#endif

#include <string>
#include <vector>
#include <memory>

namespace jngl {
	void LoadSound(const std::string&);

	class Sound {
	public:
		Sound(ALenum format, std::vector<char>& bufferData, ALsizei freq);
		Sound(const Sound&) = delete;
		Sound& operator=(const Sound&) = delete;
		~Sound();
		bool IsPlaying();
		bool Stopped();
		void SetPitch(float p);
		void setVolume(float v);

		static float masterVolume;
	private:
		ALuint buffer_;
		ALuint source_;
	};

	class SoundFile {
	public:
		SoundFile(const std::string& filename);
		SoundFile(const SoundFile&) = delete;
		SoundFile& operator=(const SoundFile&) = delete;
		void Play();
		void Stop();
		bool IsPlaying();
		void SetPitch(float p);
		void setVolume(float v);
	private:
		std::shared_ptr<Sound> sound_;
		ALenum format;
		ALsizei freq;
		std::vector<char> buffer_;
	};
}
