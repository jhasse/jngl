/*
Copyright 2007-2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#include "jngl/window.hpp"
#include "jngl/types.hpp"
#include "jngl/sprite.hpp"
#include "jngl/work.hpp"
#include "jngl/debug.hpp"
#include "jngl/framebuffer.hpp"
#include "jngl/font.hpp"
#include "jngl/drawable.hpp"
#include "jngl/screen.hpp"
#include "jngl/text.hpp"
#include "jngl/input.hpp"

#include <string>
#include <vector>
#include <stdexcept>

namespace jngl {
	bool running();

	void updateInput();

	void swapBuffers();

	void quit();

	void cancelQuit();

	void draw(const std::string& filename,
	          double xposition,
	          double yposition);

	template<class Vect>
	void draw(const std::string& filename, Vect pos) {
		draw(filename, pos.x, pos.y);
	}

	void drawScaled(const std::string& filename,
	                double xposition,
	                double yposition,
	                float xfactor,
	                float yfactor);

	void drawScaled(const std::string& filename,
	                double xposition,
	                double yposition,
	                float factor);

	void drawClipped(const std::string& filename,
					 double xposition,
					 double yposition,
					 float xstart,
					 float xend,
					 float ystart,
					 float yend);

	void drawRect(double xposition, double yposition, double width, double height);

	template<class Vect>
	void drawRect(Vect pos, Vect size) {
		DrawRect(pos.x, pos.y, size.x, size.y);
	}

	void drawLine(double xstart, double ystart, double xend, double yend);

	void drawEllipse(double xmid, double ymid, double width, double height);

	void drawPoint(double x, double y);

	void rotate(double degree);

	void translate(double x, double y);

	void scale(double factor);

	void scale(double xfactor, double yfactor);

	void pushMatrix();

	void popMatrix();

	void reset();

	void load(const std::string& filename);

	void unload(const std::string& filename);

	void unloadAll();

	int getWidth(const std::string& filename);

	int getHeight(const std::string& filename);

	double getTime();

	void beginPolygon();

	void vertex(double xposition,
				double yposition);

	void endPolygon();

	void setRelativeMouseMode(bool relative);

	bool getRelativeMouseMode();

	void setMouseVisible(bool visible);

	bool isMouseVisible();

	int getMouseX();

	int getMouseY();

	namespace mouse
	{
		enum Button
		{
			Left, Middle, Right
		};
	}

	double getMouseWheel();

	bool mouseDown(mouse::Button button = mouse::Left);

	bool mousePressed(mouse::Button button = mouse::Left);

	void setMouse(int xposition, int yposition);

	bool drawButton(const std::string& sprite,
	                double xposition,
	                double yposition,
	                const std::string& mouseover);

	void setTitle(const std::string& title);

	void setBackgroundColor(unsigned char red,
	                        unsigned char green,
	                        unsigned char blue);

	void setColor(unsigned char red,
	              unsigned char green,
	              unsigned char blue,
	              unsigned char alpha = 255);

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

	double getTextWidth(const std::string& text);

	void play(const std::string& filename);

	void stop(const std::string& filename);

	bool isPlaying(const std::string& filename);

	bool isOpenALInstalled();

	void setPlaybackSpeed(float speed);

	void setVolume(float volume);

	void setIcon(const std::string& filename);

	void mainLoop();

	void setPrefix(const std::string& path);

	std::string getPrefix();

	void setConfigPath(const std::string& path);

	std::string getConfigPath();

	std::vector<std::string> getArgs();
}