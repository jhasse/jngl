// Copyright 2007-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "dll.hpp"

#include <functional>

namespace jngl {

class Finally {
public:
	explicit Finally(std::function<void()> functionToCall);
	Finally(Finally&&);
	Finally& operator=(Finally&&);
	Finally(const Finally&) = delete;
	Finally& operator=(const Finally&) = delete;
	JNGLDLL_API ~Finally();
private:
	std::function<void()> functionToCall;
};

} // namespace jngl
