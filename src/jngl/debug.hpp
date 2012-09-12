/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#ifndef NDEBUG
#include <iostream>
#endif

namespace jngl {
	template<class T>
	void debug(const T& t) {
	#ifndef NDEBUG
		std::cout << t << std::flush;
	#endif
	}

	template<class T>
	void debugLn(const T& t) {
	#ifndef NDEBUG
		std::cout << t << std::endl;
	#endif
	}
}