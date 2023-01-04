#pragma once

#include <cstdint>
#include <cmath>

namespace psemek::audio
{

	constexpr int frequency = 44100;
	constexpr float inv_frequency = 1.f / frequency;

	inline std::int64_t seconds_to_samples(float seconds)
	{
		return static_cast<std::int64_t>(2 * std::round(seconds * frequency));
	}

	inline float samples_to_seconds(std::int64_t samples)
	{
		return static_cast<float>(samples) * 0.5f * inv_frequency;
	}

	inline float to_db(float amplitude)
	{
		return 10.f * std::log10(amplitude);
	}

	inline float from_db(float db)
	{
		return std::pow(10.f, db / 10.f);
	}

}
