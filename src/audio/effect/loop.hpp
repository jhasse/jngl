#pragma once

#include <memory>
#include <optional>

namespace jngl {
struct Stream;

namespace audio {

std::shared_ptr<Stream> loop(std::shared_ptr<Stream> stream, std::optional<std::size_t> count = {});

} // namespace audio
} // namespace jngl
