// Copyright 2007-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "Finally.hpp"

namespace jngl {

Finally::Finally(std::function<void()> functionToCall) : functionToCall(std::move(functionToCall)) {
}

Finally::~Finally() {
	if (functionToCall) {
		functionToCall();
	}
}

} // namespace jngl
