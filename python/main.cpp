#include <boost/python.hpp> // Needs to be included first. See http://bugs.python.org/issue10910

#include "../jngl.hpp"

using namespace boost::python;
using namespace jngl;

void ShowWindow1(const std::string& title, int width, int height)
{
	ShowWindow(title, width, height);
}
void ShowWindow2(const std::string& title, int width, int height, bool fullscreen)
{
	ShowWindow(title, width, height, fullscreen);
}
void DrawScaled1(const std::string& filename, double xposition, double yposition, float xfactor, float yfactor)
{
	DrawScaled(filename, xposition, yposition, xfactor, yfactor);
}
void DrawScaled2(const std::string& filename, double xposition, double yposition, float factor)
{
	DrawScaled(filename, xposition, yposition, factor);
}
void Scale1(double factor)
{
	Scale(factor);
}
void Scale2(double xfactor, double yfactor)
{
	Scale(xfactor, yfactor);
}
bool MouseDown1()
{
	return MouseDown();
}
bool MouseDown2(mouse::Button button)
{
	return MouseDown(button);
}
bool MousePressed1()
{
	return MousePressed();
}
bool MousePressed2(mouse::Button button)
{
	return MousePressed(button);
}
void SetColor1(unsigned char red, unsigned char green, unsigned char blue)
{
	SetColor(red, green, blue);
}
void SetColor2(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
{
	SetColor(red, green, blue, alpha);
}
void SetFontColor1(unsigned char red, unsigned char green, unsigned char blue)
{
	SetFontColor(red, green, blue);
}
void SetFontColor2(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
{
	SetFontColor(red, green, blue, alpha);
}
void SetSpriteColor1(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
{
	SetSpriteColor(red, green, blue, alpha);
}
void SetSpriteColor2(unsigned char red, unsigned char green, unsigned char blue)
{
	SetSpriteColor(red, green, blue);
}
bool KeyDown1(key::KeyType key)
{
	return KeyDown(key);
}
bool KeyDown2(const std::string& key)
{
	return KeyDown(key);
}
bool KeyPressed1(key::KeyType key)
{
	return KeyPressed(key);
}
bool KeyPressed2(const std::string& key)
{
	return KeyPressed(key);
}

BOOST_PYTHON_MODULE(jngl)
{
	def("ShowWindow", ShowWindow1);
	def("ShowWindow", ShowWindow2);
	def("Running", Running);
	def("SwapBuffers", SwapBuffers);
	def("BeginDraw", BeginDraw);
	def("EndDraw", EndDraw);
	def("HideWindow", HideWindow);
	def("Quit", Quit);
	def("Continue", Continue);
	def("Draw", Draw);
	def("DrawScaled", DrawScaled1);
	def("DrawScaled", DrawScaled2);
	def("DrawRect", DrawRect);
	def("DrawLine", DrawLine);
	def("DrawEllipse", DrawEllipse);
	def("DrawPoint", DrawPoint);
	def("Rotate", Rotate);
	def("Translate", Translate);
	def("Scale", Scale1);
	def("Scale", Scale2);
	def("PushMatrix", PushMatrix);
	def("PopMatrix", PopMatrix);
	def("Reset", Reset);
	def("Load", Load);
	def("Unload", Unload);
	def("GetWidth", GetWidth);
	def("GetHeight", GetHeight);
	def("Time", Time);
	def("BeginPolygon", BeginPolygon);
	def("Vertex", Vertex);
	def("EndPolygon", EndPolygon);
	def("SetMouseVisible", SetMouseVisible);
	def("IsMouseVisible", IsMouseVisible);
	def("GetMouseX", GetMouseX);
	def("GetMouseY", GetMouseY);

	enum_<mouse::Button>("mouse")
        .value("Left", mouse::Left)
        .value("Middle", mouse::Middle)
        .value("Right", mouse::Right);

	def("MouseDown", MouseDown1);
	def("MouseDown", MouseDown2);
	def("MousePressed", MousePressed1);
	def("MousePressed", MousePressed2);
	def("SetMouse", SetMouse);
	def("DrawButton", DrawButton);
	def("SetTitle", SetTitle);
	def("SetBackgroundColor", SetBackgroundColor);
	def("SetColor", SetColor1);
	def("SetColor", SetColor2);
	def("SetFontColor", SetFontColor1);
	def("SetFontColor", SetFontColor2);
	def("SetSpriteColor", SetSpriteColor1);
	def("SetSpriteColor", SetSpriteColor2);
	def("Print", Print);
	def("GetFontSize", GetFontSize);
	def("SetFontSize", SetFontSize);
	def("SetFont", SetFont);
	def("SetFontByName", SetFontByName);
	def("Sleep", Sleep);
	def("FPS", FPS);
	def("ErrorMessage", ErrorMessage);
	def("GetFullscreen", GetFullscreen);
	def("ReadPixel", ReadPixel);
	def("GetWindowWidth", GetWindowWidth);
	def("GetWindowHeight", GetWindowHeight);
	def("SetAntiAliasing", SetAntiAliasing);
	def("GetAntiAliasing", GetAntiAliasing);
	def("GetTextWidth", GetTextWidth);

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

	def("KeyDown", KeyDown1);
	def("KeyDown", KeyDown2);
	def("KeyPressed", KeyPressed1);
	def("KeyPressed", KeyPressed2);
	def("Play", Play);
	def("Stop", Stop);
	def("Load", Load);
	def("IsPlaying", IsPlaying);
	def("IsOpenALInstalled", IsOpenALInstalled);
	def("SetPlaybackSpeed", SetPlaybackSpeed);
	def("SetVolume", SetVolume);
	def("GetDesktopWidth", GetDesktopWidth);
	def("GetDesktopHeight", GetDesktopHeight);
	def("GetMouseWheel", GetMouseWheel);
}
