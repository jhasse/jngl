// Copyright 2012-2019 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "screen.hpp"

#include "window.hpp"

namespace jngl {

double factor = 1;

double getScaleFactor() {
	return factor;
}

void setScaleFactor(double f) {
	if (f < 0) {
		throw std::runtime_error("Invalid scale factor: " + std::to_string(f));
	}
	factor = f;
}

double getScreenWidth() {
	return static_cast<double>(getWindowWidth()) / factor;
}

double getScreenHeight() {
	return static_cast<double>(getWindowHeight()) / factor;
}

Vec2 getScreenSize() {
	return {getScreenWidth(), getScreenHeight()};
}

} // namespace jngl
