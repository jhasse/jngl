// Copyright 2012-2022 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

/// Functions related to the main window.
/// @file

#pragma once

#include "Pixels.hpp"

#include <array>
#include <string>
#include <vector>

namespace jngl {

/// Creates the main window
void showWindow(const std::string& title, int width, int height, bool fullscreen = false,
                std::pair<int, int> minAspectRatio = { 4, 3 },
                std::pair<int, int> maxAspectRatio = { 16, 9 });

/// Cleans up the window and unloads everything
void hideWindow();

/// Returns the width of the window in actual pixels (i.e. ignoring jngl::getScaleFactor)
int getWindowWidth();

/// Returns the height of the window in actual pixels (i.e. ignoring jngl::getScaleFactor)
int getWindowHeight();

/// Returns {width, height} of the window in actual pixels
std::array<Pixels, 2> getWindowSize();

/// Returns the width of the main display in actual pixels
int getDesktopWidth();

/// Returns the height of the main display in actual pixels
int getDesktopHeight();

/// If the window is displayed fullscreen (always true on mobile devices)
bool getFullscreen();

/// Toggle fullscreen window mode
void setFullscreen(bool);

/// Sets the main window title
void setTitle(const std::string& title);

enum class Cursor {
    ARROW,
    I,
};
void setCursor(Cursor);

/// Read red, green and blue values of the whole window frame buffer
std::vector<float> readPixels();

} // namespace jngl
