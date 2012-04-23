/*
Copyright 2011-2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "../window.hpp"
#include "../debug.hpp"
#include "sdl.hpp"
#include "windowimpl.hpp"

#include <stdexcept>

namespace jngl
{
	Window::Window(const std::string& title, const int width, const int height, const bool fullscreen)
		: fullscreen_(fullscreen), running_(false), isMouseVisible_(true), relativeMouseMode(false), isMultisampleSupported_(true),
		  anyKeyPressed_(false), fontSize_(12), width_(width), height_(height), mouseWheel_(0), fontName_(""), oldTime_(0),
		  needDraw_(true), impl(new WindowImpl), changeWork_(false)
	{
		mouseDown_.assign(false);
		mousePressed_.assign(false);

		SDL::init();

		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
		if(fullscreen) {
			flags |= SDL_WINDOW_FULLSCREEN;
		}
		impl->sdlWindow = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
						             width, height, flags);

		if(!impl->sdlWindow) {
			throw std::runtime_error(SDL_GetError());
		}

		impl->context = SDL_GL_CreateContext(impl->sdlWindow);

		SDL_EnableUNICODE(SDL_ENABLE);

		SetFontByName("Arial"); // Default font
		SetFontSize(fontSize_); // Load a font the first time

		Init(width, height);

		running_ = true;
	}

	std::string Window::GetFontFileByName(const std::string& fontname)
	{
		std::string tmp = fontname;
		if(fontname == "sans-serif") {
			tmp = "Arial";
		}
		return "/Library/Fonts/" + tmp + ".ttf";
	}

	Window::~Window()
	{
		SDL_GL_DeleteContext(impl->context);
		SDL_DestroyWindow(impl->sdlWindow);
		delete impl;
	}

	int Window::GetKeyCode(key::KeyType key)
	{
		switch(key) {
			case key::Left: return SDLK_LEFT;
			case key::Right: return SDLK_RIGHT;
			case key::Up: return SDLK_UP;
			case key::Down: return SDLK_DOWN;
			case key::Escape: return SDLK_ESCAPE;
			case key::BackSpace: return SDLK_BACKSPACE;
			case key::PageUp: return SDLK_PAGEUP;
			case key::PageDown: return SDLK_PAGEDOWN;
			case key::Return: return SDLK_RETURN;
			case key::Space: return SDLK_SPACE;
			case key::Home: return SDLK_HOME;
			case key::End: return SDLK_END;
			case key::Tab: return SDLK_TAB;
			case key::Clear: return SDLK_CLEAR;
			case key::Pause: return SDLK_PAUSE;
			case key::Delete: return SDLK_DELETE;
			case key::ControlL: return SDLK_LCTRL;
			case key::ControlR: return SDLK_RCTRL;
			case key::CapsLock: return SDLK_CAPSLOCK;
			case key::AltL: return SDLK_LALT;
			case key::AltR: return SDLK_RALT;
			case key::SuperL: return SDLK_LSUPER;
			case key::SuperR: return SDLK_RSUPER;
			case key::ShiftL: return SDLK_LSHIFT;
			case key::ShiftR: return SDLK_RSHIFT;
			case key::F1: return SDLK_F1;
			case key::F2: return SDLK_F2;
			case key::F3: return SDLK_F3;
			case key::F4: return SDLK_F4;
			case key::F5: return SDLK_F5;
			case key::F6: return SDLK_F6;
			case key::F7: return SDLK_F7;
			case key::F8: return SDLK_F8;
			case key::F9: return SDLK_F9;
			case key::F10: return SDLK_F10;
			case key::F11: return SDLK_F11;
			case key::F12: return SDLK_F12;
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

	void Window::UpdateInput()
	{
		if (relativeMouseMode) {
			mousex_ = 0;
			mousey_ = 0;
		}
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_QUIT:
					running_ = false;
					break;
				case SDL_MOUSEMOTION:
					if (relativeMouseMode) {
						mousex_ = event.motion.xrel;
						mousey_ = event.motion.yrel;
					} else {
						mousex_ = event.motion.x;
						mousey_ = event.motion.y;
					}
					break;
				case SDL_MOUSEBUTTONDOWN: {
					int button = -1;
					if (event.button.button == SDL_BUTTON_LEFT) {
						button = 0;
					}
					if (event.button.button == SDL_BUTTON_MIDDLE) {
						button = 1;
					}
					if (event.button.button == SDL_BUTTON_RIGHT) {
						button = 2;
					}
					if (button >= 0) {
						mouseDown_.at(button) = true;
						mousePressed_.at(button) = true;
					}
					break;
				}
				case SDL_MOUSEBUTTONUP: {
					int button = -1;
					if (event.button.button == SDL_BUTTON_LEFT) {
						button = 0;
					}
					if (event.button.button == SDL_BUTTON_MIDDLE) {
						button = 1;
					}
					if (event.button.button == SDL_BUTTON_RIGHT) {
						button = 2;
					}
					if (button >= 0) {
						mouseDown_.at(button) = false;
						mousePressed_.at(button) = false;
					}
					break;
				}
				case SDL_KEYDOWN: {
					keyDown_[event.key.keysym.sym] = true;
					keyPressed_[event.key.keysym.sym] = true;
					const char* name = SDL_GetKeyName(event.key.keysym.sym);
					if(strlen(name) == 1) {
						std::string tmp;
						if (KeyDown(key::ShiftL) || KeyDown(key::ShiftR)) {
							tmp.append(1, name[0]);
						} else {
							tmp.append(1, tolower(name[0]));
						}
						characterDown_[tmp] = true;
						characterPressed_[tmp] = true;
					}
					anyKeyPressed_ = true;
					break;
				}
				case SDL_KEYUP: {
					keyDown_[event.key.keysym.sym] = false;
					keyPressed_[event.key.keysym.sym] = false;
					const char* name = SDL_GetKeyName(event.key.keysym.sym);
					if(strlen(name) == 1) {
						std::string tmp;
						tmp.append(1, tolower(name[0]));
						characterDown_[tmp] = false;
						characterPressed_[tmp] = false;
					}
					break;
				}
			}
		}
	}

	void Window::SwapBuffers()
	{
		SDL_GL_SwapWindow(impl->sdlWindow);
	}

	void Window::SetMouseVisible(const bool visible)
	{
		isMouseVisible_ = visible;
		if(visible) {
			SDL_ShowCursor(SDL_ENABLE);
		} else {
			SDL_ShowCursor(SDL_DISABLE);
		}
	}

	void Window::SetTitle(const std::string& windowTitle)
	{
		SDL_SetWindowTitle(impl->sdlWindow, windowTitle.c_str());
	}

	int Window::MouseX()
	{
		return mousex_;
	}

	int Window::MouseY()
	{
		return mousey_;
	}

	void Window::SetMouse(const int xposition, const int yposition)
	{
		SDL_WarpMouseInWindow(impl->sdlWindow, xposition, yposition);
	}

	void Window::SetRelativeMouseMode(const bool relative) {
		relativeMouseMode = relative;
		int rtn = SDL_SetRelativeMouseMode(relative ? SDL_TRUE : SDL_FALSE);
		assert(rtn != -1);
		if (relative) {
			mousex_ = 0;
			mousey_ = 0;
		} else {
			SDL_GetMouseState(&mousex_, &mousey_);
		}
	}

	void Window::SetIcon(const std::string&)
	{
		return; // TODO: Not implemented yet
	}

	int GetDesktopWidth()
	{
		SDL::init();
		SDL_DisplayMode mode;
		SDL_GetDesktopDisplayMode(0, &mode);
		return mode.w;
	}

	int GetDesktopHeight()
	{
		SDL::init();
		SDL_DisplayMode mode;
		SDL_GetDesktopDisplayMode(0, &mode);
		return mode.h;
	}
}
