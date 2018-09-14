// Copyright 2009-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include <iostream>
#include <sys/wait.h> // waitpid
#include <unistd.h>   // fork

namespace jngl {

void printMessage(const std::string& text) {
	std::cout << text << std::flush;
}

void errorMessage(const std::string& text) {
	pid_t pid = fork();
	if (pid == 0) {
		if (execlp("zenity", "zenity", "--error", ("--text=" + text).c_str(), nullptr) == -1) {
			if (execlp("xmessage", "Error", "-default", "okay", "-nearmouse", text.c_str(),
			           nullptr) == -1) {
				printMessage(text);
			}
		}
	} else {
		waitpid(pid, nullptr, 0);
	}
}

} // namespace jngl
