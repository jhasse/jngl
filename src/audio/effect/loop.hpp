// Copyright 2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
// Based on the audio implementation of the psemek engine, see
// https://lisyarus.github.io/blog/programming/2022/10/15/audio-mixing.html
#pragma once

#include <memory>
#include <optional>

namespace jngl {
struct Stream;

namespace audio {

std::shared_ptr<Stream> loop(std::shared_ptr<Stream> stream, std::optional<std::size_t> count = {});

} // namespace audio
} // namespace jngl
