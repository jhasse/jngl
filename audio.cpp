/*
Copyright 2009-2011 Jan Niklas Hasse <jhasse@gmail.com>

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

#if (defined(__MINGW32__) || defined(__MINGW64__)) && (__GNUC__ == 4) && (__GNUC_MINOR__ == 4) && (__GNUC_PATCHLEVEL__ == 0)
// workaround a mingw bug, http://sourceforge.net/tracker/index.php?func=detail&aid=2373234&group_id=2435&atid=102435
#include <cstdlib>
#include <cstdarg>
int swprintf (wchar_t *, size_t, const wchar_t *, ...);
int vswprintf(wchar_t *, const wchar_t *, va_list);
#endif

#include "jngl.hpp"
#include "debug.hpp"
#include "audio.hpp"

#include <cstdio>
#include <stdexcept>
#include <map>
#include <algorithm>

namespace jngl
{
	Sound::Sound(ALenum format, std::vector<char>& bufferData, ALsizei freq) : source_(0)
	{
		alGenBuffers(1, &buffer_);
		alGenSources(1, &source_);
		alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
		alSource3f(source_, AL_POSITION, 0.0f, 0.0f, 0.0f);
		alBufferData(buffer_, format, &bufferData[0], static_cast<ALsizei>(bufferData.size()), freq);
		alSourcei(source_, AL_BUFFER, buffer_);
		alSourcePlay(source_);
	}
	Sound::~Sound()
	{
		Debug("freeing sound buffer ... ");
		alSourceStop(source_);
		alSourceUnqueueBuffers(source_, 1, &buffer_);
		alDeleteSources(1, &source_);
		alDeleteBuffers(1, &buffer_);
		Debug("OK\n");
	}
	bool Sound::IsPlaying()
	{
		ALint state;
		alGetSourcei(source_, AL_SOURCE_STATE, &state);
		return state == AL_PLAYING;
	}
	bool Sound::Stopped()
	{
		ALint state;
		alGetSourcei(source_, AL_SOURCE_STATE, &state);
		return state == AL_STOPPED;
	}
	void Sound::SetPitch(float p)
	{
		alSourcef(source_, AL_PITCH, p);
	}
	void Sound::SetVolume(float v) {
		alSourcef(source_, AL_GAIN, v);
	}

	std::map<std::string, boost::shared_ptr<SoundFile> > sounds;

	class Audio : boost::noncopyable {
	public:
		Audio() : device_(0), context_(0)
		{
			device_ = alcOpenDevice(0);
			if(!device_)
			{
				throw std::runtime_error("Could not open audio device.");
			}
			context_ = alcCreateContext(device_, 0);
			if(context_)
			{
				alcMakeContextCurrent(context_);
			}
			else
			{
				throw std::runtime_error("Could not create audio context.");
			}
		}
		~Audio()
		{
			sounds_.clear();
			sounds.clear();
			alcMakeContextCurrent(0);
			alcDestroyContext(context_);
			alcCloseDevice(device_);
		}
		static bool IsStopped(boost::shared_ptr<Sound>& s)
		{
			return s->Stopped();
		}
		void Play(boost::shared_ptr<Sound>& sound)
		{
			sounds_.erase(remove_if(sounds_.begin(), sounds_.end(), IsStopped), sounds_.end());
			sounds_.push_back(sound);
		}
		void Stop(boost::shared_ptr<Sound>& sound)
		{
			std::vector<boost::shared_ptr<Sound> >::iterator i;
			if((i = std::find(sounds_.begin(), sounds_.end(), sound)) != sounds_.end()) // sound hasn't been loaded yet?
			{
				sounds_.erase(i);
			}
		}
	private:
		std::vector<boost::shared_ptr<Sound> > sounds_;
		ALCdevice* device_;
		ALCcontext* context_;
	};

	Audio& GetAudio()
	{
		static Audio audio_;
		return audio_;
	}

	SoundFile::SoundFile(const std::string& filename) : sound_((Sound*)0)
	{
		Debug("Decoding "); Debug(filename); Debug(" ... ");
		// based on http://www.gamedev.net/reference/articles/article2031.asp
		FILE* f = fopen(filename.c_str(), "rb");
		if(!f)
		{
			throw std::runtime_error("File not found (" + filename + ").");
		}

		OggVorbis_File oggFile;
		if(ov_open(f, &oggFile, 0, 0) != 0)
		{
			throw std::runtime_error("Could not open OGG file (" + filename + ").");
		}

		vorbis_info* pInfo;
		pInfo = ov_info(&oggFile, -1);
		if(pInfo->channels == 1)
		{
			format = AL_FORMAT_MONO16;
		}
		else
		{
			format = AL_FORMAT_STEREO16;
		}
		freq = static_cast<ALsizei>(pInfo->rate);

		const int bufferSize = 32768;
		char array[bufferSize]; // 32 KB buffers
		const int endian = 0; // 0 for Little-Endian, 1 for Big-Endian
		int bitStream;
		long bytes;
		do
		{
			bytes = ov_read(&oggFile, array, bufferSize, endian, 2, 1, &bitStream);

			if (bytes < 0)
			{
				ov_clear(&oggFile);
				throw std::runtime_error("Error decoding OGG file.");
			}

			buffer_.insert(buffer_.end(), array, array + bytes);
		}
		while(bytes > 0);

		ov_clear(&oggFile);
		Debug("OK\n");
	}
	void SoundFile::Play()
	{
		sound_.reset(new Sound(format, buffer_, freq));
		GetAudio().Play(sound_);
	}
	void SoundFile::Stop()
	{
		if(sound_)
		{
			GetAudio().Stop(sound_);
			sound_.reset((Sound*)0);
		}
	}
	bool SoundFile::IsPlaying()
	{
		if(sound_)
		{
			return sound_->IsPlaying();
		}
		return false;
	}
	void SoundFile::SetPitch(float p)
	{
		if(sound_)
		{
			sound_->SetPitch(p);
		}
	}
	void SoundFile::SetVolume(float v) {
		if(sound_) {
			sound_->SetVolume(v);
		}
	}

	bool IsOpenALInstalled()
	{
#ifdef WEAK_LINKING_OPENAL
		try
		{
			GetAudio();
		}
		catch(WeakLinkingError& e)
		{
			Debug(e.what()); Debug("\n");
			return false;
		}
#endif
		return true;
	}

	SoundFile& GetSoundFile(const std::string& filename)
	{
		GetAudio();
		auto i = sounds.find(filename);
		if(i == sounds.end()) // sound hasn't been loaded yet?
		{
			sounds[filename].reset(new SoundFile(filename));
			return *(sounds[filename]);
		}
		return *(i->second);
	}

	void Play(const std::string& filename)
	{
		GetSoundFile(filename).Play();
	}

	void Stop(const std::string& filename)
	{
		GetSoundFile(filename).Stop();
	}

	void LoadSound(const std::string& filename)
	{
		GetSoundFile(filename);
	}

	bool IsPlaying(const std::string& filename)
	{
		return GetSoundFile(filename).IsPlaying();
	}

	void SetPlaybackSpeed(float speed) {
		auto end = sounds.end();
		for(auto i = sounds.begin(); i != end; ++i) {
			i->second->SetPitch(speed);
		}
	}

	void SetVolume(float volume) {
		auto end = sounds.end();
		for(auto i = sounds.begin(); i != end; ++i) {
			i->second->SetVolume(volume);
		}
	}
};
