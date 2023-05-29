// Copyright 2007-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "jngl/input.hpp"
#include "opengl.hpp"

#include <array>
#include <functional>
#include <map>
#include <stack>
#include <string>
#include <unordered_map>

#ifdef _WIN32
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#endif

namespace jngl {
class FontImpl;
class Job;
class ScaleablePixels;
class WindowImpl;
class Work;

class Window {
public:
	Window(const std::string& title, int width, int height, bool fullscreen,
	       std::pair<int, int> minAspectRatio, std::pair<int, int> maxAspectRatio);
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	Window(Window&&) = delete;
	Window& operator=(Window&&) = delete;
	~Window();
	bool isRunning() const;
	void quit() noexcept;
	void cancelQuit();
	void UpdateInput();
	void updateKeyStates();
	void SwapBuffers();
	void SetRelativeMouseMode(bool relative);
	bool getRelativeMouseMode() const;
	void SetMouseVisible(bool visible);
	bool getMouseVisible() const;
	bool isMultitouch() const;
	std::vector<Vec2> getTouchPositions() const;
	int getMouseX() const;
	int getMouseY() const;
	int getCanvasWidth() const;
	int getCanvasHeight() const;
	int getWidth() const;
	int getHeight() const;
	ScaleablePixels getTextWidth(const std::string&);
	Pixels getLineHeight();
	void setLineHeight(Pixels);
	bool getFullscreen() const;
	void setFullscreen(bool);
	bool getKeyDown(key::KeyType key);
	bool getKeyPressed(key::KeyType key);
	void setKeyPressed(key::KeyType key, bool);
	bool getKeyDown(const std::string& key);
	bool getKeyPressed(const std::string& key);
	void setKeyPressed(const std::string& key, bool);
	bool getMouseDown(mouse::Button);
	void setMouseDown(mouse::Button, bool);
	bool getMousePressed(mouse::Button button) const;
	void setMousePressed(mouse::Button button, bool);
	void SetMouse(int xposition, int yposition);
	void SetTitle(const std::string& title);
	void print(const std::string& text, int xposition, int yposition);
	int getFontSize() const;
	void setFontSize(int size);
	void setFont(const std::string&);
	void setFontByName(const std::string&);
	bool isMultisampleSupported() const;
	void SetIcon(const std::string&);
	double getMouseWheel() const;
	std::string getFont() const;
	std::shared_ptr<FontImpl> getFontImpl();
	void setWork(std::shared_ptr<Work>);
	void mainLoop();
	void stepIfNeeded();
	void sleepIfNeeded();
	void draw() const;
	std::shared_ptr<Work> getWork();
	void addJob(std::shared_ptr<Job>);
	std::shared_ptr<Job> getJob(const std::function<bool(Job&)>& predicate) const;
	void resetFrameLimiter();
	unsigned int getStepsPerSecond() const;
	void setStepsPerSecond(unsigned int);
	void addUpdateInputCallback(std::function<void()>);
#if defined(IOS) || defined(ANDROID)
	WindowImpl* getImpl() const;
#endif
	std::string getTextInput() const;
	void initGlObjects();
	static void drawTriangle(Vec2 a, Vec2 b, Vec2 c);
	void drawLine(Mat3 modelview, Vec2 b) const;
	void drawRect(Vec2 pos, Vec2 size) const;
	void drawRect(Mat3 modelview, Vec2 size) const;
	void onControllerChanged(std::function<void()>);

	friend class WindowImpl;
	std::unique_ptr<WindowImpl> impl;

private:
	static int GetKeyCode(jngl::key::KeyType key);
	static std::string GetFontFileByName(const std::string& fontname);
	void calculateCanvasSize(std::pair<int, int> minAspectRatio,
	                         std::pair<int, int> maxAspectRatio);
	void updateControllerStates();

	/// Called when a controller is added or removed
	std::function<void()> controllerChangedCallback;

	double timePerStep = 1.0 / 60.0;
	double mouseWheel = 0;
	GLuint vaoLine = 0;
	GLuint vaoRect = 0;
	unsigned int maxStepsPerFrame = 3;
	bool running = true;
	bool fullscreen_;
	bool isMouseVisible_ = true;
	bool relativeMouseMode = false;
	bool anyKeyPressed_ = false;
	bool isMultisampleSupported_ = true;
	std::array<bool, 3> mouseDown_{ { false, false, false } };
	std::array<bool, 3> mousePressed_{ { false, false, false } };
	std::map<unsigned int, bool> keyDown_;
	std::map<unsigned int, bool> keyPressed_;
	std::map<std::string, bool> characterDown_;
	std::map<std::string, bool> characterPressed_;
	std::stack<bool*> needToBeSetFalse_;
	int mousex_ = 0;
	int mousey_ = 0;
	int fontSize_ = 12;
	int width_, height_;

	/// UTF-8 string of characters that were pressed since the last frame
	std::string textInput;

	/// The usable canvas width, excluding letterboxing
	int canvasWidth = -1;

	/// The usable canvas height, excluding letterboxing
	int canvasHeight = -1;

	std::string fontName_;
	const static unsigned int PNG_BYTES_TO_CHECK = 4;
	std::shared_ptr<Work> currentWork_;
	bool changeWork = false;
	std::shared_ptr<Work> newWork_;
	std::vector<std::shared_ptr<Job>> jobs;
	unsigned int stepsPerFrame;
	double sleepPerFrame = 0; // in seconds
	double sleepCorrectionFactor;
	double timeSleptSinceLastCheck = 0;
	unsigned int numberOfSleeps = 0;
	unsigned int previousStepsPerFrame = 1;
	double lastCheckTime;
	unsigned int stepsSinceLastCheck;

	/// When VSYNC is active we will try to find out to what FPS/Hz the display is limiting us
	double maxFPS;

	/// How often the frame limiter has run
	unsigned int numberOfChecks;

	bool multitouch = false;

	// <fontSize, <fontName, FontImpl>>
	std::map<int, std::unordered_map<std::string, std::shared_ptr<FontImpl>>> fonts_;
	std::vector<std::function<void()>> updateInputCallbacks;
};
} // namespace jngl
