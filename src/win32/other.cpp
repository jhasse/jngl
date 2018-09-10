// Copyright 2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/other.hpp"

#include <windows.h>

std::string jngl::getBinaryPath() {
	char filename[MAX_PATH];
	GetModuleFileName(nullptr, filename, MAX_PATH);
	std::string tmp(filename);
	return tmp.substr(0, tmp.find_last_of("\\/") + 1);
}
