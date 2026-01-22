// Copyright 2007-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "main.hpp"

#include "App.hpp"
#include "ShaderCache.hpp"
#include "jngl/Alpha.hpp"
#include "jngl/ScaleablePixels.hpp"
#include "jngl/Scene.hpp"
#include "jngl/matrix.hpp"
#include "jngl/other.hpp"
#include "jngl/screen.hpp"
#include "jngl/shapes.hpp"
#include "jngl/time.hpp"
#include "jngl/window.hpp"
#include "log.hpp"
#include "paths.hpp"
#include "spriteimpl.hpp"
#include "windowptr.hpp"

#include <boost/qvm_lite.hpp>
#include <cstddef>
#include <fstream>
#include <numbers>
#include <sstream>
#include <stack>

#ifdef _WIN32
#include <windows.h>
#endif

#if defined(_WIN32) || (!defined(ANDROID) && __has_include(<filesystem>))
#include <filesystem>
#define HAVE_FILESYSTEM
#endif

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

#ifdef ANDROID
#include "android/fopen.hpp"

#include <sys/stat.h>
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace jngl {

std::string pathPrefix;
optional<std::string> configPath;
Rgb backgroundColor(1, 1, 1);
std::stack<jngl::Mat3> modelviewStack;

void clearBackgroundColor() {
	glClearColor(backgroundColor.getRed(), backgroundColor.getGreen(), backgroundColor.getBlue(),
	             1);
}

void updateViewportAndLetterboxing(const int width, const int height, const int canvasWidth,
                                   const int canvasHeight) {
	glViewport(0, 0, width, height);

	if (canvasWidth != width || canvasHeight != height) { // Letterboxing?
		glClearColor(0, 0, 0, 1); // black boxes
		glClear(GL_COLOR_BUFFER_BIT);

		glEnable(GL_SCISSOR_TEST);
		assert(canvasWidth <= width);
		assert(canvasHeight <= height);
		glScissor((width - canvasWidth) / 2, (height - canvasHeight) / 2, canvasWidth,
		          canvasHeight);
	} else {
		glDisable(GL_SCISSOR_TEST);
	}
}

void updateProjection(int windowWidth, int windowHeight, float originalWindowWidth,
                      float originalWindowHeight) {
	internal::trace("Updating projection matrix to {}x{} (original size: {}x{})", windowWidth,
	                windowHeight, std::lround(originalWindowWidth),
	                std::lround(originalWindowHeight));
	const auto l = static_cast<float>(-windowWidth) / 2.f;
	const auto r = static_cast<float>(windowWidth) / 2.f;
	const auto b = static_cast<float>(windowHeight) / 2.f;
	const auto t = static_cast<float>(-windowHeight) / 2.f;
	const auto scale = static_cast<float>(getScaleFactor());
	opengl::projection = {
		static_cast<float>(windowWidth) / originalWindowWidth * 2.f / (r - l) * scale,
		0.f,
		0.f,
		-(r + l) / (r - l),
		0.f,
		static_cast<float>(windowHeight) / originalWindowHeight * 2.f / (t - b) * scale,
		0.f,
		-(t + b) / (t - b),
		0.f,
		0.f,
		-1.f,
		0.f,
		0.f,
		0.f,
		0.f,
		1.f,
	};
}

WindowPointer pWindow;
namespace {
bool antiAliasingEnabled = true;
} // namespace

void showWindow(const std::string& title, const double width, const double height, bool fullscreen,
                const std::pair<int, int> minAspectRatio,
                const std::pair<int, int> maxAspectRatio) {
	++internal::gFrameNumber; // will set it to 0 for the first window
	internal::debug("jngl::showWindow(\"{}\", {}, {}, {});", title, width, height, fullscreen);
	bool isMouseVisible = pWindow ? pWindow->getMouseVisible() : true;
	hideWindow();
	int widthRounded = static_cast<int>(std::lround(width));
	int heightRounded = static_cast<int>(std::lround(height));
	if (widthRounded == 0) {
		throw std::runtime_error("Width Is 0");
	}
	if (heightRounded == 0) {
		throw std::runtime_error("Height Is 0");
	}
	pWindow.Set(
	    new Window(title, widthRounded, heightRounded, fullscreen, minAspectRatio, maxAspectRatio));
	if (App::instance().getDisplayName().empty()) {
		App::instance().setDisplayName(title);
	}
	pWindow->SetMouseVisible(isMouseVisible);
	setAntiAliasing(antiAliasingEnabled);
	pWindow->initGlObjects();
}

void hideWindow() {
	if (pWindow) {
		App::instance().callAtExitFunctions();
	}
	unloadAll();
	pWindow.Delete();
}

void atExit(std::function<void()> f) {
	App::instance().atExit(std::move(f));
}

void swapBuffers() {
	pWindow->SwapBuffers();
	clearBackBuffer();
}

void clearBackBuffer() {
	if (glIsEnabled(GL_SCISSOR_TEST)) {
		// Letterboxing with SDL_VIDEODRIVER=wayland will glitch if we don't draw the black boxes on
		// every frame
		glDisable(GL_SCISSOR_TEST);
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		glEnable(GL_SCISSOR_TEST);
		clearBackgroundColor();
	}

	reset();
	if (!modelviewStack.empty()) {
		internal::error("Uneven calls to push/popMatrix at the beginning of the frame!");
	}
	modelviewStack = {};
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void updateInput() {
	pWindow->updateKeyStates();
	pWindow->UpdateInput();
}

bool running() {
	return pWindow->isRunning();
}

bool canQuit() {
#if defined(IOS) || defined(EMSCRIPTEN)
	return false;
#else
	return true;
#endif
}

void quit() noexcept {
	if (!canQuit()) {
		internal::info("Quitting the main loop is not supported on this patform!");
		return;
	}
	if (const auto w = pWindow.get()) {
		w->quit();
	}
}

void forceQuit(const uint8_t exitcode) {
	if (!canQuit()) {
		throw std::runtime_error("Quitting the main loop is not supported on this platform!");
	}
	pWindow->forceQuit(exitcode);
}

void cancelQuit() {
	if (pWindow) {
		pWindow->cancelQuit();
	}
}

Rgb getBackgroundColor() {
	return backgroundColor;
}

void setBackgroundColor(const jngl::Rgb color) {
	pWindow.ThrowIfNull();
	backgroundColor = color;
	clearBackgroundColor();
	glClear(GL_COLOR_BUFFER_BIT);
}

void setBackgroundColor(const unsigned char red, const unsigned char green,
                        const unsigned char blue) {
	setBackgroundColor(Rgb::u8(red, green, blue));
}

Vec2 getMousePos() {
	if (getRelativeMouseMode()) {
		return { pWindow->getMouseX() / getScaleFactor(), pWindow->getMouseY() / getScaleFactor() };
	}
	return { pWindow->getMouseX() / getScaleFactor() - getScreenWidth() / 2,
		     pWindow->getMouseY() / getScaleFactor() - getScreenHeight() / 2 };
}

optional<Vec2> getCursorPos() {
	// TODO: This is a very naive approach and ignores Windows devices with touch screens or Android
	// devices with mice.
#if defined(IOS) || defined(ANDROID)
	return {};
#else
	return Vec2(pWindow->getMouseX() / getScaleFactor() - getScreenWidth() / 2,
	            pWindow->getMouseY() / getScaleFactor() - getScreenHeight() / 2);
#endif
}

int getMouseX() {
	return pWindow->getMouseX();
}

int getMouseY() {
	return pWindow->getMouseY();
}

bool keyDown(const key::KeyType key) {
	if (key == key::Ctrl) {
		return keyDown(key::ControlL) || keyDown(key::ControlR);
	}
	if (key == key::Shift) {
		return keyDown(key::ShiftL) || keyDown(key::ShiftR);
	}
	return pWindow->getKeyDown(key);
}

bool keyPressed(const key::KeyType key) {
	if (key == key::Ctrl) {
		return keyPressed(key::ControlL) || keyPressed(key::ControlR);
	}
	if (key == key::Shift) {
		return keyPressed(key::ShiftL) || keyPressed(key::ShiftR);
	}
	return pWindow->getKeyPressed(key);
}

bool keyDown(const std::string& key) {
	const static auto TOO_LONG = "Only pass one character.";
	if ((key[0] & 0x80) != 0) { // first bit (Check if this is an Unicode character)
		// sourceEnd has to be the next character after the utf-8 sequence
		const static auto ERROR_MSG = "Invalid UTF-8 string!";
		if (key.size() < 2) {
			throw std::runtime_error(ERROR_MSG);
		}
		if ((key[0] & 0x20) != 0) { // third bit
			if (key.size() < 3) {
				throw std::runtime_error(ERROR_MSG);
			}
			if ((key[0] & 0x10) != 0) {
				if (key.size() < 4) { // fourth bit
					throw std::runtime_error(ERROR_MSG);
				}
				if (key.size() > 4) {
					throw std::runtime_error(TOO_LONG);
				}
			} else if (key.size() > 3) {
				throw std::runtime_error(TOO_LONG);
			}
		} else if (key.size() > 2) {
			throw std::runtime_error(TOO_LONG);
		}
	} else if (key.size() > 1) {
		throw std::runtime_error(TOO_LONG);
	}
	return pWindow->getKeyDown(key);
}

bool keyPressed(const std::string& key) {
	return pWindow->getKeyPressed(key);
}

bool mouseDown(mouse::Button button) {
	return pWindow->getMouseDown(button);
}

bool mousePressed(mouse::Button button) {
	return pWindow->getMousePressed(button);
}

void setMouse(const jngl::Vec2 position) {
	pWindow->SetMouse(
	    static_cast<int>(std::lround(position.x * getScaleFactor() + pWindow->getWidth() / 2.)),
	    static_cast<int>(std::lround(position.y * getScaleFactor() + pWindow->getHeight() / 2.)));
}

void setRelativeMouseMode(const bool relative) {
	pWindow->SetRelativeMouseMode(relative);
}

void setMouseVisible(const bool visible) {
	pWindow->SetMouseVisible(visible);
}

bool isMouseVisible() {
	return pWindow->getMouseVisible();
}

bool getRelativeMouseMode() {
	return pWindow->getRelativeMouseMode();
}

void setTitle(const std::string& title) {
	pWindow->SetTitle(title);
}

namespace {
void readPixels(void* buffer, GLenum type) {
	auto xOffset = (pWindow->getWidth() - pWindow->getCanvasWidth());
	auto yOffset = (pWindow->getHeight() - pWindow->getCanvasHeight());
	assert(xOffset % 2 == 0);
	assert(yOffset % 2 == 0);
	GLint oldPackAlignment = 0;
	glGetIntegerv(GL_PACK_ALIGNMENT, &oldPackAlignment);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(xOffset / 2, yOffset / 2, pWindow->getCanvasWidth(), pWindow->getCanvasHeight(),
	             GL_RGB, type, buffer);
	glPixelStorei(GL_PACK_ALIGNMENT, oldPackAlignment);
}
} // namespace

std::vector<float> readPixels() {
	std::vector<float> buffer(
	    static_cast<size_t>(3 * pWindow->getCanvasWidth() * pWindow->getCanvasHeight()));
	readPixels(buffer.data(), GL_FLOAT);
	return buffer;
}

void readPixels(uint8_t* buffer) {
	readPixels(buffer, GL_UNSIGNED_BYTE);
}

double getTextWidth(const std::string& text) {
	return static_cast<double>(pWindow->getTextWidth(text));
}

double getLineHeight() {
	return pWindow->getLineHeight();
}

void setLineHeight(double h) {
	pWindow->setLineHeight(Pixels(ScaleablePixels(h)));
}

void print(const std::string& text, const Vec2 position) {
	pWindow->print(text, static_cast<int>(std::lround(position.x)),
	               static_cast<int>(std::lround(position.y)));
}

void print(const std::string& text, const int xposition, const int yposition) {
	pWindow->print(text, xposition, yposition);
}

void print(const Mat3& modelview, const std::string& text) {
	pWindow->print(modelview, text);
}

int getFontSize() {
	return pWindow->getFontSize();
}

void setFontSize(const int size) {
	pWindow->setFontSize(size);
}

std::string getFont() {
	return pWindow->getFont();
}

void setFont(const std::string& filename) {
	pWindow->setFont(filename);
}

void setFontByName(const std::string& name) {
	pWindow->setFontByName(name);
}

void setIcon(const std::string& filename) {
	pWindow->SetIcon(filename);
}

double getFPS() {
	static double lastDraw = 0.0;
	double fps = 1 / (jngl::getTime() - lastDraw);
	lastDraw = jngl::getTime();
	return fps;
}

unsigned int getStepsPerSecond() {
	return pWindow->getStepsPerSecond();
}

void setStepsPerSecond(const unsigned int stepsPerSecond) {
	pWindow->setStepsPerSecond(stepsPerSecond);
}

Mat3 modelview() {
	return opengl::modelview;
}

void reset() {
	boost::qvm::set_identity(opengl::modelview);
}

void rotate(const double degree) {
	boost::qvm::rotate_z(opengl::modelview, degree * std::numbers::pi / 180.);
}

void translate(const double x, const double y) {
	opengl::translate(static_cast<float>(x), static_cast<float>(y));
}

void scale(const double factor) {
	opengl::scale(static_cast<float>(factor), static_cast<float>(factor));
}

void scale(const double xfactor, const double yfactor) {
	opengl::scale(static_cast<float>(xfactor), static_cast<float>(yfactor));
}

void pushMatrix() {
	modelviewStack.push(opengl::modelview);
}

void popMatrix() {
	opengl::modelview = modelviewStack.top();
	modelviewStack.pop();
}

void drawRect(const double xposition, const double yposition, const double width,
              const double height) {
	drawRect(Vec2{ xposition, yposition }, { width, height });
}

void drawRect(const Vec2 position, const Vec2 size) {
	pWindow->drawSquare(modelview().translate(position + size / 2).scale(size), gShapeColor);
}

void drawRect(const Mat3& modelview, const Vec2 size, const Rgb color) {
	drawRect(modelview, size, Rgba(color, Alpha(gShapeColor.getAlpha())));
}

void drawRect(Mat3 modelview, const Vec2 size, const Rgba color) {
	pWindow->drawSquare(modelview.translate(size / 2).scale(size), color);
}

void drawSquare(const Mat3& modelview, Rgba color) {
	pWindow->drawSquare(modelview, color);
}

void drawRectOutline(Mat3 modelview, Vec2 size, float lineWidth, Rgba color) {
	drawLine(modelview.translate({ -size.x / 2., -size.y / 2. }), { size.x, 0 }, lineWidth, color);
	drawLine(modelview, { 0, size.y }, lineWidth, color);
	drawLine(modelview.translate(size), { 0, -size.y }, lineWidth, color);
	drawLine(modelview, { -size.x, 0 }, lineWidth, color);
}

void drawSquareOutline(Mat3 modelview, float lineWidth, Rgba color) {
	drawLine(modelview.translate({ -.5, -.5 }), { 1, 0 }, lineWidth / modelview.data[4], color);
	drawLine(modelview, { 0, 1 }, lineWidth / modelview.data[0], color);
	drawLine(modelview.translate({ 1, 1 }), { 0, -1 }, lineWidth / modelview.data[0], color);
	drawLine(modelview, { -1, 0 }, lineWidth / modelview.data[4], color);
}

void drawTriangle(const Vec2 a, const Vec2 b, const Vec2 c) {
	ShaderCache::handle().drawTriangle(a, b, c);
}

void drawTriangle(const double A_x, const double A_y, const double B_x, const double B_y,
                  const double C_x, const double C_y) {
	ShaderCache::handle().drawTriangle({ A_x, A_y }, { B_x, B_y }, { C_x, C_y });
}

void drawTriangle(Mat3 modelview, Rgba color) {
	ShaderCache::handle().drawTriangle(modelview, color);
}

void setLineWidth(const float width) {
	glLineWidth(width * getScaleFactor());
}

void drawLine(const double xstart, const double ystart, const double xend, const double yend) {
	pWindow->drawLine(jngl::modelview().translate(jngl::Vec2(xstart, ystart)),
	                  jngl::Vec2(xend, yend) - jngl::Vec2(xstart, ystart), gShapeColor);
}

void drawLine(const Vec2 start, const Vec2 end) {
	pWindow->drawLine(jngl::modelview().translate(start), end - start, gShapeColor);
}

void drawLine(const Vec2 start, const Vec2 end, float lineWidth) {
	drawLine(jngl::modelview().translate(start), end - start, lineWidth, gShapeColor);
}

void drawLine(const Vec2 start, const Vec2 end, float lineWidth, Rgba color) {
	drawLine(jngl::modelview().translate(start), end - start, lineWidth, color);
}

void drawLine(Mat3 modelview, const Vec2 start, const Vec2 end) {
	pWindow->drawLine(modelview.translate(start), end - start, gShapeColor);
}

void drawLine(Mat3 modelview, const Vec2 start, const Vec2 end, float lineWidth) {
	drawLine(modelview.translate(start), end - start, lineWidth, gShapeColor);
}

void drawLine(Mat3 modelview, const Vec2 start, const Vec2 end, float lineWidth, Rgba color) {
	drawLine(modelview.translate(start), end - start, lineWidth, color);
}

void drawLine(const Mat3& modelview, const Vec2 end) {
	pWindow->drawLine(modelview, end, gShapeColor);
}

void drawLine(const Mat3& modelview, const Vec2 end, Rgba color) {
	pWindow->drawLine(modelview, end, color);
}

void drawLine(Mat3 modelview, const Vec2 end, float lineWidth, Rgba color) {
	if (end.isNull()) {
		return;
	}
	pWindow->drawSquare(modelview.rotate(std::atan2(end.x, -end.y))
	                        .scale(lineWidth, boost::qvm::mag(end))
	                        .translate({ 0, -0.5 }),
	                    color);
}

void drawPoint(const double x, const double y) {
	drawEllipse(modelview().translate({ x, y }), 1, 1, 0);
}

int getWindowWidth() {
	return pWindow->getCanvasWidth();
}

int getWindowHeight() {
	return pWindow->getCanvasHeight();
}

std::array<Pixels, 2> getWindowSize() {
	return { Pixels(getWindowWidth()), Pixels(getWindowHeight()) };
}

double getMouseWheel() {
	return pWindow->getMouseWheel();
}

void setAntiAliasing(bool enabled) {
#ifdef GL_MULTISAMPLE_ARB
	if (!pWindow->isMultisampleSupported()) {
		antiAliasingEnabled = false;
		return;
	}
	if (enabled) {
		glEnable(GL_MULTISAMPLE_ARB);
	} else {
		glDisable(GL_MULTISAMPLE_ARB);
	}
	antiAliasingEnabled = enabled;
#else
	internal::warn("Anti-Aliasing not available!");
	(void)enabled;
#endif
}

bool getAntiAliasing() {
	return antiAliasingEnabled;
}

Finally loadSound(const std::string&); // definied in SoundFile.cpp

Finally load(const std::string& filename) {
	if (filename.length() >= 4 && filename.substr(filename.length() - 4) == ".ogg") {
		return loadSound(filename);
	}
	return loadSprite(filename);
}

void setScene(std::shared_ptr<Scene> scene) {
	pWindow->setWork(std::move(scene));
}

std::shared_ptr<Scene> getNextScene() {
	return pWindow->getNextScene();
}

void setWork(std::shared_ptr<Work> work) {
	pWindow->setWork(std::move(work));
}

void resetFrameLimiter() {
	pWindow->resetFrameLimiter();
}

void setWork(Work* w) {
	pWindow->setWork(std::shared_ptr<Work>(w));
}

std::shared_ptr<Work> getWork() {
	return pWindow->getWork();
}

void setPrefix(const std::string& path) {
	pathPrefix = path;
	if (!pathPrefix.empty() && pathPrefix.back() != '/') {
		pathPrefix += '/';
	}
}

std::string getPrefix() {
	return pathPrefix;
}

void setConfigPath(const std::string& path) {
	configPath = path;
	if (configPath->back() != '/') {
		*configPath += '/';
	}
}

std::string internal::getConfigPath() {
	if (configPath) {
		return *configPath;
	}
#ifndef IOS
	std::stringstream path;
#ifdef ANDROID
	path << getSystemConfigPath() << '/';
#elif defined(__EMSCRIPTEN__)
	path << "/working1/";
#else
#if defined(__APPLE__) || defined(_WIN32)
	path << getSystemConfigPath() << "/";
#else
	path << getenv("HOME") << "/.config/"; // NOLINT
#endif
	auto appDir = App::instance().getDisplayName();
	std::string invalid_chars = "\\/:?\"<>|*";
	for (const char c : invalid_chars) {
		appDir.erase(std::remove(appDir.begin(), appDir.end(), c), appDir.end());
	}
	if (appDir.empty()) {
		throw std::runtime_error("Invalid display name: " + App::instance().getDisplayName());
	}
	path << appDir << "/";
#endif
	return *(configPath = path.str());
#endif
	throw std::runtime_error("Couldn't get config path. Has the app been started?");
}

std::string getConfigPath() {
	return internal::getConfigPath();
}

std::stringstream readAsset(const std::string& filename) {
	if (!filename.empty() && filename[0] == '/') {
		throw std::runtime_error("Do not pass absolute paths to jngl::readAsset.");
	}
	std::stringstream sstream;
	FILE* const f = fopen((pathPrefix + filename).c_str(), "rb");
	if (!f) {
		sstream.setstate(std::ios::failbit);
		return sstream;
	}
	Finally closeFile([f]() {
		int result [[maybe_unused]] = fclose(f);
		assert(result == 0);
	});
	if (fseek(f, 0, SEEK_END) != 0) {
		sstream.setstate(std::ios::failbit);
		return sstream;
	}
	const auto size = ftell(f);
	if (fseek(f, 0, SEEK_SET) != 0) {
		sstream.setstate(std::ios::failbit);
		return sstream;
	}
	auto content = std::make_unique<char[]>(size);
	if (fread(content.get(), size, 1, f) != 1) {
		sstream.setstate(std::ios::failbit);
		return sstream;
	}
	sstream.write(content.get(), size);
	return sstream;
}

#if defined(HAVE_FILESYSTEM) && (!defined(__APPLE__) || !TARGET_OS_IPHONE)
std::filesystem::path u8path(const std::string& path) {
#if __cplusplus >= 202002L
	return std::filesystem::path(reinterpret_cast<const char8_t*>(path.c_str())); // NOLINT
#else
	return std::filesystem::u8path(path);
#endif
}
#endif

#if !defined(__APPLE__) || !TARGET_OS_IPHONE
std::string readConfig(const std::string& key) {
	if (!key.empty() && key[0] == '/') {
		throw std::runtime_error("Do not pass absolute paths as keys to jngl::readConfig.");
	}

#ifdef _WIN32
	std::filesystem::path p = u8path(internal::getConfigPath() + key);
	std::ifstream fin(p, std::ios::binary);
#else
	std::ifstream fin(internal::getConfigPath() + key, std::ios::binary);
#endif

	std::string out;
	constexpr size_t READ_SIZE = 4096;
	std::string buf(READ_SIZE, '\0');
	while (fin.read(buf.data(), READ_SIZE)) {
		out.append(buf, 0, fin.gcount());
	}
	out.append(buf, 0, fin.gcount());
	return out;
}

void writeConfig(const std::string& key, const std::string& value) {
	if (!key.empty() && key[0] == '/') {
		throw std::runtime_error("Do not pass absolute paths as keys to jngl::readConfig.");
	}
#ifdef ANDROID
	auto it = std::find(key.begin(), key.end(), '/');
	while (it != key.end()) {
		std::string directory(key.begin(), it);
		if (mkdir((internal::getConfigPath() + directory).c_str(), 755) != 0 && errno != EEXIST) {
			throw std::runtime_error("Couldn't create " + directory);
		}
		it = std::find(it + 1, key.end(), '/');
	}
#endif
#ifdef HAVE_FILESYSTEM
	const auto configPath = u8path(internal::getConfigPath());
	const auto directory = (configPath / u8path(key)).parent_path();
	if (!std::filesystem::exists(directory)) {
		std::filesystem::create_directories(directory);
	}
#endif
#ifdef _WIN32
	std::filesystem::path p = u8path(internal::getConfigPath() + key);
	std::ofstream fout(p, std::ios::binary);
#else
	std::ofstream fout(internal::getConfigPath() + key, std::ios::binary);
#endif
	fout.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fout << value;

#ifdef __EMSCRIPTEN__
	EM_ASM(
		FS.syncfs(false, function(err) {
			if (err) {
				console.warn("Error saving:", err);
			}
		})
	);
#endif
}
#endif

int round(double v) {
	assert(!std::isnan(v));
	return static_cast<int>(std::lround(v));
}

int round(float v) {
	assert(!std::isnan(v));
	return static_cast<int>(std::lround(v));
}

int utf8Length(std::string_view text) {
	int length = 0;
	for (const unsigned char c : text) {
		if ((c & 0xC0) != 0x80) {
			++length;
		}
	}
	return length;
}

} // namespace jngl
