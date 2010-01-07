/*
Copyright 2007-2009 Jan Niklas Hasse <jhasse@gmail.com>

This file is part of JNGL.

JNGL is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

JNGL is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with JNGL.  If not, see <http://www.gnu.org/licenses/>.
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

	void SwapBuffers();

	void BeginDraw();

	void EndDraw();

	void Quit();

	void Continue();

	void Draw(const std::string& filename,
	          double xposition,
	          double yposition);

	void DrawScaled(const std::string& filename,
	                double xposition,
	                double yposition,
	                float xfactor,
	                float yfactor);

	void DrawScaled(const std::string& filename,
	                double xposition,
	                double yposition,
	                float factor);

	void DrawRect(double xposition, double yposition, double width, double height);

	void DrawLine(double xstart, double ystart, double xend, double yend);

	void DrawEllipse(double xmid, double ymid, double width, double height);

	void DrawPoint(double x, double y);

	void Rotate(double degree);

	void Translate(double x, double y);

	void Scale(double factor);

	void Scale(double xfactor, double yfactor);

	void PushMatrix();

	void PopMatrix();

	void Reset();

	unsigned int Load(const std::string& filename);

	void Unload(const std::string& filename);

	int GetWidth(const std::string& filename);

	int GetHeight(const std::string& filename);

	double Time();

	void BeginPolygon();

	void Vertex(double xposition,
				double yposition);

	void EndPolygon();

	void SetMouseVisible(bool visible);

	bool IsMouseVisible();

	int GetMouseX();

	int GetMouseY();

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

	bool DrawButton(const std::string& texture,
	                double xposition,
	                double yposition,
	                const std::string& mouseover);

	void SetTitle(const std::string& title);

	void SetBackgroundColor(unsigned char red,
	                        unsigned char green,
	                        unsigned char blue);

	void SetColor(unsigned char red,
	              unsigned char green,
	              unsigned char blue,
				  unsigned char alpha = 255);

	void SetFontColor(unsigned char red,
	                  unsigned char green,
	                  unsigned char blue,
	                  unsigned char alpha = 255);

	void SetSpriteColor(unsigned char red,
	                    unsigned char green,
	                    unsigned char blue,
				        unsigned char alpha = 255);

	void Print(const std::string& text,
	           int xposition,
	           int yposition);

	int GetFontSize();

	void SetFontSize(int size);

	void SetFont(const std::string& filename);

	void SetFontByName(const std::string& name);

	void Sleep(int milliseconds);

	double FPS();

	void ErrorMessage(const std::string& text);

	bool GetFullscreen();

	void ReadPixel(int x, int y, unsigned char& red, unsigned char& green, unsigned char& blue);

	int GetWindowWidth();

	int GetWindowHeight();

	void SetAntiAliasing(bool enabled);

	bool GetAntiAliasing();

	double GetTextWidth(const std::string& text);

	namespace key
	{
		enum KeyType
		{
			Left,
			Up,
			Right,
			Down,
			PageUp,
			PageDown,
			Home,
			End,
			BackSpace,
			Tab,
			Clear,
			Return,
			Pause,
			Escape,
			Delete,
			ControlL,
			ControlR,
			CapsLock,
			AltL,
			AltR,
			SuperL,
			SuperR,
			Space,
			ShiftL,
			ShiftR,
			F1,
			F2,
			F3,
			F4,
			F5,
			F6,
			F7,
			F8,
			F9,
			F10,
			F11,
			F12,
			WizUp,
			WizDown,
			WizLeft,
			WizRight,
			WizA,
			WizB,
			WizX,
			WizY,
			WizL,
			WizR,
			WizMenu,
			WizSelect,
			WizVolUp,
			WizVolDown,
			Any
		};
	}

	bool KeyDown(key::KeyType key);

	bool KeyDown(char key);

	bool KeyDown(const std::string& key);

	bool KeyPressed(key::KeyType key);

	bool KeyPressed(char key);

	bool KeyPressed(const std::string& key);
}

#endif // __JNGL_HPP__
