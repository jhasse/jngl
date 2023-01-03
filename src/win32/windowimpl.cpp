// Copyright 2007-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/debug.hpp"
#include "../jngl/Finally.hpp"
#include "../jngl/ImageData.hpp"
#include "../jngl/window.hpp"
#include "../jngl/work.hpp"
#include "../main.hpp"
#include "../window.hpp"
#include "ConvertUTF.h"
#include "unicode.hpp"

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cmath>
#include <glad/wgl.h>
#include <mmsystem.h> // timeBeginPeriod
#include <stdexcept>
#include <windowsx.h> // GET_X_LPARAM
#include <xinput.h>

namespace jngl {

void setProcessSettings() {
	static bool called = false;
	if (called) {
		return;
	}
	if (!SetProcessDPIAware()) {
		debugLn("Couldn't set the process-default DPI awareness to system-DPI awareness.");
	}
	called = true;
}

class WindowImpl {
public:
	std::shared_ptr<std::remove_pointer<HGLRC>::type> pRenderingContext_;
	std::shared_ptr<std::remove_pointer<HWND>::type> pWindowHandle_;
	std::shared_ptr<std::remove_pointer<HDC>::type> pDeviceContext_;
	int arbMultisampleFormat_ = 0;
	bool touchscreenActive = false;
	int relativeX = 0;
	int relativeY = 0;
	std::function<void()> distinguishLeftRight;
	Window* window;
	std::atomic_bool clearInputAfterFocusLoss{false};

	static void ReleaseDC(HWND, HDC);
	static void ReleaseRC(HGLRC);

	bool InitMultisample(HINSTANCE, PIXELFORMATDESCRIPTOR);
	void Init(const std::string& title, bool multisample);

