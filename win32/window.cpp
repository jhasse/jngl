/*
Copyright 2007-2008 Jan Niklas Hasse <jhasse@gmail.com>

This file is part of jngl.

jngl is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

jngl is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with jngl.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "../window.hpp"
#include "../debug.hpp"
#include "../finally.hpp"
#include <gl/gl.h>
#include <boost/bind.hpp>
#include <stdexcept>
#include <iostream>
#include <windowsx.h> // GET_X_LPARAM
#include <cassert>
#include <shlobj.h>

namespace jngl
{
	LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	Window::Window(const std::string& title, const int width, const int height, const bool fullscreen) : fullscreen_(fullscreen), running_(false), fontSize_(12)
	{
		mouseDown_.assign(false);
		mousePressed_.assign(false);
		GLuint		PixelFormat;			// Holds The Results After Searching For A Match
		WNDCLASS	wc;						// Windows Class Structure
		DWORD		dwExStyle;				// Window Extended Style
		DWORD		dwStyle;				// Window Style
		RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
		WindowRect.left=(long)0;			// Set Left Value To 0
		WindowRect.right=(long)width;		// Set Right Value To Requested Width
		WindowRect.top=(long)0;				// Set Top Value To 0
		WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height

		HINSTANCE hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
		wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
		wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
		wc.cbClsExtra		= 0;									// No Extra Window Data
		wc.cbWndExtra		= 0;									// No Extra Window Data
		wc.hInstance		= hInstance;							// Set The Instance
		wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
		wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
		wc.hbrBackground	= NULL;									// No Background Required For GL
		wc.lpszMenuName		= NULL;									// We Don't Want A Menu
		wc.lpszClassName	= "OpenGL";								// Set The Class Name

		if (!RegisterClass(&wc))
			throw std::runtime_error("Failed To Register The Window Class.");

		if(fullscreen)
		{
			dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style
			dwStyle=WS_POPUP | WS_VISIBLE;
		}
		else
		{
			dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
			dwStyle=WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_VISIBLE;							// Windows Style
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
			throw std::runtime_error("Window Creation Error.");

		if(fullscreen)
		{
			DEVMODE devmode;
			memset(&devmode, 0, sizeof(devmode));
			devmode.dmSize = sizeof(devmode);
			devmode.dmPelsWidth = width;
			devmode.dmPelsHeight = height;
			devmode.dmBitsPerPel = 32;
			devmode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

			if (ChangeDisplaySettings(&devmode, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
				throw std::runtime_error("The Requested Fullscreen Mode Is Not Supported By Your Video Card.");
		}

		static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
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
			throw std::runtime_error("Can't Create A GL Device Context.");
		if (!(PixelFormat=ChoosePixelFormat(pDeviceContext_.get(), &pfd)))
			throw std::runtime_error("Can't Find A Suitable PixelFormat.");
		if(!SetPixelFormat(pDeviceContext_.get(), PixelFormat, &pfd))
			throw std::runtime_error("Can't Set The PixelFormat.");
		pRenderingContext_.reset(wglCreateContext(pDeviceContext_.get()), ReleaseRC);
		if(!pRenderingContext_)
			throw std::runtime_error("Can't Create A GL Rendering Context.");
		if(!wglMakeCurrent(pDeviceContext_.get(), pRenderingContext_.get()))
			throw std::runtime_error("Can't Activate The GL Rendering Context.");

		SetFontByName("Arial");
		FontSize(fontSize_);

		if(!Init(width, height))
			throw std::runtime_error("Initialization Failed.");

		running_ = true;
	}

	Window::~Window()
	{
		if(fullscreen_)
		{
			ChangeDisplaySettings(NULL, 0);
			ShowCursor(true);
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
		path += fontname;
		path += ".ttf";
		Debug("GetFontFileByName: "); Debug(path); Debug("\n");
		return path;
	}

	void Window::ReleaseDC(HWND hwnd, HDC hdc)
	{
		if(!::ReleaseDC(hwnd, hdc))
			std::cerr << "Release Device Context Failed." << std::endl;
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

	void Window::MouseVisible(const bool visible)
	{
		ShowCursor(visible);
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
