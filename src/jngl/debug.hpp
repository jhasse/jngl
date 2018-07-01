// Copyright 2012-2018 Jan Niklas Hasse <jhasse@gmail.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#ifndef NDEBUG
#include "message.hpp"

#include <sstream>
#endif

namespace jngl {

template <class T>
#ifdef NDEBUG
void debug(const T&) {
#else
void debug(const T& t) {
	std::stringstream s;
	s << t;
	printMessage(s.str());
#endif
}

template <class T>
#ifdef NDEBUG
void debugLn(const T&) {
#else
void debugLn(const T& t) {
	std::stringstream s;
	s << t << std::endl;
	printMessage(s.str());
#endif
}

} // namespace jngl
