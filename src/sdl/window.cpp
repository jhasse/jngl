// Copyright 2011-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../App.hpp"
#include "../jngl/ImageData.hpp"
#include "../jngl/screen.hpp"
#include "../jngl/window.hpp"
#include "../jngl/work.hpp"
#include "../log.hpp"
#include "../main.hpp"
#include "../windowptr.hpp"
#include "windowimpl.hpp"

#include <cassert>
#include <stdexcept>
#if defined(_WIN32) && !defined(JNGL_UWP)
#define NOMINMAX
#include <windows.h>
#endif

namespace jngl {

void setProcessSettings() {
#if defined(_WIN32) && !defined(JNGL_UWP)
	static bool called = false;
	if (called) {
		return;
	}
	if (!SetProcessDPIAware()) {
		internal::error("Couldn't set the process-default DPI awareness to system-DPI awareness.");
	}
	called = true;
#endif
}

Window::Window(const std::string& title, int width, int height, const bool fullscreen,
               const std::pair<int, int> minAspectRatio, const std::pair<int, int> maxAspectRatio)
: impl(std::make_unique<WindowImpl>()), fullscreen_(fullscreen), width_(width), height_(height),
  fontName_(GetFontFileByName("Arial")) {
	SDL::init();

#ifdef __APPLE__ // https://stackoverflow.com/a/26981800/647898
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI;
	if (fullscreen) {
		if (width == getDesktopWidth() && height == getDesktopHeight()) {
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		} else {
			flags |= SDL_WINDOW_FULLSCREEN;
		}
	} else {
		flags |= SDL_WINDOW_RESIZABLE; // if we make fullscreen window resizeable on GNOME, it will
		                               // be reduced in its height (SDL bug?).
	}

#ifdef JNGL_UWP
	isMultisampleSupported_ = false; // crashes on Xbox since ANGLE uses a PixelShader 4.1 for
	                                 // multi-sampling and Xbox only supports 4.0 in UWP mode.
#endif
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	const auto create = [this, &title, width, height, flags]() {
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, isMultisampleSupported_ ? 4 : 0);
		return SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		                        width, height, flags);
	};
	if ((impl->sdlWindow = create()) == nullptr) {
		internal::debug("Recreating window without Anti-Aliasing support.");
		isMultisampleSupported_ = false;
		if ((impl->sdlWindow = create()) == nullptr) {
			throw std::runtime_error(SDL_GetError());
		}
	}

	impl->context = SDL_GL_CreateContext(impl->sdlWindow);
#ifdef GLAD_GL
	const auto glVersion = gladLoadGL(reinterpret_cast<GLADloadfunc>(SDL_GL_GetProcAddress));
	if (glVersion < GLAD_MAKE_VERSION(2, 0)) {
		throw std::runtime_error("Your graphics card is missing OpenGL 2.0 support (it supports " +
		                         std::to_string(GLAD_VERSION_MAJOR(glVersion)) + "." +
		                         std::to_string(GLAD_VERSION_MINOR(glVersion)) + ").");
	}
#endif

	if (isMultisampleSupported_) {
		int openglMSAA;
		if (SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &openglMSAA) != 0) {
			internal::debug("Recreating window and OpenGL Context without Anti-Aliasing support.");
			SDL_GL_DeleteContext(impl->context);
			SDL_DestroyWindow(impl->sdlWindow);
			isMultisampleSupported_ = false;
			impl->sdlWindow = create();
			impl->context = SDL_GL_CreateContext(impl->sdlWindow);
		}
	}

#if !defined(__linux__) || SDL_VERSION_ATLEAST(2, 30, 0)
	// This code was written for UWP, Emscripten and macOS (annoying HiDPI scaling by SDL2). On
	// Linux (GNOME) it results in the top of the window being cut off (by the header bar height?).
	// The bug seems to be fixed in newer SDL2 (or GNOME) versions.
	{
		assert(width_ == width);
		assert(height_ == height);
		// on some platforms (e.g. UWP or Emscripten) the size we specify for the window might be
		// ignored. Check if we actually got what we asked for and correct if not:
		SDL_GetWindowSize(impl->sdlWindow, &width, &height);
		if (width_ != width || height_ != height) {
			internal::debug("Wanted window dimensions {}x{}, but got {}x{} instead.", width_,
			                height_, width, height);
			setScaleFactor(getScaleFactor() * std::min(static_cast<double>(width) / width_,
			                                           static_cast<double>(height) / height_));
			width_ = width;
			height_ = height;
		}
	}

	SDL_GL_GetDrawableSize(impl->sdlWindow, &width_, &height_);
