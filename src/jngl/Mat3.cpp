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

Mat3::Mat3() : data{ 1, 0, 0, 0, 1, 0, 0, 0, 1 } {
}
} // namespace jngl
