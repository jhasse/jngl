// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#pragma once

#include "../Stream.hpp"

#include <array>
#include <atomic>
#include <memory>

namespace jngl::audio {

/// Pans by attenuating (interaural level difference) and delaying (interaural time difference) the
/// channel further away from the sound source. Changing the panning while playing smoothly changes
/// the delay, which results in a Doppler effect instead of clicks.
///
/// When the underlying stream ends, the samples still in the delay lines are played, too. Therefore
/// this must not be wrapped by a loop, as that would cause short gaps.
class PanningControl : public Stream {
public:
	explicit PanningControl(std::shared_ptr<Stream> stream);

	/// [-1, 1], -1 = left, 0 = center, 1 = right. Can be called from any thread.
	void setPanning(float panning);

	void rewind() override;
	bool isPlaying() const override;
	size_t read(float* data, size_t sample_count) override;

private:
	/// Maximum delay of one channel in frames, about 0.7 ms at 44.1 kHz which is the maximum delay a
	/// human head produces
	constexpr static float MAX_DELAY = 30.f;

	/// How fast the delay may change in frames per frame. This limits the pitch shift of the
	/// Doppler effect to ±5 %.
	constexpr static float MAX_DELAY_CHANGE = 0.05f;

	/// Attenuation of the channel further away from the sound source at panning ±1. A real head
	/// causes about 3 dB for low and up to 20 dB for high frequencies.
	constexpr static float MAX_ATTENUATION_DB = 10.f;

	/// How fast the gain may change per frame, so that changing the panning doesn't click
	constexpr static float MAX_GAIN_CHANGE = 0.001f;

	/// Must be a power of two and bigger than MAX_DELAY + 1
	constexpr static size_t BUFFER_SIZE = 64;

	/// After this many frames of silence all samples have left the delay lines
	constexpr static size_t TAIL_FRAMES = static_cast<size_t>(MAX_DELAY) + 2;

	std::atomic<float> panning{ 0.f };
	std::shared_ptr<Stream> stream;

	/// Ring buffers of past samples for each channel
	std::array<std::array<float, BUFFER_SIZE>, 2> history{};
	size_t writePosition = 0;

	/// Frames left to play after the underlying stream has ended
	size_t remainingTailFrames = TAIL_FRAMES;

	/// Current delay of each channel in frames, slides towards the target given by panning
	std::array<float, 2> delay{ 0.f, 0.f };

	/// Current gain of each channel, slides towards the target given by panning
	std::array<float, 2> gain{ 1.f, 1.f };

	bool initialized = false;
};

} // namespace jngl::audio
