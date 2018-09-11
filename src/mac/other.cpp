// Copyright 2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/other.hpp"

#include <mach-o/dyld.h>
#include <CoreServices/CoreServices.h>

std::string jngl::getBinaryPath() {
	static std::unique_ptr<std::string> binpath;
	if (binpath) {
		return *binpath;
	}
	binpath = std::make_unique<std::string>();
	uint32_t size = 0;
	if (_NSGetExecutablePath(nullptr, &size) == 0) {
		throw std::runtime_error("Can't get executable path!");
	}
	std::vector<char> tmp(size);
	_NSGetExecutablePath(&tmp[0], &size);
	binpath->assign(tmp.begin(), tmp.end());
	*binpath = binpath->substr(0, binpath->find_last_of('/') + 1);
	return *binpath;
}
