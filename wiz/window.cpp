/*
Copyright 2009 Jan Niklas Hasse <jhasse@gmail.com>

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
#include "../opengl.hpp"
#include "../jngl.hpp"

#include <cassert>

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>

#define BIT(number) (1<<(number))

/* GPIO */
#define GPIOAPAD		(memregs16[0xA018>>1])
#define GPIOBPAD		(memregs16[0xA058>>1])
#define GPIOCPAD		(memregs16[0xA098>>1])

enum {
	BTN_A = BIT(20),
	BTN_B = BIT(21),
	BTN_X = BIT(22),
	BTN_Y = BIT(23),
	BTN_L = BIT(7),
	BTN_R = BIT(6),
	BTN_MENU = BIT(9),
	BTN_SELECT = BIT(8),
	BTN_LEFT = BIT(16),
	BTN_RIGHT = BIT(17),
	BTN_UP = BIT(18),
	BTN_DOWN = BIT(19),
	BTN_VOLUP = BIT(10),
	BTN_VOLDOWN = BIT(11)
};

int memfd;
volatile uint32_t *memregs32;
volatile uint16_t *memregs16;

namespace jngl
{
	Window::Window(const std::string& title, const int width, const int height, const bool fullscreen)
		: fullscreen_(fullscreen), running_(false), isMouseVisible_(true), isMultisampleSupported_(true),
		  anyKeyPressed_(false), fontSize_(12), width_(width), height_(height), fontName_("")
	{
		mouseDown_.assign(false);
		mousePressed_.assign(false);

		memfd = open("/dev/mem", O_RDWR);
		assert(memfd >= 0);

		memregs32 = (volatile uint32_t*)mmap(0, 0x20000, PROT_READ|PROT_WRITE, MAP_SHARED, memfd, 0xC0000000);
		assert(memregs32 != (volatile uint32_t*)0xFFFFFFFF);
		memregs16 = (volatile uint16_t*)memregs32;

		nativeWindow_ = OS_CreateWindow();
		display_ = eglGetDisplay((NativeDisplayType)0);

		EGLint majorVersion;
		EGLint minorVersion;
		eglInitialize(display_, &majorVersion, &minorVersion);
		Debug("eglInitialize() finished. Version: "); Debug(majorVersion); Debug("."); Debug(minorVersion); Debug("\n");

		EGLint attribList[] =
		{
		  EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		  EGL_BUFFER_SIZE, 16,
		  EGL_DEPTH_SIZE, 16,
		  EGL_NONE, EGL_NONE
		};
		EGLConfig config;
		EGLint numConfigs;
		eglChooseConfig(display_, attribList, &config, 1, &numConfigs);

		surface_ = eglCreateWindowSurface(display_, config, nativeWindow_, NULL);
		context_ = eglCreateContext(display_, config, EGL_NO_CONTEXT, NULL);
		eglMakeCurrent(display_, surface_, surface_, context_);

		SetFont("/usr/gp2x/HYUni_GPH_B_V1.01.ttf");
		SetFontSize(fontSize_); // Load a font the first time
		Init(width, height);

		running_ = true;
	}

	std::string Window::GetFontFileByName(const std::string& fontname)
	{
		// Not Implemented yet
		return "";
	}

	Window::~Window()
	{
		Debug("Cleaning up Window ... ");
		eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		eglTerminate(display_);
		free(nativeWindow_);

		memregs16 = NULL;
		memregs32 = NULL;
		close(memfd);
		Debug("OK\n");
	}
	
	int Window::GetKeyCode(key::KeyType key)
	{
		switch(key)
		{
			case key::WizUp: return BTN_UP;
			case key::WizDown: return BTN_DOWN;
			case key::WizLeft: return BTN_LEFT;
			case key::WizRight: return BTN_RIGHT;
			case key::WizA: return BTN_A;
			case key::WizB: return BTN_B;
			case key::WizX: return BTN_X;
			case key::WizY: return BTN_Y;
			case key::WizL: return BTN_L;
			case key::WizR: return BTN_R;
			case key::WizMenu: return BTN_MENU;
			case key::WizSelect: return BTN_SELECT;
			case key::WizVolUp: return BTN_VOLUP;
			case key::WizVolDown: return BTN_VOLDOWN;
			default:
				return 0;
		};
	}

	bool Window::KeyDown(const std::string& key)
	{
		return false;
	}

	bool Window::KeyPressed(const std::string& key)
	{
		return false;
	}

	void Window::BeginDraw()
	{
		for(int i = 6; i < 24; ++i)
		{
			if(~((GPIOCPAD << 16) | GPIOBPAD) & BIT(i))
			{
				if(!keyDown_[BIT(i)])
				{
					anyKeyPressed_ = true;
					keyPressed_[BIT(i)] = true;
					keyTimeout_[BIT(i)] = jngl::Time() + 0.4;
				}
				keyDown_[BIT(i)] = true;
				if(keyTimeout_[BIT(i)] - jngl::Time() < 0)
				{
					keyTimeout_[BIT(i)] = jngl::Time() + 0.07;
					keyPressed_[BIT(i)] = true;
				}
			}
			else if(keyDown_[BIT(i)])
			{
				keyDown_[BIT(i)] = false;
				keyPressed_[BIT(i)] = false;
			}
		}
		glLoadIdentity();
	}

	void Window::EndDraw()
	{
		eglSwapBuffers(display_, surface_);
	}

	void Window::SetMouseVisible(const bool visible)
	{
	}

	void Window::SetTitle(const std::string& windowTitle)
	{
	}

	int Window::MouseX()
	{
		return 0;
	}

	int Window::MouseY()
	{
		return 0;
	}

	void Window::SetMouse(const int xposition, const int yposition)
	{
		// Not implemented yet
	}
	
	void Window::SetIcon(const std::string&)
	{
		// Not supported
	}
}