#endif
	impl->actualWidth = width_;
	impl->actualHeight = height_;
	impl->hidpiScaleFactor = static_cast<float>(width_) / width;
	setScaleFactor(getScaleFactor() * impl->hidpiScaleFactor);
	calculateCanvasSize(minAspectRatio, maxAspectRatio);
	impl->actualCanvasWidth = canvasWidth;
	impl->actualCanvasHeight = canvasHeight;
	Init(width_, height_, canvasWidth, canvasHeight);
}

Window::~Window() {
	// This is rather dirty, but needed for the rare case that one wants to create a window after
	// hiding a previous one and doesn't reset the scale factor:
	setScaleFactor(getScaleFactor() / impl->hidpiScaleFactor);
}

int Window::GetKeyCode(key::KeyType key) {
	switch (key) {
	case key::Left:
		return SDLK_LEFT;
	case key::Right:
		return SDLK_RIGHT;
	case key::Up:
		return SDLK_UP;
	case key::Down:
		return SDLK_DOWN;
	case key::Escape:
		return SDLK_ESCAPE;
	case key::BackSpace:
		return SDLK_BACKSPACE;
	case key::PageUp:
		return SDLK_PAGEUP;
	case key::PageDown:
		return SDLK_PAGEDOWN;
	case key::Return:
		return SDLK_RETURN;
	case key::Space:
		return SDLK_SPACE;
	case key::Home:
		return SDLK_HOME;
	case key::End:
		return SDLK_END;
	case key::Tab:
		return SDLK_TAB;
	case key::Clear:
		return SDLK_CLEAR;
	case key::Pause:
		return SDLK_PAUSE;
	case key::Delete:
		return SDLK_DELETE;
	case key::ControlL:
		return SDLK_LCTRL;
	case key::ControlR:
		return SDLK_RCTRL;
	case key::CapsLock:
		return SDLK_CAPSLOCK;
	case key::AltL:
		return SDLK_LALT;
	case key::AltR:
		return SDLK_RALT;
	case key::SuperL:
		return SDLK_LGUI;
	case key::SuperR:
		return SDLK_RGUI;
	case key::ShiftL:
		return SDLK_LSHIFT;
	case key::ShiftR:
		return SDLK_RSHIFT;
	case key::F1:
		return SDLK_F1;
	case key::F2:
		return SDLK_F2;
	case key::F3:
		return SDLK_F3;
	case key::F4:
		return SDLK_F4;
	case key::F5:
		return SDLK_F5;
	case key::F6:
		return SDLK_F6;
	case key::F7:
		return SDLK_F7;
	case key::F8:
		return SDLK_F8;
	case key::F9:
		return SDLK_F9;
	case key::F10:
		return SDLK_F10;
	case key::F11:
		return SDLK_F11;
	case key::F12:
		return SDLK_F12;
	default:
		return 0;
	}
}

bool Window::getKeyDown(const std::string& key) {
	return characterDown_[key];
}

bool Window::getKeyPressed(const std::string& key) {
	return characterPressed_[key];
}

