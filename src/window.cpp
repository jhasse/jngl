// Copyright 2007-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "window.hpp"
#include "jngl.hpp"

#include <cmath>
#include <chrono>
#include <thread>

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

	void Window::print(const std::string& text, const int xposition, const int yposition) {
		fonts_[fontSize_][fontName_]->print(xposition, yposition, text);
	}

	void Window::setFont(const std::string& filename) {
		if (fonts_[fontSize_].find(filename) == fonts_[fontSize_].end()) {
			auto font = std::make_shared<FontImpl>(filename, fontSize_);
			fonts_[fontSize_][filename] = font;
		}
		fontName_ = filename;
	}

	std::string Window::getFont() const {
		return fontName_;
	}

	void Window::setFontByName(const std::string& name) {
		Window::setFont(GetFontFileByName(name));
	}

	int Window::getFontSize() const {
		return fontSize_;
	}

	void Window::setFontSize(const int size) {
		const int oldSize = fontSize_;
		fontSize_ = size;
		try {
			setFont(fontName_); // We changed the size we also need to reload the current font
		} catch(std::exception& e) { // Something went wrong ...
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

	bool Window::getFullscreen() const {
		return fullscreen_;
	}

	bool Window::getMouseVisible() const {
		return isMouseVisible_;
	}

	bool Window::getRelativeMouseMode() const {
		return relativeMouseMode;
	}

	int Window::getWidth() const {
		return screenWidth;
	}

	int Window::getHeight() const {
		return screenHeight;
	}

	bool Window::isMultisampleSupported() const {
		return isMultisampleSupported_;
	}

	bool Window::isRunning() const {
		return running_;
	}

	void Window::quit() {
		running_ = false;
	}

	void Window::cancelQuit() {
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

	bool keyDown(const char key) {
		std::string temp; temp.append(1, key);
		return keyDown(temp);
	}

	bool keyPressed(const char key) {
		std::string temp; temp.append(1, key);
		return keyPressed(temp);
	}

	void Window::updateKeyStates() {
		while (!needToBeSetFalse_.empty()) {
			*(needToBeSetFalse_.top()) = false;
			needToBeSetFalse_.pop();
		}
		mouseWheel_ = 0;
		if (!updateInputCallbacks.empty()) {
			for (auto& updateInputCallback : updateInputCallbacks) {
				updateInputCallback();
			}
			updateInputCallbacks.clear();
		}
	}

	double Window::getMouseWheel() const {
		return mouseWheel_;
	}

	void Window::mainLoop() {
		Finally _([&]() {
			currentWork_.reset();
		});
		while (running_) {
			stepIfNeeded();
			draw();
			jngl::swapBuffers();
		}
	}

	void Window::resetFrameLimiter() {
		numberOfChecks = 0;
		stepsPerFrame = 1;
		sleepCorrectionFactor = 1;
	}

	unsigned int Window::getStepsPerSecond() const {
		return static_cast<unsigned int>(1.0 / timePerStep);
	}

	void Window::setStepsPerSecond(const unsigned int stepsPerSecond) {
		timePerStep = 1.0 / static_cast<double>(stepsPerSecond);
	}

	void Window::stepIfNeeded() {
		// TODO: These variables are static, but should rather be members of Window:
		static auto lastCheckTime = jngl::getTime();
		static unsigned int stepsSinceLastCheck = 0;
		static double timeSleptSinceLastCheck = 0;

		const auto currentTime = jngl::getTime();
		const auto secondsSinceLastCheck = currentTime - lastCheckTime;
		const auto targetStepsPerSecond = 1.0 / timePerStep;
		// If SPS == FPS, this would mean that we check about every second, but in the beginning we
		// want to check more often, e.g. to quickly adjust to high refresh rate monitors:
		if (stepsSinceLastCheck > targetStepsPerSecond || stepsSinceLastCheck > numberOfChecks) {
			const auto actualStepsPerSecond = stepsSinceLastCheck / secondsSinceLastCheck;
			const auto doableStepsPerSecond =
			    stepsSinceLastCheck / (secondsSinceLastCheck - timeSleptSinceLastCheck);
			// TODO: Improve logging. Log level? jngl::trace?
			jngl::debug("SPS: ");
			jngl::debug(std::lround(actualStepsPerSecond));
			jngl::debug(" (doable ");
			jngl::debug(std::lround(doableStepsPerSecond));
			jngl::debug(", should be ");
			jngl::debug(std::lround(targetStepsPerSecond));
			jngl::debug("); ");

			// The sleep function is actually inaccurate (or at least less accurate than getTime),
			// se we try to find a factor to correct this:
			sleepCorrectionFactor += 0.1 * // don't change it too fast
			    (sleepPerFrame * stepsSinceLastCheck / stepsPerFrame - timeSleptSinceLastCheck);
			//   ↑__________seconds we should have slept___________↑   ↑___actual seconds____↑

			// Clamp it in case of some bug:
			sleepCorrectionFactor = std::max(0.1, std::min(sleepCorrectionFactor, 2.0));

			// Round up, because if we can do 40 FPS, but need 60 SPS, we need at least 2 SPF. We
			// don't round up exactly to be a little bit "optimistic" of what we can do.
			auto newStepsPerFrame = std::min(static_cast<unsigned int>(std::max(1,
			    int(0.98 + stepsPerFrame * targetStepsPerSecond / doableStepsPerSecond))),
				stepsPerFrame * 2); // never increase too much
			// Divide doableStepsPerSecond by the previous stepsPerFrame and multiply it with
			// newStepsPerFrame so that we know what can be doable in the future and not what
			// could have been doable:
			double shouldSleepPerFrame =
			    newStepsPerFrame * // we sleep per frame, not per step
			    (timePerStep - 1.0 / (newStepsPerFrame * doableStepsPerSecond / stepsPerFrame));
			if (shouldSleepPerFrame < 0) {
				shouldSleepPerFrame = 0;
			}
			// The factor means that we quickly go down when needed, but hesitate to go up:
			sleepPerFrame += ((shouldSleepPerFrame < sleepPerFrame) ? 0.95 : 0.55) *
			                 (shouldSleepPerFrame - sleepPerFrame);
			jngl::debug("stepsPerFrame -> ");
			jngl::debug(newStepsPerFrame);
			jngl::debug(", msSleepPerFrame -> ");
			jngl::debug(sleepPerFrame);
			jngl::debug(" * ");
			jngl::debugLn(sleepCorrectionFactor);
			lastCheckTime = currentTime;
			stepsSinceLastCheck = 0;
			timeSleptSinceLastCheck = 0;
			stepsPerFrame = newStepsPerFrame;
			++numberOfChecks;
		}
		const int micros = std::lround(sleepPerFrame * sleepCorrectionFactor * 1e6);
		if (micros > 0) {
			const auto start = getTime();
			std::this_thread::sleep_for(std::chrono::microseconds(micros));
			timeSleptSinceLastCheck += jngl::getTime() - start;
		}
		for (unsigned int i = 0; i < stepsPerFrame; ++i) {
			++stepsSinceLastCheck;
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
				newWork_.reset();
				currentWork_->onLoad();
			}
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

	void Window::setWork(std::shared_ptr<Work> work) {
		if (!currentWork_) {
			debug("setting current work to "); debug(work.get()); debug("\n");
			currentWork_ = std::move(work);
		} else {
			debug("change work to "); debug(work.get()); debug("\n");
			changeWork_ = true;
			newWork_ = std::move(work);
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

	void Window::addUpdateInputCallback(std::function<void()> c) {
		updateInputCallbacks.emplace_back(std::move(c));
	}
} // namespace jngl