	/// Attention: Will be called on a different thread as WM_SETFOCUS and WM_KILLFOCUS are unqueued
	/// messages.
	void onFocusChange(bool focus) {
		static bool timePeriodActive = false;
		if (focus) {
			if (!timePeriodActive) {
				timeBeginPeriod(1); // Tells Windows to use more accurate timers
				timePeriodActive = true;
				window->resetFrameLimiter(); // sleepCorrectionFactor needs to be resetted since we
				                             // just changed the behaviour of the sleep function.
			}
		} else {
			if (timePeriodActive) {
				timeEndPeriod(1);
				timePeriodActive = false;
			}
			clearInputAfterFocusLoss = true;
		}
	}
};

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// based on: http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=46
bool WindowImpl::InitMultisample(HINSTANCE, PIXELFORMATDESCRIPTOR) {
	if (!GLAD_WGL_ARB_pixel_format) {
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
               const std::pair<int, int> minAspectRatio, const std::pair<int, int> maxAspectRatio)
: impl(std::make_unique<WindowImpl>()), fullscreen_(fullscreen), isMouseVisible_(true),
  relativeMouseMode(false), anyKeyPressed_(false), width_(width), height_(height),
  fontName_(GetFontFileByName("Arial")) {
	impl->window = this;
	impl->distinguishLeftRight = [this]() {
		int codesToCheck[] = { GetKeyCode(jngl::key::ShiftL),   GetKeyCode(jngl::key::ShiftR),
			                   GetKeyCode(jngl::key::ControlL), GetKeyCode(jngl::key::ControlR),
			                   GetKeyCode(jngl::key::AltL),     GetKeyCode(jngl::key::AltR) };
		for (int code : codesToCheck) {
			bool value = ((GetKeyState(code) & 0xf0) != 0);
			keyDown_[code] = value;
			keyPressed_[code] = value;
		}
	};
	calculateCanvasSize(minAspectRatio, maxAspectRatio);
	const std::function<void(bool)> init = [this, &title, &init](const bool multisample) {
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
		wc.lpszClassName = L"OpenGL";

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
		impl->pWindowHandle_.reset(
		    CreateWindowEx(dwExStyle,                  // Extended Style For The Window
		                   L"OpenGL",                  // Class Name
		                   utf8ToUtf16(title).c_str(), // Window Title
		                   dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, CW_USEDEFAULT,
		                   CW_USEDEFAULT,
		                   WindowRect.right - WindowRect.left, // Calculate Window Width
		                   WindowRect.bottom - WindowRect.top, // Calculate Window Height
		                   nullptr, nullptr, hInstance, impl.get()),
		    DestroyWindow);
		if (!impl->pWindowHandle_) {
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

		impl->pDeviceContext_.reset(GetDC(impl->pWindowHandle_.get()), [this](HDC hdc) {
			WindowImpl::ReleaseDC(impl->pWindowHandle_.get(), hdc);
		});
		if (!impl->pDeviceContext_) {
			throw std::runtime_error("Can't create a GL device context.");
		}
		GLuint pixelFormat;
		if (multisample) {
			pixelFormat = impl->arbMultisampleFormat_;
		} else {
			pixelFormat = ChoosePixelFormat(impl->pDeviceContext_.get(), &pfd);
		}
		if (!pixelFormat) {
			throw std::runtime_error("Can't find a suitable PixelFormat.");
		}
		if (!SetPixelFormat(impl->pDeviceContext_.get(), pixelFormat, &pfd)) {
			throw std::runtime_error("Can't set the PixelFormat.");
		}
		impl->pRenderingContext_.reset(wglCreateContext(impl->pDeviceContext_.get()),
		                               WindowImpl::ReleaseRC);
		if (!impl->pRenderingContext_) {
			throw std::runtime_error("Can't create a GL rendering context.");
		}
		if (!wglMakeCurrent(impl->pDeviceContext_.get(), impl->pRenderingContext_.get())) {
			throw std::runtime_error("Can't activate the GL rendering context.");
		}

		gladLoaderLoadWGL(impl->pDeviceContext_.get());

		if (!multisample && isMultisampleSupported_ && impl->InitMultisample(hInstance, pfd)) {
			impl->pDeviceContext_.reset((HDC)nullptr); // Destroy window
			try {
				jngl::debugLn("Recreating window with Anti-Aliasing support.");
				init(true);
			} catch (...) {
				// If Anti-Aliasing still doesn't work for some reason, let's turn it off again.
				isMultisampleSupported_ = false;
				init(false);
			}
			return;
		}

		const int glVersion = gladLoaderLoadGL();
		if (glVersion < GLAD_MAKE_VERSION(2, 0)) {
			throw std::runtime_error(
			    "Your graphics card is missing OpenGL 2.0 support (it supports " +
			    std::to_string(GLAD_VERSION_MAJOR(glVersion)) + "." +
			    std::to_string(GLAD_VERSION_MINOR(glVersion)) + ").");
		}

		::ShowWindow(impl->pWindowHandle_.get(), SW_SHOWNORMAL);

		if (!jngl::Init(width_, height_, canvasWidth, canvasHeight)) {
			throw std::runtime_error("Initialization failed.");
		}
	};
	init(false);
}

Window::~Window() {
	if (fullscreen_) {
		ChangeDisplaySettings(nullptr, 0);
	}
}

void WindowImpl::ReleaseDC(HWND hwnd, HDC hdc) {
	if (!::ReleaseDC(hwnd, hdc)) {
		debug("Release device context failed.");
	}
}

void WindowImpl::ReleaseRC(HGLRC hrc) {
	if (!wglMakeCurrent(nullptr, nullptr)) {
		debugLn("Release of DC and RC failed.");
	}
	if (!wglDeleteContext(hrc)) {
		debugLn("Release rendering context failed.");
	}
}

void Window::UpdateInput() {
	textInput.clear();
	updateControllerStates();
	if (relativeMouseMode && impl->touchscreenActive) {
		impl->relativeX = mousex_;
		impl->relativeY = mousey_;
	}
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		static std::map<int, std::string> scanCodeToCharacter;
		switch (msg.message) {
		case WM_QUIT:
			quit();
			break;
		case WM_MOUSEMOVE:
			mousex_ = GET_X_LPARAM(msg.lParam);
			mousey_ = GET_Y_LPARAM(msg.lParam);
			break;
		case WM_MOUSEWHEEL:
			mouseWheel += double(GET_WHEEL_DELTA_WPARAM(msg.wParam)) / WHEEL_DELTA;
			break;
		case WM_POINTERDOWN: // WINVER >= 0x0602 (Windows 8)
			impl->touchscreenActive = true;
			break;
		case WM_POINTERUP:
			impl->touchscreenActive = false;
			SetRelativeMouseMode(relativeMouseMode);
			break;
		case WM_LBUTTONDOWN:
			mouseDown_.at(0) = true;
			mousePressed_.at(0) = true;
			needToBeSetFalse_.push(&mousePressed_[0]);
			break;
		case WM_MBUTTONDOWN:
			mouseDown_.at(1) = true;
			mousePressed_.at(1) = true;
			needToBeSetFalse_.push(&mousePressed_[1]);
			break;
		case WM_RBUTTONDOWN:
			mouseDown_.at(2) = true;
			mousePressed_.at(2) = true;
			needToBeSetFalse_.push(&mousePressed_[2]);
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
		case WM_SYSKEYDOWN:
			debugLn("WM_SYSKEYDOWN");
		case WM_KEYDOWN:
			keyDown_[msg.wParam] = true;
			keyPressed_[msg.wParam] = true;
			anyKeyPressed_ = true;
			impl->distinguishLeftRight();
			break;
		case WM_SYSKEYUP:
			debugLn("WM_SYSKEYUP");
		case WM_KEYUP: {
			keyDown_[msg.wParam] = false;
			keyPressed_[msg.wParam] = false;
			impl->distinguishLeftRight();
			int scanCode = msg.lParam & 0x7f8000;
			characterDown_[scanCodeToCharacter[scanCode]] = false;
			characterPressed_[scanCodeToCharacter[scanCode]] = false;
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
			if (character.size() == 1) {
				assert(character[0] > 0);
				if (GetKeyState(VK_CONTROL) < 0 && character[0] >= 1 /* ctrl-a */ &&
				    character[0] <= 26 /* ctrl-z */) {
					character[0] = 'a' - 1 + character[0];
				} else if (character[0] < ' ') {
					break; // non-printable character (e.g. Escape)
				}
			}
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
	if (relativeMouseMode && !impl->touchscreenActive) {
		SetMouse(width_ / 2, height_ / 2);
		mousex_ -= width_ / 2;
		mousey_ -= height_ / 2;
	}
	if (impl->clearInputAfterFocusLoss) {
		impl->clearInputAfterFocusLoss = false;
		for (auto& it : keyDown_) {
			it.second = false;
		}
		for (auto& it : keyPressed_) {
			it.second = false;
		}
		for (auto& it : characterDown_) {
			it.second = false;
		}
		for (auto& it : characterPressed_) {
			it.second = false;
		}
		for (auto& b : mouseDown_) {
			b = false;
		}
		for (auto& b : mousePressed_) {
			b = false;
		}
	}
}

void Window::SwapBuffers() {
	::SwapBuffers(impl->pDeviceContext_.get());
}

void Window::SetMouseVisible(const bool visible) {
	if (isMouseVisible_ != visible) {
		ShowCursor(visible);
	}
	isMouseVisible_ = visible;
}

void Window::SetTitle(const std::string& windowTitle) {
	SetWindowText(impl->pWindowHandle_.get(), utf8ToUtf16(windowTitle).c_str());
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
	case key::Escape:
		return VK_ESCAPE;
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
	switch (uMsg) {
	case WM_NCCREATE: {
		// Store WindowImpl* pointer passed to CreateWindowEx:
		SetWindowLongPtr(
		    hWnd, GWLP_USERDATA,
		    reinterpret_cast<LONG_PTR>(reinterpret_cast<LPCREATESTRUCT>(lParam)->lpCreateParams));
		break;
	}
	case WM_SYSCOMMAND: // Intercept System Commands
		switch (wParam) {
		case SC_KEYMENU:
			debugLn("Ignoring SC_KEYMENU (AltL / F10 menu).");
			return 0;
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
		reinterpret_cast<WindowImpl*>(GetWindowLongPtr(hWnd, GWLP_USERDATA))->onFocusChange(true);
		break;
	case WM_KILLFOCUS:
		debugLn("Window lost focus.");
		reinterpret_cast<WindowImpl*>(GetWindowLongPtr(hWnd, GWLP_USERDATA))->onFocusChange(false);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int Window::getMouseX() const {
	return mousex_ - impl->relativeX - (width_ - canvasWidth) / 2;
}

int Window::getMouseY() const {
	return mousey_ - impl->relativeY - (height_ - canvasHeight) / 2;
}

void Window::SetMouse(const int xposition, const int yposition) {
	POINT pnt;
	pnt.x = xposition;
	pnt.y = yposition;
	ClientToScreen(impl->pWindowHandle_.get(), &pnt);
	SetCursorPos(pnt.x, pnt.y);
}

void Window::SetRelativeMouseMode(bool relative) {
	relativeMouseMode = relative;
	SetMouseVisible(!relative);
	if (relative) {
		if (impl->touchscreenActive) {
			impl->relativeX = mousex_;
			impl->relativeY = mousey_;
		} else {
			SetMouse(width_ / 2, height_ / 2);
			impl->relativeX = impl->relativeY = mousex_ = mousey_ = 0;
		}
	} else {
		impl->relativeX = impl->relativeY = 0;
	}
}

void Window::SetIcon(const std::string& filename) {
	try {
		auto imageData = ImageData::load(filename);

		const int CHANNELS = 4;

		auto bgra =
		    std::make_unique<char[]>(imageData->getWidth() * imageData->getHeight() * CHANNELS);
		for (size_t x = 0; x < imageData->getWidth(); ++x) {
			for (size_t y = 0; y < imageData->getHeight(); ++y) {
				// transform RGBA to BGRA:
				bgra[y * imageData->getWidth() * CHANNELS + x * CHANNELS] =
				    imageData->pixels()[y * imageData->getWidth() * CHANNELS + x * CHANNELS + 2];
				bgra[y * imageData->getWidth() * CHANNELS + x * CHANNELS + 1] =
				    imageData->pixels()[y * imageData->getWidth() * CHANNELS + x * CHANNELS + 1];
				bgra[y * imageData->getWidth() * CHANNELS + x * CHANNELS + 2] =
				    imageData->pixels()[y * imageData->getWidth() * CHANNELS + x * CHANNELS];
				bgra[y * imageData->getWidth() * CHANNELS + x * CHANNELS + 3] =
				    imageData->pixels()[y * imageData->getWidth() * CHANNELS + x * CHANNELS + 3];
			}
		}

		ICONINFO icon;
		icon.fIcon = true;
		std::vector<char> blackMask(imageData->getWidth() * imageData->getHeight());
		icon.hbmMask =
		    CreateBitmap(imageData->getWidth(), imageData->getHeight(), 1, 8, &blackMask[0]);
		icon.hbmColor = CreateBitmap(imageData->getWidth(), imageData->getHeight(), 1, CHANNELS * 8,
		                             bgra.get());

		HICON hIcon = CreateIconIndirect(&icon);
		SendMessage(impl->pWindowHandle_.get(), WM_SETICON, WPARAM(ICON_SMALL), LPARAM(hIcon));
	} catch (std::runtime_error& e) {
		debug("jngl::setIcon: ");
		debugLn(e.what());
	}
}

int getDesktopWidth() {
	setProcessSettings();
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	return desktop.right;
}

int getDesktopHeight() {
	setProcessSettings();
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	return desktop.bottom;
}

void Window::setFullscreen(bool) {
	throw std::runtime_error("Not implemented.");
}

std::string getPreferredLanguage() {
	ULONG numLanguages;
	DWORD bufferLength = 0;
	std::string lang = "en";
	if (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &numLanguages, nullptr, &bufferLength) ==
	    FALSE) {
		return lang;
	}
	auto languagesBuffer = std::make_unique<wchar_t[]>(bufferLength);
	if (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &numLanguages, languagesBuffer.get(),
	                                &bufferLength) == FALSE ||
	    bufferLength < 3) {
		return lang;
	}
	lang[0] = languagesBuffer[0];
	lang[1] = languagesBuffer[1];
	return lang;
}

void openURL(const std::string& url) {
	ShellExecute(0, 0, utf8ToUtf16(url).c_str(), 0, 0, SW_SHOW);
}

void setCursor(Cursor) {}

} // namespace jngl
