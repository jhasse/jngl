#pragma once

#include "stream.hpp"
#include "track.hpp"
#include "recorder.hpp"

#include <utility>

namespace psemek::audio
{

	track_ptr make_duplicator(std::shared_ptr<recorder> recorder);

	inline track_ptr make_duplicator(stream_ptr stream)
	{
		return make_duplicator(make_recorder(std::move(stream)));
	}

	inline std::pair<stream_ptr, stream_ptr> duplicate(stream_ptr stream)
	{
		auto track = make_duplicator(std::move(stream));
		return {track->stream(), track->stream()};
	}

}
