// Copyright 2009-2019 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../src/jngl.hpp"

#include <pybind11/pybind11.h>
#include <thread>

namespace py = pybind11;
using namespace jngl;

void showWindow1(const std::string& title, int width, int height) {
	showWindow(title, width, height);
}
void showWindow2(const std::string& title, int width, int height, bool fullscreen) {
	showWindow(title, width, height, fullscreen);
}
void showWindow3(const std::string& title, int width, int height, bool fullscreen,
                 const py::tuple& minAspectRatio, const py::tuple& maxAspectRatio) {
	showWindow(title, width, height, fullscreen,
	           { minAspectRatio[0].cast<int>(), minAspectRatio[1].cast<int>() },
	           { maxAspectRatio[0].cast<int>(), maxAspectRatio[1].cast<int>() });
}
void drawScaled1(const std::string& filename, double xposition, double yposition, float xfactor,
                 float yfactor) {
	drawScaled(filename, xposition, yposition, xfactor, yfactor);
}
void drawScaled2(const std::string& filename, double xposition, double yposition, float factor) {
	drawScaled(filename, xposition, yposition, factor);
}
void scale1(double factor) {
	scale(factor);
}
void scale2(double xfactor, double yfactor) {
	scale(xfactor, yfactor);
}
bool mouseDown1() {
	return mouseDown();
}
bool mouseDown2(mouse::Button button) {
	return mouseDown(button);
}
bool mousePressed1() {
	return mousePressed();
}
bool mousePressed2(mouse::Button button) {
	return mousePressed(button);
}
void setColor1(unsigned char red, unsigned char green, unsigned char blue) {
	setColor(red, green, blue);
}
void setColor2(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha) {
	setColor(red, green, blue, alpha);
}
void setFontColor1(unsigned char red, unsigned char green, unsigned char blue) {
	setFontColor(red, green, blue);
}
void setFontColor2(unsigned char red, unsigned char green, unsigned char blue,
                   unsigned char alpha) {
	setFontColor(red, green, blue, alpha);
}
void setSpriteColor1(unsigned char red, unsigned char green, unsigned char blue,
                     unsigned char alpha) {
	setSpriteColor(red, green, blue, alpha);
}
void setSpriteColor2(unsigned char red, unsigned char green, unsigned char blue) {
	setSpriteColor(red, green, blue);
}
bool keyDown1(key::KeyType key) {
	return keyDown(key);
}
bool keyDown2(const std::string& key) {
	return keyDown(key);
}
bool keyPressed1(key::KeyType key) {
	return keyPressed(key);
}
bool keyPressed2(const std::string& key) {
	return keyPressed(key);
}
void draw1(const std::string& filename, int x, int y) {
	return draw(filename, x, y);
}
void drawRect1(double xposition, double yposition, double width, double height) {
	return drawRect(xposition, yposition, width, height);
}
void print1(const std::string& text, float x, float y) {
	return print(text, int(x), int(y));
}
void loadWrapper(const std::string& filename) {
	load(filename);
}

