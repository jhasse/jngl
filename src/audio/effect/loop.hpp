#pragma once

#include <memory>
#include <optional>

namespace jngl {
class Stream;
}

namespace psemek::audio {

using jngl::Stream; // FIXME

std::shared_ptr<Stream> loop(std::shared_ptr<Stream> stream, std::optional<std::size_t> count = {});

} // namespace psemek::audio
