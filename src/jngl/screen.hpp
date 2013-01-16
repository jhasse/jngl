/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#include "types.hpp"

#ifndef _MSC_VER
#pragma GCC visibility push(default)
#endif
namespace jngl {
	Float getScaleFactor();

	void setScaleFactor(Float);

	int getScreenWidth();

	int getScreenHeight();

	enum DeviceOrientation {
		Portrait,
		PortraitUpsideDown,
		LandscapeLeft,
		LandscapeRight
	};

	void setDeviceOrientationSupported(DeviceOrientation, bool enabled);

	bool getDeviceOrientationSupported(DeviceOrientation);
}
#ifndef _MSC_VER
#pragma GCC visibility pop
#endif