/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#include "window.hpp"
#include "types.hpp"
#include "sprite.hpp"
#include "work.hpp"
#include "debug.hpp"
#include "framebuffer.hpp"
#include "font.hpp"
#include "drawable.hpp"
#include "screen.hpp"
#include "text.hpp"
#include "input.hpp"
#include "job.hpp"
#include "shapes.hpp"
#include "matrix.hpp"
#include "sound.hpp"

#include <string>
#include <vector>
#include <stdexcept>

#ifndef _MSC_VER
#pragma GCC visibility push(default)
#endif
namespace jngl {
	bool running();

	void updateInput();

	void swapBuffers();

	void quit();

	void cancelQuit();

	void drawRect(double xposition, double yposition, double width, double height);

	double getTime();

	void beginPolygon();

	void vertex(double xposition,
				double yposition);

	void endPolygon();

	void setTitle(const std::string& title);

	void setBackgroundColor(unsigned char red,
	                        unsigned char green,
	                        unsigned char blue);

	void print(const std::string& text,
	           int xposition,
	           int yposition);

	int getFontSize();

	void setFontSize(int size);

	std::string getFont();

	void setFont(const std::string& filename);

	void setFontByName(const std::string& name);

	void sleep(int milliseconds);

	double getFPS();

	void errorMessage(const std::string& text);

	bool getFullscreen();

	void readPixel(int x, int y, unsigned char& red, unsigned char& green, unsigned char& blue);

	int getWindowWidth();

	int getWindowHeight();

	void setAntiAliasing(bool enabled);

	bool getAntiAliasing();

	void setVerticalSync(bool enabled);

	bool getVerticalSync();

	void setIcon(const std::string& filename);

	void mainLoop();

	void setPrefix(const std::string& path);

	std::string getPrefix();

	void setConfigPath(const std::string& path);

	std::string getConfigPath();

	std::vector<std::string> getArgs();
}
#ifndef _MSC_VER
#pragma GCC visibility pop
#endif