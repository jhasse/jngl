#pragma once

#include "../stream.hpp"
#include "../duration.hpp"

namespace psemek::audio
{

	stream_ptr fade_out(stream_ptr stream, duration length, duration start = {});

}
