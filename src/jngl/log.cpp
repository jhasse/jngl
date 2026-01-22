// Copyright 2024-2025 Jan Niklas Hasse <jhasse@gmail.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "log.hpp"

#include "../App.hpp"
#include "../log.hpp"

#include <sstream>

namespace jngl {

namespace {
std::string shortenDisplayName() {
	if (auto displayName = App::instance().getDisplayName(); !displayName.empty()) {
		std::ostringstream tmp;
		tmp << "\x1b[1m";
		const size_t MAX_LENGTH = 12;
		if (displayName.size() > MAX_LENGTH) {
			tmp << displayName.substr(0, MAX_LENGTH - 1) << "…";
		} else {
			tmp << displayName;
		}
		tmp << "\x1b[0m";
		return tmp.str();
	}
	return {};
}
} // namespace

void trace(const std::string& line [[maybe_unused]]) {
#ifndef NDEBUG
	internal::log(shortenDisplayName(), "\x1b[37mtrace\x1b[0m", line);
#endif
}

void debug(Vec2 v) {
	debug("{}", v);
}

void debug(const std::string& line [[maybe_unused]]) {
#ifndef NDEBUG
	internal::log(shortenDisplayName(), "\x1b[36mdebug\x1b[0m", line);
#endif
}

void info(const std::string& line) {
	internal::log(shortenDisplayName(), "\x1b[32minfo\x1b[0m", line);
}

void log(std::string_view levelName, const std::string& line) {
	internal::log(shortenDisplayName(), std::format("\x1b[35m{}\x1b[0m", std::string(levelName)), line);
}

void warn(const std::string& line) {
	internal::log(shortenDisplayName(), "\x1b[1;33mwarn\x1b[0m", line);
}

void error(const std::string& line) {
	internal::log(shortenDisplayName(), "\x1b[1;31merror\x1b[0m", line);
}

} // namespace jngl
