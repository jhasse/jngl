#pragma once

#include "stream.hpp"
#include <span>

#include <optional>
#include <vector>

namespace psemek::audio
{

	struct track
	{
		virtual stream_ptr stream() const = 0;
		virtual std::optional<std::size_t> length() const = 0;

		virtual ~track(){}
	};

	using track_ptr = std::shared_ptr<track>;

	track_ptr load_raw(std::span<float const> data);
	track_ptr load_raw(std::vector<float> data);

	track_ptr load_mp3(std::span<char const> data);
	track_ptr load_mp3(std::vector<char> data);

	track_ptr load_ogg(std::span<char const> data);
	track_ptr load_ogg(std::vector<char> data);

}
