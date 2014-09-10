/*
Copyright 2012-2014 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#include "types.hpp"

#include "dll.hpp"

namespace jngl {
	Float JNGLDLL_API getScaleFactor();

	void JNGLDLL_API setScaleFactor(Float);

	int JNGLDLL_API getScreenWidth();

	int JNGLDLL_API getScreenHeight();

	enum DeviceOrientation {
		Portrait,
		PortraitUpsideDown,
		LandscapeLeft,
		LandscapeRight
	};

	void JNGLDLL_API setDeviceOrientationSupported(DeviceOrientation, bool enabled);

	bool JNGLDLL_API getDeviceOrientationSupported(DeviceOrientation);
}
#ifndef _MSC_VER
#pragma GCC visibility pop
#endif
