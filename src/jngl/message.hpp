// Copyright 2015-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// @file
#pragma once

#include <string>

namespace jngl {

/// Shows a message box with an error \a text
void errorMessage(const std::string& text);

/// Prints \a text on stdout
///
/// Normally this is the same as `std::cout << text << std::flush;`. On Android this will use
/// `__android_log_print` for example.
void printMessage(const std::string& text);

} // namespace jngl
