#pragma once

#include "../stream.hpp"

namespace psemek::audio
{

	struct distortion_control
		: stream
	{
		virtual float strength() const = 0;
		virtual float strength(float value) = 0;
	};

	stream_ptr distortion(stream_ptr stream, float strength = 100.f);

}
