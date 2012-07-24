/*
Copyright 2007-2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "window.hpp"
#include "main.hpp"
#include "debug.hpp"

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
			boost::assign::ptr_map_insert(fonts_[fontSize_])(filename, (pathPrefix + filename).c_str(), fontSize_);
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

	bool keyDown(const char key)
	{
		std::string temp; temp.append(1, key);
		return keyDown(temp);
	}

	bool keyPressed(const char key)
	{
		std::string temp; temp.append(1, key);
		return keyPressed(temp);
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
		while(running_)
		{
			if (!stepIfNeeded() && needDraw_) {
				needDraw_ = false;
				draw();
				jngl::swapBuffers();
			}
		}
	}

	bool Window::stepIfNeeded() {
		if(jngl::getTime() - oldTime_ > 0.5) // Is half a second missing?
		{
			oldTime_ += 0.5; // Let's slowdown
		}
		const static double timePerStep = 1.0 / 60.0;
		if(jngl::getTime() - oldTime_ > timePerStep)
		{
			// This stuff needs to be done 100 times per second
			oldTime_ += timePerStep;
			jngl::updateInput();
			if (currentWork_) {
				currentWork_->step();
			}
			needDraw_ = true;
			if (!jngl::running()) {
				currentWork_->onQuitEvent();
			}
			while (changeWork_) {
				changeWork_ = false;
				currentWork_ = newWork_;
				currentWork_->onLoad();
			}
			return true;
		}
		return false;
	}

	void Window::draw() const {
		if (currentWork_) {
			currentWork_->draw();
		} else {
			jngl::print("No work set. Use jngl::setWork", -50, -5);
		}
	}

	void Window::SetWork(boost::shared_ptr<Work> work) {
		if (!currentWork_) {
			Debug("setting current work to "); Debug(work.get()); Debug("\n");
			currentWork_ = work;
		}
		else {
			Debug("change work to "); Debug(work.get()); Debug("\n");
			changeWork_ = true;
			newWork_ = work;
		}
	}

	boost::shared_ptr<Work> Window::getWork() {
		return currentWork_;
	}

	void Window::setConfigPath(const std::string& path) {
		configPath = path;
	}

	std::string Window::getConfigPath() const {
		return configPath;
	}
}
