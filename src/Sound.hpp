// Copyright 2019-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include <memory>
#include <span>
#include <vector>

namespace jngl {

struct Stream;
struct SoundParams;

class Sound {
public:
	explicit Sound(std::shared_ptr<std::vector<float>> bufferData);
	Sound(const Sound&) = delete;
	Sound& operator=(const Sound&) = delete;
	Sound(Sound&&) = default;
	Sound& operator=(Sound&&) = default;
	~Sound();
	bool isPlaying() const;
	bool isLooping() const;
	void loop();
	void setVolume(float v);
	std::shared_ptr<Stream> getStream();

	/// returns 0...1 and will reset when looping
	float progress() const;

	std::span<float> getSamples();

private:
	struct Impl;
	std::unique_ptr<Impl> impl;
};

} // namespace jngl
