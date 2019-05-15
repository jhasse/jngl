// Copyright 2010-2019 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include <string>
#include <vector>
#include <memory>

namespace jngl {
	void LoadSound(const std::string&);
	void checkAlError();
	void pauseAudioDevice();
	void resumeAudioDevice();

	class Audio;

	Audio& GetAudio();

	struct SoundParams;

	class Sound {
	public:
		Sound(const SoundParams&, std::vector<char>& bufferData);
		Sound(const Sound&) = delete;
		Sound& operator=(const Sound&) = delete;
		~Sound();
		bool IsPlaying();
		void loop();
		bool Stopped();
		void SetPitch(float p);
		void setVolume(float v);

		static float masterVolume;
	private:
		struct Impl;
		std::unique_ptr<Impl> impl;
	};
}