void Window::UpdateInput() {
	textInput.clear();
	updateControllerStates();
	if (relativeMouseMode) {
		mousex_ = 0;
		mousey_ = 0;
	}
	SDL_Event event;
	while (SDL_PollEvent(&event) != 0) {
		switch (event.type) {
		case SDL_QUIT:
			quit();
			break;
		case SDL_MOUSEMOTION:
			if (relativeMouseMode) {
				mousex_ += event.motion.xrel;
				mousey_ += event.motion.yrel;
			} else {
				mousex_ = event.motion.x;
				mousey_ = event.motion.y;
			}
			break;
#ifndef __APPLE__ // Somehow the trackpad on a Macbook is handled as a touch screen?
		case SDL_FINGERUP:
			mouseDown_.at(0) = false;
			mouseDown_.at(0) = false;
			impl->currentFingerId = nullopt;
			multitouch = false;
			break;
		case SDL_FINGERDOWN:
			if (impl->currentFingerId && *impl->currentFingerId != event.tfinger.fingerId) {
				multitouch = true;
			}
			impl->currentFingerId = event.tfinger.fingerId;
			mouseDown_.at(0) = true;
			mousePressed_.at(0) = true;
			needToBeSetFalse_.push(&mousePressed_[0]);
			[[fallthrough]];
		case SDL_FINGERMOTION:
			if (relativeMouseMode) {
				mousex_ = int(std::lround(event.tfinger.dx * float(width_)));
				mousey_ = int(std::lround(event.tfinger.dy * float(height_)));
			} else {
				mousex_ = int(std::lround(event.tfinger.x * float(width_)));
				mousey_ = int(std::lround(event.tfinger.y * float(height_)));
			}
			break;
#endif
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
				needToBeSetFalse_.push(&mousePressed_[button]);
			}
			break;
		}
		case SDL_MOUSEWHEEL: {
			mouseWheel += event.wheel.y;
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
		case SDL_TEXTINPUT:
			textInput += event.text.text;
			break;
		case SDL_KEYDOWN: {
			static bool wasFullscreen = fullscreen_;
			if (event.key.repeat != 0u && fullscreen_ != wasFullscreen) {
				// SDL2 with Xorg has a bug, that it sends a key repeat event when toggling
				// fullscreen. So let's ignore the second event
				break;
			}
			wasFullscreen = fullscreen_;

			keyDown_[event.key.keysym.sym] = true;
			keyPressed_[event.key.keysym.sym] = true;
			const char* name = SDL_GetKeyName(event.key.keysym.sym);
			if (strlen(name) == 1) {
				std::string tmp;
				if (getKeyDown(key::ShiftL) || getKeyDown(key::ShiftR)) {
					tmp.append(1, name[0]);
				} else {
					tmp.append(1, tolower(name[0]));
				}
				characterDown_[tmp] = true;
				characterPressed_[tmp] = true;
				needToBeSetFalse_.push(&characterPressed_[tmp]);
			}
			if (event.key.keysym.sym == SDLK_SPACE) {
				characterDown_[" "] = true;
				characterPressed_[" "] = true;
				needToBeSetFalse_.push(&characterPressed_[" "]);
			} else if (event.key.keysym.sym == SDLK_ESCAPE) {
				if (const auto& work = getWork()) {
					work->onBackEvent();
				}
			}
			anyKeyPressed_ = true;
			break;
		}
		case SDL_KEYUP: {
			keyDown_[event.key.keysym.sym] = false;
			keyPressed_[event.key.keysym.sym] = false;
			const char* name = SDL_GetKeyName(event.key.keysym.sym);
			if (strlen(name) == 1) {
				std::string tmp(1, name[0]);
				characterDown_[tmp] = false;
				tmp[0] = tolower(name[0]);
				characterDown_[tmp] = false;
			}
			if (event.key.keysym.sym == SDLK_SPACE) {
				characterDown_[" "] = false;
			}
			break;
		}
		case SDL_JOYDEVICEADDED:
		case SDL_JOYDEVICEREMOVED:
			if (controllerChangedCallback) {
				controllerChangedCallback();
			}
			break;
		case SDL_WINDOWEVENT:
			if (!impl->firstFrame && event.window.event == SDL_WINDOWEVENT_RESIZED) {
				const int originalWidth = width_;
				const int originalHeight = height_;
				SDL_GL_GetDrawableSize(impl->sdlWindow, &width_, &height_);
				impl->actualWidth = width_;
				impl->actualHeight = height_;
				impl->actualCanvasWidth = canvasWidth;
				impl->actualCanvasHeight = canvasHeight;
				calculateCanvasSize({ canvasWidth, canvasHeight }, { canvasWidth, canvasHeight });
				const float tmpWidth = (float(width_) / canvasWidth) * impl->actualCanvasWidth;
				const float tmpHeight = (float(height_) / canvasHeight) * impl->actualCanvasHeight;
				const auto l = -1.f / 2.f;
				const auto r = 1.f / 2.f;
				const auto b = 1.f / 2.f;
				const auto t = -1.f / 2.f;
				opengl::projection = { 1.f / float(tmpWidth) * 2.f / (r - l),
					                   0.f,
					                   0.f,
					                   -(r + l) / (r - l),
					                   0.f,
					                   1.f / float(tmpHeight) * 2.f / (t - b),
					                   0.f,
					                   -(t + b) / (t - b),
					                   0.f,
					                   0.f,
					                   -1.f,
					                   0.f,
					                   0.f,
					                   0.f,
					                   0.f,
					                   1.f };
				App::instance().updateProjectionMatrix();
				updateViewportAndLetterboxing(width_, height_, canvasWidth, canvasHeight);
				// restore the values in canvasWidth and canvasHeight because our scaleFactor didn't
				// change:
				std::swap(canvasWidth, impl->actualCanvasWidth);
				std::swap(canvasHeight, impl->actualCanvasHeight);
				width_ = originalWidth;
				height_ = originalHeight;
			}
		}
	}
}

void Window::SwapBuffers() {
	SDL_GL_SwapWindow(impl->sdlWindow);
	impl->firstFrame = false;
}

void Window::SetMouseVisible(const bool visible) {
	isMouseVisible_ = visible;
	if (visible) {
		SDL_ShowCursor(SDL_ENABLE);
	} else {
		SDL_ShowCursor(SDL_DISABLE);
	}
}

