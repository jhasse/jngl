// Copyright 2007-2021 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::Finally class
/// @file
#pragma once

#include <functional>

namespace jngl {

/// Helper class which calls a function when being destroyed
///
/// Useful when using C interfaces without the need to write a wrapper class. For example:
///
/// \code
/// void foo() {
/// 	FILE* f = fopen("foo.txt", "r");
/// 	Finally _([f]() { fclose(f); };
/// 	// ...
/// }
/// \endcode
class Finally {
public:
	/// \a functionToCall will be moved into the member with the same name
	explicit Finally(std::function<void()> functionToCall);

	/// The moved-from Finally object will no longer call the function
	Finally(Finally&&) noexcept;

	/// The moved-from Finally object will no longer call the function
	Finally& operator=(Finally&&) noexcept;

	Finally(const Finally&) = delete;
	Finally& operator=(const Finally&) = delete;

	/// Calls the function which has been passed into the constructor
	~Finally();

private:
	std::function<void()> functionToCall;
};

} // namespace jngl
