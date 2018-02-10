// Copyright 2010-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include <string>
#include <vector>
#include <memory>

namespace jngl {
	void LoadSound(const std::string&);

	class Sound {
	public:
		struct Params;

		Sound(const Params&, std::vector<char>& bufferData);
		Sound(const Sound&) = delete;
		Sound& operator=(const Sound&) = delete;
		~Sound();
		bool IsPlaying();
		bool Stopped();
		void SetPitch(float p);
		void setVolume(float v);

		static float masterVolume;
	private:
		struct Impl;
		std::unique_ptr<Impl> impl;
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
		std::unique_ptr<Sound::Params> params;
		std::vector<char> buffer_;
	};
}
