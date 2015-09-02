/*
Copyright 2012-2015 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#ifndef NDEBUG
#include "message.hpp"

#include <sstream>
#endif

namespace jngl {
	template<class T>
	void debug(const T& t) {
	#ifndef NDEBUG
		std::stringstream s;
		s << t;
		printMessage(s.str());
	#endif
	}

	template<class T>
	void debugLn(const T& t) {
	#ifndef NDEBUG
		std::stringstream s;
		s << t << std::endl;
		printMessage(s.str());
	#endif
	}
}
