// Copyright 2016-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "helper.hpp"

#include <algorithm>

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
	for (; *c != 0; ++c) {
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
	if (path.size() > 1 && path.at(0) == '.' && path.at(1) == '/') {
		path.erase(0, 2);
	}
	return path;
}

bool isOneCharacter(const std::string_view text) {
	if (text.empty()) {
		return false;
	}
	// How many bytes the first one says the character has
	const auto first = static_cast<unsigned char>(text[0]);
	size_t length = 0;
	if (first < 0x80) {
		length = 1;
	} else if (first >= 0xC2 && first <= 0xDF) {
		length = 2;
	} else if (first >= 0xE0 && first <= 0xEF) {
		length = 3;
	} else if (first >= 0xF0 && first <= 0xF4) {
		length = 4;
	} else {
		return false; // a continuation byte, or one that isn't used in UTF-8
	}
	return text.size() == length && std::all_of(text.begin() + 1, text.end(), [](const char c) {
		return (static_cast<unsigned char>(c) & 0xC0) == 0x80;
	});
}

// À-Þ (U+00C0 to U+00DE) and à-þ (U+00E0 to U+00FE) differ in their second byte in UTF-8, apart
// from × (U+00D7) and ÷ (U+00F7) in between
std::string withShift(std::string character) {
	if (character.size() == 1 && character[0] >= 'a' && character[0] <= 'z') {
		character[0] = static_cast<char>(character[0] - 'a' + 'A');
	} else if (character.size() == 2 && static_cast<unsigned char>(character[0]) == 0xC3) {
		const auto second = static_cast<unsigned char>(character[1]);
		if (second >= 0xA0 && second <= 0xBE && second != 0xB7) {
			character[1] = static_cast<char>(second - 0x20);
		}
	}
	return character;
}

std::string withoutShift(std::string character) {
	if (character.size() == 1 && character[0] >= 'A' && character[0] <= 'Z') {
		character[0] = static_cast<char>(character[0] - 'A' + 'a');
	} else if (character.size() == 2 && static_cast<unsigned char>(character[0]) == 0xC3) {
		const auto second = static_cast<unsigned char>(character[1]);
		if (second >= 0x80 && second <= 0x9E && second != 0x97) {
			character[1] = static_cast<char>(second + 0x20);
		}
	}
	return character;
}

} // namespace jngl
