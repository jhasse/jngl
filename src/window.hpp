/*
Copyright 2007-2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#include "freetype.hpp"
#include "opengl.hpp"
#include "jngl/input.hpp"

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/type_traits.hpp>
#include <boost/array.hpp>
#include <boost/ptr_container/ptr_unordered_map.hpp>
#include <boost/noncopyable.hpp>
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
    #endif
#endif

namespace jngl
{
	class WindowImpl;
	class Job;
	class Work;

	bool Init(const int width, const int height); // Definied in main.cpp

	class Window : boost::noncopyable {
	public:
		Window(const std::string& title, const int width, const int height, const bool fullscreen);
		~Window();
		bool Running();
		void Quit();
		void Continue();
		void UpdateInput();
		void UpdateKeyStates();
		void SwapBuffers();
		void SetRelativeMouseMode(bool relative);
		bool GetRelativeMouseMode() const;
		void SetMouseVisible(bool visible);
		bool GetMouseVisible() const;
		bool isMultitouch() const;
		int MouseX();
		int MouseY();
		int GetWidth() const;
		int GetHeight() const;
		double GetTextWidth(const std::string&);
		int getLineHeight();
		void setLineHeight(int);
		bool GetFullscreen() const;
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
		void Print(const std::string& text, const int xposition, const int yposition);
		int GetFontSize() const;
		void SetFontSize(const int size);
		void SetFont(const std::string&);
		void SetFontByName(const std::string&);
		bool IsMultisampleSupported() const;
		void SetIcon(const std::string&);
		double GetMouseWheel() const;
		std::string GetFont() const;
		boost::shared_ptr<FontImpl> getFontImpl();
		void SetWork(boost::shared_ptr<Work>);
		void MainLoop();
		void stepIfNeeded();
		void draw() const;
		boost::shared_ptr<Work> getWork();
		void setConfigPath(const std::string&);
		std::string getConfigPath() const;
		void addJob(boost::shared_ptr<Job>);
		void resetFrameLimiter();
#if !defined (__APPLE__) && !defined (ANDROID)
	#ifdef __linux
		boost::shared_ptr<Display> pDisplay_;
		static void ReleaseXData(void*);
	#else
		static void ReleaseDC(HWND, HDC);
		static void ReleaseRC(HGLRC);
	#endif
#else
		WindowImpl* getImpl() const;
#endif
	private:
		int GetKeyCode(jngl::key::KeyType key);
		std::string GetFontFileByName(const std::string& fontname);

		bool fullscreen_, running_, isMouseVisible_, relativeMouseMode, isMultisampleSupported_, anyKeyPressed_;
		boost::array<bool, 3> mouseDown_;
		boost::array<bool, 3> mousePressed_;
		std::map<unsigned int, bool> keyDown_;
		std::map<unsigned int, bool> keyPressed_;
		std::map<std::string, bool> characterDown_;
		std::map<std::string, bool> characterPressed_;
		std::stack<bool*> needToBeSetFalse_;
		int mousex_, mousey_, fontSize_, width_, height_;
		double mouseWheel_;
		std::string fontName_;
		const static unsigned int PNG_BYTES_TO_CHECK = 4;
		double oldTime;
		boost::shared_ptr<Work> currentWork_;
		bool changeWork_;
		boost::shared_ptr<Work> newWork_;
		std::string configPath;
		std::vector<boost::shared_ptr<Job>> jobs;
#if defined _MSC_VER || defined ANDROID
		float stepsPerFrame;
		bool multitouch;
#else
		float stepsPerFrame = 1;
		bool multitouch = false;
#endif

		// <fontSize, <fontName, FontImpl>>
		boost::ptr_unordered_map<int, boost::unordered_map<std::string, boost::shared_ptr<FontImpl>>> fonts_;
#if !defined (__APPLE__) && !defined (ANDROID)
	#ifdef __linux
		::Window window_;
		GLXContext context_;
		int screen_;
		XF86VidModeModeInfo oldMode_;
	#else
		boost::shared_ptr<boost::remove_pointer<HGLRC>::type> pRenderingContext_;
		boost::shared_ptr<boost::remove_pointer<HWND>::type> pWindowHandle_;
		boost::shared_ptr<boost::remove_pointer<HDC>::type> pDeviceContext_;
		int arbMultisampleFormat_;

		bool InitMultisample(HINSTANCE, PIXELFORMATDESCRIPTOR);
		void Init(const std::string& title, bool multisample);
		void DistinguishLeftRight();
	#endif
#else
		WindowImpl* const impl;

		friend class WindowImpl;
#endif
	};
}
