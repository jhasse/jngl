// Copyright 2012-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

/// @file
/// Functions related to the main window.

#pragma once

#include <string>

#include "dll.hpp"

namespace jngl {

/// Creates the main window
void JNGLDLL_API showWindow(const std::string& title, int width, int height,
                            bool fullscreen = false, std::pair<int, int> minAspectRatio = { 4, 3 },
                            std::pair<int, int> maxAspectRatio = { 16, 9 });

/// Cleans up the window and unloads everything
void JNGLDLL_API hideWindow();

int JNGLDLL_API getWindowWidth();

int JNGLDLL_API getWindowHeight();

int JNGLDLL_API getDesktopWidth();

int JNGLDLL_API getDesktopHeight();

bool JNGLDLL_API getFullscreen();

void JNGLDLL_API setFullscreen(bool);

/// Sets the main window title
void JNGLDLL_API setTitle(const std::string& title);

} // namespace jngl
