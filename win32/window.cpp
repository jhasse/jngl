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

#include "../window.hpp"
#include "../debug.hpp"
#include "../finally.hpp"

#include "wglext.h"

#include <gl/gl.h>
#include <gl/glext.h>
#include <boost/bind.hpp>
#include <stdexcept>
#include <iostream>
#include <windowsx.h> // GET_X_LPARAM
#include <cassert>
#include <shlobj.h>

namespace jngl
{
	LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	// based on: http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=46
	bool Window::InitMultisample(HINSTANCE hInstance, PIXELFORMATDESCRIPTOR pfd)
	{
		PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
		if(!wglChoosePixelFormatARB)
		{
			return false;
		}

		HDC hDC = pDeviceContext_.get();

		int pixelFormat;
		UINT numFormats;
		float fAttributes[] = {0,0};

		// These Attributes Are The Bits We Want To Test For In Our Sample
		// Everything Is Pretty Standard, The Only One We Want To
		// Really Focus On Is The SAMPLE BUFFERS ARB And WGL SAMPLES
		// These Two Are Going To Do The Main Testing For Whether Or Not
		// We Support Multisampling On This Hardware
		int iAttributes[] = { WGL_DRAW_TO_WINDOW_ARB,GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB,GL_TRUE,
			WGL_ACCELERATION_ARB,WGL_FULL_ACCELERATION_ARB,
			WGL_COLOR_BITS_ARB,24,
			WGL_ALPHA_BITS_ARB,8,
			WGL_DEPTH_BITS_ARB,16,
			WGL_STENCIL_BITS_ARB,0,
			WGL_DOUBLE_BUFFER_ARB,GL_TRUE,
			WGL_SAMPLE_BUFFERS_ARB,GL_TRUE,
			WGL_SAMPLES_ARB, 4 ,						// Check For 4x Multisampling
			0,0};

		// First We Check To See If We Can Get A Pixel Format For 4 Samples
		bool valid = wglChoosePixelFormatARB(hDC, iAttributes, fAttributes, 1, &pixelFormat, &numFormats);

		// if We Returned True, And Our Format Count Is Greater Than 1
		if (valid && numFormats >= 1)
		{
			arbMultisampleFormat_ = pixelFormat;
			return true;
		}

		// Our Pixel Format With 4 Samples Failed, Test For 2 Samples
		iAttributes[19] = 2;
		valid = wglChoosePixelFormatARB(hDC, iAttributes, fAttributes, 1, &pixelFormat, &numFormats);
		if (valid && numFormats >= 1)
		{
			arbMultisampleFormat_ = pixelFormat;
			return true;
		}

		return false;
	}

	Window::Window(const std::string& title, const int width, const int height, const bool fullscreen)
		: fullscreen_(fullscreen), running_(false), isMouseVisible_(true), isMultisampleSupported_(false), fontSize_(12), width_(width), height_(height)
	{
		mouseDown_.assign(false);
		mousePressed_.assign(false);

		Init(title, false);
	}

