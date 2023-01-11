// Copyright 2009-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include <iostream>
#include <sys/wait.h> // waitpid
#include <unistd.h>   // fork

namespace jngl {

void printMessage(const std::string& text) {
	std::cout << text << std::flush;
}

} // namespace jngl
