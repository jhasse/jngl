#pragma once

#include "../stream.hpp"
#include "../duration.hpp"

namespace psemek::audio
{

	stream_ptr fade_in(stream_ptr stream, duration length, duration start = {});

}
