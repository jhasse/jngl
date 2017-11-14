// Copyright 2007-2017 Jan Niklas Hasse <jhasse@gmail.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "Finally.hpp"

#include "debug.hpp"

namespace jngl {

Finally::Finally(std::function<void()> functionToCall) : functionToCall(std::move(functionToCall)) {
}

Finally::~Finally() {
	if (functionToCall) {
		functionToCall();
	}
}

}
