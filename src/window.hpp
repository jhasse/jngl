// Copyright 2007-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#pragma once

#include "jngl/MouseInfo.hpp"
#include "jngl/Scene.hpp"
#include "jngl/TextInputSession.hpp"
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
	void setMouseConfined(bool confined);
	bool getMouseConfined() const;
	void SetMouseVisible(bool visible);
	void increaseMouseHiddenCount();
	void decreaseMouseHiddenCount();
	bool getMouseVisible() const;
	bool isMultitouch() const;
	std::vector<Vec2> getTouchPositions() const;
	int getMouseX() const;
	int getMouseY() const;
	MouseInfo& getMouseInfo();
	/// Size of the canvas in the same units as getWidth(), i.e. dividing it by
	/// jngl::getScaleFactor() gives jngl::getScreenSize()
	int getCanvasWidth() const;
	int getCanvasHeight() const;
	int getWidth() const;
	int getHeight() const;

	/// Size of the window in actual pixels
	int getActualWidth() const;
	int getActualHeight() const;

	/// Size of the canvas inside of the window in actual pixels, i.e. excluding letter-boxing
	int getActualCanvasWidth() const;
	int getActualCanvasHeight() const;

	/// When the Window gets resized this returns the scaling for each direction (since letterboxing
	/// might result in different values) which has to be taken into account for FrameBuffers
	float getResizedWindowScalingX() const;
	float getResizedWindowScalingY() const;

	/// Called by the backends when the window has been resized to \a width x \a height actual
	/// pixels. Fits the current canvas into it, see updateScreenSize() for changing the canvas.
	void setActualSize(int width, int height);

	/// Asks the active Scene whether it supports the screen size which would fill the whole window
	/// and changes the canvas accordingly, see Scene::supportsScreenSize()
	void updateScreenSize();

	/// Converts from actual pixels of the (maybe resized) window to the coordinates of the window
	/// with the size width_ x height_, which the canvas is centered in
	Vec2 toWindowCoordinates(float x, float y) const;

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
	void draw() const;
	std::shared_ptr<Scene> getScene();
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

	/// Tells the OS/IME to start delivering typed characters, with a hint about what kind of text
	/// is expected (used by TextInputSession)
	void startTextInputSession(TextInputType type);

	/// Tells the OS/IME that no field wants typed characters right now (used by TextInputSession)
	void stopTextInputSession();

	/// Places the IME candidate window; \a area and \a cursor are in JNGL Screen coordinates, i.e.
	/// (0, 0) is the center of the screen and jngl::getScaleFactor() hasn't been applied yet, same
	/// as jngl::Rect / jngl::getMousePos(). Backends are responsible for converting to actual
	/// window pixels themselves (used by TextInputSession)
	void setTextInputArea(Rect area, double cursor);

	void initGlObjects();
	void drawSquare(const Mat3& modelview, Rgba color) const;
	void drawRoundedSquare(const Mat3& modelview, Rgba color, Vec2 size, float topLeft,
	                       float topRight, float bottomLeft, float bottomRight) const;
	void onControllerChanged(std::function<void()>);
	void bindSystemFramebufferAndRenderbuffer();


private:
	static int GetKeyCode(jngl::key::KeyType key);
	static std::string GetFontFileByName(const std::string& fontname);
	/// Calculates canvasWidth and canvasHeight for a window of width_ x height_ actual pixels,
	/// letter-boxing the canvas to the aspect ratios. Also applies AppParameters::scaleFactor.
	void calculateCanvasSize(std::pair<int, int> minAspectRatio,
	                         std::pair<int, int> maxAspectRatio);

	/// width_, height_, canvasWidth and canvasHeight, i.e. in the units of getScaleFactor()
	struct Canvas {
		int width;
		int height;
		int canvasWidth;
		int canvasHeight;
		bool operator==(const Canvas&) const = default;
	};

	/// The Canvas for the actual window, either filling it or letter-boxed to the aspect ratios
	/// passed to calculateCanvasSize(). As the scale factor can't change anymore, it's zoomed so
	/// that its screen size is what AppParameters::scaleFactor returns for it.
	Canvas calculateCanvas(bool letterboxed) const;

	/// Initializes OpenGL (again, on Android) after the context has been created for a window of
	/// actualWidth x actualHeight
	void initGl();

	/// Fits the canvas into the actual window, letter-boxing it where the aspect ratios differ, and
	/// updates the projection matrix and the viewport accordingly
	void updateLetterboxing();
	void updateControllerStates();

	/// Destroys the scenes, jobs and fonts, which might hold OpenGL resources. Called first thing
	/// by ~Window, as `impl` is the last member and therefore destroyed first, taking the OpenGL
	/// context with it (SDL even unloads the OpenGL library with its last window). Normally
	/// hideWindow() has destroyed the ShaderCache by then, so that Textures don't touch OpenGL
	/// anymore, but not when the Window is destroyed by std::exit() calling ~WindowPointer.
	void releaseResources();

	/// Called when a controller is added or removed
	std::function<void()> controllerChangedCallback;

	unsigned int stepsPerSecond = 60;
	double mouseWheel = 0;
	GLuint vaoSquare = 0;
	bool shouldExit = false;
	std::optional<int> forceExitCode;
	bool fullscreen_;
	bool isMouseVisible_ = true;
	bool relativeMouseMode = false;
	bool mouseConfined = false;
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

	/// As passed to calculateCanvasSize()
	std::pair<int, int> minAspectRatio;
	std::pair<int, int> maxAspectRatio;

	/// As calculated by calculateCanvasSize() when the window got created. The canvas goes back to
	/// this when the active Scene doesn't support any of the candidates.
	Canvas originalCanvas{ -1, -1, -1, -1 };

	/// What the active Scene gets asked for in this order, see Scene::supportsScreenSize(). Empty
	/// if it has to be calculated again for the actual window.
	std::vector<Canvas> canvasCandidates;

	/// Size of the window in actual pixels. Differs from width_ and height_ after the window has
	/// been resized (e.g. by rotating the device), as long as the active Scene doesn't support the
	/// new screen size.
	int actualWidth = -1;
	int actualHeight = -1;

	/// The usable canvas inside of actualWidth x actualHeight, excluding letterboxing
	int actualCanvasWidth = -1;
	int actualCanvasHeight = -1;

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

public:
	friend class WindowImpl;
	std::unique_ptr<WindowImpl> impl;
};
} // namespace jngl
