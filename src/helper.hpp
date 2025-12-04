// Copyright 2016-2025 Jan Niklas Hasse <jhasse@bixense.com>
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

// https://www.cppstories.com/2021/heterogeneous-access-cpp20/
struct string_hash {
	using is_transparent = void;
	[[nodiscard]] size_t operator()(const char* txt) const {
		return std::hash<std::string_view>{}(txt);
	}
	[[nodiscard]] size_t operator()(std::string_view txt) const {
		return std::hash<std::string_view>{}(txt);
	}
	[[nodiscard]] size_t operator()(const std::string& txt) const {
		return std::hash<std::string>{}(txt);
	}
};

} // namespace jngl
