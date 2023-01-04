#pragma once

#include "stream.hpp"
#include <span>

#include <vector>

namespace psemek::audio
{

	struct recorder
	{
		virtual std::optional<std::size_t> length() const = 0;

		virtual std::size_t request(std::size_t samples) = 0;

		virtual std::span<float const> buffer() const = 0;

		virtual ~recorder() {}
	};

	std::shared_ptr<recorder> make_recorder(stream_ptr stream);
	std::shared_ptr<recorder> make_recorder(std::vector<float> samples);
	std::shared_ptr<recorder> make_recorder(std::span<float const> samples);

}
