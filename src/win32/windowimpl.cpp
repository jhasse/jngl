// Copyright 2007-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../window.hpp"
#include "../jngl/Finally.hpp"
#include "../jngl/window.hpp"
#include "../jngl/work.hpp"
#include "../jngl/debug.hpp"
#include "ConvertUTF.h"
#include "../main.hpp"

#ifndef NOPNG
#include <png.h>
#endif

#include <algorithm>
#include <cassert>
#include <cmath>
#include <epoxy/wgl.h>
#include <shlobj.h>
#include <sstream>
#include <mmsystem.h> // timeBeginPeriod
#include <windowsx.h> // GET_X_LPARAM

XINPUT_STATE states[XUSER_MAX_COUNT];

namespace jngl {

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// based on: http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=46
bool Window::InitMultisample(HINSTANCE, PIXELFORMATDESCRIPTOR) {
	if (!wglChoosePixelFormatARB) {
		return false;
	}

	HDC hDC = pDeviceContext_.get();

	int pixelFormat;
	UINT numFormats;
	float fAttributes[] = { 0, 0 };

	// These Attributes Are The Bits We Want To Test For In Our Sample
	// Everything Is Pretty Standard, The Only One We Want To
	// Really Focus On Is The SAMPLE BUFFERS ARB And WGL SAMPLES
	// These Two Are Going To Do The Main Testing For Whether Or Not
	// We Support Multisampling On This Hardware
	int iAttributes[] = { WGL_DRAW_TO_WINDOW_ARB,
		                  GL_TRUE,
		                  WGL_SUPPORT_OPENGL_ARB,
		                  GL_TRUE,
		                  WGL_ACCELERATION_ARB,
		                  WGL_FULL_ACCELERATION_ARB,
		                  WGL_COLOR_BITS_ARB,
		                  24,
		                  WGL_ALPHA_BITS_ARB,
		                  8,
		                  WGL_DEPTH_BITS_ARB,
		                  16,
		                  WGL_STENCIL_BITS_ARB,
		                  0,
		                  WGL_DOUBLE_BUFFER_ARB,
		                  GL_TRUE,
		                  WGL_SAMPLE_BUFFERS_ARB,
		                  GL_TRUE,
		                  WGL_SAMPLES_ARB,
		                  4, // Check For 4x Multisampling
		                  0,
		                  0 };

	// First We Check To See If We Can Get A Pixel Format For 4 Samples
	BOOL valid =
	    wglChoosePixelFormatARB(hDC, iAttributes, fAttributes, 1, &pixelFormat, &numFormats);

	// if We Returned True, And Our Format Count Is Greater Than 1
	if (valid && numFormats >= 1) {
		arbMultisampleFormat_ = pixelFormat;
		return true;
	}

	// Our Pixel Format With 4 Samples Failed, Test For 2 Samples
	iAttributes[19] = 2;
	valid = wglChoosePixelFormatARB(hDC, iAttributes, fAttributes, 1, &pixelFormat, &numFormats);
	if (valid && numFormats >= 1) {
		arbMultisampleFormat_ = pixelFormat;
		return true;
	}

	return false;
}

Window::Window(const std::string& title, const int width, const int height, const bool fullscreen,
               const int screenWidth, const int screenHeight)
: fullscreen_(fullscreen), running_(false), isMouseVisible_(true), relativeMouseMode(false),
  anyKeyPressed_(false), fontSize_(12), width_(width), height_(height), screenWidth(screenWidth),
  screenHeight(screenHeight), changeWork_(false), stepsPerFrame(1), multitouch(false) {
	mouseDown_.fill(false);
	mousePressed_.fill(false);

	Init(title, false);
}

void Window::Init(const std::string& title, const bool multisample) {
	WNDCLASS wc;
	DWORD dwExStyle;
	DWORD dwStyle;
	RECT WindowRect;
	WindowRect.left = 0;
	WindowRect.right = width_;
	WindowRect.top = 0;
	WindowRect.bottom = height_;

	HINSTANCE hInstance = GetModuleHandle(nullptr);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = "OpenGL"; // Set The Class Name

	static bool alreadyRegistered = false;
	if (!alreadyRegistered && !RegisterClass(&wc)) {
		throw std::runtime_error("Failed To Register The Window Class.");
	}
	alreadyRegistered = true; // Only register once

	if (fullscreen_) {
		dwExStyle = WS_EX_APPWINDOW;
		dwStyle = WS_POPUP;
	} else {
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE,
	                   dwExStyle); // Adjust Window To True Requested Size

	// Create The Window
	pWindowHandle_.reset(
	    CreateWindowEx(dwExStyle,     // Extended Style For The Window
	                   "OpenGL",      // Class Name
	                   title.c_str(), // Window Title
	                   dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT,
	                   WindowRect.right - WindowRect.left, // Calculate Window Width
	                   WindowRect.bottom - WindowRect.top, // Calculate Window Height
	                   nullptr, nullptr, hInstance, nullptr),
	    DestroyWindow);
	if (!pWindowHandle_) {
		throw std::runtime_error("Window creation error.");
	}

	if (fullscreen_) {
		DEVMODE devmode;
		memset(&devmode, 0, sizeof(devmode));
		devmode.dmSize = sizeof(devmode);
		devmode.dmPelsWidth = width_;
		devmode.dmPelsHeight = height_;
		devmode.dmBitsPerPel = 32;
		devmode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		if (ChangeDisplaySettings(&devmode, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
			throw std::runtime_error(
			    "The requested fullscreen mode is not supported by your video card.");
		}
	}

	static PIXELFORMATDESCRIPTOR pfd = // pfd Tells Windows How We Want Things To Be
	    {
		  sizeof(PIXELFORMATDESCRIPTOR), // Size Of This Pixel Format Descriptor
		  1,                             // Version Number
		  PFD_DRAW_TO_WINDOW |           // Format Must Support Window
		      PFD_SUPPORT_OPENGL |       // Format Must Support OpenGL
		      PFD_DOUBLEBUFFER,          // Must Support Double Buffering
		  PFD_TYPE_RGBA,                 // Request An RGBA Format
		  32,                            // Select Our Color Depth
		  0,
		  0,
		  0,
		  0,
		  0,
		  0, // Color Bits Ignored
		  0, // No Alpha Buffer
		  0, // Shift Bit Ignored
		  0, // No Accumulation Buffer
		  0,
		  0,
		  0,
		  0,              // Accumulation Bits Ignored
		  16,             // 16Bit Z-Buffer (Depth Buffer)
		  0,              // No Stencil Buffer
		  0,              // No Auxiliary Buffer
		  PFD_MAIN_PLANE, // Main Drawing Layer
		  0,              // Reserved
		  0,
		  0,
		  0 // Layer Masks Ignored
		};

	pDeviceContext_.reset(GetDC(pWindowHandle_.get()),
	                      [this](HDC hdc) { ReleaseDC(pWindowHandle_.get(), hdc); });
	if (!pDeviceContext_) {
		throw std::runtime_error("Can't create a GL device context.");
	}
	GLuint pixelFormat;
	if (multisample) {
		pixelFormat = arbMultisampleFormat_;
	} else {
		pixelFormat = ChoosePixelFormat(pDeviceContext_.get(), &pfd);
	}
	if (!pixelFormat) {
		throw std::runtime_error("Can't find a suitable PixelFormat.");
	}
	if (!SetPixelFormat(pDeviceContext_.get(), pixelFormat, &pfd)) {
		throw std::runtime_error("Can't set the PixelFormat.");
	}
	pRenderingContext_.reset(wglCreateContext(pDeviceContext_.get()), ReleaseRC);
	if (!pRenderingContext_) {
		throw std::runtime_error("Can't create a GL rendering context.");
	}
	if (!wglMakeCurrent(pDeviceContext_.get(), pRenderingContext_.get())) {
		throw std::runtime_error("Can't activate the GL rendering context.");
	}

	if (!multisample && isMultisampleSupported_ && InitMultisample(hInstance, pfd)) {
		pDeviceContext_.reset((HDC)0); // Destroy window
		try {
			jngl::debugLn("Recreating window with Anti-Aliasing support.");
			Init(title, true);
		} catch (...) {
			// If Anti-Aliasing still doesn't work for some reason, let's turn it off again.
			isMultisampleSupported_ = false;
			Init(title, false);
		}
		return;
	}

	::ShowWindow(pWindowHandle_.get(), SW_SHOWNORMAL);
	setFontByName("Arial");
	setFontSize(fontSize_);

	if (!jngl::Init(width_, height_, screenWidth, screenHeight)) {
		throw std::runtime_error("Initialization failed.");
	}

	running_ = true;
}

Window::~Window() {
	if (fullscreen_) {
		ChangeDisplaySettings(nullptr, 0);
	}
	fonts_.clear(); // Delete fonts before destroying the OpenGL context
}

std::string Window::GetFontFileByName(const std::string& fontName) {
	static std::unordered_map<std::string, std::string> cache;
	const auto it = cache.find(fontName);
	if (it != cache.end()) { return it->second; }
	std::string fontNameLower = fontName;
	std::transform(fontNameLower.begin(), fontNameLower.end(), fontNameLower.begin(), ::tolower);
	if (fontNameLower == "serif") {
		fontNameLower = "times";
	} else if (fontNameLower == "mono" || fontNameLower == "monospace") {
		fontNameLower = "cour";
	} else if (fontNameLower == "sans" || fontNameLower == "sans-serif") {
		fontNameLower = "arial";
	}
	HKEY hKey;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts",
	                 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
		throw std::runtime_error("Couldn't open registry.");
	}

	DWORD maxValueNameSize, maxValueDataSize;
	if (RegQueryInfoKey(hKey, 0, 0, 0, 0, 0, 0, 0, &maxValueNameSize, &maxValueDataSize, 0, 0) !=
	    ERROR_SUCCESS) {
		throw std::runtime_error("Couldn't query registry.");
	}

	DWORD valueIndex = 0;
	const auto valueName = std::make_unique<char[]>(maxValueNameSize);
	const auto valueData = std::make_unique<BYTE[]>(maxValueDataSize);
	std::string fontFile;
	LONG result;
	do {
		DWORD valueDataSize = maxValueDataSize;
		DWORD valueNameSize = maxValueNameSize;
		DWORD valueType;
		result = RegEnumValue(hKey, valueIndex, valueName.get(), &valueNameSize, 0, &valueType,
		                      valueData.get(), &valueDataSize);

		valueIndex++;

		if (result != ERROR_SUCCESS || valueType != REG_SZ) {
			continue;
		}

		std::string valueNameLower(valueName.get(), valueNameSize);
		std::transform(valueNameLower.begin(), valueNameLower.end(), valueNameLower.begin(), ::tolower);
		if (fontNameLower + " (truetype)" == valueNameLower) {
			fontFile.assign(reinterpret_cast<const char*>(valueData.get()), valueDataSize);
			break;
		}
		if ((fontNameLower == "arial" && valueNameLower == "liberation sans (truetype)") ||
		    (fontNameLower == "courier new" && valueNameLower == "liberation mono (truetype)") ||
		    (fontNameLower == "times new roman" &&
		     valueNameLower == "liberation serif (truetype)")) {
			// Fallbacks for WINE. No break, so that exact matches take priority
			fontFile.assign(reinterpret_cast<const char*>(valueData.get()), valueDataSize);
		}
	} while (result != ERROR_NO_MORE_ITEMS);

	RegCloseKey(hKey);
	if (fontFile.size() < 2 || fontFile[1] != ':') { // relative path?
		char fontDir[MAX_PATH];
		if (SHGetFolderPath(NULL, CSIDL_FONTS, NULL, 0, fontDir) != S_OK) {
			throw std::runtime_error("Couldn't locate font directory.");
		}
		std::ostringstream ss;
		ss << fontDir << "\\";
		if (fontFile.empty()) {
			ss << fontName << ".ttf";
		} else {
			ss << fontFile;
		}
		fontFile = ss.str();
	}
	cache.emplace(fontName, fontFile);
	return fontFile;
}

void Window::ReleaseDC(HWND hwnd, HDC hdc) {
	if (!::ReleaseDC(hwnd, hdc)) {
		debug("Release device context failed.");
	}
}

void Window::ReleaseRC(HGLRC hrc) {
	if (!wglMakeCurrent(nullptr, nullptr)) {
		debugLn("Release of DC and RC failed.");
	}
	if (!wglDeleteContext(hrc)) {
		debugLn("Release rendering context failed.");
	}
}

void Window::DistinguishLeftRight() {
	int codesToCheck[] = { GetKeyCode(jngl::key::ShiftL),   GetKeyCode(jngl::key::ShiftR),
		                   GetKeyCode(jngl::key::ControlL), GetKeyCode(jngl::key::ControlR),
		                   GetKeyCode(jngl::key::AltL),     GetKeyCode(jngl::key::AltR) };
	for (unsigned int i = 0; i < sizeof(codesToCheck) / sizeof(codesToCheck[0]); ++i) {
		bool value = ((GetKeyState(codesToCheck[i]) & 0xf0) != 0);
		keyDown_[codesToCheck[i]] = value;
		keyPressed_[codesToCheck[i]] = value;
	}
}

void calculateStick(short& x, short& y, int deadzone) {
	float magnitude = float(std::sqrt(x * x + y * y));
	float normX = x / magnitude;
	float normY = y / magnitude;

	const int max = 32767;

	if (magnitude > deadzone) {
		if (magnitude > max) magnitude = max;
		magnitude -= deadzone;
		x = short(max * normX * magnitude / (max - deadzone));
		y = short(max * normY * magnitude / (max - deadzone));
	} else {
		x = y = 0;
	}
}

void calculateTrigger(BYTE& v) {
	if (v > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) {
		v -= XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
		v = BYTE(255 * v / float(255 - XINPUT_GAMEPAD_TRIGGER_THRESHOLD));
	} else {
		v = 0;
	}
}

void Window::UpdateInput() {
	textInput.clear();
	for (int i = 0; i < XUSER_MAX_COUNT; ++i) {
		DWORD result = XInputGetState(i, &states[i]);
		if (result == ERROR_SUCCESS) {
			calculateStick(states[i].Gamepad.sThumbLX, states[i].Gamepad.sThumbLY,
			               XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
			calculateStick(states[i].Gamepad.sThumbRX, states[i].Gamepad.sThumbRY,
			               XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
			calculateTrigger(states[i].Gamepad.bLeftTrigger);
			calculateTrigger(states[i].Gamepad.bRightTrigger);
		}
	}
	if (relativeMouseMode && touchscreenActive) {
		relativeX = mousex_;
		relativeY = mousey_;
	}
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		static std::map<int, std::string> scanCodeToCharacter;
		switch (msg.message) {
			case WM_QUIT:
				running_ = false;
				break;
			case WM_MOUSEMOVE:
				mousex_ = GET_X_LPARAM(msg.lParam);
				mousey_ = GET_Y_LPARAM(msg.lParam);
				break;
			case WM_MOUSEWHEEL:
				mouseWheel_ += double(GET_WHEEL_DELTA_WPARAM(msg.wParam)) / WHEEL_DELTA;
				break;
			case WM_POINTERDOWN: // WINVER >= 0x0602 (Windows 8)
				touchscreenActive = true;
				break;
			case WM_POINTERUP:
				touchscreenActive = false;
				SetRelativeMouseMode(relativeMouseMode);
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
				mousePressed_.at(2) = true;
				break;
			case WM_LBUTTONUP:
				if (mousePressed_.at(0)) {
					// wait one frame
					addUpdateInputCallback([&]() {
						mouseDown_.at(0) = false;
						mousePressed_.at(0) = false;
					});
				} else {
					mouseDown_.at(0) = false;
					mousePressed_.at(0) = false;
				}
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
				anyKeyPressed_ = true;
				DistinguishLeftRight();
				// A to Z pressed? E.g. when Ctrl is pressed, WM_CHAR doesn't work
				if (msg.wParam >= 65 && msg.wParam <= 90) {
					char c[2];
					if (keyDown_[GetKeyCode(key::ShiftL)] || keyDown_[GetKeyCode(key::ShiftR)]) {
						c[0] = 'A';
					} else {
						c[0] = 'a';
					}
					c[0] += msg.wParam - 65;
					c[1] = 0;
					characterDown_[c] = true;
					characterPressed_[c] = true;
				}
				break;
			case WM_KEYUP: {
				keyDown_[msg.wParam] = false;
				keyPressed_[msg.wParam] = false;
				DistinguishLeftRight();
				int scanCode = msg.lParam & 0x7f8000;
				characterDown_[scanCodeToCharacter[scanCode]] = false;
				characterPressed_[scanCodeToCharacter[scanCode]] = false;
				if (msg.wParam >= 65 && msg.wParam <= 90) {
					char c[2];
					c[0] = 'a' + msg.wParam - 65;
					c[1] = 0;
					characterDown_[c] = false;
					characterPressed_[c] = false;
					c[0] = 'A' + msg.wParam - 65;
					characterDown_[c] = false;
					characterPressed_[c] = false;
				}
			} break;
			case WM_CHAR: {
				std::vector<char> buf(4);
				UTF8* temp = reinterpret_cast<UTF8*>(&buf[0]);
				UTF8** targetStart = &temp;
				UTF8* targetEnd = *targetStart + buf.size();
				const UTF16* temp2 = reinterpret_cast<UTF16*>(&msg.wParam);
				const UTF16** sourceStart = &temp2;
				const UTF16* sourceEnd = temp2 + 2;
				ConversionResult result = ConvertUTF16toUTF8(sourceStart, sourceEnd, targetStart,
				                                             targetEnd, lenientConversion);
				if (result != conversionOK) {
					debug("WARNING: Couldn't convert UTF16 to UTF8.\n");
					break;
				}
				auto end = ++(buf.begin());
				if (buf[0] & 0x80) {
					++end;
					if (buf[0] & 0x20) {
						++end;
						if (buf[0] & 0x10) {
							++end;
						}
					}
				}
				std::string character(buf.begin(), end);
				int scanCode = msg.lParam & 0x7f8000;
				scanCodeToCharacter[scanCode] = character;
				characterDown_[character] = true;
				characterPressed_[character] = true;
				textInput += character;
			} break;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if (relativeMouseMode && !touchscreenActive) {
		SetMouse(width_ / 2, height_ / 2);
		mousex_ -= width_ / 2;
		mousey_ -= height_ / 2;
	}
}

void Window::SwapBuffers() {
	::SwapBuffers(pDeviceContext_.get());
}

void Window::SetMouseVisible(const bool visible) {
	if (isMouseVisible_ != visible) {
		ShowCursor(visible);
	}
	isMouseVisible_ = visible;
}

void Window::SetTitle(const std::string& windowTitle) {
	SetWindowText(pWindowHandle_.get(), windowTitle.c_str());
}

int Window::GetKeyCode(jngl::key::KeyType key) {
	switch (key) {
		case key::Left:
			return 0x25;
		case key::Up:
			return 0x26;
		case key::Right:
			return 0x27;
		case key::Down:
			return 0x28;
		case key::PageUp:
			return 0x21;
		case key::PageDown:
			return 0x22;
		case key::Home:
			return 0x24;
		case key::End:
			return 0x23;
		case key::BackSpace:
			return 0x08;
		case key::Tab:
			return 0x09;
		case key::Clear:
			return 0x0C;
		case key::Return:
			return 0x0D;
		case key::Pause:
			return 0x13;
		case key::Escape: return VK_ESCAPE;
		case key::Delete:
			return 0x2E;
		case key::ControlL:
			return 0xA2;
		case key::ControlR:
			return 0xA3;
		case key::CapsLock:
			return 0x14;
		case key::AltL:
			return 0xA4;
		case key::AltR:
			return 0xA5;
		case key::SuperL:
			return 0x5B;
		case key::SuperR:
			return 0x5C;
		case key::Space:
			return 0x20;
		case key::ShiftL:
			return 0xA0;
		case key::ShiftR:
			return 0xA1;
		case key::F1:
			return 0x70;
		case key::F2:
			return 0x71;
		case key::F3:
			return 0x72;
		case key::F4:
			return 0x73;
		case key::F5:
			return 0x74;
		case key::F6:
			return 0x75;
		case key::F7:
			return 0x76;
		case key::F8:
			return 0x77;
		case key::F9:
			return 0x78;
		case key::F10:
			return 0x79;
		case key::F11:
			return 0x7a;
		case key::F12:
			return 0x7b;
		default:
			return 0;
	}
}

bool Window::getKeyDown(const std::string& key) {
	return characterDown_[key];
}

bool Window::getKeyPressed(const std::string& key) {
	if (characterPressed_[key]) {
		characterPressed_[key] = false;
		return true;
	}
	return characterPressed_[key];
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static bool timePeriodActive = false;
	switch (uMsg) {
		case WM_SYSCOMMAND: // Intercept System Commands
			switch (wParam) {
				case SC_SCREENSAVE:   // Screensaver Trying To Start?
				case SC_MONITORPOWER: // Monitor Trying To Enter Powersave?
					return 0;         // Prevent From Happening
			}
			break;
		case WM_CLOSE:          // Did We Receive A Close Message?
			PostQuitMessage(0); // Send A Quit Message
			return 0;           // Jump Back
		case WM_SETFOCUS:
			debugLn("Window got focus.");
			if (!timePeriodActive) {
				timeBeginPeriod(1); // Tells Windows to use more accurate timers
				timePeriodActive = true;
				try {
					resetFrameLimiter(); // sleepCorrectionFactor needs to be resetted since we just
					                     // changed the behaviour of the sleep function.
				} catch(std::exception&) {
					// The window is about to be created, ignore "pWindow == nullptr" exception
				}
			}
			break;
		case WM_KILLFOCUS:
			debugLn("Window lost focus.");
			if (timePeriodActive) {
				timeEndPeriod(1);
				timePeriodActive = false;
			}
			break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int Window::getMouseX() {
	return mousex_ - relativeX - (width_ - screenWidth) / 2;
}

int Window::getMouseY() {
	return mousey_ - relativeY - (height_ - screenHeight) / 2;
}

void Window::SetMouse(const int xposition, const int yposition) {
	POINT pnt;
	pnt.x = xposition;
	pnt.y = yposition;
	ClientToScreen(pWindowHandle_.get(), &pnt);
	SetCursorPos(pnt.x, pnt.y);
}

void Window::SetRelativeMouseMode(bool relative) {
	relativeMouseMode = relative;
	SetMouseVisible(!relative);
	if (relative) {
		if (touchscreenActive) {
			relativeX = mousex_;
			relativeY = mousey_;
		} else {
			SetMouse(width_ / 2, height_ / 2);
			relativeX = relativeY = mousex_ = mousey_ = 0;
		}
	} else {
		relativeX = relativeY = 0;
	}
}

void Window::SetIcon(const std::string& filename) {
#ifndef NOPNG
	FILE* fp = fopen(filename.c_str(), "rb");
	if (!fp) throw std::runtime_error(std::string("File not found: ") + filename);
	Finally _([&fp]() { fclose(fp); });
	png_byte buf[PNG_BYTES_TO_CHECK];
	assert(PNG_BYTES_TO_CHECK >= sizeof(unsigned short));

	// Read in some of the signature bytes
	if (fread(buf, 1, PNG_BYTES_TO_CHECK, fp) != PNG_BYTES_TO_CHECK)
		throw std::runtime_error(std::string("Error reading signature bytes."));

	assert(png_sig_cmp(buf, (png_size_t)0, PNG_BYTES_TO_CHECK) == 0);
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	if (!png_ptr) {
		throw std::runtime_error("libpng error while reading");
	}

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		throw std::runtime_error("libpng error while reading");
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		// Free all of the memory associated with the png_ptr and info_ptr
		png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
		throw std::runtime_error("Error reading file.");
	}
	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, PNG_BYTES_TO_CHECK);
	int colorType = png_get_color_type(png_ptr, info_ptr);
	if (colorType == PNG_COLOR_TYPE_GRAY || colorType == PNG_COLOR_TYPE_GRAY_ALPHA) {
		png_set_gray_to_rgb(png_ptr);
	}
	png_read_png(png_ptr, info_ptr,
	             PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_BGR, nullptr);

	const int x = png_get_image_width(png_ptr, info_ptr);
	const int y = png_get_image_height(png_ptr, info_ptr);
	const int channels = png_get_channels(png_ptr, info_ptr);

	const auto row_pointers = png_get_rows(png_ptr, info_ptr);
	std::vector<char> imageData(x * y * channels);
	for (int i = 0; i < y; ++i) {
		memcpy(&imageData[i * x * channels], row_pointers[i], x * channels);
	}

	ICONINFO icon;
	icon.fIcon = true;
	std::vector<char> blackMask(x * y);
	icon.hbmMask = CreateBitmap(x, y, 1, 8, &blackMask[0]);
	icon.hbmColor = CreateBitmap(x, y, 1, channels * 8, &imageData[0]);

	png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);

	HICON hIcon = CreateIconIndirect(&icon);
	SendMessage(pWindowHandle_.get(), WM_SETICON, WPARAM(ICON_SMALL), LPARAM(hIcon));
#endif
}

int getDesktopWidth() {
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	return desktop.right;
}

int getDesktopHeight() {
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	return desktop.bottom;
}

void Window::setFullscreen(bool) {
	throw std::runtime_error("Not implemented.");
}

} // namespace jngl
