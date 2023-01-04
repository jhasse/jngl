#pragma once

#include "../stream.hpp"

namespace psemek::audio
{

	struct pitch_control
		: stream
	{
		virtual float pitch() const = 0;
		virtual float pitch(float ratio) = 0;

		virtual float smoothness() const = 0;
		virtual float smoothness(float value) = 0;
	};

	std::shared_ptr<pitch_control> pitch(stream_ptr stream, float ratio = 1.f, float smoothness = 0.01f);

}
