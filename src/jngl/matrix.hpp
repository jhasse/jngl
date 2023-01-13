// Copyright 2012-2022 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Functions related to the global ModelView matrix
/// @file
#pragma once

#include "Mat3.hpp"

namespace jngl {

/// Returns a copy of the global ModelView matrix
///
/// This is the matrix which is used in all of JNGL's shaders to position objects. Using this
/// function you can get a local copy of it. This allows you to more precisely position an object
/// without affecting any global state.
///
/// A common use-case where you actually want to modify the ModelView matrix for all objects is the
/// camera. So in your drawing loop you would do:
/// \code
/// void MyGame::draw() const {
///     jngl::translate(cameraPosition);
///     {
///         auto mv = jngl::modelview();
///         sprite.draw(mv.translate(position1);
///     } // mv which is translated by position1 gets discarded
///     {
///         auto mv = jngl::modelview();
///         sprite.draw(mv.translate(position2);
///     }
/// }
/// \endcode
jngl::Mat3 modelview();

/// Multiplies the global ModelView matrix with a rotation matrix
void rotate(double degree);

/// Multiplies the global ModelView matrix with a translation matrix
void translate(double x, double y);

/// Multiplies the global ModelView matrix with a translation matrix
///
/// Equivalent to calling `jngl::translate(v.x, v.y);`.
template <class Vect> void translate(Vect v) {
	translate(v.x, v.y);
}

/// Multiplies the global ModelView matrix by a scaling matrix
///
/// Equivalent to calling `jngl::scale(factor, factor)`.
void scale(double factor);

/// Multiplies the global ModelView matrix by a scaling matrix
void scale(double xfactor, double yfactor);

/// Pushes the current ModelView matrix on a global stack
void pushMatrix();

/// Replaces the current ModelView matrix with the top element of the global stack
void popMatrix();

/// Resets the global ModelView matrix to the identity matrix
void reset();

} // namespace jngl
