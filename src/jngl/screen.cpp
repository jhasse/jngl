/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "screen.hpp"
#include "types.hpp"
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

	int getScreenWidth() {
		return static_cast<int>(getWindowWidth() / factor);
	}

	int getScreenHeight() {
		return static_cast<int>(getWindowHeight() / factor);
	}

	bool deviceOrientationSupported[] = { false, false, true, true };

	void setDeviceOrientationSupported(DeviceOrientation i, bool enabled) {
		deviceOrientationSupported[i] = enabled;
	}

	bool getDeviceOrientationSupported(DeviceOrientation i) {
		return deviceOrientationSupported[i];
	}
}