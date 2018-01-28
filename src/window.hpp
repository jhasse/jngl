// Copyright 2007-2017 Jan Niklas Hasse <jhasse@gmail.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "freetype.hpp"
#include "opengl.hpp"
#include "jngl/input.hpp"

#include <string>
#include <boost/type_traits.hpp>
#include <array>
#include <boost/ptr_container/ptr_unordered_map.hpp>
#include <stack>

#ifdef __linux
	#ifdef ANDROID

	#else
		#include <GL/glx.h>
		#include <GL/glu.h>
		#include <X11/extensions/xf86vmode.h>
		#include <X11/keysym.h>
	#endif
#else
	#ifdef _WIN32
		#include <windows.h>
		#include <xinput.h>

		extern XINPUT_STATE states[XUSER_MAX_COUNT];
	#endif
#endif

namespace jngl {
	class WindowImpl;
	class Job;
	class Work;

	class Window {
	public:
		Window(const std::string& title, int width, int height, bool fullscreen,
		       int screenWidth, int screenHeight);
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		~Window();
		bool isRunning();
		void quit();
		void cancelQuit();
		void UpdateInput();
		void updateKeyStates();
		void SwapBuffers();
		void SetRelativeMouseMode(bool relative);
		bool getRelativeMouseMode() const;
		void SetMouseVisible(bool visible);
		bool getMouseVisible() const;
		bool isMultitouch() const;
		int getMouseX();
		int getMouseY();
		int getWidth() const;
		int getHeight() const;
		double GetTextWidth(const std::string&);
		int getLineHeight();
		void setLineHeight(int);
		bool getFullscreen() const;
		void setFullscreen(bool);
		bool getKeyDown(const key::KeyType key);
		bool getKeyPressed(const key::KeyType key);
		void setKeyPressed(const key::KeyType key, bool);
		bool getKeyDown(const std::string& key);
		bool getKeyPressed(const std::string& key);
		void setKeyPressed(const std::string& key, bool);
		bool getMouseDown(mouse::Button);
		void setMouseDown(mouse::Button, bool);
		bool getMousePressed(mouse::Button button);
		void setMousePressed(mouse::Button button, bool);
		void SetMouse(const int xposition, const int yposition);
		void SetTitle(const std::string& title);
		void print(const std::string& text, const int xposition, const int yposition);
		int getFontSize() const;
		void setFontSize(const int size);
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
		void draw() const;
		std::shared_ptr<Work> getWork();
		void setConfigPath(const std::string&);
		std::string getConfigPath() const;
		void addJob(std::shared_ptr<Job>);
		void resetFrameLimiter();
		unsigned int getStepsPerSecond() const;
		void setStepsPerSecond(unsigned int);
		void addUpdateInputCallback(std::function<void()>);
#ifdef _WIN32
		static void ReleaseDC(HWND, HDC);
		static void ReleaseRC(HGLRC);
#else
		WindowImpl* getImpl() const;
#endif
	private:
		int GetKeyCode(jngl::key::KeyType key);
		std::string GetFontFileByName(const std::string& fontname);

		double timePerStep = 1.0 / 60.0;
		bool fullscreen_, running_, isMouseVisible_, relativeMouseMode, isMultisampleSupported_, anyKeyPressed_;
		std::array<bool, 3> mouseDown_;
		std::array<bool, 3> mousePressed_;
		std::map<unsigned int, bool> keyDown_;
		std::map<unsigned int, bool> keyPressed_;
		std::map<std::string, bool> characterDown_;
		std::map<std::string, bool> characterPressed_;
		std::stack<bool*> needToBeSetFalse_;
		int mousex_ = 0;
		int mousey_ = 0;
		int fontSize_, width_, height_;

		/// The usable canvas width, excluding letterboxing
		const int screenWidth;

		/// The usable canvas height, excluding letterboxing
		const int screenHeight;

		double mouseWheel_ = 0;
		std::string fontName_;
		const static unsigned int PNG_BYTES_TO_CHECK = 4;
		double oldTime;
		std::shared_ptr<Work> currentWork_;
		bool changeWork_;
		std::shared_ptr<Work> newWork_;
		std::string configPath;
		std::vector<std::shared_ptr<Job>> jobs;
#ifdef _MSC_VER
		double stepsPerFrame;
		bool multitouch;
#else
		float stepsPerFrame = 1;
		bool multitouch = false;
#endif

		// <fontSize, <fontName, FontImpl>>
		boost::ptr_unordered_map<int, boost::unordered_map<std::string, std::shared_ptr<FontImpl>>> fonts_;
		std::vector<std::function<void()>> updateInputCallbacks;
#ifdef _WIN32
		std::shared_ptr<boost::remove_pointer<HGLRC>::type> pRenderingContext_;
		std::shared_ptr<boost::remove_pointer<HWND>::type> pWindowHandle_;
		std::shared_ptr<boost::remove_pointer<HDC>::type> pDeviceContext_;
		int arbMultisampleFormat_;
		bool touchscreenActive = false;
		int relativeX = 0;
		int relativeY = 0;

		bool InitMultisample(HINSTANCE, PIXELFORMATDESCRIPTOR);
		void Init(const std::string& title, bool multisample);
		void DistinguishLeftRight();
#else
		WindowImpl* const impl;

		friend class WindowImpl;
#endif
	};
}
