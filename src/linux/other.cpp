// Copyright 2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/other.hpp"

#include "binreloc.h"

#include <memory>
#include <sstream>

std::string jngl::getBinaryPath() {
	static std::unique_ptr<std::string> binpath;
	if (binpath) {
		return *binpath;
	}
	binpath = std::make_unique<std::string>();
	BrInitError error;
	if (br_init(&error) == 0) {
		std::stringstream tmp;
		tmp << "BinReloc failed to initialize (error code " << error << ")";
		throw std::runtime_error(tmp.str());
	}
	std::shared_ptr<char> tmp(br_find_exe_dir(""), free);
	*binpath = tmp.get();
	*binpath += '/';
	return *binpath;
}
