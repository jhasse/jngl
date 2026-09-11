// Copyright 2021-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "Mat3.hpp"

#include "Pixels.hpp"
#include "screen.hpp"

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
	return *this *= boost::qvm::translation_mat(boost::qvm::vec<double, 2>{ { v.x, v.y } });
}

Mat3& Mat3::translate(const Pixels x, const Pixels y) {
	return *this *= boost::qvm::translation_mat(
	           boost::qvm::vec<double, 2>{ { static_cast<float>(x / getScaleFactor()),
	                                         static_cast<float>(y / getScaleFactor()) } });
}

Mat3& Mat3::scale(const float factor) {
	return scale(factor, factor);
}

Mat3& Mat3::scale(const float xfactor, const float yfactor) {
	return *this *= boost::qvm::diag_mat(boost::qvm::vec<float, 3>{ { xfactor, yfactor, 1 } });
}

Mat3& Mat3::scale(const Vec2& v) {
	return scale(v.x, v.y);
}

Mat3& Mat3::rotate(const float radian) {
#ifndef NDEBUG
	if (std::isinf(radian)) {
		throw std::runtime_error("Rotation by infinity");
	}
	if (std::isnan(radian)) {
		throw std::runtime_error("Rotation by NaN");
	}
#endif
	boost::qvm::rotate_z(*this, radian);
	return *this;
}

} // namespace jngl
