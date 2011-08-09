/*
Copyright 2011 Jan Niklas Hasse <jhasse@gmail.com>

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

#include <fontconfig/fontconfig.h>
#include <stdexcept>


/* Print modifier info */
void PrintModifiers( SDLMod mod ){
	printf( "Modifiers: " );
	
	/* If there are none then say so and return */
	if( mod == KMOD_NONE ){
		printf( "None\n" );
		return;
	}
	
	/* Check for the presence of each SDLMod value */
	/* This looks messy, but there really isn't    */
	/* a clearer way.                              */
	if( mod & KMOD_NUM ) printf( "NUMLOCK " );
	if( mod & KMOD_CAPS ) printf( "CAPSLOCK " );
	if( mod & KMOD_LCTRL ) printf( "LCTRL " );
	if( mod & KMOD_RCTRL ) printf( "RCTRL " );
	if( mod & KMOD_RSHIFT ) printf( "RSHIFT " );
	if( mod & KMOD_LSHIFT ) printf( "LSHIFT " );
	if( mod & KMOD_RALT ) printf( "RALT " );
	if( mod & KMOD_LALT ) printf( "LALT " );
	if( mod & KMOD_CTRL ) printf( "CTRL " );
	if( mod & KMOD_SHIFT ) printf( "SHIFT " );
	if( mod & KMOD_ALT ) printf( "ALT " );
	printf( "\n" );
}

/* Print all information about a key event */
void PrintKeyInfo( SDL_KeyboardEvent *key ){
	/* Is it a release or a press? */
	if( key->type == SDL_KEYUP )
		printf( "Release:- " );
	else
		printf( "Press:- " );
	
	/* Print the hardware scancode first */
	printf( "Scancode: 0x%02X", key->keysym.scancode );
	/* Print the name of the key */
	printf( ", Name: %s", SDL_GetKeyName( key->keysym.sym ) );
	/* We want to print the unicode info, but we need to make */
	/* sure its a press event first (remember, release events */
	/* don't have unicode info                                */
	if( key->type == SDL_KEYDOWN ){
		/* If the Unicode value is less than 0x80 then the    */
		/* unicode value can be used to get a printable       */
		/* representation of the key, using (char)unicode.    */
		printf(", Unicode: " );
		if( key->keysym.unicode < 0x80 && key->keysym.unicode > 0 ){
			printf( "%c (0x%04X)", (char)key->keysym.unicode,
				   key->keysym.unicode );
		}
		else{
			printf( "? (0x%04X)", key->keysym.unicode );
		}
	}
	printf( "\n" );
	/* Print modifier info */
	PrintModifiers((SDLMod) key->keysym.mod );
}

namespace jngl
{
	Window::Window(const std::string& title, const int width, const int height, const bool fullscreen)
		: fullscreen_(fullscreen), running_(false), isMouseVisible_(true), isMultisampleSupported_(true),
		  anyKeyPressed_(false), fontSize_(12), width_(width), height_(height), mouseWheel_(0), fontName_("")
	{
		mouseDown_.assign(false);
		mousePressed_.assign(false);
		
		if(SDL_Init(SDL_INIT_VIDEO) < 0) {
			throw std::runtime_error(SDL_GetError());
		}
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		
		sdlWindow = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
						             width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

		if(!sdlWindow) {
			throw std::runtime_error(SDL_GetError());
		}
		
		context = SDL_GL_CreateContext(sdlWindow);
		
		SDL_EnableUNICODE(SDL_ENABLE);

		SetFontByName("Arial"); // Default font
		SetFontSize(fontSize_); // Load a font the first time

		Init(width, height);

		running_ = true;
	}

	std::string Window::GetFontFileByName(const std::string& fontname)
	{
		const double size = 12;
		FcFontSet* fontSet = FcFontSetCreate();

		FcPattern* pattern = FcPatternBuild(NULL, FC_FAMILY, FcTypeString, fontname.c_str(), FC_SIZE, FcTypeDouble, size, NULL);
		FcConfigSubstitute(NULL,pattern, FcMatchPattern);
		FcDefaultSubstitute(pattern);

		FcResult result;
		FcPattern* match = FcFontMatch(NULL, pattern, &result);
		FcPatternDestroy(pattern);

		if(!match)
		{
			FcFontSetDestroy(fontSet);
			throw std::runtime_error(std::string("Couldn't load " + fontname));
		}
		FcFontSetAdd(fontSet, match);

		if(fontSet->nfont == 0)
		{
			throw std::runtime_error(std::string("Couldn't load " + fontname));
		}

		FcChar8* filename = NULL;
		FcPatternGetString(fontSet->fonts[0], FC_FILE, 0, &filename);
		return (const char*)filename;
	}

	Window::~Window()
	{
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(sdlWindow);
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

	void Window::BeginDraw()
	{
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_QUIT:
					running_ = false;
					break;
				case SDL_MOUSEMOTION:
					mousex_ = event.motion.x;
					mousey_ = event.motion.y;
					break;
				case SDL_KEYDOWN: {
					keyDown_[event.key.keysym.sym] = true;
					keyPressed_[event.key.keysym.sym] = true;
					const char* name = SDL_GetKeyName(event.key.keysym.sym);
					if(strlen(name) == 1) {
						std::string tmp;
						tmp.append(1, tolower(name[0]));
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
		glLoadIdentity();
	}

	void Window::EndDraw()
	{
		SDL_GL_SwapWindow(sdlWindow);
	}

	void Window::SetMouseVisible(const bool visible)
	{
		if(visible) {
			SDL_ShowCursor(SDL_ENABLE);
		} else {
			SDL_ShowCursor(SDL_DISABLE);
		}
	}

	void Window::SetTitle(const std::string& windowTitle)
	{
		
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
	}
	
	void Window::SetIcon(const std::string&)
	{
		return; // TODO: Not implemented yet
	}

	int GetDesktopWidth()
	{
		return SDL_GetVideoInfo()->current_w;
	}
	
	int GetDesktopHeight()
	{
		return SDL_GetVideoInfo()->current_h;
	}
}
