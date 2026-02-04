// Copyright 2007-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#pragma once

#include "jngl/MouseInfo.hpp"
#include "jngl/Scene.hpp"
#include "jngl/input.hpp"
#include "opengl.hpp"
#include "timing/FrameLimiter.hpp"

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
class Rgba;
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
	void forceQuit(uint8_t exitcode);
	void cancelQuit();
	void UpdateInput();
	void updateKeyStates();
	void SwapBuffers();
	void SetRelativeMouseMode(bool relative);
	bool getRelativeMouseMode() const;
	void SetMouseVisible(bool visible);
	void increaseMouseHiddenCount();
	void decreaseMouseHiddenCount();
	bool getMouseVisible() const;
	bool isMultitouch() const;
	std::vector<Vec2> getTouchPositions() const;
	int getMouseX() const;
	int getMouseY() const;
	MouseInfo& getMouseInfo();
	int getCanvasWidth() const;
	int getCanvasHeight() const;
	int getWidth() const;
	int getHeight() const;

	/// When the Window gets resized this returns the scaling for each direction (since letterboxing
	/// might result in different values) which has to be taken into account for FrameBuffers (SDL
	/// backend only)
	float getResizedWindowScalingX() const;
	float getResizedWindowScalingY() const;

	ScaleablePixels getTextWidth(const std::string&);
	double getLineHeight();
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
	void print(const Mat3& modelview, const std::string& text);
	int getFontSize() const;
	void setFontSize(int size);
	void setFont(const std::string&);
	void setFontByName(const std::string&);
	bool isMultisampleSupported() const;
	void SetIcon(const std::string&);
	double getMouseWheel() const;
	std::string getFont() const;
	std::shared_ptr<FontImpl> getFontImpl();
	void setWork(std::shared_ptr<Scene>);

	/// Returns exitcode for process
	[[nodiscard]] uint8_t mainLoop();

	void stepIfNeeded();
	void sleepIfNeeded();
	void draw() const;
	std::shared_ptr<Scene> getWork();
	std::shared_ptr<Scene> getNextScene() const;
	void addJob(std::shared_ptr<Job>);
	void removeJob(Job*);
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
	void drawLine(Mat3 modelview, Vec2 b, Rgba color) const;
	void drawSquare(const Mat3& modelview, Rgba color) const;
	void onControllerChanged(std::function<void()>);
	void bindSystemFramebufferAndRenderbuffer();

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

	unsigned int stepsPerSecond = 60;
	double mouseWheel = 0;
	GLuint vaoLine = 0;
	GLuint vaoSquare = 0;
	bool shouldExit = false;
	std::optional<int> forceExitCode;
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
	MouseInfo mouseInfo;

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
	std::vector<std::shared_ptr<Job>> jobsToAdd;
	std::vector<Job*> jobsToRemove;
	int mouseHiddenCount = 0;

	FrameLimiter frameLimiter{ 1.0 / static_cast<double>(stepsPerSecond) };

	bool multitouch = false;

	// <fontSize, <fontName, FontImpl>>
	std::map<int, std::unordered_map<std::string, std::shared_ptr<FontImpl>>> fonts_;
	std::vector<std::function<void()>> updateInputCallbacks;

	GLuint systemFramebuffer = 0;
	GLuint systemRenderbuffer = 0;

#ifdef JNGL_PERFORMANCE_OVERLAY
	double lastStepDuration = 0;
#endif
};
} // namespace jngl
