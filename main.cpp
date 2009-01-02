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

#include "jngl.hpp"
#include "window.hpp"
#include "debug.hpp"
#include "texture.hpp"
#include "windowptr.hpp"

#include <boost/shared_ptr.hpp>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include <stdexcept>
#include <sstream>
#include <cmath>

extern "C"
{
	void InitCallbacks(); // see callbacks.c
}

namespace jngl
{
    float bgRed = 1.0f, bgGreen = 1.0f, bgBlue = 1.0f; // Background Colors
	bool Init(const int width, const int height)
	{
		glShadeModel(GL_SMOOTH);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glClearColor(bgRed, bgGreen, bgBlue, 0.0f);
		glClearDepth(1.0f);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glViewport(0, 0, width, height);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0f, width, height, 0.0f, -100.0f, 100.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glFlush();
		InitCallbacks();
		return true;
	}

	WindowPointer pWindow;
	bool antiAliasingEnabled = false;

	void ShowWindow(const std::string& title, const int width, const int height, bool fullscreen)
	{
		bool isMouseVisible = pWindow ? pWindow->GetMouseVisible() : true;
		HideWindow();
		if(width == 0)
		{
			throw std::runtime_error("Width Is 0");
		}
		if(height == 0)
		{
			throw std::runtime_error("Height Is 0");
		}
		pWindow.Set(new Window(title, width, height, fullscreen));
		pWindow->SetMouseVisible(isMouseVisible);
		SetAntiAliasing(antiAliasingEnabled);
	}

	void HideWindow()
	{
		UnloadAll();
		pWindow.Delete();
	}

