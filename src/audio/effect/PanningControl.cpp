// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "PanningControl.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>

namespace jngl::audio {

PanningControl::PanningControl(std::shared_ptr<Stream> stream) : stream(std::move(stream)) {
	static_assert((BUFFER_SIZE & (BUFFER_SIZE - 1)) == 0);
	static_assert(BUFFER_SIZE > MAX_DELAY + 1);
}

void PanningControl::setPanning(float panning) {
	this->panning = std::clamp(panning, -1.f, 1.f);
}

void PanningControl::rewind() {
	stream->rewind();
	remainingTailFrames = TAIL_FRAMES;
}

bool PanningControl::isPlaying() const {
	return stream->isPlaying();
}

size_t PanningControl::read(float* data, size_t sample_count) {
	assert(sample_count % 2 == 0);
	auto result = stream->read(data, sample_count);
	if (result < sample_count) {
		// The stream has ended, but the delayed channel might still contain samples. Feed silence
		// into the delay lines until they're empty:
		const auto tailFrames = std::min((sample_count - result) / 2, remainingTailFrames);
		std::fill_n(data + result, tailFrames * 2, 0.f);
		result += tailFrames * 2;
		remainingTailFrames -= tailFrames;
	}

	const float pan = panning;
	// sound on the right reaches the left ear later and vice versa:
	const std::array<float, 2> targetDelay{
		std::max(pan, 0.f) * MAX_DELAY,
		std::max(-pan, 0.f) * MAX_DELAY,
	};
	// the ear closer to the sound source keeps full volume, the other one is shadowed by the head:
	const float farGain = std::pow(10.f, -std::fabs(pan) * MAX_ATTENUATION_DB / 20.f);
	const std::array<float, 2> targetGain{ pan > 0 ? farGain : 1.f, pan < 0 ? farGain : 1.f };
	if (!initialized) {
		delay = targetDelay;
		gain = targetGain;
		initialized = true;
	}

	for (size_t frame = 0; frame < result / 2; ++frame) {
		for (size_t channel = 0; channel < 2; ++channel) {
			auto& buffer = history[channel];
			buffer[writePosition] = data[frame * 2 + channel];

			delay[channel] += std::clamp(targetDelay[channel] - delay[channel], -MAX_DELAY_CHANGE,
			                             MAX_DELAY_CHANGE);

			// fractional delay using linear interpolation between the two nearest samples:
			const auto whole = static_cast<size_t>(delay[channel]);
			const float fraction = delay[channel] - static_cast<float>(whole);
			const float newer = buffer[(writePosition - whole) & (BUFFER_SIZE - 1)];
			const float older = buffer[(writePosition - whole - 1) & (BUFFER_SIZE - 1)];
			gain[channel] += std::clamp(targetGain[channel] - gain[channel], -MAX_GAIN_CHANGE,
			                            MAX_GAIN_CHANGE);

			data[frame * 2 + channel] = (newer + fraction * (older - newer)) * gain[channel];
		}
		writePosition = (writePosition + 1) & (BUFFER_SIZE - 1);
	}
	return result;
}

} // namespace jngl::audio
