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

Mat4::Mat4() : data{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 } {
}
} // namespace jngl
