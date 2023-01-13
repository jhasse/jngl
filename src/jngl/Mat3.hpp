// Copyright 2021-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::Mat3 class
/// @file
#pragma once

#include <boost/qvm_lite.hpp>
#include <initializer_list>

namespace jngl {

class Pixels;
class Vec2;

/// 3x3 matrix
///
/// identical to boost::qvm::mat<float, 3, 3>, but stores the elements column-major in memory
class Mat3 {
public:
	/// creates identity matrix
	Mat3() = default;

	/// construct matrix from row-major array with 9 elements
	Mat3(std::initializer_list<float>);

	/// Multiplies the matrix with a translation matrix generated from v
	///
	/// \return *this
	Mat3& translate(const Vec2& v);

	/// Multiplies the matrix with a translation matrix generated from (x,y)
	///
	/// \return *this
	Mat3& translate(Pixels x, Pixels y);

	/// Multiplies the matrix by a scaling matrix
	///
	/// Equivalent to calling `Mat3::scale(factor, factor)`.
	/// \return *this
	Mat3& scale(float factor);

	/// Multiplies the matrix by a scaling matrix
	///
	/// \return *this
	///
	/// If you want to draw a sprite with double the width, but the correct height:
	/// \code
	/// yourSprite.draw(jngl::modelview().scale(2, 1));
	/// \endcode
	Mat3& scale(float xfactor, float yfactor);

	/// Multiplies the matrix with a rotation matrix
	///
	/// \return *this
	Mat3& rotate(float radian);

	/// column-major
	float data[9] = { 1, 0, 0, 0, 1, 0, 0, 0, 1 };
};

} // namespace jngl

namespace boost::qvm {
template <> struct mat_traits<jngl::Mat3> {
	static int const rows = 3;
	static int const cols = 3;
	using scalar_type = float;

	template <int R, int C> static scalar_type read_element(const jngl::Mat3& m) {
		return m.data[C * 3 + R];
	}
	template <int R, int C> static scalar_type& write_element(jngl::Mat3& m) {
		return m.data[C * 3 + R];
	}
};
} // namespace boost::qvm
