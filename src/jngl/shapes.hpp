// Copyright 2012-2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Functions for drawing shapes
/// @file
#pragma once

#include "Rgba.hpp"
#include "Vec2.hpp"

#include <cstdint>

namespace jngl {

class Mat3;
class Rgba;

/// Sets the color which should be used to draw primitives
///
/// Doesn't change the alpha value currently set by setAlpha()
void setColor(Rgb);

/// Sets the color (including alpha) which should be used to draw primitives
void setColor(Rgba);

/// Sets the color and alpha which should be used to draw primitives
/// @param alpha [0...255]
void setColor(Rgb, unsigned char alpha);

void setColor(unsigned char red, unsigned char green, unsigned char blue);

void setColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha);

/// Sets the alpha value which should be used to draw primitives (0 = fully transparent, 255 = fully
/// opaque)
void setAlpha(uint8_t alpha);

[[deprecated("Use setAlpha instead")]]
/// \deprecated Use jngl::setAlpha instead
void pushAlpha(unsigned char alpha);

[[deprecated("Use setAlpha instead")]]
/// \deprecated Use jngl::setAlpha instead
void popAlpha();

[[deprecated("Pass line width to drawLine instead")]]
/// \deprecated Pass line width to drawLine instead
void setLineWidth(float width);

/// Draws a line from start to end, the width can be set using setLineWidth
/// \deprecated Use drawLine(Vec2, Vec2, float lineWidth) instead
[[deprecated("Use drawLine(Vec2, Vec2, float lineWidth) instead")]]
void drawLine(Vec2 start, Vec2 end);

/// Draws a line from \a start to \a end with width \a lineWidth in the color set by jngl::setColor
void drawLine(Vec2 start, Vec2 end, float lineWidth);

/// Draws a line from \a start to \a end with width \a lineWidth in \a color
void drawLine(Vec2 start, Vec2 end, float lineWidth, Rgba color);

[[deprecated("Use drawLine(Vec2, Vec2, float lineWidth) instead")]]
/// \deprecated Use drawLine(Vec2, Vec2, float lineWidth) instead
void drawLine(double xstart, double ystart, double xend, double yend);

/// Draws a line from \a start to \a end
/// \deprecated Use drawLine(Mat3, Vec2, Vec2, float lineWidth) instead
[[deprecated("Use drawLine(Mat3, Vec2, Vec2, float lineWidth) instead")]]
void drawLine(Mat3 modelview, Vec2 start, Vec2 end);

/// Draws a line from \a start to \a end with width \a lineWidth in the color set by jngl::setColor
void drawLine(Mat3 modelview, Vec2 start, Vec2 end, float lineWidth);

/// Draws a line from \a start to \a end with width \a lineWidth in \a color
void drawLine(Mat3 modelview, Vec2 start, Vec2 end, float lineWidth, Rgba color);

/// Draws a line from (0, 0) to \a end in the color set by jngl::setColor
/// \deprecated Use drawLine(const Mat3&, Vec2, float lineWidth, Rgba) instead
[[deprecated("Use drawLine(const Mat3&, Vec2, float lineWidth, Rgba) instead")]]
void drawLine(const Mat3& modelview, Vec2 end);

/// Draws a line from (0, 0) to \a end in \a color
/// \deprecated Use drawLine(const Mat3&, Vec2, float lineWidth, Rgba) instead
[[deprecated("Use drawLine(const Mat3&, Vec2, float lineWidth, Rgba) instead")]]
void drawLine(const Mat3& modelview, Vec2 end, Rgba color);

/// Draws a line from (0, 0) to \a end with width \a lineWidth in \a color
void drawLine(Mat3 modelview, Vec2 end, float lineWidth, Rgba color);

[[deprecated("Use drawEllipse(Mat3, float, float, float) instead")]]
/// \deprecated Use drawEllipse(Mat3, float, float, float) instead
void drawEllipse(float xmid, float ymid, float width, float height, float startAngle = 0);

void drawEllipse(Vec2, float width, float height, float startAngle = 0);

void drawEllipse(Mat3 modelview, float width, float height, float startAngle = 0);

void drawEllipse(Mat3 modelview, float width, float height, float startAngle, Rgba color);

