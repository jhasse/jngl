// Copyright 2007-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "draw.hpp"
#include "jngl.hpp"
#include "spriteimpl.hpp"

#include <boost/lexical_cast.hpp>

#ifdef ANDROID
PFNGLGENVERTEXARRAYSOESPROC glGenVertexArrays;
PFNGLBINDVERTEXARRAYOESPROC glBindVertexArray;
PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArrays;
#endif

namespace jngl {

std::string pathPrefix;
std::vector<std::string> args;
float bgRed = 1.0f, bgGreen = 1.0f, bgBlue = 1.0f; // Background Colors

void clearBackgroundColor() {
	glClearColor(bgRed, bgGreen, bgBlue, 1);
}

bool Init(const int width, const int height, const int screenWidth, const int screenHeight) {
#ifdef GLEW_OK
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		throw std::runtime_error(boost::lexical_cast<std::string>(glewGetErrorString(err)));
	}
	debug("Using GLEW "); debugLn(glewGetString(GLEW_VERSION));
#endif

	glShadeModel(GL_SMOOTH);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glViewport(0, 0, width, height);

	if (screenWidth != width || screenHeight != height) { // Letterboxing?
		glClearColor(0, 0, 0, 1); // black boxes
		glClear(GL_COLOR_BUFFER_BIT);

		glEnable(GL_SCISSOR_TEST);
		assert(screenWidth <= width);
		assert(screenHeight <= height);
		glScissor((width - screenWidth) / 2, (height - screenHeight) / 2, screenWidth,
		          screenHeight);
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

#ifdef OPENGLES
#define f2x(x) ((int)((x)*65536))
	glOrthox(f2x(-width / 2), f2x(width / 2), f2x(height / 2), f2x(-height / 2), f2x(-1), f2x(1));
	jngl::translate(-width / 2, height / 2);
	jngl::rotate(-90);
	jngl::translate(height / 2, width / 2);

#ifdef ANDROID
	glGenVertexArrays = (PFNGLGENVERTEXARRAYSOESPROC)eglGetProcAddress("glGenVertexArraysOES");
	glBindVertexArray = (PFNGLBINDVERTEXARRAYOESPROC)eglGetProcAddress("glBindVertexArrayOES");
	glDeleteVertexArrays =
	    (PFNGLDELETEVERTEXARRAYSOESPROC)eglGetProcAddress("glDeleteVertexArraysOES");
#endif
#else
	glOrtho(-width / 2, width / 2, height / 2, -height / 2, -100.0f, 100.0f);
#endif

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	clearBackgroundColor();
	glClear(GL_COLOR_BUFFER_BIT);

	glFlush();
	setVerticalSync(true);
	return true;
}

WindowPointer pWindow;
bool antiAliasingEnabled = false;
bool vsyncEnabled = false;

void showWindow(const std::string& title, const int width, const int height, bool fullscreen,
                const std::pair<int, int> minAspectRatio,
                const std::pair<int, int> maxAspectRatio) {
	debug("jngl::showWindow(\""); debug(title); debug("\", "); debug(width); debug(", ");
	debug(height); debug(", "); debug(fullscreen); debug(");\n");
	if (pWindow && width == pWindow->getWidth() && height == pWindow->getHeight() &&
	    fullscreen == pWindow->getFullscreen()) {
		return jngl::setTitle(title);
	}
	bool isMouseVisible = pWindow ? pWindow->getMouseVisible() : true;
	hideWindow();
	if (width == 0) {
		throw std::runtime_error("Width Is 0");
	}
	if (height == 0) {
		throw std::runtime_error("Height Is 0");
	}
	int screenWidth = width;
	int screenHeight = height;
	if (minAspectRatio.first * height > minAspectRatio.second * width) {
		// Are we below the minimal aspect ratio? -> Letterboxing at the top and bottom
		screenHeight = boost::numeric_cast<int>(
		    std::lround(float(minAspectRatio.second * width) / float(minAspectRatio.first)));
	} else if (maxAspectRatio.first * height < maxAspectRatio.second * width) {
		// Are we above the maximal aspect ratio? -> Letterboxing at the left and right
		screenWidth = boost::numeric_cast<int>(
		    std::lround(float(maxAspectRatio.first * height) / float(maxAspectRatio.second)));
	}
	if (screenWidth != width || screenHeight != height) {
		debug("Letterboxing to "); debug(screenWidth); debug("x"); debugLn(screenHeight);
	}
	pWindow.Set(new Window(title, width, height, fullscreen, screenWidth, screenHeight));
	pWindow->SetMouseVisible(isMouseVisible);
	setAntiAliasing(antiAliasingEnabled);
}

void hideWindow() {
	unloadAll();
	pWindow.Delete();
}

void swapBuffers() {
	pWindow->SwapBuffers();

	if (glIsEnabled(GL_SCISSOR_TEST)) {
		// Letterboxing with SDL_VIDEODRIVER=wayland will glitch if we don't draw the black boxes on
		// every frame
		glDisable(GL_SCISSOR_TEST);
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		glEnable(GL_SCISSOR_TEST);
		clearBackgroundColor();
	}

	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void updateInput() {
	pWindow->updateKeyStates();
	pWindow->UpdateInput();
}

bool running() {
	return pWindow->isRunning();
}

void quit() {
	pWindow->quit();
}

void cancelQuit() {
	pWindow->cancelQuit();
}

void setBackgroundColor(const unsigned char red, const unsigned char green,
                        const unsigned char blue) {
	pWindow.ThrowIfNull();
	bgRed = red / 255.0f;
	bgGreen = green / 255.0f;
	bgBlue = blue / 255.0f;
	clearBackgroundColor();
	glClear(GL_COLOR_BUFFER_BIT);
}

#if BOOST_VERSION >= 106200
boost::qvm::vec<double, 2> getMousePos() {
	return { { pWindow->getMouseX() / getScaleFactor() - getScreenWidth() / 2,
		       pWindow->getMouseY() / getScaleFactor() - getScreenHeight() / 2 } };
}
#endif

int getMouseX() {
	return pWindow->getMouseX();
}

int getMouseY() {
	return pWindow->getMouseY();
}

bool keyDown(const key::KeyType key) {
	return pWindow->getKeyDown(key);
}

bool keyPressed(const key::KeyType key) {
	return pWindow->getKeyPressed(key);
}

bool keyDown(const std::string& key) {
	return pWindow->getKeyDown(key);
}

bool keyPressed(const std::string& key) {
	return pWindow->getKeyPressed(key);
}

bool mouseDown(mouse::Button button) {
	return pWindow->getMouseDown(button);
}

bool mousePressed(mouse::Button button) {
	return pWindow->getMousePressed(button);
}

void setMouse(const int xposition, const int yposition) {
	pWindow->SetMouse(xposition, yposition);
}

void setRelativeMouseMode(const bool relative) {
	return pWindow->SetRelativeMouseMode(relative);
}

void setMouseVisible(const bool visible) {
	return pWindow->SetMouseVisible(visible);
}

bool isMouseVisible() {
	return pWindow->getMouseVisible();
}

bool getRelativeMouseMode() {
	return pWindow->getRelativeMouseMode();
}

void setTitle(const std::string& title) {
	return pWindow->SetTitle(title);
}

double getTextWidth(const std::string& text) {
	return pWindow->GetTextWidth(text);
}

int getLineHeight() {
	return pWindow->getLineHeight();
}

void setLineHeight(int h) {
	pWindow->setLineHeight(h);
}

void print(const std::string& text, const int xposition, const int yposition) {
	pWindow->print(text, xposition, yposition);
	glColor4ub(spriteColorRed, spriteColorGreen, spriteColorBlue, spriteColorAlpha);
}

int getFontSize() {
	return pWindow->getFontSize();
}

void setFontSize(const int size) {
	pWindow->setFontSize(size);
}

std::string getFont() {
	return pWindow->getFont();
}

void setFont(const std::string& filename) {
	pWindow->setFont(filename);
}

void setFontByName(const std::string& name) {
	pWindow->setFontByName(name);
}

void setIcon(const std::string& filename) {
	pWindow->SetIcon(filename);
}

double getFPS() {
	static double lastDraw = 0.0;
	double fps = 1 / (jngl::getTime() - lastDraw);
	lastDraw = jngl::getTime();
	return fps;
}

unsigned int getStepsPerSecond() {
	return pWindow->getStepsPerSecond();
}

void setStepsPerSecond(const unsigned int stepsPerSecond) {
	pWindow->setStepsPerSecond(stepsPerSecond);
}

void reset() {
	glLoadIdentity();
}

void rotate(const double degree) {
#ifdef GL_DOUBLE
	glRotated(degree, 0, 0, 1);
#else
	glRotatef(degree, 0, 0, 1);
#endif
}

void translate(const double x, const double y) {
	opengl::translate(x * getScaleFactor(), y * getScaleFactor());
}

void scale(const double factor) {
	opengl::scale(factor, factor);
}

void scale(const double xfactor, const double yfactor) {
	opengl::scale(xfactor, yfactor);
}

void pushMatrix() {
	glPushMatrix();
}

void popMatrix() {
	glPopMatrix();
}

void drawRect(const double xposition, const double yposition, const double width,
              const double height) {
	glColor4ub(colorRed, colorGreen, colorBlue, colorAlpha);
	draw::Rect(xposition, yposition, width, height);
	glColor4ub(spriteColorRed, spriteColorGreen, spriteColorBlue, spriteColorAlpha);
}

void drawTriangle(const double A_x, const double A_y, const double B_x, const double B_y,
                  const double C_x, const double C_y) {
	glColor4ub(colorRed, colorGreen, colorBlue, colorAlpha);
	draw::Triangle(A_x, A_y, B_x, B_y, C_x, C_y);
	glColor4ub(spriteColorRed, spriteColorGreen, spriteColorBlue, spriteColorAlpha);
}

void setLineWidth(const float width) {
	glLineWidth(width);
}

void drawLine(const double xstart, const double ystart, const double xend, const double yend) {
	glColor4ub(colorRed, colorGreen, colorBlue, colorAlpha);
	draw::Line(xstart, ystart, xend, yend);
	glColor4ub(spriteColorRed, spriteColorGreen, spriteColorBlue, spriteColorAlpha);
}

void drawPoint(const double x, const double y) {
	glColor4ub(colorRed, colorGreen, colorBlue, colorAlpha);
	draw::Point(x, y);
	glColor4ub(spriteColorRed, spriteColorGreen, spriteColorBlue, spriteColorAlpha);
}

void readPixel(const int x, const int y, unsigned char& red, unsigned char& green,
               unsigned char& blue) {
	unsigned char data[3];
	data[0] = static_cast<unsigned char>(bgRed * 255.0f);
	data[1] = static_cast<unsigned char>(bgGreen * 255.0f);
	data[2] = static_cast<unsigned char>(bgBlue * 255.0f);
	glReadPixels(x, getWindowHeight() - y - 1, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, data);
	red = data[0];
	green = data[1];
	blue = data[2];
}

int getWindowWidth() {
	return pWindow->getWidth();
}

int getWindowHeight() {
	return pWindow->getHeight();
}

double getMouseWheel() {
	return pWindow->getMouseWheel();
}

void setAntiAliasing(bool enabled) {
#ifdef GL_MULTISAMPLE_ARB
	if (!pWindow->isMultisampleSupported()) {
		antiAliasingEnabled = false;
		return;
	}
	if (enabled) {
		glEnable(GL_MULTISAMPLE_ARB);
	} else {
		glDisable(GL_MULTISAMPLE_ARB);
	}
	antiAliasingEnabled = enabled;
#else
	debug("WARNING: anti aliasing not available!\n");
#endif
}

bool getAntiAliasing() {
	return antiAliasingEnabled;
}

void loadSound(const std::string&); // definied in audio.cpp

Finally load(const std::string& filename) {
	if (filename.length() >= 4 && filename.substr(filename.length() - 4) == ".ogg") {
		loadSound(filename);
		return Finally(nullptr);
	}
	return loadSprite(filename);
}

#ifdef _WIN32
#include <GL/wglew.h>

void setVerticalSync(bool enabled) {
	if (WGL_EXT_swap_control) {
		if (wglSwapIntervalEXT(enabled)) {
			vsyncEnabled = enabled;
		}
	}
#else
void setVerticalSync(bool) {
#endif
#ifdef __APPLE__
	// OSX always enables V-SYNC
	vsyncEnabled = true;
#endif
}

bool getVerticalSync() {
	return vsyncEnabled;
}

void setWork(std::shared_ptr<Work> work) {
	return pWindow->setWork(std::move(work));
}

void resetFrameLimiter() {
	pWindow->resetFrameLimiter();
}

void setWork(Work* w) {
	return pWindow->setWork(std::shared_ptr<Work>(w));
}

void mainLoop() {
	return pWindow->mainLoop();
}

std::shared_ptr<Work> getWork() {
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

} // namespace jngl