	void Window::Init(const std::string& title, const bool multisample)
	{
		WNDCLASS	wc;
		DWORD		dwExStyle;
		DWORD		dwStyle;
		RECT		WindowRect;
		WindowRect.left   = 0;
		WindowRect.right  = width_;
		WindowRect.top    = 0;
		WindowRect.bottom = height_;

		HINSTANCE hInstance = GetModuleHandle(NULL);
		wc.style            = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc      = (WNDPROC) WndProc;
		wc.cbClsExtra       = 0;
		wc.cbWndExtra       = 0;
		wc.hInstance        = hInstance;
		wc.hIcon            = LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
		wc.hCursor          = LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
		wc.hbrBackground    = NULL;									// No Background Required For GL
		wc.lpszMenuName     = NULL;
		wc.lpszClassName    = "OpenGL";								// Set The Class Name

		static bool alreadyRegistered = false;
		if(!alreadyRegistered && !RegisterClass(&wc))
		{
			throw std::runtime_error("Failed To Register The Window Class.");
		}
		alreadyRegistered = true; // Only register once

		if(fullscreen_)
		{
			dwExStyle = WS_EX_APPWINDOW;
			dwStyle = WS_POPUP | WS_VISIBLE;
		}
		else
		{
			dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
			dwStyle = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_VISIBLE;							// Windows Style
		}

		AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

		// Create The Window
		pWindowHandle_.reset(CreateWindowEx(	dwExStyle,							// Extended Style For The Window
									"OpenGL",							// Class Name
									title.c_str(),								// Window Title
									dwStyle |							// Defined Window Style
									WS_CLIPSIBLINGS |					// Required Window Style
									WS_CLIPCHILDREN,					// Required Window Style
									CW_USEDEFAULT,
									CW_USEDEFAULT,
									WindowRect.right-WindowRect.left,	// Calculate Window Width
									WindowRect.bottom-WindowRect.top,	// Calculate Window Height
									NULL,								// No Parent Window
									NULL,								// No Menu
									hInstance,							// Instance
									NULL), DestroyWindow);
		if(!pWindowHandle_)
		{
			throw std::runtime_error("Window creation error.");
		}

		if(fullscreen_)
		{
			DEVMODE devmode;
			memset(&devmode, 0, sizeof(devmode));
			devmode.dmSize = sizeof(devmode);
			devmode.dmPelsWidth = width_;
			devmode.dmPelsHeight = height_;
			devmode.dmBitsPerPel = 32;
			devmode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

			if(ChangeDisplaySettings(&devmode, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
			{
				throw std::runtime_error("The requested fullscreen mode is not supported by your video card.");
			}
		}

		static PIXELFORMATDESCRIPTOR pfd =				// pfd Tells Windows How We Want Things To Be
		{
			sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
			1,											// Version Number
			PFD_DRAW_TO_WINDOW |						// Format Must Support Window
			PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
			PFD_DOUBLEBUFFER,							// Must Support Double Buffering
			PFD_TYPE_RGBA,								// Request An RGBA Format
			32,										// Select Our Color Depth
			0, 0, 0, 0, 0, 0,							// Color Bits Ignored
			0,											// No Alpha Buffer
			0,											// Shift Bit Ignored
			0,											// No Accumulation Buffer
			0, 0, 0, 0,									// Accumulation Bits Ignored
			16,											// 16Bit Z-Buffer (Depth Buffer)
			0,											// No Stencil Buffer
			0,											// No Auxiliary Buffer
			PFD_MAIN_PLANE,								// Main Drawing Layer
			0,											// Reserved
			0, 0, 0										// Layer Masks Ignored
		};

		pDeviceContext_.reset(GetDC(pWindowHandle_.get()), boost::bind(ReleaseDC, pWindowHandle_.get(), _1));
		if (!pDeviceContext_)
		{
			throw std::runtime_error("Can't create a GL device context.");
		}
		GLuint pixelFormat;
		if(multisample)
		{
			pixelFormat = arbMultisampleFormat_;
		}
		else
		{
			pixelFormat = ChoosePixelFormat(pDeviceContext_.get(), &pfd);
		}
		if (!pixelFormat)
		{
			throw std::runtime_error("Can't find a suitable PixelFormat.");
		}
		if(!SetPixelFormat(pDeviceContext_.get(), pixelFormat, &pfd))
		{
			throw std::runtime_error("Can't set the PixelFormat.");
		}
		pRenderingContext_.reset(wglCreateContext(pDeviceContext_.get()), ReleaseRC);
		if(!pRenderingContext_)
		{
			throw std::runtime_error("Can't create a GL rendering context.");
		}
		if(!wglMakeCurrent(pDeviceContext_.get(), pRenderingContext_.get()))
		{
			throw std::runtime_error("Can't activate the GL rendering context.");
		}

		if(!multisample && InitMultisample(hInstance, pfd))
		{
			pDeviceContext_.reset((HDC)0); // Destroy window
			try
			{
				Init(title, true); // Recreate with Anti-Aliasing support
				isMultisampleSupported_ = true;
			}
			catch(...)
			{
				// If Anti-Aliasing still doesn't work for some reason, let's turn it off again.
				Init(title, false);
			}
			return;
		}

		SetFontByName("Arial");
		FontSize(fontSize_);

		if(!jngl::Init(width_, height_))
		{
			throw std::runtime_error("Initialization failed.");
		}

		running_ = true;
	}

	Window::~Window()
	{
		if(fullscreen_)
		{
			ChangeDisplaySettings(NULL, 0);
		}
	}

	std::string Window::GetFontFileByName(const std::string& fontname)
	{
		// TODO: This is not a good way to find fonts
		char temp[MAX_PATH];
		if(SHGetFolderPath(NULL, CSIDL_FONTS, NULL, 0, temp) != S_OK)
		{
			throw std::runtime_error("Couldn't locate font directory.");
		}
		std::string path(temp);
		path += "\\";
		std::string file;
		if(fontname == "Times New Roman" || fontname == "serif")
		{
			file = "times";
		}
		else if(fontname == "Courier" || fontname == "Courier New" || fontname == "mono")
		{
			file = "cour";
		}
		else if(fontname == "sans" || fontname == "sans-serif")
		{
			file = "arial";
		}
		else
		{
			file = fontname;
		}
		path += file;
		path += ".ttf";
		return path;
	}

	void Window::ReleaseDC(HWND hwnd, HDC hdc)
	{
		if(!::ReleaseDC(hwnd, hdc))
		{
			std::cerr << "Release device context failed." << std::endl;
		}
	}

	void Window::ReleaseRC(HGLRC hrc)
	{
		if(!wglMakeCurrent(NULL, NULL))
			std::cerr << "Release Of DC And RC Failed." << std::endl;
		if(!wglDeleteContext(hrc))
			std::cerr << "Release Rendering Context Failed." << std::endl;
	}

	bool Window::Running()
	{
		return running_;
	}

	void Window::BeginDraw()
	{
		MSG msg;
		while(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			switch(msg.message)
			{
				case WM_QUIT:
					running_ = false;
				break;
				case WM_MOUSEMOVE:
					mousex_ = GET_X_LPARAM(msg.lParam);
					mousey_ = GET_Y_LPARAM(msg.lParam);
				break;
				case WM_LBUTTONDOWN:
					mouseDown_.at(0) = true;
					mousePressed_.at(0) = true;
				break;
				case WM_MBUTTONDOWN:
					mouseDown_.at(1) = true;
					mousePressed_.at(1) = true;
				break;
				case WM_RBUTTONDOWN:
					mouseDown_.at(2) = true;
					mouseDown_.at(2) = true;
				break;
				case WM_LBUTTONUP:
					mouseDown_.at(0) = false;
					mousePressed_.at(0) = false;
				break;
				case WM_MBUTTONUP:
					mouseDown_.at(1) = false;
					mousePressed_.at(1) = false;
				break;
				case WM_RBUTTONUP:
					mouseDown_.at(2) = false;
					mousePressed_.at(2) = false;
				break;
				case WM_KEYDOWN:
					keyDown_[msg.wParam] = true;
					keyPressed_[msg.wParam] = true;
				break;
				case WM_KEYUP:
					keyDown_[msg.wParam] = false;
					keyPressed_[msg.wParam] = false;
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		glLoadIdentity();
	}

	void Window::EndDraw()
	{
		SwapBuffers(pDeviceContext_.get());
	}

	void Window::Quit()
	{
		running_ = false;
	}

	void Window::SetMouseVisible(const bool visible)
	{
		if(isMouseVisible_ != visible)
		{
			ShowCursor(visible);
		}
		isMouseVisible_ = visible;
	}

	void Window::SetTitle(const std::string& windowTitle)
	{
		SetWindowText(pWindowHandle_.get(), windowTitle.c_str());
	}

	bool Window::KeyDown(const int key)
	{
		return keyDown_[key];
	}

	bool Window::KeyPressed(const int key)
	{
		if(keyPressed_[key])
		{
			keyPressed_[key] = false;
			return true;
		}
		return false;
	}

	bool Window::KeyDown(const char key)
	{
		return KeyDown((const int)toupper(key));
	}

	bool Window::KeyPressed(const char key)
	{
		return KeyPressed((const int)toupper(key));
	}

	LRESULT CALLBACK WndProc(	HWND	hWnd,			// Handle For This Window
								UINT	uMsg,			// Message For This Window
								WPARAM	wParam,			// Additional Message Information
								LPARAM	lParam)			// Additional Message Information
	{
		switch (uMsg)									// Check For Windows Messages
		{
			case WM_ACTIVATE:							// Watch For Window Activate Message
			{
				if (!HIWORD(wParam))					// Check Minimization State
				{
					//active=TRUE;						// Program Is Active
				}
				else
				{
					//active=FALSE;						// Program Is No Longer Active
				}

				return 0;								// Return To The Message Loop
			}
			case WM_SYSCOMMAND:							// Intercept System Commands
			{
				switch (wParam)							// Check System Calls
				{
					case SC_SCREENSAVE:					// Screensaver Trying To Start?
					case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
					return 0;							// Prevent From Happening
				}
				break;									// Exit
			}

			case WM_CLOSE:								// Did We Receive A Close Message?
			{
				PostQuitMessage(0);						// Send A Quit Message
				return 0;								// Jump Back
			}
		}

		// Pass All Unhandled Messages To DefWindowProc
		return DefWindowProc(hWnd,uMsg,wParam,lParam);
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
		POINT pnt;
		pnt.x = xposition;
		pnt.y = yposition;
		assert(ClientToScreen(pWindowHandle_.get(), &pnt));
		SetCursorPos(pnt.x, pnt.y);
	}
}
