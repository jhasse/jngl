// Copyright 2012-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "dll.hpp"
#include "types.hpp"
#include "Vec2.hpp"

namespace jngl {
	Float JNGLDLL_API getScaleFactor();

	void JNGLDLL_API setScaleFactor(Float);

	double JNGLDLL_API getScreenWidth();

	double JNGLDLL_API getScreenHeight();

	Vec2 JNGLDLL_API getScreenSize();

	enum DeviceOrientation {
		Portrait,
		PortraitUpsideDown,
		LandscapeLeft,
		LandscapeRight
	};

	void JNGLDLL_API setDeviceOrientationSupported(DeviceOrientation, bool enabled);

	bool JNGLDLL_API getDeviceOrientationSupported(DeviceOrientation);
}
