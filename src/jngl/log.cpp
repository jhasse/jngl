// Copyright 2024 Jan Niklas Hasse <jhasse@gmail.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "log.hpp"

#include "../App.hpp"
#include "message.hpp"

#include <sstream>

namespace jngl {

void trace(const std::string& line) {
	std::ostringstream tmp;
	if (auto displayName = App::instance().getDisplayName(); !displayName.empty()) {
		tmp << '[';
		if (displayName.size() > 4) {
			tmp << displayName.substr(0, 3) << "…]";
		} else {
			tmp << displayName << ']';
		}
	}
	tmp << "[\x1b[1;36mtrace\x1b[0m] " << line << '\n';
	printMessage(tmp.str());
}

void info(const std::string& line) {
	std::ostringstream tmp;
	if (auto displayName = App::instance().getDisplayName(); !displayName.empty()) {
		tmp << '[';
		if (displayName.size() > 4) {
			tmp << displayName.substr(0, 3) << "…]";
		} else {
			tmp << displayName << ']';
		}
	}
	tmp << "[\x1b[32minfo\x1b[0m] " << line << '\n';
	printMessage(tmp.str());
}

void warn(const std::string& line) {
	std::ostringstream tmp;
	if (auto displayName = App::instance().getDisplayName(); !displayName.empty()) {
		tmp << '[';
		if (displayName.size() > 4) {
			tmp << displayName.substr(0, 3) << "…]";
		} else {
			tmp << displayName << ']';
		}
	}
	tmp << "[\x1b[1;33mwarn\x1b[0m] " << line << '\n';
	printMessage(tmp.str());
}

void error(const std::string& line) {
	std::ostringstream tmp;
	if (auto displayName = App::instance().getDisplayName(); !displayName.empty()) {
		tmp << '[';
		if (displayName.size() > 4) {
			tmp << displayName.substr(0, 3) << "…]";
		} else {
			tmp << displayName << ']';
		}
	}
	tmp << "[\x1b[1;31merror\x1b[0m] " << line << '\n';
	printMessage(tmp.str());
}

} // namespace jngl
