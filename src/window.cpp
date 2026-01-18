// Copyright 2007-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "window.hpp"

#include "FontImpl.hpp"
#include "ShaderCache.hpp"
#include "audio.hpp"
#include "freetype.hpp"
#include "jngl/ScaleablePixels.hpp"
#include "jngl/font.hpp"
#include "jngl/other.hpp"
#include "jngl/time.hpp"
#include "jngl/work.hpp"
#include "log.hpp"
#include "windowptr.hpp"

#ifdef ANDROID
#include "main.hpp"
#endif

#ifdef __EMSCRIPTEN__
#include "emscripten/window.hpp"

#include <emscripten.h>
#endif

#include <algorithm>
#include <gsl/narrow>
#include <ranges>
#include <thread>

namespace jngl {

ScaleablePixels Window::getTextWidth(const std::string& text) {
	return static_cast<ScaleablePixels>(fonts_[fontSize_][fontName_]->getTextWidth(text));
}

double Window::getLineHeight() {
	return fonts_[fontSize_][fontName_]->getLineHeight();
}

void Window::setLineHeight(Pixels h) {
	fonts_[fontSize_][fontName_]->setLineHeight(h);
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
	getFontImpl()->print(ScaleablePixels(xposition), ScaleablePixels(yposition), text);
}

void Window::print(const Mat3& modelview, const std::string& text) {
	getFontImpl()->print(modelview, text, gFontColor);
}

void Window::setFont(const std::string& filename) {
	if (!fonts_[fontSize_].contains(filename)) {
		auto font = std::make_shared<FontImpl>(filename, fontSize_, 0);
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
		setFont(fontName_);       // We changed the size we also need to reload the current font
	} catch (std::exception& e) { // Something went wrong ...
		fontSize_ = oldSize;      // ... so let's set fontSize_ back to the previous size
		throw e;
	}
}

bool Window::getMouseDown(mouse::Button button) {
	return mouseDown_.at(button);
}

bool Window::getMousePressed(mouse::Button button) const {
	return mousePressed_[button];
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

#if defined(ANDROID) || defined(IOS)
int Window::getMouseX() const {
	if (relativeMouseMode) {
		return mousex_;
	}
	return mousex_ - (width_ - canvasWidth) / 2;
}

int Window::getMouseY() const {
	if (relativeMouseMode) {
		return mousey_;
	}
	return mousey_ - (height_ - canvasHeight) / 2;
}
#endif

bool Window::getRelativeMouseMode() const {
	return relativeMouseMode;
}

void Window::increaseMouseHiddenCount() {
	if (mouseHiddenCount == 0) {
		setMouseVisible(false);
	}
	++mouseHiddenCount;
}

void Window::decreaseMouseHiddenCount() {
	assert(mouseHiddenCount > 0);
	--mouseHiddenCount;
	if (mouseHiddenCount == 0) {
		setMouseVisible(true);
	}
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
	return !shouldExit;
}

void Window::quit() noexcept {
	shouldExit = true;
}

void Window::forceQuit(uint8_t exitcode) {
	if (forceExitCode) {
		throw std::runtime_error("Already exitting.");
	}
	forceExitCode = exitcode;
}

void Window::cancelQuit() {
	shouldExit = false;
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
	} else if (keyPressed_[GetKeyCode(key)]) {
		needToBeSetFalse_.push(&keyPressed_[GetKeyCode(key)]);
		return true;
	}
	return false;
}

void Window::setKeyPressed(const key::KeyType key, bool p) {
	keyPressed_[GetKeyCode(key)] = p;
}

void Window::setKeyPressed(const std::string& key, bool p) {
	characterPressed_[key] = p;
	if (p) {
		needToBeSetFalse_.push(&characterPressed_[key]);
	}
}

bool keyDown(const char key) {
	std::string temp;
	temp.append(1, key);
	if (key >= 'a' && key <= 'z') {
		if (keyDown(std::string(1, static_cast<char>(key - 32)))) {
			return true;
		}
	}
	if (key >= 'A' && key <= 'Z') {
		if (keyDown(std::string(1, static_cast<char>(key + 32)))) {
			return true;
		}
	}
	return keyDown(temp);
}

bool keyPressed(const char key) {
	std::string temp;
	temp.append(1, key);
	if (key >= 'a' && key <= 'z') {
		if (keyPressed(std::string(1, static_cast<char>(key - 32)))) {
			return true;
		}
	}
	if (key >= 'A' && key <= 'Z') {
		if (keyPressed(std::string(1, static_cast<char>(key + 32)))) {
			return true;
		}
	}
	return keyPressed(temp);
}

void Window::updateKeyStates() {
	textInput.clear();
	while (!needToBeSetFalse_.empty()) {
		*(needToBeSetFalse_.top()) = false;
		needToBeSetFalse_.pop();
	}
	mouseWheel = 0;
	if (!updateInputCallbacks.empty()) {
		for (auto& updateInputCallback : updateInputCallbacks) {
			updateInputCallback();
		}
		updateInputCallbacks.clear();
	}

	// this is done here, because Window::updateKeyStates() is called by jngl::updateInput() and if
	// someone writes their own main loop, Audio::step() needs to be called. Maybe updateKeyStates()
	// should be renamed to updateInput() and UpdateInput() moved to WindowImpl ...
	if (auto audio = Audio::handleIfAlive()) {
		audio->step();
	}

	mouseInfo.setMousePos(getMousePos());
}

double Window::getMouseWheel() const {
	return mouseWheel;
}

MouseInfo& Window::getMouseInfo() {
	return mouseInfo;
}

MouseInfo& input() {
	return pWindow->getMouseInfo();
}

uint8_t Window::mainLoop() {
#ifdef __EMSCRIPTEN__
	g_jnglMainLoop = [this]() {
#else
	Finally _([&]() {
		newWork_.reset();
		currentWork_.reset();
	});
	while (!shouldExit && !forceExitCode) {
#endif
		stepIfNeeded();
		clearBackBuffer();
		draw();
		pWindow->SwapBuffers();
		sleepIfNeeded();
	}
#ifdef __EMSCRIPTEN__
	;
	emscripten_set_main_loop(jnglMainLoop, 0, true);
	return 0;
#else
	return forceExitCode ? *forceExitCode : 0;
#endif
}

void Window::resetFrameLimiter() {
	frameLimiter = {};
}

void Window::dontSkipNextFrame() {
	frameLimiter.stepsPerFrame = 1;
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
	const auto currentTime = getTime();
	const auto secondsSinceLastCheck = currentTime - frameLimiter.lastCheckTime;
	const auto targetStepsPerSecond = 1.0 / timePerStep;
	// If SPS == FPS, this would mean that we check about every second, but in the beginning we
	// want to check more often, e.g. to quickly adjust to high refresh rate monitors:
	if (frameLimiter.stepsSinceLastCheck > targetStepsPerSecond ||
	    frameLimiter.stepsSinceLastCheck > frameLimiter.numberOfChecks) {
		++frameLimiter.numberOfChecks;
		const auto actualStepsPerSecond = frameLimiter.stepsSinceLastCheck / secondsSinceLastCheck;
		auto doableStepsPerSecond =
		    frameLimiter.stepsSinceLastCheck / (secondsSinceLastCheck - timeSleptSinceLastCheck);
		if (previousStepsPerFrame > frameLimiter.stepsPerFrame &&
		    actualStepsPerSecond < targetStepsPerSecond) {
			frameLimiter.maxFPS =
			    0.5 * frameLimiter.maxFPS + 0.5 * actualStepsPerSecond / frameLimiter.stepsPerFrame;
		} else {
			frameLimiter.maxFPS += sleepPerFrame;
		}
		previousStepsPerFrame = frameLimiter.stepsPerFrame;
		const auto cappedOrDoable =
		    std::min(doableStepsPerSecond, frameLimiter.maxFPS * frameLimiter.stepsPerFrame);

		// The sleep function is actually inaccurate (or at least less accurate than getTime),
		// se we try to find a factor to correct this:
		frameLimiter.sleepCorrectionFactor +=
		    0.1 * // don't change it too fast
		    (sleepPerFrame * frameLimiter.stepsSinceLastCheck / frameLimiter.stepsPerFrame -
		     timeSleptSinceLastCheck);
		//   ↑__________seconds we should have slept___________↑   ↑___actual seconds____↑

		// Clamp it in case of some bug:
		frameLimiter.sleepCorrectionFactor =
		    std::max(0.1, std::min(frameLimiter.sleepCorrectionFactor, 2.0));

		// Round up, because if we can do 40 FPS, but need 60 SPS, we need at least 2 SPF. We
		// don't round up exactly to be a little bit "optimistic" of what we can do.
		auto newStepsPerFrame = std::min(
		    static_cast<unsigned int>(
		        std::max(1, static_cast<int>(0.98 + frameLimiter.stepsPerFrame *
		                                                targetStepsPerSecond / cappedOrDoable))),
		    std::min(frameLimiter.stepsPerFrame * 2, maxStepsPerFrame)); // never increase too much
		// Divide doableStepsPerSecond by the previous stepsPerFrame and multiply it with
		// newStepsPerFrame so that we know what can be doable in the future and not what
		// could have been doable:
		double shouldSleepPerFrame = newStepsPerFrame * // we sleep per frame, not per step
		                             (timePerStep - 1.0 / (newStepsPerFrame * doableStepsPerSecond /
		                                                   frameLimiter.stepsPerFrame));
		if (shouldSleepPerFrame < 0) {
			shouldSleepPerFrame = 0;
		}
		// The factor means that we quickly go down when needed, but hesitate to go up:
		sleepPerFrame += ((shouldSleepPerFrame < sleepPerFrame) ? 0.95 : 0.55) *
		                 (shouldSleepPerFrame - sleepPerFrame);

		internal::trace("SPS: {} ({} {}, should be {}); stepsPerFrame -> {}, msSleepPerFrame -> {} "
		                "* {}, slept({}): {}µs, maxFPS: {}",
		                std::lround(actualStepsPerSecond),
		                (cappedOrDoable < doableStepsPerSecond) ? "capped" : "doable",
		                std::lround(doableStepsPerSecond), std::lround(targetStepsPerSecond),
		                newStepsPerFrame, sleepPerFrame, frameLimiter.sleepCorrectionFactor,
		                numberOfSleeps, std::lround(1e6 * timeSleptSinceLastCheck),
		                frameLimiter.maxFPS);

		frameLimiter.lastCheckTime = currentTime;
		numberOfSleeps = 0;
		frameLimiter.stepsSinceLastCheck = 0;
		timeSleptSinceLastCheck = 0;
		frameLimiter.stepsPerFrame = newStepsPerFrame;
	}
	for (unsigned int i = 0; i < frameLimiter.stepsPerFrame; ++i) {
		++frameLimiter.stepsSinceLastCheck;
		updateKeyStates();
		UpdateInput();
#ifdef JNGL_PERFORMANCE_OVERLAY
		auto start = std::chrono::steady_clock::now();
#endif

		for (const auto& job : jobs) {
			job->step();
		}
		for (auto& job : jobsToAdd) {
			job->step();
			jobs.emplace_back(std::move(job));
		}
		jobsToAdd.clear();

		for (auto job : jobsToRemove) {
			const auto it = std::find_if(jobs.begin(), jobs.end(),
			                             [job](const auto& p) { return p.get() == job; });
			if (it != jobs.end()) {
				jobs.erase(it);
			}
		}
		jobsToRemove.clear();
		if (currentWork_) {
			currentWork_->step();
		}
#ifdef JNGL_PERFORMANCE_OVERLAY
		lastStepDuration = static_cast<double>(
			std::chrono::duration_cast<std::chrono::microseconds>(
				std::chrono::steady_clock::now() - start
			).count()
		) / 1000.;
#endif
		if (forceExitCode) {
			break;
		}
		if (keyPressed(key::Escape)) {
			currentWork_->onBackEvent();
		}
		if (shouldExit && currentWork_) {
			currentWork_->onQuitEvent();
		}
		while (!shouldExit && changeWork) {
			changeWork = false;
			if (currentWork_) {
				currentWork_->onUnload();
			}
			currentWork_ = std::move(newWork_);
			currentWork_->onLoad();
			if (shouldExit) {
				currentWork_->onQuitEvent();
			}
		}
	}
}

void Window::sleepIfNeeded() {
	const auto start = getTime();
	const auto shouldBe =
	    frameLimiter.lastCheckTime + timePerStep * frameLimiter.stepsSinceLastCheck;
	const int64_t micros = std::lround((sleepPerFrame - (start - shouldBe)) *
	                                   frameLimiter.sleepCorrectionFactor * 1e6);
	if (micros > 0) {
		std::this_thread::sleep_for(std::chrono::microseconds(micros));
		timeSleptSinceLastCheck += jngl::getTime() - start;
		++numberOfSleeps;
	}
}

void Window::draw() const {
#ifdef JNGL_PERFORMANCE_OVERLAY
	auto start = std::chrono::steady_clock::now();
#endif
	if (currentWork_) {
		currentWork_->draw();
	} else {
		jngl::print("No scene set. Use jngl::setScene", -50, -5);
	}
	for (auto& job : std::ranges::reverse_view(jobs)) {
		job->draw();
	}
#ifdef JNGL_PERFORMANCE_OVERLAY
	auto us = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start);

	if (currentWork_) {
		jngl::reset();
		jngl::setColor(0xffffff_rgb, 255);
		jngl::drawRect(-getScreenSize() / 2., jngl::Vec2(400, 100));
		jngl::setFontColor(0x000000_rgb, 1.f);
		{
			std::ostringstream tmp;
			tmp << "step: " << lastStepDuration << " ms";
			jngl::print(tmp.str(), -getScreenSize() / 2. + jngl::Vec2(50, 10));
		}
		{
			std::ostringstream tmp;
			tmp << "draw: " << static_cast<double>(us.count()) / 1000. << " ms";
			jngl::print(tmp.str(), -getScreenSize() / 2. + jngl::Vec2(50, 60));
		}
	}
#endif
}

std::string simpleDemangle(std::string_view mangled) {
	// Simple demangler: strip leading 'N' and trailing 'E' for namespaces, remove digits, etc.
	// This is a naive implementation and won't handle all cases.
	std::string result;
	size_t i = 0;

	// Remove leading 'N' (namespace) and trailing 'E'
	if (!mangled.empty() && mangled[0] == 'N' && mangled.back() == 'E') {
		++i;
		mangled.remove_suffix(1);
	}

	while (i < mangled.size()) {
		if (std::isdigit(mangled[i]) != 0) {
			// Skip length prefixes
			size_t len = 0;
			while (i < mangled.size() && std::isdigit(mangled[i]) != 0) {
				len = len * 10 + (mangled[i] - '0');
				++i;
			}
			if (i + len <= mangled.size()) {
				if (!result.empty()) {
					result += "::";
				}
				result.append(mangled.substr(i, len));
				i += len;
			} else {
				break;
			}
		} else {
			// Copy non-digit characters as-is
			result += mangled[i++];
		}
	}
	return result.empty() ? std::string(mangled) : result;
}

void Window::setWork(std::shared_ptr<Scene> work) {
	if (work == currentWork_) {
		if (changeWork) {
			changeWork = false;
			newWork_.reset();
			internal::debug("Discarding scene change.");
		}
		return;
	}
#ifndef NDEBUG
	const auto& ref = *work;
	internal::debug("{} scene to {} ({}).", currentWork_ ? "Change" : "Setting current",
	                simpleDemangle(typeid(ref).name()), // NOLINT
	                static_cast<void*>(work.get()));
#endif
	if (!currentWork_) {
		currentWork_ = std::move(work);
		currentWork_->onLoad();
	} else {
		changeWork = true;
		newWork_ = std::move(work);
	}
}

void Window::addJob(std::shared_ptr<Job> job) {
	jobsToAdd.emplace_back(std::move(job));
}

void Window::removeJob(Job* job) {
	jobsToRemove.emplace_back(job);
}

std::shared_ptr<Job> Window::getJob(const std::function<bool(Job&)>& predicate) const {
	for (const auto* const container : { &jobs, &jobsToAdd }) {
		for (const auto& job : *container) {
			if (job && predicate(*job)) {
				return job;
			}
		}
	}
	return nullptr;
}

std::shared_ptr<Work> Window::getWork() {
	return currentWork_;
}

std::shared_ptr<Scene> Window::getNextScene() const {
	assert(!changeWork || newWork_);
	return newWork_;
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
		canvasHeight =
		    gsl::narrow<int>(std::lround(static_cast<float>(minAspectRatio.second * width_) /
		                                 static_cast<float>(minAspectRatio.first)));
	} else if (maxAspectRatio.first * height_ < maxAspectRatio.second * width_) {
		// Are we above the maximal aspect ratio? -> Letterboxing at the left and right
		canvasWidth =
		    gsl::narrow<int>(std::lround(static_cast<float>(maxAspectRatio.first * height_) /
		                                 static_cast<float>(maxAspectRatio.second)));
	}
	if (canvasWidth != width_ || canvasHeight != height_) {
		internal::debug("Letterboxing to {}x{}.", canvasWidth, canvasHeight);
	}
}

