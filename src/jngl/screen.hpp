/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#include "types.hpp"

#pragma GCC visibility push(default)
namespace jngl {
	Float getScaleFactor();

	void setScaleFactor(Float);

	int getScreenWidth();

	int getScreenHeight();
}
#pragma GCC visibility pop