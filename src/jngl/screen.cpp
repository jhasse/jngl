// Copyright 2012-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "screen.hpp"

#include "../App.hpp"
#include "../windowptr.hpp"
#include "window.hpp"

#include <cmath>
#include <stdexcept>

namespace jngl {

double factor = 1;

namespace internal {
void resetScaleFactor() {
	factor = 1;
}
} // namespace internal

double getScaleFactor() {
	return factor;
}

void setScaleFactor(double f) {
	if (f < 0) {
		throw std::runtime_error("Invalid scale factor: " + std::to_string(f));
	}
	if (pWindow) {
		throw std::runtime_error("You may only call jngl::setScaleFactor() before creating the "
		                         "window. If you want to make adjustment depending on the window "
		                         "size use jngl::AppParameters::scaleFactor.");
	}
	factor = f;
}

double getScreenWidth() {
	return getScreenSize().x;
}

double getScreenHeight() {
	return getScreenSize().y;
}

Vec2 getScreenSize() {
	const auto screenSize = App::instance().getScreenSize();
	const Vec2 canvas{ static_cast<double>(getWindowWidth()) / factor,
		               static_cast<double>(getWindowHeight()) / factor };
	// The canvas can differ from the size the application asked for (or the desktop's, if it
	// didn't), e.g. when AppParameters::scaleFactor is set or AppParameters::minAspectRatio and
	// maxAspectRatio give the window a different shape. If it doesn't though, return the asked
	// for size rather than the one derived from the window's size in pixels: factor is computed
	// from the latter and therefore loses precision. On a display with fractional HiDPI scaling a
	// canvas of 1136x640 would come out as 1135.999975x640.077703, i.e. the result would depend on
	// which display the window happens to open on.
	if (screenSize && std::abs(canvas.x - screenSize->x) < 1 &&
	    std::abs(canvas.y - screenSize->y) < 1) {
		return *screenSize;
	}
	return canvas;
}

} // namespace jngl
