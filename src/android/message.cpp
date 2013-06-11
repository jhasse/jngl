/*
Copyright 2013 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "../jngl.hpp"

#include <android/log.h>

namespace jngl {
	void errorMessage(const std::string& text) {
		__android_log_print(ANDROID_LOG_ERROR, "JNGL", text.c_str());
	}
}
