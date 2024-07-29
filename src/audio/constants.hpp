// Copyright 2023-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
// Based on the audio implementation of the psemek engine, see
// https://lisyarus.github.io/blog/programming/2022/10/15/audio-mixing.html
#pragma once

#include <cmath>
#include <cstdint>

namespace jngl::audio {

#ifdef __EMSCRIPTEN__
constexpr int frequency = 48000;
#else
constexpr int frequency = 44100;
#endif
constexpr float inv_frequency = 1.f / frequency;

inline std::int64_t seconds_to_samples(float seconds) {
	return static_cast<std::int64_t>(2 * std::round(seconds * frequency));
}

inline float samples_to_seconds(std::int64_t samples) {
	return static_cast<float>(samples) * 0.5f * inv_frequency;
}

inline float to_db(float amplitude) {
	return 10.f * std::log10(amplitude);
}

inline float from_db(float db) {
	return std::pow(10.f, db / 10.f);
}

} // namespace jngl::audio
