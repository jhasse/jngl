/*
Copyright 2007-2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "jngl.hpp"
#include "window.hpp"
#include "spriteimpl.hpp"
#include "windowptr.hpp"
#include "draw.hpp"
#include "opengl.hpp"
#include "main.hpp"

#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <sstream>

extern "C" {
	void InitCallbacks(); // see callbacks.c
}

namespace jngl
{
	std::string pathPrefix;
	std::vector<std::string> args;
    float bgRed = 1.0f, bgGreen = 1.0f, bgBlue = 1.0f; // Background Colors
	bool Init(const int width, const int height)
	{
#ifdef GLEW_OK
		GLenum err = glewInit();
		if(err != GLEW_OK) {
			throw std::runtime_error(boost::lexical_cast<std::string>(glewGetErrorString(err)));
		}
		debug("Using GLEW "); debug(glewGetString(GLEW_VERSION)); debug("\n");
#endif

		glShadeModel(GL_SMOOTH);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glClearColor(bgRed, bgGreen, bgBlue, 0.0f);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glViewport(0, 0, width, height);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

	#ifdef OPENGLES
		#define f2x(x) ((int)((x) * 65536))
		glOrthox(f2x(-width/2), f2x(width/2), f2x(height/2), f2x(-height/2), f2x(-1), f2x(1));
		jngl::translate(-width/2, height/2);
		jngl::rotate(-90);
		jngl::translate(height/2, width/2);
	#else
		glOrtho(-width/2, width/2, height/2, -height/2, -100.0f, 100.0f);
	#endif

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glClear(GL_COLOR_BUFFER_BIT);

		glFlush();
		InitCallbacks();
		setVerticalSync(true);
		return true;
	}

	WindowPointer pWindow;
	bool antiAliasingEnabled = false;
	bool vsyncEnabled = false;

	void showWindow(const std::string& title, const int width, const int height, bool fullscreen)
	{
		debug("jngl::showWindow(\""); debug(title); debug("\", "); debug(width); debug(", "); debug(height);
		debug(", "); debug(fullscreen); debug(");\n");
		if (pWindow &&
		    width == pWindow->GetWidth() &&
		    height == pWindow->GetHeight() &&
		    fullscreen == pWindow->GetFullscreen()) {
			return jngl::setTitle(title);
		}
		bool isMouseVisible = pWindow ? pWindow->GetMouseVisible() : true;
		hideWindow();
		if(width == 0)
		{
			throw std::runtime_error("Width Is 0");
		}
		if(height == 0)
		{
			throw std::runtime_error("Height Is 0");
		}
		pWindow.Set(new Window(title, width, height, fullscreen));
		pWindow->SetMouseVisible(isMouseVisible);
		setAntiAliasing(antiAliasingEnabled);
	}

	void hideWindow()
	{
		unloadAll();
		opengl::BindArrayBuffer(0);
		pWindow.Delete();
	}

	void swapBuffers()
	{
	    pWindow->SwapBuffers();
		glLoadIdentity();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void updateInput()
	{
		pWindow->UpdateKeyStates();
		pWindow->UpdateInput();
	}

	bool running()
	{
		return pWindow->Running();
	}

	void quit()
	{
		pWindow->Quit();
	}

	void cancelQuit()
	{
		pWindow->Continue();
	}

	void clearBackgroundColor() {
		glClearColor(bgRed, bgGreen, bgBlue, 1);
	}

	void setBackgroundColor(const unsigned char red, const unsigned char green, const unsigned char blue)
	{
		pWindow.ThrowIfNull();
		bgRed = red / 255.0f;
		bgGreen = green / 255.0f;
		bgBlue = blue / 255.0f;
		clearBackgroundColor();
		glClear(GL_COLOR_BUFFER_BIT);
	}

	int getMouseX()
	{
		return pWindow->MouseX();
	}

	int getMouseY()
	{
		return pWindow->MouseY();
	}

	bool keyDown(const key::KeyType key)
	{
		return pWindow->getKeyDown(key);
	}

	bool keyPressed(const key::KeyType key)
	{
		return pWindow->getKeyPressed(key);
	}

	bool keyDown(const std::string& key)
	{
		return pWindow->getKeyDown(key);
	}

	bool keyPressed(const std::string& key)
	{
		return pWindow->getKeyPressed(key);
	}

	bool mouseDown(mouse::Button button)
	{
		return pWindow->MouseDown(button);
	}

	bool mousePressed(mouse::Button button)
	{
		return pWindow->getMousePressed(button);
	}

	void setMouse(const int xposition, const int yposition)
	{
		pWindow->SetMouse(xposition, yposition);
	}

	void setRelativeMouseMode(const bool relative) {
		return pWindow->SetRelativeMouseMode(relative);
	}

	void setMouseVisible(const bool visible)
	{
		return pWindow->SetMouseVisible(visible);
	}

	bool isMouseVisible()
	{
		return pWindow->GetMouseVisible();
	}

	bool getRelativeMouseMode() {
		return pWindow->GetRelativeMouseMode();
	}

	void setTitle(const std::string& title)
	{
		return pWindow->SetTitle(title);
	}

	unsigned char colorRed = 0, colorGreen = 0, colorBlue = 0, colorAlpha = 255;

	void setColor(const unsigned char red, const unsigned char green, const unsigned char blue, const unsigned char alpha)
	{
		colorRed = red;
		colorGreen = green;
		colorBlue = blue;
		colorAlpha = alpha;
	}

	double getTextWidth(const std::string& text)
	{
		return pWindow->GetTextWidth(text);
	}

	int getLineHeight() {
		return pWindow->getLineHeight();
	}

	void setLineHeight(int h) {
		pWindow->setLineHeight(h);
	}

	void print(const std::string& text, const int xposition, const int yposition)
	{
		pWindow->Print(text, xposition, yposition);
		glColor4ub(spriteColorRed, spriteColorGreen, spriteColorBlue, spriteColorAlpha);
	}

	int getFontSize()
	{
		return pWindow->GetFontSize();
	}

	void setFontSize(const int size)
	{
		pWindow->SetFontSize(size);
	}

	std::string getFont()
	{
		return pWindow->GetFont();
	}

	void setFont(const std::string& filename)
	{
		pWindow->SetFont(filename);
	}

	void setFontByName(const std::string& name)
	{
		pWindow->SetFontByName(name);
	}

	bool getFullscreen()
	{
		return pWindow->GetFullscreen();
	}

	void setIcon(const std::string& filename)
	{
		pWindow->SetIcon(filename);
	}

	double getFPS()
	{
		static double lastDraw = 0.0;
		double fps = 1/(jngl::getTime() - lastDraw);
		lastDraw = jngl::getTime();
		return fps;
	}

	void reset()
	{
		glLoadIdentity();
	}

	void rotate(const double degree)
	{
#ifdef GL_DOUBLE
		glRotated(degree, 0, 0, 1);
#else
		glRotatef(degree, 0, 0, 1);
#endif
	}

	void translate(const double x, const double y)
	{
		opengl::translate(x, y);
	}

	void scale(const double factor)
	{
		opengl::scale(factor, factor);
	}

	void scale(const double xfactor, const double yfactor)
	{
		opengl::scale(xfactor, yfactor);
	}

	void pushMatrix()
	{
		glPushMatrix();
	}

	void popMatrix()
	{
		glPopMatrix();
	}

	void drawRect(const double xposition, const double yposition, const double width, const double height)
	{
		glColor4ub(colorRed, colorGreen, colorBlue, colorAlpha);
		draw::Rect(xposition, yposition, width, height);
		glColor4ub(spriteColorRed, spriteColorGreen, spriteColorBlue, spriteColorAlpha);
	}

	void drawLine(const double xstart, const double ystart, const double xend, const double yend)
	{
		glColor4ub(colorRed, colorGreen, colorBlue, colorAlpha);
		draw::Line(xstart, ystart, xend, yend);
		glColor4ub(spriteColorRed, spriteColorGreen, spriteColorBlue, spriteColorAlpha);
	}

	void drawEllipse(const double xmid, const double ymid, const double width, const double height)
	{
		glColor4ub(colorRed, colorGreen, colorBlue, colorAlpha);
		draw::Ellipse(xmid, ymid, width, height);
		glColor4ub(spriteColorRed, spriteColorGreen, spriteColorBlue, spriteColorAlpha);
	}

	void drawPoint(const double x, const double y)
	{
		glColor4ub(colorRed, colorGreen, colorBlue, colorAlpha);
		draw::Point(x, y);
		glColor4ub(spriteColorRed, spriteColorGreen, spriteColorBlue, spriteColorAlpha);
	}

	void readPixel(const int x, const int y, unsigned char& red, unsigned char& green, unsigned char& blue)
	{
		unsigned char data[3];
		data[0] = static_cast<unsigned char>(bgRed * 255.0f);
		data[1] = static_cast<unsigned char>(bgGreen * 255.0f);
		data[2] = static_cast<unsigned char>(bgBlue * 255.0f);
		glReadPixels(x, getWindowHeight() - y - 1, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, data);
		red = data[0];
		green = data[1];
		blue = data[2];
	}

	int getWindowWidth()
	{
		return pWindow->GetWidth();
	}

	int getWindowHeight()
	{
		return pWindow->GetHeight();
	}

	double getMouseWheel()
	{
		return pWindow->GetMouseWheel();
	}

	void setAntiAliasing(bool enabled)
	{
#ifdef GL_MULTISAMPLE_ARB
		if(!pWindow->IsMultisampleSupported())
		{
			antiAliasingEnabled = false;
			return;
		}
		if(enabled)
		{
			glEnable(GL_MULTISAMPLE_ARB);
		}
		else
		{
			glDisable(GL_MULTISAMPLE_ARB);
		}
		antiAliasingEnabled = enabled;
#else
		debug("WARNING: anti aliasing not available!\n");
#endif
	}

	bool getAntiAliasing()
	{
		return antiAliasingEnabled;
	}

	void loadSound(const std::string&); // definied in audio.cpp

	void load(const std::string& filename)
	{
		if(filename.length() >= 4 && filename.substr(filename.length() - 4) == ".ogg")
		{
			loadSound(filename);
		}
		else
		{
			loadSprite(filename);
		}
	}

#ifdef _WIN32
	#include <GL/wglew.h>
#endif

	void setVerticalSync(bool enabled) {
#ifdef _WIN32
		if (WGL_EXT_swap_control) {
			if (wglSwapIntervalEXT(enabled)) {
				vsyncEnabled = enabled;
			}
		}
#endif
#ifdef __APPLE__
		// OSX always enables V-SYNC
		vsyncEnabled = true;
#endif
	}

	bool getVerticalSync() {
		return vsyncEnabled;
	}

	void setWork(boost::shared_ptr<Work> w) {
		return pWindow->SetWork(w);
	}

	void setWork(Work* w) {
		return pWindow->SetWork(boost::shared_ptr<Work>(w));
	}

	void mainLoop() {
		return pWindow->MainLoop();
	}

	boost::shared_ptr<Work> getWork() {
		return pWindow->getWork();
	}

	void setPrefix(const std::string& path) {
		pathPrefix = path;
	}

	std::string getPrefix() {
		return pathPrefix;
	}

	void setConfigPath(const std::string& path) {
		pWindow->setConfigPath(path);
	}

	std::string getConfigPath() {
		return pWindow->getConfigPath();
	}

	std::vector<std::string> getArgs() {
		return args;
	}
}
