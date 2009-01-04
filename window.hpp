/*
Copyright 2007-2009 Jan Niklas Hasse <jhasse@gmail.com>

This file is part of JNGL.

JNGL is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

JNGL is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with JNGL.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "jngl.hpp"
#include "freetype.hpp"
#include "opengl.hpp"

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/type_traits.hpp>
#include <boost/array.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <map>

#ifdef linux
	#include <GL/glx.h>
	#include <GL/glu.h>
	#include <X11/extensions/xf86vmode.h>
	#include <X11/keysym.h>
#else
	#include <windows.h>
#endif

namespace jngl
{
	bool Init(const int width, const int height); // Definied in main.cpp
	class Window
	{
	public:
		Window(const std::string& title, const int width, const int height, const bool fullscreen);
		~Window();
		bool Running();
		void BeginDraw();
		void EndDraw();
		void Quit();
		void Continue();
		void SetMouseVisible(const bool visible);
		bool GetMouseVisible() const;
		int MouseX();
		int MouseY();
		int GetWidth() const;
		int GetHeight() const;
		double GetTextWidth(const std::string&);
		bool GetFullscreen() const;
		bool KeyDown(const int key);
		bool KeyPressed(const int key);
		bool KeyDown(const char key);
		bool KeyPressed(const char key);
		bool MouseDown(mouse::Button button);
		bool MousePressed(mouse::Button button);
		void SetMouse(const int xposition, const int yposition);
		void SetTitle(const std::string& title);
		void Print(const std::string& text, const double xposition, const double yposition);
		void FontSize(const int size);
		void SetFont(const std::string&);
		void SetFontByName(const std::string&);
		bool IsMultisampleSupported() const;
#ifdef linux
		boost::shared_ptr<Display> pDisplay_;
		static void ReleaseXData(void*);
#else
		static void ReleaseDC(HWND, HDC);
		static void ReleaseRC(HGLRC);
#endif
	private:
		std::string GetFontFileByName(const std::string& fontname);
		bool fullscreen_, running_, isMouseVisible_, isMultisampleSupported_;
		boost::array<bool, 3> mouseDown_;
		boost::array<bool, 3> mousePressed_;
		std::map<unsigned int, bool> keyDown_;
		std::map<unsigned int, bool> keyPressed_;
		int mousex_, mousey_, fontSize_, width_, height_;
		std::string fontName_;

		// <fontSize, <fontName, Font> >
		boost::ptr_map<int, boost::ptr_map<std::string, Font> > fonts_;
#ifdef linux
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
		void Init(const std::string& multisample, bool multisample);
		void DistinguishLeftRight();
#endif
	};
}
