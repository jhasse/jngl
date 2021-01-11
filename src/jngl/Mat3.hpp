// Copyright 2021 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// @file Contains jngl::Mat3 class 
#pragma once

#include <boost/qvm/mat_traits.hpp>
#include <boost/qvm/mat.hpp>
#include <initializer_list>

namespace jngl {

/// 3x3 matrix
///
/// identical to boost::qvm::mat<float, 3, 3>, but stores the elements column-major in memory
class Mat3 {
public:
	/// creates identity matrix
	Mat3();

	/// construct matrix from row-major array with 9 elements
	Mat3(std::initializer_list<float>);

	/// column-major
	float data[9];
};

} // namespace jngl

namespace boost::qvm {
template <> struct mat_traits<jngl::Mat3> {
	static int const rows = 3;
	static int const cols = 3;
	typedef float scalar_type;

	template <int R, int C> static scalar_type read_element(const jngl::Mat3& m) {
		return m.data[C * 3 + R];
	}
	template <int R, int C> static scalar_type& write_element(jngl::Mat3& m) {
		return m.data[C * 3 + R];
	}
};
} // namespace boost::qvm