/// Angles in radian
void drawCircle(Vec2, float radius, float startAngle = 0);

void drawCircle(Mat3 modelview, float radius, float startAngle);

/// Draws a circle at (0, 0) with \a radius in \a color with \a startAngle cut out
///
/// Passing 0 as \a startAngle will draw a full circle. Passing pi/2 would look like this:
///
/// @verbatim
///       **
///     **##
///    *####
///   *#####
///  *######
/// *#######
/// *##############*
///  *############*
///   *##########*
///    *########*
///     **####**
///       ****
/// @endverbatim
void drawCircle(Mat3 modelview, float radius, float startAngle, Rgba color);

void drawCircle(Mat3 modelview, float radius);

/// Draws a circle at (0, 0) with \a radius in \a color
void drawCircle(Mat3 modelview, float radius, Rgba color);

/// Draws a circle at (0, 0) with radius of 1 in \a color
void drawCircle(Mat3 modelview, Rgba color);

[[deprecated("Use drawCircle instead")]]
/// \deprecated Use drawCircle instead
void drawPoint(double x, double y);

/// Draws the triangle a -> b -> c
void drawTriangle(Vec2 a, Vec2 b, Vec2 c);

[[deprecated("Use drawTriangle(Vec2, Vec2, Vec2) instead")]]
/// \deprecated Use drawTriangle(Vec2, Vec2, Vec2) instead
void drawTriangle(double A_x, double A_y, double B_x, double B_y, double C_x, double C_y);

/// Draws a equilateral triangle centered at (0, 0) and the top point at (0, 1)
///
/// The side length of the triangle is sqrt(3) ≈ 1.732.
void drawTriangle(Mat3 modelview, Rgba color);

/// Draws a rectangle at { \a xposition, \a yposition }
/// \deprecated Use drawRect(const Mat3&, Vec2, Color) instead
[[deprecated("Use drawRect(const Mat3&, Vec2, Color) instead")]]
void drawRect(double xposition, double yposition, double width, double height);

/// Draws a rectangle at \a position
///
/// Use setColor(Rgb) to change the color and setAlpha(uint8_t) to change the translucency.
void drawRect(Vec2 position, Vec2 size);

/// Draws a rectangle spawning from (0, 0) to (size.x, size.y) with the specified color
///
/// Use setAlpha to set the opacity.
void drawRect(const Mat3& modelview, Vec2 size, Rgb);

/// Draws a rectangle spawning from (0, 0) to (size.x, size.y) with the specified color
///
/// Use setAlpha to set the opacity.
void drawRect(Mat3 modelview, Vec2 size, Rgba color);

/// Draws the outline of a rectangle (so □ instead of ■) of \a size centered at (0, 0) in \a color
void drawRectOutline(Mat3 modelview, Vec2 size, float lineWidth, Rgba color);

/// Draws a (filled) square of size 1x1 centered at (0, 0) with the specified color
///
/// By squaling the modelview matrix you can change the size of the square, effectively turning it
/// into a rectangle:
/// \code
/// // draws a rectangle at (12 - 56 / 2, 34 - 78 / 2) with a size of 56x78 in red:
/// drawSquare(jngl::modelview().translate(12, 34).scale(56, 78), 0xff0000ff_rgba);
/// \endcode
void drawSquare(const Mat3& modelview, Rgba color);

/// Draws the outline of a square (so □ instead of ■) of size 1x1 centered at (0, 0) with the
/// specified color
///
/// By squaling the modelview matrix you can change the size of the square, effectively turning it
/// into a rectangle:
/// \code
/// // draws a rectangle at (12 - 56 / 2, 34 - 78 / 2) with a size of 56x78 in red:
/// drawSquareOutline(jngl::modelview().translate(12, 34).scale(56, 78), 0xff0000ff_rgba, 2.f);
/// \endcode
///
/// \note The line width is not affected by the modelview matrix scaling! Use drawRectOutline
/// instead if you want scaling to work correctly.
void drawSquareOutline(Mat3 modelview, float lineWidth, Rgba color);

template <class Vect> void drawRect(Vect pos, Vect size) {
	drawRect(pos.x, pos.y, size.x, size.y);
}

} // namespace jngl
