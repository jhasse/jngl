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

#ifndef __JNGL_HPP__
#define __JNGL_HPP__

#include <string>

namespace jngl
{
	void ShowWindow(const std::string& title,
	                int width,
	                int heigt,
	                bool fullscreen = false);

	void HideWindow();

	bool Running();

	void BeginDraw();

	void EndDraw();

	void Quit();

	void Draw(const std::string& filename,
	          int xposition,
	          int yposition);
	
	void DrawScaled(const std::string& filename,
	                double xposition,
	                double yposition,
	                float xfactor,
	                float yfactor);

	void DrawScaled(const std::string& filename,
	                double xposition,
	                double yposition,
	                float factor);

	void DrawQuad(int xposition, int yposition, int width, int height);
	
	void DrawLine(double xstart, double ystart, double xend, double yend);

	void Rotate(double radian);
	
	void Translate(int x, int y);

	void Reset();

	void Load(const std::string& filename);

	void Unload(const std::string& filename);

	int Width(const std::string& filename);

	int Height(const std::string& filename);

	double Time();

	void BeginPolygon();

	void Vertex(int xposition,
	            int yposition);

	void EndPolygon();

	bool KeyDown(int key);

	bool KeyDown(char key);

	bool KeyPressed(int key);

	bool KeyPressed(char key);

	void MouseVisible(bool visible);

	int MouseX();

	int MouseY();

	namespace mouse
	{
		enum Button
		{
			Left, Middle, Right
		};
	}

	bool MouseDown(mouse::Button button = mouse::Left);

	bool MousePressed(mouse::Button button = mouse::Left);

	void SetMouse(int xposition, int yposition);

	bool Button(const std::string& texture,
	            int xposition,
	            int yposition,
	            const std::string& mouseover);

	void SetTitle(const std::string& title);

	void BackgroundColor(unsigned char red,
	                     unsigned char green,
	                     unsigned char blue);

	void Color(unsigned char red,
	           unsigned char green,
	           unsigned char blue,
	           unsigned char alpha = 255);

	void FontColor(unsigned char red,
	               unsigned char green,
	               unsigned char blue,
	               unsigned char alpha = 255);

	void Print(const std::string& text,
	           int xposition,
	           int yposition);

	void FontSize(int size);
	
	void SetFont(const std::string& filename);
	
	void SetFontByName(const std::string& name);

	void Scale(int width, int height);

	void Sleep(int milliseconds);

	double FPS();
	
	int ScaleWidth();
	
	int ScaleHeight();

	namespace key
	{
#ifdef linux
		const int Left          = 0xff51;
		const int Up            = 0xff52;
		const int Right         = 0xff53;
		const int Down          = 0xff54;
		const int PageUp        = 0xff55;
		const int PageDown      = 0xff56;
		const int Home          = 0xff50;
		const int End           = 0xff57;
		const int BackSpace     = 0xff08;
		const int Tab           = 0xff09;
//		const int Clear         = 0xff0b;
		const int Return        = 0xff0d;
		const int Pause         = 0xff13;
		const int Escape        = 0xff1b;
		const int Delete        = 0xffff;
		const int ControlL      = 0xffe3;
		const int ControlR      = 0xffe4;
		const int CapsLock      = 0xffe5;
		const int AltL          = 0xffe9;
		const int AltR          = 0xffea;
		const int SuperL        = 0xffeb;
		const int SuperR        = 0xffec;
		const int Space         = 0x0020;
		const int ShiftL        = 0xffe1;
		const int ShiftR        = 0xffe2;
		const int F1            = 0xffbe;
		const int F2            = 0xffbf;
		const int F3            = 0xffc0;
		const int F4            = 0xffc1;
		const int F5            = 0xffc2;
		const int F6            = 0xffc3;
		const int F7            = 0xffc4;
		const int F8            = 0xffc5;
		const int F9            = 0xffc6;
		const int F10           = 0xffc7;
		const int F11           = 0xffc8;
		const int F12           = 0xffc9;
#else
		const int Left          = 0x25;
		const int Up            = 0x26;
		const int Right         = 0x27;
		const int Down          = 0x28;
		const int PageUp        = 0x21;
		const int PageDown      = 0x22;
		const int Home          = 0x24;
		const int End           = 0x23;
		const int BackSpace     = 0x08;
		const int Tab           = 0x09;
//		const int Clear         = 0x0C;
		const int Return        = 0x0D;
		const int Pause         = 0x13;
		const int Escape        = 0x1b;
		const int Delete        = 0x2E;
		const int ControlL      = 0xA2;
		const int ControlR      = 0xA3;
		const int CapsLock      = 0x14;
		const int AltL          = 0xA4;
		const int AltR          = 0xA5;
		const int SuperL        = 0x5B;
		const int SuperR        = 0x5C;
		const int Space         = 0x20;
		const int ShiftL        = 0xA0;
		const int ShiftR        = 0xA1;
		const int F1            = 0x70;
		const int F2            = 0x71;
		const int F3            = 0x72;
		const int F4            = 0x73;
		const int F5            = 0x74;
		const int F6            = 0x75;
		const int F7            = 0x76;
		const int F8            = 0x77;
		const int F9            = 0x78;
		const int F10           = 0x79;
		const int F11           = 0x7a;
		const int F12           = 0x7b;
#endif
	}
}

#endif // __JNGL_HPP__
