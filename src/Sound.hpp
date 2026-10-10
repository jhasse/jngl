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

	/// has to be called before playing the sound, because otherwise getStream() has already been
	/// called
	void loop();
	void setVolume(float v);

	/// Has to be called before playing the sound, together with an initial setPanning(). If the
	/// sound should loop, loop() has to be called before this.
	void enablePanning();

	/// enablePanning() must have been called before
	void setPanning(float panning);
	std::shared_ptr<Stream> getStream();

	/// returns 0...1 and will reset when looping
	float progress() const;

	std::span<float> getSamples();

private:
	struct Impl;
	std::unique_ptr<Impl> impl;
};

} // namespace jngl
