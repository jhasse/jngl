#pragma once

#include <gsl/span>
#include <optional>
#include <vector>

namespace jngl {
struct Stream;
}

namespace psemek::audio {

using jngl::Stream; // FIXME

struct track {
	virtual std::shared_ptr<Stream> stream() const = 0;
	virtual std::optional<std::size_t> length() const = 0;

	virtual ~track() {
	}
};

using track_ptr = std::shared_ptr<track>;

track_ptr load_raw(gsl::span<float const> data);
track_ptr load_raw(std::vector<float> data);

track_ptr load_ogg(gsl::span<char const> data);
track_ptr load_ogg(std::vector<char> data);

} // namespace psemek::audio
