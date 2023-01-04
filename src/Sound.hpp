// Copyright 2019-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "audio/stream.hpp"
#include <memory>
#include <vector>

using namespace psemek; // FIXME

namespace jngl {
struct SoundParams;

class Sound {
public:
	Sound(std::vector<char>& bufferData);
	Sound(const Sound&) = delete;
	Sound& operator=(const Sound&) = delete;
	Sound(Sound&&) = default;
	Sound& operator=(Sound&&) = default;
	~Sound();
	bool isPlaying() const;
	void loop();
	bool isStopped() const;
	void SetPitch(float p);
	void setVolume(float v);
	std::shared_ptr<audio::stream> getStream();

	static float masterVolume;

private:
	struct Impl;
	std::unique_ptr<Impl> impl;
};

} // namespace jngl
