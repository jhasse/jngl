// Copyright 2016-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include <string>
#include <vector>

namespace jngl {

std::vector<std::string> splitlines(const std::string&);

/// Checks whether the file exists on the filesystem or in the APK (on Android)
bool fileExists(const std::string& path);

/// Used on Android and iOS to remove valid Unix path constructs which would result in problems
///
/// ./foo => foo
/// foo/./bar => foo/bar
/// foo//bar => foo/bar
/// ././/././foo.bar => foo.bar
std::string sanitizePath(std::string);

} // namespace jngl
