// Copyright 2007-2019 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// @file
#pragma once

#include <functional>

namespace jngl {

/// Helper class which calls a function when being destroyed
class Finally {
public:
	explicit Finally(std::function<void()> functionToCall);
	Finally(Finally&&) noexcept;
	Finally& operator=(Finally&&) noexcept;
	Finally(const Finally&) = delete;
	Finally& operator=(const Finally&) = delete;
	~Finally();

private:
	std::function<void()> functionToCall;
};

} // namespace jngl
