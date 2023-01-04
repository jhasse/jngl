#pragma once

#include "../stream.hpp"

namespace psemek::audio
{

	struct volume_control
		: stream
	{
		virtual float gain() const = 0;
		virtual float gain(float value) = 0;

		virtual float smoothness() const = 0;
		virtual float smoothness(float value) = 0;
	};

	struct volume_control_stereo
		: stream
	{
		virtual float gain_left() const = 0;
		virtual float gain_right() const = 0;
		virtual float gain_left(float value) = 0;
		virtual float gain_right(float value) = 0;

		virtual float smoothness() const = 0;
		virtual float smoothness(float value) = 0;
	};

	std::shared_ptr<volume_control> volume(stream_ptr stream, float gain = 1.f, float smoothness = 0.f);
	std::shared_ptr<volume_control_stereo> volume_stereo(stream_ptr stream, float gain_left = 1.f, float gain_right = 1.f, float smoothness = 0.f);

}
