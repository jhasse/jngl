// Copyright 2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#pragma once

#include "../job.hpp"

namespace jngl {

class VideoRecorder : public jngl::Job {
public:
	explicit VideoRecorder(std::string_view filename);
	~VideoRecorder() override;
	VideoRecorder(const VideoRecorder&) = delete;
	VideoRecorder& operator=(const VideoRecorder&) = delete;
	VideoRecorder(VideoRecorder&&) = delete;
	VideoRecorder& operator=(VideoRecorder&&) = delete;

	/// Assumes 44.1 kHz, stereo audio input and getting called exactly getStepsPerSecond() times
	/// per second
	void fillAudioBuffer(std::unique_ptr<float[]> samples);

	void step() override;
	void draw() const override;

private:
	struct Impl;
	std::unique_ptr<Impl> impl;
};

} // namespace jngl
