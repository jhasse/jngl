#include <cmath> // see http://lists.boost.org/boost-users/2010/12/65159.php
#include <boost/python.hpp> // Needs to be included first. See http://bugs.python.org/issue10910

#include "../src/jngl.hpp"

using namespace boost::python;
using namespace jngl;

void showWindow1(const std::string& title, int width, int height)
{
	showWindow(title, width, height);
}
void showWindow2(const std::string& title, int width, int height, bool fullscreen)
{
	showWindow(title, width, height, fullscreen);
}
void drawScaled1(const std::string& filename, double xposition, double yposition, float xfactor, float yfactor)
{
	drawScaled(filename, xposition, yposition, xfactor, yfactor);
}
void drawScaled2(const std::string& filename, double xposition, double yposition, float factor)
{
	drawScaled(filename, xposition, yposition, factor);
}
void scale1(double factor)
{
	scale(factor);
}
void scale2(double xfactor, double yfactor)
{
	scale(xfactor, yfactor);
}
bool mouseDown1()
{
	return mouseDown();
}
bool mouseDown2(mouse::Button button)
{
	return mouseDown(button);
}
bool mousePressed1()
{
	return mousePressed();
}
bool mousePressed2(mouse::Button button)
{
	return mousePressed(button);
}
void setColor1(unsigned char red, unsigned char green, unsigned char blue)
{
	setColor(red, green, blue);
}
void setColor2(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
{
	setColor(red, green, blue, alpha);
}
void setFontColor1(unsigned char red, unsigned char green, unsigned char blue)
{
	setFontColor(red, green, blue);
}
void setFontColor2(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
{
	setFontColor(red, green, blue, alpha);
}
void setSpriteColor1(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
{
	setSpriteColor(red, green, blue, alpha);
}
void setSpriteColor2(unsigned char red, unsigned char green, unsigned char blue)
{
	setSpriteColor(red, green, blue);
}
bool keyDown1(key::KeyType key)
{
	return keyDown(key);
}
bool keyDown2(const std::string& key)
{
	return keyDown(key);
}
bool keyPressed1(key::KeyType key)
{
	return keyPressed(key);
}
bool keyPressed2(const std::string& key)
{
	return keyPressed(key);
}
void draw1(const std::string& filename, int x, int y) {
	return draw(filename, x, y);
}
void drawRect1(double xposition, double yposition, double width, double height) {
	return drawRect(xposition, yposition, width, height);
}
void print1(const std::string& text, float x, float y) {
	return print(text, x, y);
}

BOOST_PYTHON_MODULE(jngl)
{
	def("showWindow", showWindow1);
	def("showWindow", showWindow2);
	def("running", running);
	def("updateInput", updateInput);
	def("swapBuffers", swapBuffers);
	def("hideWindow", hideWindow);
	def("quit", quit);
	def("cancelQuit", cancelQuit);
	def("draw", draw1);
	def("draw", static_cast<void(*)(const std::string&, jngl::Float, jngl::Float)>(draw));
	def("drawScaled", drawScaled1);
	def("drawScaled", drawScaled2);
	def("drawRect", drawRect1);
	def("drawLine", drawLine);
	def("drawEllipse", drawEllipse);
	def("drawPoint", drawPoint);
	def("rotate", rotate);
	def("translate", static_cast<void(*)(jngl::Float, jngl::Float)>(translate));
	def("scale", scale1);
	def("scale", scale2);
	def("pushMatrix", pushMatrix);
	def("popMatrix", popMatrix);
	def("reset", reset);
	def("load", load);
	def("unload", unload);
	def("getWidth", getWidth);
	def("getHeight", getHeight);
	def("getTime", getTime);
	def("beginPolygon", beginPolygon);
	def("vertex", vertex);
	def("endPolygon", endPolygon);
	def("setMouseVisible", setMouseVisible);
	def("isMouseVisible", isMouseVisible);
	def("getMouseX", getMouseX);
	def("getMouseY", getMouseY);

	enum_<mouse::Button>("mouse")
        .value("Left", mouse::Left)
        .value("Middle", mouse::Middle)
        .value("Right", mouse::Right);

	def("mouseDown", mouseDown1);
	def("mouseDown", mouseDown2);
	def("mousePressed", mousePressed1);
	def("mousePressed", mousePressed2);
	def("setMouse", setMouse);
	def("setTitle", setTitle);
	def("setBackgroundColor", setBackgroundColor);
	def("setColor", setColor1);
	def("setColor", setColor2);
	def("setAlpha", setAlpha);
	def("setFontColor", setFontColor1);
	def("setFontColor", setFontColor2);
	def("setSpriteColor", setSpriteColor1);
	def("setSpriteColor", setSpriteColor2);
	def("print", print);
	def("print", print1);
	def("getFontSize", getFontSize);
	def("setFontSize", setFontSize);
	def("setFont", setFont);
	def("setFontByName", setFontByName);
	def("sleep", jngl::sleep);
	def("getFPS", getFPS);
	def("errorMessage", errorMessage);
	def("getFullscreen", getFullscreen);
	def("readPixel", readPixel);
	def("getWindowWidth", getWindowWidth);
	def("getWindowHeight", getWindowHeight);
	def("getScreenWidth", getScreenWidth);
	def("getScreenHeight", getScreenHeight);
	def("setAntiAliasing", setAntiAliasing);
	def("getAntiAliasing", getAntiAliasing);
	def("getTextWidth", getTextWidth);

	enum_<key::KeyType>("key")
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

	def("keyDown", keyDown1);
	def("keyDown", keyDown2);
	def("keyPressed", keyPressed1);
	def("keyPressed", keyPressed2);
	def("play", play);
	def("stop", stop);
	def("load", load);
	def("isPlaying", isPlaying);
	def("isOpenALInstalled", isOpenALInstalled);
	def("setPlaybackSpeed", setPlaybackSpeed);
	def("setVolume", setVolume);
	def("getDesktopWidth", getDesktopWidth);
	def("getDesktopHeight", getDesktopHeight);
	def("getMouseWheel", getMouseWheel);
	def("mainLoop", mainLoop);
	def("setPrefix", setPrefix);
	def("getPrefix", getPrefix);
	def("setConfigPath", setConfigPath);
	def("getConfigPath", getConfigPath);
	def("pushSpriteAlpha", pushSpriteAlpha);
	def("popSpriteAlpha", popSpriteAlpha);
	def("setMasking", setMasking);

	class_<FrameBuffer, boost::noncopyable>("FrameBuffer", init<int, int>())
        .def("draw", &FrameBuffer::draw)
        .def("beginDraw", &FrameBuffer::beginDraw)
        .def("endDraw", &FrameBuffer::endDraw)
    ;
}