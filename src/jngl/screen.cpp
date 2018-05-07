// Copyright 2012-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "screen.hpp"

#include "window.hpp"

namespace jngl {

Float factor = 1;

Float getScaleFactor() {
	return factor;
}

void setScaleFactor(Float f) {
	factor = f;
}

double getScreenWidth() {
	return static_cast<double>(getWindowWidth()) / factor;
}

double getScreenHeight() {
	return static_cast<double>(getWindowHeight()) / factor;
}

bool deviceOrientationSupported[] = { false, false, true, true };

void setDeviceOrientationSupported(DeviceOrientation i, bool enabled) {
	deviceOrientationSupported[i] = enabled;
}

bool getDeviceOrientationSupported(DeviceOrientation i) {
	return deviceOrientationSupported[i];
}

} // namespace jngl
