// Copyright 2011-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#ifndef NOPNG
#include <png.h> // We need to include it first, I don't know why
#endif

#include "../jngl/debug.hpp"
#include "../jngl/window.hpp"
#include "../main.hpp"
#include "../window.hpp"
#include "windowimpl.hpp"

#include <boost/math/special_functions/round.hpp>

namespace jngl {

Window::Window(const std::string& title, const int width, const int height, const bool fullscreen,
               const std::pair<int, int> minAspectRatio, const std::pair<int, int> maxAspectRatio)
: impl(std::make_unique<WindowImpl>()), fullscreen_(fullscreen), isMouseVisible_(true),
  relativeMouseMode(false), anyKeyPressed_(false), width_(width), height_(height),
  fontName_(GetFontFileByName("Arial")) {
	SDL::init();

#ifdef __APPLE__ // https://stackoverflow.com/a/26981800/647898
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
	if (fullscreen) {
		if (width == getDesktopWidth() && height == getDesktopHeight()) {
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		} else {
			flags |= SDL_WINDOW_FULLSCREEN;
		}
	}

#ifdef JNGL_UWP
	isMultisampleSupported_ = false; // crashes on Xbox since ANGLE uses a PixelShader 4.1 for multi-sampling and Xbox only supports 4.0 in UWP mode.
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
#endif
	const auto create = [this, &title, width, height, flags]() {
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, isMultisampleSupported_ ? 4 : 0);
		return SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		                        width, height, flags);
	};
	if ((impl->sdlWindow = create()) == nullptr) {
		jngl::debugLn("Recreating window without Anti-Aliasing support.");
		isMultisampleSupported_ = false;
		if ((impl->sdlWindow = create()) == nullptr) {
			throw std::runtime_error(SDL_GetError());
		}
	}

	impl->context = SDL_GL_CreateContext(impl->sdlWindow);

	if (isMultisampleSupported_) {
		int openglMSAA;
		if (SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &openglMSAA) != 0) {
			debugLn("Recreating window and OpenGL Context without Anti-Aliasing support.");
			SDL_GL_DeleteContext(impl->context);
			SDL_DestroyWindow(impl->sdlWindow);
			isMultisampleSupported_ = false;
			impl->sdlWindow = create();
			impl->context = SDL_GL_CreateContext(impl->sdlWindow);
		}
	}

	calculateCanvasSize(minAspectRatio, maxAspectRatio);
	Init(width, height, canvasWidth, canvasHeight);
}

Window::~Window() = default;

	int Window::GetKeyCode(key::KeyType key) {
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
			case key::SuperL: return SDLK_LGUI;
			case key::SuperR: return SDLK_RGUI;
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

	void Window::UpdateInput() {
		textInput.clear();
		if (relativeMouseMode) {
			mousex_ = 0;
			mousey_ = 0;
		}
		SDL_Event event;
		while (SDL_PollEvent(&event) != 0) {
			switch(event.type) {
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
					[[fallthrough]];
				case SDL_FINGERMOTION:
					if (relativeMouseMode) {
						mousex_ = boost::math::iround(event.tfinger.dx * float(width_));
						mousey_ = boost::math::iround(event.tfinger.dy * float(height_));
					} else {
						mousex_ = boost::math::iround(event.tfinger.x * float(width_));
						mousey_ = boost::math::iround(event.tfinger.y * float(height_));
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
					}
					if (event.key.keysym.sym == SDLK_SPACE) {
						characterDown_[" "] = true;
						characterPressed_[" "] = true;
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
						characterPressed_[tmp] = false;
						tmp[0] = tolower(name[0]);
						characterDown_[tmp] = false;
						characterPressed_[tmp] = false;
					}
					if (event.key.keysym.sym == SDLK_SPACE) {
						characterDown_[" "] = false;
						characterPressed_[" "] = false;
					}
					break;
				}
				case SDL_JOYDEVICEADDED:
				case SDL_JOYDEVICEREMOVED:
					if (controllerChangedCallback) {
						controllerChangedCallback();
					}
					break;
			}
		}
	}

	void Window::SwapBuffers() {
		SDL_GL_SwapWindow(impl->sdlWindow);
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
		SDL_WarpMouseInWindow(impl->sdlWindow, xposition, yposition);
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
#ifndef NOPNG
		FILE* fp = fopen(filepath.c_str(), "rb");
		if (fp == nullptr) {
			throw std::runtime_error(std::string("File not found: ") + filepath);
		}
		Finally _([&fp]() {
			fclose(fp);
		});
		std::array<png_byte, PNG_BYTES_TO_CHECK> buf{};
		static_assert(PNG_BYTES_TO_CHECK >= sizeof(uint16_t), "Invalid PNG signature size.");

		// Read in some of the signature bytes
		if (fread(&buf[0], 1, PNG_BYTES_TO_CHECK, fp) != PNG_BYTES_TO_CHECK or
		    png_sig_cmp(&buf[0], 0, PNG_BYTES_TO_CHECK) != 0) {
			throw std::runtime_error(std::string("Error reading signature bytes."));
		}
		png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
		if (png_ptr == nullptr) {
			throw std::runtime_error("libpng error while reading");
		}

		png_infop info_ptr = png_create_info_struct(png_ptr);
		if (info_ptr == nullptr) {
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
		png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_STRIP_16, nullptr);

		const int x = png_get_image_width(png_ptr, info_ptr);
		const int y = png_get_image_height(png_ptr, info_ptr);
		const int channels = png_get_channels(png_ptr, info_ptr);

		const auto row_pointers = png_get_rows(png_ptr, info_ptr);
		std::vector<char> imageData(x * y * channels);
		for (int i = 0; i < y; ++i) {
			memcpy(&imageData[i*x*channels], row_pointers[i], x * channels);
		}

		SDL_Surface* const surface =
		    SDL_CreateRGBSurfaceFrom(&imageData[0], x, y, channels * 8, channels * x, 0x000000ff,
		                             0x0000ff00, 0x00ff0000, 0xff000000);

		if (surface == nullptr) {
			jngl::debugLn(SDL_GetError());
			return;
		}

		png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);

		SDL_SetWindowIcon(impl->sdlWindow, surface);
		SDL_FreeSurface(surface);
#endif
	}

	int getDesktopWidth() {
		SDL::init();
		SDL_DisplayMode mode;
		SDL_GetDesktopDisplayMode(0, &mode);
		return mode.w;
	}

	int getDesktopHeight() {
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

#ifdef _WIN32
	// TODO: Avoid this duplicated code

	int Window::getMouseX() const {
		if (relativeMouseMode) { return mousex_; }
		return mousex_ - (width_ - canvasWidth) / 2;
	}

	int Window::getMouseY() const {
		if (relativeMouseMode) { return mousey_; }
		return mousey_ - (height_ - canvasHeight) / 2;
	}
#endif

} // namespace jngl
