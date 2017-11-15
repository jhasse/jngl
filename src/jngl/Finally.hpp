// Copyright 2007-2017 Jan Niklas Hasse <jhasse@gmail.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "dll.hpp"

#include <functional>

namespace jngl {

class Finally {
public:
	Finally(std::function<void()> functionToCall);
	Finally(const Finally&) = delete;
	Finally& operator=(const Finally&) = delete;
	Finally(Finally&&) = default;
	JNGLDLL_API ~Finally();
private:
	std::function<void()> functionToCall;
};

} // namespace jngl
