/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>

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

#include "../window.hpp"
#include "../debug.hpp"
#include "windowimpl.hpp"

#include <stdexcept>

namespace jngl
{		
	Window::Window(const std::string& title, const int width, const int height, const bool fullscreen)
	: fullscreen_(fullscreen), running_(false), isMouseVisible_(true),
	  relativeMouseMode(false), isMultisampleSupported_(true),
	  anyKeyPressed_(false), mousex_(0), mousey_(0), fontSize_(12), width_(width), height_(height),
	  mouseWheel_(0), fontName_(""), impl(new WindowImpl(this)),
	  changeWork_(false)
	{
		mouseDown_.assign(false);
		mousePressed_.assign(false);
		
		Init(height, width);
		
		running_ = true;
	}
	
	std::string Window::GetFontFileByName(const std::string& fontname)
	{
		std::string tmp = fontname;
		if(fontname == "sans-serif") {
			tmp = "Arial";
		}
		return tmp + ".ttf";
	}
	
	Window::~Window()
	{
		delete impl;
	}
	
	int Window::GetKeyCode(key::KeyType key)
	{
		switch(key) {
			default: return 0;
		}
	}
	
	bool Window::KeyDown(const std::string& key)
	{
		return characterDown_[key];
	}
	
	bool Window::KeyPressed(const std::string& key)
	{
		if(characterPressed_[key])
		{
			characterPressed_[key] = false;
			return true;
		}
		return characterPressed_[key];
	}
	
	void Window::UpdateInput() {
		impl->updateInput();
	}
	
	void Window::SwapBuffers()
	{
	}
	
	void Window::SetMouseVisible(const bool visible)
	{
	}
	
	void Window::SetTitle(const std::string& windowTitle)
	{
	}
	
	int Window::MouseX() {
		return mousex_;
	}
	
	int Window::MouseY() {
		return mousey_;
	}
	
	void Window::SetMouse(const int xposition, const int yposition)
	{
	}
	
	void Window::SetRelativeMouseMode(const bool relative) {
		relativeMouseMode = relative;
		impl->setRelativeMouseMode(relative);
	}
	
	void Window::SetIcon(const std::string&)
	{
	}
	
	int GetDesktopWidth()
	{
		return jngl::GetWindowWidth();
	}
	
	int GetDesktopHeight()
	{
		return jngl::GetWindowHeight();
	}
	
	WindowImpl* Window::getImpl() const {
		return impl;
	}
}
