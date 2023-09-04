#pragma once

#include <memory>
#include <string_view>
#include <vector>

namespace jngl {
struct Stream;

namespace audio {

struct engine {
	explicit engine(std::shared_ptr<Stream> output);
	~engine();

	void setPause(bool);

private:
	struct Impl;
	std::unique_ptr<Impl> impl;
};

} // namespace audio
} // namespace jngl
