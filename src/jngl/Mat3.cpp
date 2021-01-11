// Copyright 2021 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "Mat3.hpp"

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
} // namespace jngl
