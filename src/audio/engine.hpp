// Copyright 2023-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
// Based on the audio implementation of the psemek engine, see
// https://lisyarus.github.io/blog/programming/2022/10/15/audio-mixing.html
#pragma once

#include <memory>

namespace jngl {
struct Stream;

namespace audio {

struct engine {
	explicit engine(std::shared_ptr<Stream> output);
	~engine();

	void setPause(bool);
	void step();

private:
	struct Impl;
	std::unique_ptr<Impl> impl;
};

} // namespace audio
} // namespace jngl
