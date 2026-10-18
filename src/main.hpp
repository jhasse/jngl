// Copyright 2012-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#pragma once

#include <array>
#include <stack>
#include <string>
#if defined(__has_include) && __has_include(<optional>)
#include <optional>
using std::optional;
#else
#include <experimental/optional>
using std::experimental::optional;
#endif

namespace jngl {

class Mat3;

/// Sets the global OpenGL background color, doesn't actually call glClear
void clearBackgroundColor();

void updateViewportAndLetterboxing(int width, int height, int canvasWidth, int canvasHeight);
void updateProjection(int windowWidth, int windowHeight, float, float);

/// Sets OpenGL's scissor test to the rectangles of the scissor() calls that are alive, mapped onto
/// the window's canvas or onto the FrameBuffer in use, or to the window's letterboxing if there
/// are none and nothing else is drawn on, or else turns it off
void applyScissor();

/// The size in pixels of the FrameBuffer in use, if any, see FrameBuffer::use()
optional<std::array<int, 2>> activeFrameBufferSize();

extern std::string pathPrefix;
extern optional<std::string> configPath;
extern std::stack<jngl::Mat3> modelviewStack;

namespace internal {

/// Whether windows created afterwards should use the full resolution of high-DPI displays
/// (default: true)
///
/// When disabled, the window's size in actual pixels is exactly what has been passed to
/// showWindow() and the operating system upscales it on displays with a scale factor greater
/// than 1. Used by the unittests, only has an effect with the SDL backend.
void setHighDpi(bool);
bool getHighDpi();

} // namespace internal

} // namespace jngl
