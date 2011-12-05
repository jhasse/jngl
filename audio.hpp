/*
Copyright 2010-2011 Jan Niklas Hasse <jhasse@gmail.com>

This file is part of JNGL.

JNGL is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

JNGL is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with JNGL.  If not, see <http://www.gnu.org/licenses/>.
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
 #include <vorbis/vorbisfile.h>
#endif

#include <string>
#include <vector>
#include <memory>
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
		void SetVolume(float v);
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
		void SetVolume(float v);
	private:
		std::shared_ptr<Sound> sound_;
		ALint state;
		ALenum format;
		ALsizei freq;
		std::vector<char> buffer_;
	};
};
