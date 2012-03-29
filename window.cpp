/*
Copyright 2007-2011 Jan Niklas Hasse <jhasse@gmail.com>

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

#include "window.hpp"

#include <boost/assign/ptr_map_inserter.hpp>

namespace jngl
{
	double Window::GetTextWidth(const std::string& text)
	{
		return fonts_[fontSize_][fontName_].GetTextWidth(text);
	}

	int Window::GetLineHeight()
	{
		return fonts_[fontSize_][fontName_].GetLineHeight();
	}

	void Window::Print(const std::string& text, const int xposition, const int yposition)
	{
		fonts_[fontSize_][fontName_].Print(xposition, yposition, text);
	}

	void Window::SetFont(const std::string& filename)
	{
		if(fonts_[fontSize_].find(filename) == fonts_[fontSize_].end()) // Only load font if it doesn't exist yet
		{
			boost::assign::ptr_map_insert(fonts_[fontSize_])(filename, filename.c_str(), fontSize_);
		}
		fontName_ = filename;
	}
	
	std::string Window::GetFont() const
	{
		return fontName_;
	}

	void Window::SetFontByName(const std::string& name)
	{
		Window::SetFont(GetFontFileByName(name));
	}

	int Window::GetFontSize() const
	{
		return fontSize_;
	}

	void Window::SetFontSize(const int size)
	{
		const int oldSize = fontSize_;
		fontSize_ = size;
		try
		{
			SetFont(fontName_); // We changed the size we also need to reload the current font
		}
		catch(std::exception& e) // Someting went wrong ...
		{
			fontSize_ = oldSize; // ... so let's set fontSize_ back to the previous size
			throw e;
		}
	}

	bool Window::MouseDown(mouse::Button button)
	{
		return mouseDown_.at(button);
	}

	bool Window::MousePressed(mouse::Button button)
	{
		if(mousePressed_.at(button))
		{
			needToBeSetFalse_.push(&mousePressed_[button]);
			return true;
		}
		return false;
	}

	bool Window::GetFullscreen() const
	{
		return fullscreen_;
	}

	bool Window::GetMouseVisible() const
	{
		return isMouseVisible_;
	}
	
	bool Window::GetRelativeMouseMode() const {
		return relativeMouseMode;
	}

	int Window::GetWidth() const
	{
		return width_;
	}

	int Window::GetHeight() const
	{
		return height_;
	}

	bool Window::IsMultisampleSupported() const
	{
		return isMultisampleSupported_;
	}

	bool Window::Running()
	{
		return running_;
	}

	void Window::Quit()
	{
		running_ = false;
	}

	void Window::Continue()
	{
		running_ = true;
	}

	bool Window::KeyDown(key::KeyType key)
	{
		if(key == key::Any)
		{
			for(std::map<unsigned int, bool>::iterator it = keyDown_.begin(); it != keyDown_.end(); ++it)
			{
				if(it->second)
				{
					return true;
				}
			}
			for(std::map<std::string, bool>::iterator it = characterDown_.begin(); it != characterDown_.end(); ++it)
			{
				if(it->second)
				{
					return true;
				}
			}
			return false;
		}
		return keyDown_[GetKeyCode(key)];
	}

	bool Window::KeyPressed(key::KeyType key)
	{
		if(key == key::Any)
		{
			if(anyKeyPressed_)
			{
				needToBeSetFalse_.push(&anyKeyPressed_);
				return KeyDown(jngl::key::Any);
			}
		}
		else if(keyPressed_[GetKeyCode(key)])
		{
			needToBeSetFalse_.push(&keyPressed_[GetKeyCode(key)]);
			return true;
		}
		return false;
	}

	bool KeyDown(const char key)
	{
		std::string temp; temp.append(1, key);
		return KeyDown(temp);
	}

	bool KeyPressed(const char key)
	{
		std::string temp; temp.append(1, key);
		return KeyPressed(temp);
	}

	void Window::UpdateKeyStates()
	{
		while(!needToBeSetFalse_.empty())
		{
			*(needToBeSetFalse_.top()) = false;
			needToBeSetFalse_.pop();
		}
		mouseWheel_ = 0;
	}

	double Window::GetMouseWheel() const
	{
		return mouseWheel_;
	}
	
	void Window::MainLoop()
	{
		const static double timePerStep = 1.0 / 60.0;
		while(running_)
		{
			if(jngl::Time() - oldTime_ > 0.5) // Is half a second missing?
			{
				oldTime_ += 0.5; // Let's slowdown
			}
			if(jngl::Time() - oldTime_ > timePerStep)
			{
				// This stuff needs to be done 100 times per second
				oldTime_ += timePerStep;
				jngl::UpdateInput();
				currentWork_->Step();
				needDraw_ = true;
				if(jngl::KeyPressed(jngl::key::Escape) || !jngl::Running())
				{
					jngl::Continue(); // Don't let JNGL send the quit event again
					currentWork_->QuitEvent();
				}
			}
			else if(needDraw_)
			{
				needDraw_ = false;
				draw();
				jngl::SwapBuffers();
			}
			if(changeWork_)
			{
				changeWork_ = false;
				currentWork_ = newWork_;
			}
		}
	}
	
	void Window::draw() const {
		currentWork_->Draw();
	}
	
	void Window::SetWork(Work* w) {
		boost::shared_ptr<Work> work(w);
		if (!currentWork_) {
			currentWork_ = work;
		}
		else {
			changeWork_ = true;
			newWork_ = work;
		}
	}
}