	void BeginDraw()
	{
		pWindow->BeginDraw();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void EndDraw()
	{
		pWindow->EndDraw();
	}

	bool Running()
	{
		return pWindow->Running();
	}

	void Quit()
	{
		pWindow->Quit();
	}

	void Continue()
	{
		pWindow->Continue();
	}

	void SetBackgroundColor(const unsigned char red, const unsigned char green, const unsigned char blue)
	{
		bgRed = red / 255.0f;
		bgGreen = green / 255.0f;
		bgBlue = blue / 255.0f;
		glClearColor(bgRed, bgGreen, bgBlue, 0.0f);
	}

	int GetMouseX()
	{
		return pWindow->MouseX();
	}

	int GetMouseY()
	{
		return pWindow->MouseY();
	}

	bool KeyDown(const int key)
	{
		return pWindow->KeyDown(key);
	}

	bool KeyPressed(const int key)
	{
		return pWindow->KeyPressed(key);
	}

	bool KeyDown(const char key)
	{
		return pWindow->KeyDown(key);
	}

	bool KeyPressed(const char key)
	{
		return pWindow->KeyPressed(key);
	}

	bool MouseDown(mouse::Button button)
	{
		return pWindow->MouseDown(button);
	}

	bool MousePressed(mouse::Button button)
	{
		return pWindow->MousePressed(button);
	}

	void SetMouse(const int xposition, const int yposition)
	{
		pWindow->SetMouse(xposition, yposition);
	}

	void SetMouseVisible(const bool visible)
	{
		return pWindow->SetMouseVisible(visible);
	}

	bool IsMouseVisible()
	{
		return pWindow->GetMouseVisible();
	}

	void SetTitle(const std::string& title)
	{
		return pWindow->SetTitle(title);
	}

	unsigned char colorRed = 255, colorGreen = 255, colorBlue = 255, colorAlpha = 255;

	void SetColor(const unsigned char red, const unsigned char green, const unsigned char blue, const unsigned char alpha)
	{
		colorRed = red;
		colorGreen = green;
		colorBlue = blue;
		colorAlpha = alpha;
		glColor4ub(colorRed, colorGreen, colorBlue, colorAlpha);
	}

	double GetTextWidth(const std::string& text)
	{
		return pWindow->GetTextWidth(text);
	}

	void Print(const std::string& text, const double xposition, const double yposition)
	{
		pWindow->Print(text, xposition, yposition);
		glColor4ub(colorRed, colorGreen,  colorBlue, colorAlpha);
	}

	void SetFontSize(const int size)
	{
		pWindow->FontSize(size);
	}

	void SetFont(const std::string& filename)
	{
		pWindow->SetFont(filename);
	}

	void SetFontByName(const std::string& name)
	{
		pWindow->SetFontByName(name);
	}

	bool GetFullscreen()
	{
		return pWindow->GetFullscreen();
	}

	double FPS()
	{
		static double lastDraw = 0.0;
		double fps = 1/(jngl::Time() - lastDraw);
		lastDraw = jngl::Time();
		return fps;
	}

	void Reset()
	{
		glLoadIdentity();
	}

	void Rotate(const double degree)
	{
		glRotated(degree, 0, 0, 1);
	}

	void Translate(const double x, const double y)
	{
		glTranslated(x, y, 0);
	}

	void PushMatrix()
	{
		glPushMatrix();
	}

	void PopMatrix()
	{
		glPopMatrix();
	}

	void DrawRect(const double xposition, const double yposition, const double width, const double height)
	{
		glPushMatrix();
		glTranslated(xposition, yposition, 0);
		GLdouble rect[] = { 0, 0, width, 0, width, height, 0, height };
		glVertexPointer(2, GL_DOUBLE, 0, rect);
		glDrawArrays(GL_QUADS, 0, 4);
		glPopMatrix();
	}

	void DrawLine(const double xstart, const double ystart, const double xend, const double yend)
	{
		glPushMatrix();
		GLdouble line[] = { xstart, ystart, xend, yend };
		glVertexPointer(2, GL_DOUBLE, 0, line);
		glDrawArrays(GL_LINES, 0, 2);
		glPopMatrix();
	}

	void DrawEllipse(const double xmid, const double ymid, const double width, const double height)
	{
		glPushMatrix();
		glTranslated(xmid, ymid, 0);
		std::vector<GLdouble> vertexes;
		int count = 0;
		for(double t = 0; t <= 2*M_PI; t +=0.1)
		{
			vertexes.push_back(width * sin(t));
			vertexes.push_back(height * cos(t));
			++count;
		}
		glVertexPointer(2, GL_DOUBLE, 0, &vertexes[0]);
		glDrawArrays(GL_POLYGON, 0, count);
		glPopMatrix();
	}

	void DrawPoint(const double x, const double y)
	{
		GLdouble point[] = { x, y };
		glVertexPointer(2, GL_DOUBLE, 0, point);
		glDrawArrays(GL_POINTS, 0, 1);
	}

	void ReadPixel(const int x, const int y, unsigned char& red, unsigned char& green, unsigned char& blue)
	{
		unsigned char data[3];
		data[0] = static_cast<unsigned char>(bgRed * 255.0f);
		data[1] = static_cast<unsigned char>(bgGreen * 255.0f);
		data[2] = static_cast<unsigned char>(bgBlue * 255.0f);
		glReadPixels(x, GetWindowHeight() - y - 1, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, data);
		red = data[0];
		green = data[1];
		blue = data[2];
	}

	int GetWindowWidth()
	{
		return pWindow->GetWidth();
	}

	int GetWindowHeight()
	{
		return pWindow->GetHeight();
	}

	void SetAntiAliasing(bool enabled)
	{
		if(!pWindow->IsMultisampleSupported())
		{
			antiAliasingEnabled = false;
			return;
		}
		if(enabled)
		{
			glEnable(GL_MULTISAMPLE_ARB);
		}
		else
		{
			glDisable(GL_MULTISAMPLE_ARB);
		}
		antiAliasingEnabled = enabled;
	}

	bool GetAntiAliasing()
	{
		return antiAliasingEnabled;
	}
}
