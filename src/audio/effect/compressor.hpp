#pragma once

#include "../stream.hpp"
#include "../constants.hpp"

namespace psemek::audio
{

	stream_ptr compressor(stream_ptr stream, float volume_threshold = from_db(-2.f), float strength = 0.9f, float envelope_attack = 0.01f, float envelope_release = 1.f,
		float knee = from_db(1.f));

	stream_ptr limiter(stream_ptr stream, float volume_threshold = from_db(0.f), float envelope_attack = 0.01f, float envelope_release = 1.f, float knee = 0.f);

}
