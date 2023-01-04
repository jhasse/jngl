#pragma once

#include "stream.hpp"

#include <memory>

namespace psemek::audio
{

	stream_ptr stereo(stream_ptr left, stream_ptr right);

}
