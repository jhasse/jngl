#pragma once

#include "track.hpp"

#include <memory>
#include <string_view>
#include <vector>

namespace jngl {
struct Stream;
}

namespace psemek::audio {

using jngl::Stream; // FIXME

struct engine {
	explicit engine(std::shared_ptr<Stream> output);
	~engine();

	void setPause(bool);

private:
	struct Impl;
	std::unique_ptr<Impl> impl;
};

} // namespace psemek::audio
