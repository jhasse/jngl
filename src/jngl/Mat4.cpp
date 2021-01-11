// Copyright 2021 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "Mat4.hpp"

namespace jngl {
Mat4::Mat4(std::initializer_list<float> elements) {
	int row = 0;
	int column = 0;
	for (float element : elements) {
		data[column * 4 + row] = element;
		++column;
		if (column == 4) {
			++row;
			column = 0;
		}
	}
}
} // namespace jngl