PYBIND11_MODULE(jngl, m) {
	py::class_<Sprite>(m, "Sprite")
	    .def(py::init<const std::string&>())
	    .def("draw", static_cast<void (Sprite::*)() const>(&Sprite::draw));

	py::class_<Vec2>(m, "Vec2")
	    .def(py::init<double, double>())
	    .def_readwrite("x", &Vec2::x)
	    .def_readwrite("y", &Vec2::y);

	py::class_<Color>(m, "Color")
	    .def(py::init<uint8_t, uint8_t, uint8_t>())
	    .def("getRed", &Color::getRed)
	    .def("getGreen", &Color::getGreen)
	    .def("getBlue", &Color::getBlue)
	    .def("setRed", &Color::setRed)
	    .def("setGreen", &Color::setGreen)
	    .def("setBlue", &Color::setBlue);

	m.def("showWindow", showWindow1);
	m.def("showWindow", showWindow2);
	m.def("showWindow", showWindow3);
	m.def("running", running);
	m.def("updateInput", updateInput);
	m.def("swapBuffers", swapBuffers);
	m.def("hideWindow", hideWindow);
	m.def("quit", quit);
	m.def("cancelQuit", cancelQuit);
	m.def("draw", draw1);
	m.def("draw", static_cast<void (*)(const std::string&, double, double)>(draw));
	m.def("drawScaled", drawScaled1);
	m.def("drawScaled", drawScaled2);
	m.def("drawRect", drawRect1);
	m.def("drawLine", static_cast<void (*)(double, double, double, double)>(drawLine));
	m.def("drawEllipse", static_cast<void (*)(float, float, float, float, float)>(drawEllipse));
	m.def("drawPoint", drawPoint);
	m.def("rotate", rotate);
	m.def("translate", static_cast<void (*)(double, double)>(translate));
	m.def("scale", scale1);
	m.def("scale", scale2);
	m.def("pushMatrix", pushMatrix);
	m.def("popMatrix", popMatrix);
	m.def("reset", reset);
	m.def("load", loadWrapper);
	m.def("unload", unload);
	m.def("getWidth", getWidth);
	m.def("getHeight", getHeight);
	m.def("getTime", getTime);
	m.def("setMouseVisible", setMouseVisible);
	m.def("isMouseVisible", isMouseVisible);
	m.def("getMousePos", getMousePos);
	m.def("getScaleFactor", getScaleFactor);
	m.def("setScaleFactor", setScaleFactor);

	py::enum_<mouse::Button>(m, "mouse")
	    .value("Left", mouse::Left)
	    .value("Middle", mouse::Middle)
	    .value("Right", mouse::Right);

	m.def("mouseDown", mouseDown1);
	m.def("mouseDown", mouseDown2);
	m.def("mousePressed", mousePressed1);
	m.def("mousePressed", mousePressed2);
	m.def("setMouse", setMouse);
	m.def("setTitle", setTitle);
	m.def("setBackgroundColor", static_cast<void (*)(Color)>(setBackgroundColor));
	m.def("setColor", setColor1);
	m.def("setColor", setColor2);
	m.def("setAlpha", setAlpha);
	m.def("setFontColor", setFontColor1);
	m.def("setFontColor", setFontColor2);
	m.def("setSpriteColor", setSpriteColor1);
	m.def("setSpriteColor", setSpriteColor2);
	m.def("print", static_cast<void (*)(const std::string&, Vec2)>(print));
	m.def("print", print1);
	m.def("getFontSize", getFontSize);
	m.def("setFontSize", setFontSize);
	m.def("setFont", setFont);
	m.def("setFontByName", setFontByName);
	m.def("sleep", +[](const int ms) { std::this_thread::sleep_for(std::chrono::milliseconds(ms)); });
	m.def("getFPS", getFPS);
	m.def("errorMessage", errorMessage);
	m.def("getFullscreen", getFullscreen);
	m.def("getWindowWidth", getWindowWidth);
	m.def("getWindowHeight", getWindowHeight);
	m.def("getScreenWidth", getScreenWidth);
	m.def("getScreenHeight", getScreenHeight);
	m.def("setAntiAliasing", setAntiAliasing);
	m.def("getAntiAliasing", getAntiAliasing);
	m.def("getTextWidth", getTextWidth);

	py::enum_<key::KeyType>(m, "key")
	    .value("Left", key::Left)
	    .value("Up", key::Up)
	    .value("Right", key::Right)
	    .value("Down", key::Down)
	    .value("PageUp", key::PageUp)
	    .value("PageDown", key::PageDown)
	    .value("Home", key::Home)
	    .value("End", key::End)
	    .value("BackSpace", key::BackSpace)
	    .value("Tab", key::Tab)
	    .value("Clear", key::Clear)
	    .value("Return", key::Return)
	    .value("Pause", key::Pause)
	    .value("Escape", key::Escape)
	    .value("Delete", key::Delete)
	    .value("ControlL", key::ControlL)
	    .value("ControlR", key::ControlR)
	    .value("CapsLock", key::CapsLock)
	    .value("AltL", key::AltL)
	    .value("AltR", key::AltR)
	    .value("SuperL", key::SuperL)
	    .value("SuperR", key::SuperR)
	    .value("Space", key::Space)
	    .value("ShiftL", key::ShiftL)
	    .value("ShiftR", key::ShiftR)
	    .value("F1", key::F1)
	    .value("F2", key::F2)
	    .value("F3", key::F3)
	    .value("F4", key::F4)
	    .value("F5", key::F5)
	    .value("F6", key::F6)
	    .value("F7", key::F7)
	    .value("F8", key::F8)
	    .value("F9", key::F9)
	    .value("F10", key::F10)
	    .value("F11", key::F11)
	    .value("F12", key::F12)
	    .value("Any", key::Any);

	m.def("keyDown", keyDown1);
	m.def("keyDown", keyDown2);
	m.def("keyPressed", keyPressed1);
	m.def("keyPressed", keyPressed2);
	m.def("play", play);
	m.def("stop", stop);
	m.def("isPlaying", isPlaying);
	m.def("isOpenALInstalled", isOpenALInstalled);
	m.def("setPlaybackSpeed", setPlaybackSpeed);
	m.def("setVolume", setVolume);
	m.def("getDesktopWidth", getDesktopWidth);
	m.def("getDesktopHeight", getDesktopHeight);
	m.def("getMouseWheel", getMouseWheel);
	m.def("setPrefix", setPrefix);
	m.def("getPrefix", getPrefix);
	m.def("setConfigPath", setConfigPath);
	m.def("getConfigPath", getConfigPath);
	m.def("pushSpriteAlpha", pushSpriteAlpha);
	m.def("popSpriteAlpha", popSpriteAlpha);

	py::class_<FrameBuffer>(m, "FrameBuffer")
	    .def(py::init<int, int>())
	    // .def("draw", &FrameBuffer::draw) FIXME
	    .def("beginDraw", &FrameBuffer::beginDraw)
	    .def("endDraw", &FrameBuffer::endDraw);

	py::enum_<controller::Button>(m, "controller")
	    .value("LeftStickX", controller::LeftStickX)
	    .value("LeftStickY", controller::LeftStickY)
	    .value("RightStickX", controller::RightStickX)
	    .value("RightStickY", controller::RightStickY)
	    .value("A", controller::A)
	    .value("B", controller::B)
	    .value("X", controller::X)
	    .value("Y", controller::Y)
	    .value("LeftButton", controller::LeftButton)
	    .value("RightButton", controller::RightButton)
	    .value("LeftTrigger", controller::LeftTrigger)
	    .value("RightTrigger", controller::RightTrigger)
	    .value("Start", controller::Start)
	    .value("Back", controller::Back)
	    .value("DpadUp", controller::DpadUp)
	    .value("DpadDown", controller::DpadDown)
	    .value("DpadLeft", controller::DpadLeft)
	    .value("DpadRight", controller::DpadRight)
	    .value("LeftStick", controller::LeftStick)
	    .value("RightStick", controller::RightStick);
}
