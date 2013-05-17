/*
Copyright 2007-2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "window.hpp"
#include "main.hpp"
#include "jngl.hpp"

namespace jngl {
	double Window::GetTextWidth(const std::string& text) {
		return fonts_[fontSize_][fontName_]->getTextWidth(text);
	}

	int Window::getLineHeight() {
		return fonts_[fontSize_][fontName_]->getLineHeight();
	}

	void Window::setLineHeight(int h) {
		return fonts_[fontSize_][fontName_]->setLineHeight(h);
	}

	std::shared_ptr<FontImpl> Window::getFontImpl() {
		return fonts_[fontSize_][fontName_];
	}

	void Window::Print(const std::string& text, const int xposition, const int yposition) {
		fonts_[fontSize_][fontName_]->print(xposition, yposition, text);
	}

	void Window::SetFont(const std::string& filename) {
		if (fonts_[fontSize_].find(filename) == fonts_[fontSize_].end()) {
			auto font = std::make_shared<FontImpl>(filename, fontSize_);
			fonts_[fontSize_][filename] = font;
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
		catch(std::exception& e) // Something went wrong ...
		{
			fontSize_ = oldSize; // ... so let's set fontSize_ back to the previous size
			throw e;
		}
	}

	bool Window::getMouseDown(mouse::Button button) {
		return mouseDown_.at(button);
	}

	bool Window::getMousePressed(mouse::Button button) {
		if (mousePressed_.at(button)) {
			needToBeSetFalse_.push(&mousePressed_[button]);
			return true;
		}
		return false;
	}

	void Window::setMousePressed(mouse::Button button, bool p) {
		mousePressed_.at(button) = p;
	}

	void Window::setMouseDown(mouse::Button button, bool d) {
		mouseDown_.at(button) = d;
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

	bool Window::getKeyDown(key::KeyType key) {
		if (key == key::Any) {
			for (auto it = keyDown_.begin(); it != keyDown_.end(); ++it) {
				if (it->second) {
					return true;
				}
			}
			for (auto it = characterDown_.begin(); it != characterDown_.end(); ++it) {
				if (it->second) {
					return true;
				}
			}
			return false;
		}
		return keyDown_[GetKeyCode(key)];
	}

	bool Window::getKeyPressed(key::KeyType key) {
		if (key == key::Any) {
			if (anyKeyPressed_) {
				needToBeSetFalse_.push(&anyKeyPressed_);
				return getKeyDown(jngl::key::Any);
			}
		} else if(keyPressed_[GetKeyCode(key)]) {
			needToBeSetFalse_.push(&keyPressed_[GetKeyCode(key)]);
			return true;
		}
		return false;
	}

	void Window::setKeyPressed(const key::KeyType key, bool p) {
		keyPressed_[GetKeyCode(key)] = p;
	}

	void Window::setKeyPressed(const std::string &key, bool p) {
		characterPressed_[key] = p;
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

	void Window::MainLoop() {
		Finally _([&]() {
			currentWork_.reset((jngl::Work*)0);
		});
		while (running_) {
			stepIfNeeded();
			draw();
			jngl::swapBuffers();
		}
	}

	void Window::resetFrameLimiter() {
		oldTime = jngl::getTime();
		stepsPerFrame = 1;
	}

	void Window::stepIfNeeded() {
		const static float timePerStep = 1.0f / 60.0f;
		if (jngl::getTime() - oldTime > timePerStep * stepsPerFrame) {
			stepsPerFrame += 0.1f;
		}
		if (stepsPerFrame < 0.51f) {
			stepsPerFrame = 1.0f;
		}
		for (int i = 0; i < int(stepsPerFrame + 0.5); ++i) {
			oldTime += timePerStep;
			jngl::updateInput();
			if (currentWork_) {
				currentWork_->step();
			}
			for (auto& job : jobs) {
				job->step();
			}
			if (!jngl::running() && currentWork_) {
				currentWork_->onQuitEvent();
			}
			while (changeWork_) {
				changeWork_ = false;
				currentWork_ = newWork_;
				newWork_.reset((jngl::Work*)0);
				currentWork_->onLoad();
			}
		}
		auto timeToSleep = oldTime - jngl::getTime();
		if (timeToSleep > 0.01) {
			jngl::sleep(int(timeToSleep * 900));
			stepsPerFrame -= 0.1f;
		} else if (timeToSleep > 0) {
			oldTime = jngl::getTime();
		}
		if (timeToSleep > timePerStep && stepsPerFrame > 0.6) {
			stepsPerFrame -= float(timeToSleep - timePerStep);
		}
	}

	void Window::draw() const {
		if (currentWork_) {
			currentWork_->draw();
		} else {
			jngl::print("No work set. Use jngl::setWork", -50, -5);
		}
		for (auto& job : jobs) {
			job->draw();
		}
	}

	void Window::SetWork(std::shared_ptr<Work> work) {
		if (!currentWork_) {
			debug("setting current work to "); debug(work.get()); debug("\n");
			currentWork_ = work;
		}
		else {
			debug("change work to "); debug(work.get()); debug("\n");
			changeWork_ = true;
			newWork_ = work;
		}
	}

	void Window::addJob(std::shared_ptr<Job> job) {
		jobs.push_back(job);
	}

	std::shared_ptr<Work> Window::getWork() {
		return currentWork_;
	}

	void Window::setConfigPath(const std::string& path) {
		configPath = path;
	}

	std::string Window::getConfigPath() const {
		return configPath;
	}

	bool Window::isMultitouch() const {
		return multitouch;
	}
}
