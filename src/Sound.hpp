// Copyright 2019 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include <memory>
#include <vector>

namespace jngl {
struct SoundParams;

class Sound {
public:
	Sound(const SoundParams&, std::vector<char>& bufferData);
	Sound(const Sound&) = delete;
	Sound& operator=(const Sound&) = delete;
	~Sound();
	bool IsPlaying();
	void loop();
	bool isStopped() const;
	void SetPitch(float p);
	void setVolume(float v);

	static float masterVolume;

private:
	struct Impl;
	std::unique_ptr<Impl> impl;
};

} // namespace jngl