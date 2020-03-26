// Copyright 2012-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../window.hpp"
#include "../jngl.hpp"
#include "windowimpl.hpp"
#include "../main.hpp"

#include <stdexcept>

namespace jngl {

Window::Window(const std::string& title, const int width, const int height, const bool fullscreen,
               const std::pair<int, int> minAspectRatio, const std::pair<int, int> maxAspectRatio)
: impl(std::make_unique<WindowImpl>(this)), fullscreen_(fullscreen), isMouseVisible_(true),
  relativeMouseMode(false), anyKeyPressed_(false), mousex_(0), mousey_(0), fontSize_(12),
  width_(width), height_(height), mouseWheel_(0), fontName_("") {
	calculateCanvasSize(minAspectRatio, maxAspectRatio);
	Init(width, height, canvasWidth, canvasHeight);
}

std::string Window::GetFontFileByName(const std::string& fontname) {
	std::string tmp = fontname;
	if (fontname == "sans-serif") {
		tmp = "Arial";
	}
	return tmp + ".ttf";
}

Window::~Window() {};

int Window::GetKeyCode(key::KeyType key) {
	return key;
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
	impl->updateInput();
}

void Window::SwapBuffers() {
}

void Window::SetMouseVisible(const bool) {
}

std::vector<Vec2> Window::getTouchPositions() const {
	std::vector<Vec2> positions;
	for (auto [id, pos] : impl->touches) {
		positions.emplace_back(
		    (pos.x - (width_ - canvasWidth) / 2) / getScaleFactor() - getScreenWidth() / 2,
		    (pos.y - (height_ - canvasHeight) / 2) / getScaleFactor() - getScreenHeight() / 2);
	}
	return positions;
}

void Window::SetTitle(const std::string&) {
}

void Window::SetMouse(const int /*xposition*/, const int /*yposition*/) {
}

void Window::SetRelativeMouseMode(const bool relative) {
	relativeMouseMode = relative;
	impl->setRelativeMouseMode(relative);
}

void Window::SetIcon(const std::string&) {
}

int getDesktopWidth() {
	return jngl::getWindowWidth();
}

int getDesktopHeight() {
	return jngl::getWindowHeight();
}

WindowImpl* Window::getImpl() const {
	return impl.get();
}

void Window::setFullscreen(bool fullscreen) {
	if (!fullscreen) {
		debugLn("Can't unset fullscreen on iOS!");
	}
}

} // namespace jngl