void Window::SetTitle(const std::string& title) {
	SDL_SetWindowTitle(impl->sdlWindow, title.c_str());
}

void Window::SetMouse(const int xposition, const int yposition) {
	SDL_WarpMouseInWindow(
	    impl->sdlWindow,
	    static_cast<int>(std::lround(static_cast<float>(xposition) / impl->hidpiScaleFactor)),
	    static_cast<int>(std::lround(static_cast<float>(yposition) / impl->hidpiScaleFactor)));
}

void Window::SetRelativeMouseMode(const bool relative) {
	int rtn = SDL_SetRelativeMouseMode(relative ? SDL_TRUE : SDL_FALSE);
	if (rtn == -1) {
		throw std::runtime_error("Relative mouse mode not supported.");
	}
	relativeMouseMode = relative;
	if (relative) {
		mousex_ = 0;
		mousey_ = 0;
	} else {
		SDL_GetMouseState(&mousex_, &mousey_);
	}
}

void Window::SetIcon(const std::string& filepath) {
	auto imageData = ImageData::load(filepath);
	const int CHANNELS = 4;
	SDL_Surface* const surface = SDL_CreateRGBSurfaceFrom(
	    const_cast<uint8_t*>(imageData->pixels()) /* NOLINT */, imageData->getWidth(),
	    imageData->getHeight(), CHANNELS * 8, CHANNELS * imageData->getWidth(), 0x000000ff,
	    0x0000ff00, 0x00ff0000, 0xff000000);

	if (surface == nullptr) {
		internal::error(SDL_GetError());
		return;
	}

	SDL_SetWindowIcon(impl->sdlWindow, surface);
	SDL_FreeSurface(surface);
}

int getDesktopWidth() {
	setProcessSettings();
	SDL::init();
	SDL_DisplayMode mode;
	SDL_GetDesktopDisplayMode(0, &mode);
	return mode.w;
}

int getDesktopHeight() {
	setProcessSettings();
	SDL::init();
	SDL_DisplayMode mode;
	SDL_GetDesktopDisplayMode(0, &mode);
	return mode.h;
}

void Window::setFullscreen(bool f) {
	Uint32 flag = 0;
	if (f) {
		if (width_ == getDesktopWidth() && height_ == getDesktopHeight()) {
			flag = SDL_WINDOW_FULLSCREEN_DESKTOP;
		} else {
			flag = SDL_WINDOW_FULLSCREEN;
		}
	}
	SDL_SetWindowFullscreen(impl->sdlWindow, flag);
	fullscreen_ = f;
}

int Window::getMouseX() const {
	if (relativeMouseMode) {
		return mousex_ * impl->hidpiScaleFactor;
	}
	return std::lround(
	    (mousex_ * impl->hidpiScaleFactor - (impl->actualWidth - impl->actualCanvasWidth) / 2) *
	    (float(canvasWidth) / impl->actualCanvasWidth));
}

int Window::getMouseY() const {
	if (relativeMouseMode) {
		return mousey_ * impl->hidpiScaleFactor;
	}
	return std::lround(
	    (mousey_ * impl->hidpiScaleFactor - (impl->actualHeight - impl->actualCanvasHeight) / 2) *
	    (float(canvasHeight) / impl->actualCanvasHeight));
}

void setCursor(Cursor type) {
	SDL_SystemCursor sdlType = SDL_SYSTEM_CURSOR_ARROW;
	switch (type) {
	case Cursor::ARROW:
		sdlType = SDL_SYSTEM_CURSOR_ARROW;
		break;
	case Cursor::I:
		sdlType = SDL_SYSTEM_CURSOR_IBEAM;
		break;
	};
	static SDL_Cursor* cursor = nullptr;
	if (cursor) {
		SDL_FreeCursor(cursor);
	}
	cursor = SDL_CreateSystemCursor(sdlType);
	SDL_SetCursor(cursor);
}

void errorMessage(const std::string &text) {
	SDL_Window* sdlWindow = nullptr;
	bool old;
	Window* window = pWindow.get();
	if (window) {
		old = window->getMouseVisible();
		window->SetMouseVisible(true);
		sdlWindow = window->impl->sdlWindow;
	}
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", text.c_str(), sdlWindow);
	if (window) {
		window->SetMouseVisible(old);
	}
}

float Window::getResizedWindowScalingX() const {
	return impl->actualWidth / static_cast<float>(impl->actualCanvasWidth);
}

float Window::getResizedWindowScalingY() const {
	return impl->actualHeight / static_cast<float>(impl->actualCanvasHeight);
}

} // namespace jngl
