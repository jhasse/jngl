/*
Copyright 2009-2015 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include <string>
#include <unistd.h>    // fork
#include <sys/types.h> // pid_t
#include <sys/wait.h>  // waitpid

namespace jngl {
	void errorMessage(const std::string& text) {
		pid_t pid = fork();
		if (pid == 0) {
			execlp("xmessage", "Error", "-default", "okay", "-nearmouse", text.c_str(), NULL);
		} else {
			waitpid(pid, 0, 0);
		}
	}
}
