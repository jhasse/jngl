#pragma once

#include "stream.hpp"
#include "track.hpp"

#include <memory>
#include <string_view>
#include <vector>

namespace psemek::audio
{

	struct engine
	{
		engine(std::shared_ptr<stream> output);
		~engine();

		void setPause(bool);

	private:
		struct Impl;
		std::unique_ptr<Impl> impl;
	};

}
