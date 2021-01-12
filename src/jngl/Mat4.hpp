// Copyright 2021 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::Mat4 class
/// @file
#pragma once

#include <boost/qvm/mat.hpp>
#include <boost/qvm/mat_traits.hpp>
#include <initializer_list>

namespace jngl {

/// 4x4 matrix
///
/// Identical to `boost::qvm::mat<float, 4, 4>`, but stores the elements column-major in memory
class Mat4 {
public:
	/// creates identity matrix
	Mat4() = default;

	/// construct matrix from row-major array with 16 elements
	Mat4(std::initializer_list<float>);

	/// column-major
	float data[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
};

} // namespace jngl

namespace boost::qvm {
template <> struct mat_traits<jngl::Mat4> {
	static int const rows = 4;
	static int const cols = 4;
	using scalar_type = float;

	template <int R, int C> static scalar_type read_element(const jngl::Mat4& m) {
		return m.data[C * 4 + R];
	}
	template <int R, int C> static scalar_type& write_element(jngl::Mat4& m) {
		return m.data[C * 4 + R];
	}
};
} // namespace boost::qvm