void Window::initGlObjects() {
#ifdef ANDROID
	Init(width_, height_, canvasWidth, canvasHeight);
#endif
	glGenBuffers(1, &opengl::vboStream);
	glGenVertexArrays(1, &opengl::vaoStream);

	glGenVertexArrays(1, &vaoLine);
	glBindVertexArray(vaoLine);
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	const static float line[] = { 0, 0, 1, 1 };
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);

	glGenVertexArrays(1, &vaoSquare);
	glBindVertexArray(vaoSquare);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	const static float rect[] = { -.5, -.5, .5, -.5, .5, .5, -.5, .5 };
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);

	GLint tmp;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &tmp);
	systemFramebuffer = tmp;
	glGetIntegerv(GL_RENDERBUFFER_BINDING, &tmp);
	systemRenderbuffer = tmp;
}

void Window::drawLine(Mat3 modelview, const Vec2 b, const Rgba color) const {
	glBindVertexArray(vaoLine);
	auto tmp = ShaderCache::handle().useSimpleShaderProgram(modelview.scale(b), color);
	glDrawArrays(GL_LINES, 0, 2);
}

void Window::drawSquare(const Mat3& modelview, Rgba color) const {
	glBindVertexArray(vaoSquare);
	auto context = ShaderCache::handle().useSimpleShaderProgram(modelview, color);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void Window::onControllerChanged(std::function<void()> callback) {
	controllerChangedCallback = std::move(callback);
}

void Window::bindSystemFramebufferAndRenderbuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, systemFramebuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, systemRenderbuffer);
}

} // namespace jngl
