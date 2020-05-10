// Copyright 2007-2019 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "window.hpp"

#include "freetype.hpp"
#include "jngl.hpp"
#include "main.hpp"

#include <boost/numeric/conversion/cast.hpp>
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
		const auto itFontsMap = fonts_.find(fontSize_);
		if (itFontsMap != fonts_.end()) {
			const auto itFont = itFontsMap->second.find(fontName_);
			if (itFont != itFontsMap->second.end()) {
				return itFont->second;
			}
		}
		setFont(fontName_);
		return getFontImpl();
	}

	void Window::print(const std::string& text, const int xposition, const int yposition) {
		getFontImpl()->print(xposition, yposition, text);
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

#ifndef _WIN32
	int Window::getMouseX() const {
		if (relativeMouseMode) { return mousex_; }
		return mousex_ - (width_ - canvasWidth) / 2;
	}

	int Window::getMouseY() const {
		if (relativeMouseMode) { return mousey_; }
		return mousey_ - (height_ - canvasHeight) / 2;
	}
#endif

	bool Window::getRelativeMouseMode() const {
		return relativeMouseMode;
	}

	int Window::getCanvasWidth() const {
		return canvasWidth;
	}

	int Window::getCanvasHeight() const {
		return canvasHeight;
	}

	int Window::getWidth() const {
		return width_;
	}

	int Window::getHeight() const {
		return height_;
	}

	bool Window::isMultisampleSupported() const {
		return isMultisampleSupported_;
	}

	bool Window::isRunning() const {
		return running;
	}

	void Window::quit() {
		running = false;
	}

	void Window::cancelQuit() {
		running = true;
	}

	bool Window::getKeyDown(key::KeyType key) {
		if (key == key::Any) {
			for (const auto& it : keyDown_) {
				if (it.second) {
					return true;
				}
			}
			for (const auto& it : characterDown_) {
				if (it.second) {
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
		textInput.clear();
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
		while (isRunning()) {
			stepIfNeeded();
			draw();
			jngl::swapBuffers();
		}
	}

	void Window::resetFrameLimiter() {
		numberOfChecks = 0;
		stepsPerFrame = 1;
		sleepCorrectionFactor = 1;
		maxFPS = 300;
	}

	unsigned int Window::getStepsPerSecond() const {
		return static_cast<unsigned int>(1.0 / timePerStep);
	}

	void Window::setStepsPerSecond(const unsigned int stepsPerSecond) {
		timePerStep = 1.0 / static_cast<double>(stepsPerSecond);
		maxStepsPerFrame = static_cast<unsigned int>(
		    std::lround(1.0 / 20.0 / timePerStep)); // Never drop below 20 FPS, instead slow down
	}

	void Window::stepIfNeeded() {
		// TODO: These variables are static, but should rather be members of Window:
		static auto lastCheckTime = jngl::getTime();
		static unsigned int stepsSinceLastCheck = 0;

		const auto currentTime = jngl::getTime();
		const auto secondsSinceLastCheck = currentTime - lastCheckTime;
		const auto targetStepsPerSecond = 1.0 / timePerStep;
		// If SPS == FPS, this would mean that we check about every second, but in the beginning we
		// want to check more often, e.g. to quickly adjust to high refresh rate monitors:
		if (stepsSinceLastCheck > targetStepsPerSecond || stepsSinceLastCheck > numberOfChecks) {
			++numberOfChecks;
			const auto actualStepsPerSecond = stepsSinceLastCheck / secondsSinceLastCheck;
			auto doableStepsPerSecond =
			    stepsSinceLastCheck /
			    (secondsSinceLastCheck - timeSleptSinceLastCheck);
			if (previousStepsPerFrame > stepsPerFrame &&
			    actualStepsPerSecond < targetStepsPerSecond) {
				maxFPS = 0.5 * maxFPS + 0.5 * actualStepsPerSecond / stepsPerFrame;
			} else {
				maxFPS += sleepPerFrame;
			}
			previousStepsPerFrame = stepsPerFrame;
			const auto cappedOrDoable = std::min(doableStepsPerSecond, maxFPS * stepsPerFrame);
			// TODO: Improve logging. Log level? jngl::trace?
			jngl::debug("SPS: ");
			jngl::debug(std::lround(actualStepsPerSecond));
			if (cappedOrDoable < doableStepsPerSecond) {
				jngl::debug(" (capped ");
			} else {
				jngl::debug(" (doable ");
			}
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
			    int(0.98 + stepsPerFrame * targetStepsPerSecond / cappedOrDoable))),
				std::min(stepsPerFrame * 2, maxStepsPerFrame)); // never increase too much
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
			jngl::debug(sleepCorrectionFactor);
			jngl::debug(", slept("); jngl::debug(numberOfSleeps); jngl::debug("): ");
			jngl::debug(std::lround(1e6 * timeSleptSinceLastCheck));
			jngl::debug("µs, maxFPS: ");
			jngl::debugLn(maxFPS);
			lastCheckTime = currentTime;
			numberOfSleeps = 0;
			stepsSinceLastCheck = 0;
			timeSleptSinceLastCheck = 0;
			stepsPerFrame = newStepsPerFrame;
		}
		const auto start = getTime();
		const auto shouldBe = lastCheckTime + timePerStep * stepsSinceLastCheck;
		const int64_t micros =
		    std::lround((sleepPerFrame - (start - shouldBe)) * sleepCorrectionFactor * 1e6);
		if (micros > 0) {
			std::this_thread::sleep_for(std::chrono::microseconds(micros));
			timeSleptSinceLastCheck += jngl::getTime() - start;
			++numberOfSleeps;
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
			while (changeWork) {
				changeWork = false;
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
			changeWork = true;
			newWork_ = std::move(work);
		}
	}

	void Window::addJob(std::shared_ptr<Job> job) {
		jobs.emplace_back(std::move(job));
	}

	std::shared_ptr<Work> Window::getWork() {
		return currentWork_;
	}

	bool Window::isMultitouch() const {
		return multitouch;
	}

#if !defined(ANDROID) && !defined(IOS)
	std::vector<Vec2> Window::getTouchPositions() const { // NOLINT
		return {};
	}
#endif

	void Window::addUpdateInputCallback(std::function<void()> c) {
		updateInputCallbacks.emplace_back(std::move(c));
	}

	std::string Window::getTextInput() const {
		return textInput;
	}

	void Window::calculateCanvasSize(const std::pair<int, int> minAspectRatio,
	                                 const std::pair<int, int> maxAspectRatio) {
		canvasWidth = width_;
		canvasHeight = height_;
		if (minAspectRatio.first * height_ > minAspectRatio.second * width_) {
			// Are we below the minimal aspect ratio? -> Letterboxing at the top and bottom
			canvasHeight = boost::numeric_cast<int>(
			    std::lround(float(minAspectRatio.second * width_) / float(minAspectRatio.first)));
		} else if (maxAspectRatio.first * height_ < maxAspectRatio.second * width_) {
			// Are we above the maximal aspect ratio? -> Letterboxing at the left and right
			canvasWidth = boost::numeric_cast<int>(
			    std::lround(float(maxAspectRatio.first * height_) / float(maxAspectRatio.second)));
		}
		if (canvasWidth != width_ || canvasHeight != height_) {
			debug("Letterboxing to "); debug(canvasWidth); debug("x"); debugLn(canvasHeight);
		}
	}

	void Window::initGlObjects() {
		glGenBuffers(1, &opengl::vboStream);
		glGenVertexArrays(1, &opengl::vaoStream);

		glGenVertexArrays(1, &vaoLine);
		glBindVertexArray(vaoLine);
		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		const static std::array<float, 4> line{ 0, 0, 1, 1 };
		glBufferData(GL_ARRAY_BUFFER, sizeof(line), &line[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);

		glGenVertexArrays(1, &vaoRect);
		glBindVertexArray(vaoRect);
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		const static float rect[] = { 0, 0, 1, 0, 1, 1, 0, 1 };
		glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);
	}

	void Window::drawTriangle(const Vec2 a, const Vec2 b, const Vec2 c) {
		glBindVertexArray(opengl::vaoStream);
		auto tmp = useSimpleShaderProgram();
		const float vertexes[] = { static_cast<float>(a.x * jngl::getScaleFactor()),
		                           static_cast<float>(a.y * jngl::getScaleFactor()),
		                           static_cast<float>(b.x * jngl::getScaleFactor()),
		                           static_cast<float>(b.y * jngl::getScaleFactor()),
		                           static_cast<float>(c.x * jngl::getScaleFactor()),
		                           static_cast<float>(c.y * jngl::getScaleFactor()) };
		glBindBuffer(GL_ARRAY_BUFFER, opengl::vboStream); // VAO does NOT save the VBO binding
		// STREAM because we're using the buffer only once
		glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), vertexes, GL_STREAM_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

	void Window::drawEllipse(const Vec2 mid, const Vec2 size, float startAngle) {
		glBindVertexArray(opengl::vaoStream);
		jngl::pushMatrix();
		jngl::translate(mid);
		opengl::scale(static_cast<float>(jngl::getScaleFactor()),
		              static_cast<float>(jngl::getScaleFactor()));
		auto tmp = jngl::useSimpleShaderProgram();
		std::vector<float> vertexes;
		vertexes.push_back(0.f);
		vertexes.push_back(0.f);
		for (float t = startAngle; t < 2.f * M_PI; t += 0.1f) {
			vertexes.push_back(size.x * std::sin(t));
			vertexes.push_back(-size.y * std::cos(t));
		}
		vertexes.push_back(0.f);
		vertexes.push_back(-size.y);
		glBindBuffer(GL_ARRAY_BUFFER, opengl::vboStream); // VAO does NOT save the VBO binding
		glBufferData(GL_ARRAY_BUFFER, vertexes.size() * sizeof(float) * 2, &vertexes[0],
		             GL_STREAM_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
		glDrawArrays(GL_TRIANGLE_FAN, 0, static_cast<GLsizei>(vertexes.size() / 2));
		jngl::popMatrix();
	}

	void Window::drawLine(const Vec2 a, const Vec2 b) const {
		glBindVertexArray(vaoLine);
		jngl::pushMatrix();
		jngl::translate(a);
		opengl::scale(static_cast<float>((b.x - a.x) * jngl::getScaleFactor()),
		              static_cast<float>((b.y - a.y) * jngl::getScaleFactor()));
		auto tmp = useSimpleShaderProgram();
		glDrawArrays(GL_LINES, 0, 2);
		jngl::popMatrix();
	}

	void Window::drawRect(const Vec2 pos, const Vec2 size) const {
		glBindVertexArray(vaoRect);
		jngl::pushMatrix();
		jngl::translate(pos);
		opengl::scale(static_cast<float>(size.x * jngl::getScaleFactor()),
		              static_cast<float>(size.y * jngl::getScaleFactor()));
		auto tmp = useSimpleShaderProgram();
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		jngl::popMatrix();
	}

	void Window::onControllerChanged(std::function<void()> callback) {
		controllerChangedCallback = std::move(callback);
	}

} // namespace jngl
