/*
Copyright 2015 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "../jngl/message.hpp"

#include <iostream>

namespace jngl {
	void errorMessage(const std::string& text) {
		printMessage(text + "\n");
	}

	void printMessage(const std::string& text) {
		std::cout << text << std::flush;
	}
}
