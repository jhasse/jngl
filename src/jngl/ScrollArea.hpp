// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#pragma once

namespace jngl {

class Mat3;

class ScrollArea {
public:
	explicit ScrollArea(float height);

	/// Call this method during step of your Scene. It adjusts the jngl::input() by calling
	/// MouseInfo::transform.
	void stepAndTransformInput();

	/// Call this method during draw of your Scene. It updates the passed Modelview matrix.
	void updateModelview(Mat3& modelview) const;

private:
	float height;
	float scrollY = 0;
	float targetScrollY = 0;
	bool active = false;
	double startMouseX = 0;
	double startMouseY = 0;
	double previousMouseY = 0;
	bool notSureIfHorizontalOrVeriticalScroll = false;
	float speed = 0;
};

} // namespace jngl
