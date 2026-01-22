// Copyright 2024-2026 Jan Niklas Hasse <jhasse@gmail.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "log.hpp"

#include "jngl/message.hpp"

#include <cctype>
#include <iomanip>
#include <sstream>

#ifdef ANDROID
#include <android/log.h>
#endif

namespace jngl::internal {

int64_t gFrameNumber = -1;

void trace(const std::string& line [[maybe_unused]]) {
#ifdef JNGL_TRACE
	log("JNGL", "\x1b[37mtrace\x1b[0m", line);
#endif
}

namespace {
bool gDebugLogEnabled = []() {
	const char* env = std::getenv("JNGL_DEBUG_LOG");
	if (!env) {
#ifdef NDEBUG
		return false;
#else
		return true;
#endif
	}
	return env && (std::string(env) != "0");
}();
} // namespace

void debug(const std::string& line [[maybe_unused]]) {
	if (gDebugLogEnabled) {
		log("JNGL", "\x1b[36mdebug\x1b[0m", line);
	}
}

void info(const std::string& line) {
	log("JNGL", "\x1b[32minfo\x1b[0m", line);
}

void warn(const std::string& line) {
	log("JNGL", "\x1b[33mwarn\x1b[0m", line);
}

void error(const std::string& line) {
	log("JNGL", "\x1b[31merror\x1b[0m", line);
}

namespace {
std::string stripAnsiEscapeCodes(const std::string& in) {
	std::string stripped;
	for (size_t i = 0; i < in.size(); ++i) {
		if (in[i] != '\33') {
			stripped.push_back(in[i]);
			continue;
		}

		// Only strip CSIs for now.
		if (i + 1 >= in.size()) {
			break;
		}
		if (in[i + 1] != '[') {
			continue;
		} // Not a CSI.
		i += 2;

		// Skip everything up to and including the next [a-zA-Z].
		while (i < in.size() && std::isalpha(in[i]) == 0) {
			++i;
		}
	}
	return stripped;
}
} // namespace

void log(const std::string& appName, const std::string& level, const std::string& message) {
#ifdef ANDROID
	std::string levelStripped = stripAnsiEscapeCodes(level);
	auto androidLevel = ANDROID_LOG_FATAL;
	if (levelStripped == "debug") {
		androidLevel = ANDROID_LOG_DEBUG;
	} else if (levelStripped == "info") {
		androidLevel = ANDROID_LOG_INFO;
	} else if (levelStripped == "warn") {
		androidLevel = ANDROID_LOG_WARN;
	} else if (levelStripped == "trace") {
		androidLevel = ANDROID_LOG_VERBOSE;
	} else if (levelStripped == "error") {
		androidLevel = ANDROID_LOG_ERROR;
	}
	__android_log_print(androidLevel, appName.c_str(), "%s", message.c_str());
#else
	std::ostringstream tmp;
	tmp << "\x1b[2m" << std::setw(4);
	if (gFrameNumber >= 0) {
		tmp << gFrameNumber;
	} else {
		tmp << ' ';
	}
	tmp << " \x1b[0m";
	if (appName == "JNGL" || (!appName.empty() && gDebugLogEnabled)) {
#ifdef __EMSCRIPTEN__
		tmp << '[' << stripAnsiEscapeCodes(appName) << ']';
#else
		tmp << '[' << appName << ']';
#endif
	}
#ifdef __EMSCRIPTEN__
	tmp << '[' << stripAnsiEscapeCodes(level) << "] ";
#else
	tmp << '[' << level << "] ";
#endif
	bool first = true;
	std::stringstream lines(message);
	std::string line;
	while (std::getline(lines, line)) {
		if (first) {
			first = false;
		} else {
			const size_t indentation =
			    stripAnsiEscapeCodes(appName).size() + 5 + stripAnsiEscapeCodes(level).size();
			tmp << std::string(indentation, ' ');
		}
		tmp << line << '\n';
	}
	printMessage(tmp.str());
#endif
}

} // namespace jngl::internal
