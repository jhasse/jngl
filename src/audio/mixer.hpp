#pragma once

#include "stream.hpp"
#include "channel.hpp"

#include <memory>

namespace psemek::audio
{

	struct mixer
		: stream
	{
		virtual channel_ptr add(stream_ptr stream) = 0;
	};

	using mixer_ptr = std::shared_ptr<mixer>;

	mixer_ptr make_mixer();

}
