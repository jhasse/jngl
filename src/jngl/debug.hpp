// Copyright 2012-2025 Jan Niklas Hasse <jhasse@gmail.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Logging functions for debug purposes, deprecated in favor of log.hpp
/// @file
#pragma once

#ifndef NDEBUG
#include "message.hpp"

#include <sstream>
#endif

namespace jngl {

template <class T>
#ifdef NDEBUG
void debugLn(const T&) {
#else
[[deprecated("Use jngl::debug instead (also adds a newline)")]]
/// \deprecated Use jngl::debug instead (also adds a newline)
void debugLn(const T& t) {
	std::stringstream s;
	s << t << std::endl;
	printMessage(s.str());
#endif
}

} // namespace jngl
