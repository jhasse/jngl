// Copyright 2012-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "screen.hpp"

#include "../App.hpp"
#include "../windowptr.hpp"
#include "window.hpp"

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
	if (const auto screenSize = App::instance().getScreenSize()) {
		// Return the size the application asked for rather than deriving it from the window's
		// actual size in pixels. factor is computed from the latter and therefore loses
		// precision: On a display with fractional HiDPI scaling a canvas of 1136x640 would come
		// out as 1135.999975x640.077703, i.e. the result would depend on which display the window
		// happens to open on.
		return *screenSize;
	}
	// AppParameters::screenSize hasn't been set (yet), e.g. in unit tests which don't create an
	// App at all.
	return { static_cast<double>(getWindowWidth()) / factor,
		     static_cast<double>(getWindowHeight()) / factor };
}

} // namespace jngl
