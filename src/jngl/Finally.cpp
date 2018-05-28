// Copyright 2007-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "Finally.hpp"

namespace jngl {

Finally::Finally(std::function<void()> functionToCall) : functionToCall(std::move(functionToCall)) {
}

Finally::Finally(Finally&& other) noexcept {
	if (other.functionToCall) {
		functionToCall = std::move(other.functionToCall);
		other.functionToCall = nullptr;
	}
}

Finally& Finally::operator=(Finally&& other) noexcept {
	if (functionToCall) { functionToCall(); }
	functionToCall = std::move(other.functionToCall);
	other.functionToCall = nullptr;
	return *this;
}

Finally::~Finally() {
	if (functionToCall) {
		functionToCall();
	}
}

} // namespace jngl
