// Copyright 2019-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include <memory>
#include <vector>

namespace jngl {

struct Stream;
struct SoundParams;

class Sound {
public:
	Sound(std::vector<float>& bufferData, long frequency);
	Sound(const Sound&) = delete;
	Sound& operator=(const Sound&) = delete;
	Sound(Sound&&) = default;
	Sound& operator=(Sound&&) = default;
	~Sound();
	bool isPlaying() const;
	bool isLooping() const;
	void loop();
	bool isStopped() const;
	void SetPitch(float p);
	void setVolume(float v);
	std::shared_ptr<Stream> getStream();

private:
	struct Impl;
	std::unique_ptr<Impl> impl;
};

} // namespace jngl
