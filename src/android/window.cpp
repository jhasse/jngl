// Copyright 2013-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../window.hpp"
#include "../windowptr.hpp"
#include "windowimpl.hpp"

namespace jngl {

Window::Window(const std::string& /*title*/, const int width, const int height,
               const bool fullscreen, const std::pair<int, int> minAspectRatio,
               const std::pair<int, int> maxAspectRatio)
: fullscreen_(fullscreen), isMouseVisible_(true), relativeMouseMode(false), anyKeyPressed_(false),
  mousex_(0), mousey_(0), fontSize_(12), width_(width), height_(height), mouseWheel_(0),
  fontName_(""), impl(new WindowImpl(this, minAspectRatio, maxAspectRatio)) {
	mouseDown_.fill(false);
	mousePressed_.fill(false);

	// Calling UpdateInput() for the first time will set-up GLES among other things:
	UpdateInput();
}

std::string Window::GetFontFileByName(const std::string& fontname) {
	std::string tmp = fontname;
	if (fontname == "sans-serif") {
		tmp = "Arial";
	}
	return tmp + ".ttf";
}

Window::~Window() {
	delete impl;
}

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
	impl->swapBuffers();
}

void Window::SetMouseVisible(const bool) {
}

void Window::SetTitle(const std::string&) {
}

int Window::getMouseX() {
	return mousex_ - (width_ - canvasWidth) / 2;
}

int Window::getMouseY() {
	return mousey_ - (height_ - canvasHeight) / 2;
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
	return pWindow ? pWindow->getWidth() : -1;
}

int getDesktopHeight() {
	return pWindow ? pWindow->getHeight() : -1;
}

} // namespace jngl
