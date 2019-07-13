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

/// Returns the width of the window in actual pixels (i.e. ignoring jngl::getScaleFactor)
int JNGLDLL_API getWindowWidth();

/// Returns the height of the window in actual pixels (i.e. ignoring jngl::getScaleFactor)
int JNGLDLL_API getWindowHeight();

/// Returns the width of the main display in actual pixels
int JNGLDLL_API getDesktopWidth();

/// Returns the height of the main display in actual pixels
int JNGLDLL_API getDesktopHeight();

/// If the window is displayed fullscreen (always true on mobile devices)
bool JNGLDLL_API getFullscreen();

/// Toggle fullscreen window mode
void JNGLDLL_API setFullscreen(bool);

/// Sets the main window title
void JNGLDLL_API setTitle(const std::string& title);

} // namespace jngl
