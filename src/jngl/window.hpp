/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#include <string>

namespace jngl {
	void showWindow(const std::string& title,
	                int width,
	                int heigt,
	                bool fullscreen = false);

	void hideWindow();

	int getDesktopWidth();

	int getDesktopHeight();
}