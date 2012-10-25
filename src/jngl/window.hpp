/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#include <string>

#ifndef _MSC_VER
#pragma GCC visibility push(default)
#endif
namespace jngl {
	void showWindow(const std::string& title,
	                int width,
	                int heigt,
	                bool fullscreen = false);

	void hideWindow();

	int getDesktopWidth();

	int getDesktopHeight();
}
#ifndef _MSC_VER
#pragma GCC visibility pop
#endif