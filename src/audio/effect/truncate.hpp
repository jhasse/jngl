#pragma once

#include "../stream.hpp"
#include "../duration.hpp"

namespace psemek::audio
{

	stream_ptr truncate(stream_ptr stream, duration length);

}
