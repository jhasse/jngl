#include "wave/silence.hpp"
#include "wave/generator.hpp"

namespace psemek::audio
{

	stream_ptr silence()
	{
		return make_generator([]{ return 0.f; });
	}

}
