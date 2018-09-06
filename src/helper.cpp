// Copyright 2016-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "helper.hpp"

#ifdef ANDROID
#include "android/fopen.hpp"
#endif

namespace jngl {

std::vector<std::string> splitlines(const std::string& text) {
	std::vector<std::string> lines;
	const char* start_line = text.c_str();
	const char* c;
	for (c = text.c_str(); *c; c++) {
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
	if (FILE* const f = fopen(path.c_str(), "r")) {
		fclose(f);
		return true;
	}
	return false;
}

} // namespace jngl
