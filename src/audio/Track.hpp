#pragma once

#include <gsl/span>
#include <optional>
#include <vector>

namespace jngl {

struct Stream;

struct Track {
	virtual std::shared_ptr<Stream> stream() const = 0;
	virtual std::optional<std::size_t> length() const = 0;

	virtual ~Track() = default;
};

std::shared_ptr<Track> load_raw(gsl::span<float const> samples);
std::shared_ptr<Track> load_raw(std::vector<float> samples);

std::shared_ptr<Track> load_ogg(gsl::span<char const> data);
std::shared_ptr<Track> load_ogg(std::vector<char> data);

} // namespace jngl
