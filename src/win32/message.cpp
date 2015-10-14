/*
Copyright 2009-2015 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "../jngl/message.hpp"

#include <windows.h>
#include <string>
#include <iostream>

namespace jngl {
	void errorMessage(const std::string& text) {
		MessageBox(NULL, text.c_str(), "Error", MB_OK | MB_ICONERROR);
	}

	void printMessage(const std::string& text) {
		std::cout << text << std::flush;
	}
}
