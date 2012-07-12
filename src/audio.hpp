/*
Copyright 2010-2011 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

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
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

namespace jngl
{
	void LoadSound(const std::string&);

	class Sound : boost::noncopyable {
	public:
		Sound(ALenum format, std::vector<char>& bufferData, ALsizei freq);
		~Sound();
		bool IsPlaying();
		bool Stopped();
		void SetPitch(float p);
		void setVolume(float v);
	private:
		ALuint buffer_;
		ALuint source_;
	};
	
	class SoundFile : boost::noncopyable {
	public:
		SoundFile(const std::string& filename);
		void Play();
		void Stop();
		bool IsPlaying();
		void SetPitch(float p);
		void setVolume(float v);
	private:
		boost::shared_ptr<Sound> sound_;
		ALint state;
		ALenum format;
		ALsizei freq;
		std::vector<char> buffer_;
	};
};
