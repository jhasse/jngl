#include "track.hpp"
#include "duplicate.hpp"

namespace psemek::audio
{

	track_ptr load_raw(std::span<float const> data)
	{
		return make_duplicator(make_recorder(data));
	}

	track_ptr load_raw(std::vector<float> data)
	{
		return make_duplicator(make_recorder(std::move(data)));
	}

}
