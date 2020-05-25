// Copyright 2014-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Timing functions
/// @file
#pragma once

namespace jngl {

/// Returns seconds that have passed since program start
double getTime();

/// Sleep current thread for \a milliseconds
/// \deprecated Use std::this_thread::sleep_for instead
[[deprecated("Use std::this_thread::sleep_for instead")]]
void sleep(int milliseconds);

} // namespace jngl
