/*
Copyright 2013-2015 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "../jngl/message.hpp"

#include <android/log.h>

namespace jngl {
	void errorMessage(const std::string& text) {
		printMessage(text + "\n");
	}

	void printMessage(const std::string& text) {
		static std::string cache;
		const char* startLine = text.c_str();
		const char* c;
		for (c = text.c_str(); *c; ++c) {
			if (*c == '\n') {
				for (auto* n = startLine; n < c; ++n) {
					cache.append(1, *n);
				}
				startLine = c + 1;
				__android_log_print(ANDROID_LOG_ERROR, "libjngl", "%s", cache.c_str());
				cache = "";
			}
		}
		if (startLine) {
			cache += startLine;
		}
	}
}
