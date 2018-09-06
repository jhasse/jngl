// Copyright 2016-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include <string>
#include <vector>

namespace jngl {

std::vector<std::string> splitlines(const std::string&);

/// Checks whether the file exists on the filesystem or in the APK (on Android)
bool fileExists(const std::string& path);

} // namespace jngl
