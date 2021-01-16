// Copyright 2021 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "Mat3.hpp"

#include "Vec2.hpp"
#include "screen.hpp"

#include <boost/qvm/map_vec_mat.hpp>
#include <boost/qvm/mat_operations.hpp>
#include <boost/qvm/mat_operations3.hpp>
#include <boost/qvm/vec.hpp>

namespace jngl {
Mat3::Mat3(std::initializer_list<float> elements) {
	int row = 0;
	int column = 0;
	for (float element : elements) {
		data[column * 3 + row] = element;
		++column;
		if (column == 3) {
			++row;
			column = 0;
		}
	}
}

Mat3& Mat3::translate(const jngl::Vec2& v) {
	return *this *= boost::qvm::translation_mat(
	           boost::qvm::vec<double, 2>{ { v.x * getScaleFactor(), v.y * getScaleFactor() } });
}

Mat3& Mat3::scale(const float factor) {
	return *this *= boost::qvm::diag_mat(boost::qvm::vec<float, 3>{ { factor, factor, 1 } });
}

Mat3& Mat3::rotate(const float radian) {
	boost::qvm::rotate_z(*this, radian);
	return *this;
}

} // namespace jngl
