// Copyright 2016-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "helper.hpp"

#ifdef _WIN32
#include "win32/unicode.hpp"
#endif

#ifdef ANDROID
#include "android/fopen.hpp"
#endif

namespace jngl {

std::vector<std::string> splitlines(const std::string& text) {
	std::vector<std::string> lines;
	const char* start_line = text.c_str();
	const char* c = text.c_str();
	for (; *c; ++c) {
		if (*c == '\n') {
			std::string line;
			for (auto* n = start_line; n < c; ++n) {
				line.append(1, *n);
			}
			lines.push_back(line);
			start_line = c + 1;
		}
	}
	if (start_line) {
		std::string line;
		for (auto n = start_line; n < c; ++n) {
			line.append(1, *n);
		}
		lines.push_back(line);
	}
	return lines;
}

bool fileExists(const std::string& path) {
#ifdef _WIN32
	if (FILE* const f = _wfopen(utf8ToUtf16(path).c_str(), L"r")) {
#else
	if (FILE* const f = fopen(path.c_str(), "r")) {
#endif
		fclose(f);
		return true;
	}
	return false;
}

std::string sanitizePath(std::string path) {
	while (true) { // /./ => /
		size_t pos = path.find("/./");
		if (pos == std::string::npos) {
			break;
		}
		path.erase(pos, 2);
	}
	while (true) { // // => /
		size_t pos = path.find("//");
		if (pos == std::string::npos) {
			break;
		}
		path.erase(pos, 1);
	}
	if (path.size() > 1 && path[0] == '.' && path[1] == '/') {
		path.erase(0, 2);
	}
	return path;
}

} // namespace jngl
