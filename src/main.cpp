// Copyright 2007-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "main.hpp"

#include "jngl.hpp"
#include "paths.hpp"
#include "spriteimpl.hpp"

#include <boost/math/special_functions/round.hpp>
#include <boost/qvm/mat_operations.hpp>
#include <fstream>
#include <sstream>

#ifdef _WIN32
#include <filesystem>
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

#ifdef ANDROID
#include "android/fopen.hpp"
#endif

namespace jngl {

std::string pathPrefix;
optional<std::string> configPath;
std::vector<std::string> args;
float bgRed = 1.0f, bgGreen = 1.0f, bgBlue = 1.0f; // Background Colors
std::stack<boost::qvm::mat<float, 3, 3>> modelviewStack;
std::unique_ptr<ShaderProgram> simpleShaderProgram;
int simpleModelviewUniform;
int simpleColorUniform;

void clearBackgroundColor() {
	glClearColor(bgRed, bgGreen, bgBlue, 1);
}

#if defined(GL_DEBUG_OUTPUT) && !defined(NDEBUG)
#ifdef _WIN32
void __stdcall
#else
void
#endif
debugCallback(GLenum /*source*/, GLenum /*type*/, GLuint /*id*/, GLenum severity,
              GLsizei /*length*/, const GLchar* message, const void* /*userParam*/) {
	if (severity == GL_DEBUG_SEVERITY_HIGH) {
		jngl::debugLn(std::string("\x1b[1;31m") + message + "\x1b[0m");
	} else if (severity != GL_DEBUG_SEVERITY_NOTIFICATION) {
		jngl::debugLn(message);
	}
}
#endif

bool Init(const int width, const int height, const int canvasWidth, const int canvasHeight) {
#if defined(GL_DEBUG_OUTPUT) && !defined(NDEBUG)
	if (epoxy_gl_version() >= 43 || epoxy_has_gl_extension("GL_KHR_debug")) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(reinterpret_cast<GLDEBUGPROC>(debugCallback), nullptr); // NOLINT
	}
#endif

	const auto l = static_cast<float>( -width) / 2.f;
	const auto r = static_cast<float>(  width) / 2.f;
	const auto b = static_cast<float>( height) / 2.f;
	const auto t = static_cast<float>(-height) / 2.f;
	opengl::projection = {{
		{ 2.f / (r - l),           0.f,  0.f, -(r + l) / (r - l) },
		{           0.f, 2.f / (t - b),  0.f, -(t + b) / (t - b) },
		{           0.f,           0.f, -1.f, 0.f },
		{           0.f,           0.f,  0.f, 1.f }
	}};

	Shader vertexShader(R"(#version 300 es
		in mediump vec2 position;
		uniform highp mat3 modelview;
		uniform mediump mat4 projection;

		void main() {
			vec3 tmp = modelview * vec3(position, 1);
			gl_Position = projection * vec4(tmp.x, tmp.y, 0, 1);
		})", Shader::Type::VERTEX
	);
	Shader fragmentShader(R"(#version 300 es
		uniform lowp vec4 color;
		out lowp vec4 outColor;

		void main() {
			outColor = color;
		})", Shader::Type::FRAGMENT
	);
	simpleShaderProgram = std::make_unique<ShaderProgram>(vertexShader, fragmentShader);
	simpleModelviewUniform = simpleShaderProgram->getUniformLocation("modelview");
	simpleColorUniform = simpleShaderProgram->getUniformLocation("color");

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glViewport(0, 0, width, height);

	if (canvasWidth != width || canvasHeight != height) { // Letterboxing?
		glClearColor(0, 0, 0, 1); // black boxes
		glClear(GL_COLOR_BUFFER_BIT);

		glEnable(GL_SCISSOR_TEST);
		assert(canvasWidth <= width);
		assert(canvasHeight <= height);
		glScissor((width - canvasWidth) / 2, (height - canvasHeight) / 2, canvasWidth,
		          canvasHeight);
	}

	reset();
	modelviewStack = {};

	clearBackgroundColor();
	glClear(GL_COLOR_BUFFER_BIT);

	glFlush();
	setVerticalSync(true);
	return true;
}

WindowPointer pWindow;
bool antiAliasingEnabled = true;
bool vsyncEnabled = false;

void showWindow(const std::string& title, const int width, const int height, bool fullscreen,
                const std::pair<int, int> minAspectRatio,
                const std::pair<int, int> maxAspectRatio) {
	debug("jngl::showWindow(\""); debug(title); debug("\", "); debug(width); debug(", ");
	debug(height); debug(", "); debug(fullscreen); debug(");\n");
	bool isMouseVisible = pWindow ? pWindow->getMouseVisible() : true;
	hideWindow();
	if (width == 0) {
		throw std::runtime_error("Width Is 0");
	}
	if (height == 0) {
		throw std::runtime_error("Height Is 0");
	}
	pWindow.Set(new Window(title, width, height, fullscreen, minAspectRatio, maxAspectRatio));
	pWindow->SetMouseVisible(isMouseVisible);
	setAntiAliasing(antiAliasingEnabled);
	pWindow->initGlObjects();
}

void hideWindow() {
	simpleShaderProgram.reset();
	unloadAll();
	pWindow.Delete();
}

void swapBuffers() {
	pWindow->SwapBuffers();

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

void quit() {
	pWindow->quit();
}

void cancelQuit() {
	if (pWindow) { pWindow->cancelQuit(); }
}

void setBackgroundColor(const jngl::Color color) {
	pWindow.ThrowIfNull();
	bgRed = float(color.getRed()) / 255.f;
	bgGreen = float(color.getGreen()) / 255.f;
	bgBlue = float(color.getBlue()) / 255.f;
	clearBackgroundColor();
	glClear(GL_COLOR_BUFFER_BIT);
}

void setBackgroundColor(const unsigned char red, const unsigned char green,
                        const unsigned char blue) {
	setBackgroundColor(Color{ red, green, blue });
}

Vec2 getMousePos() {
	if (getRelativeMouseMode()) {
		return { pWindow->getMouseX() / getScaleFactor(), pWindow->getMouseY() / getScaleFactor() };
	}
	return { pWindow->getMouseX() / getScaleFactor() - getScreenWidth() / 2,
		     pWindow->getMouseY() / getScaleFactor() - getScreenHeight() / 2 };
}

int getMouseX() {
	return pWindow->getMouseX();
}

int getMouseY() {
	return pWindow->getMouseY();
}

bool keyDown(const key::KeyType key) {
	return pWindow->getKeyDown(key);
}

bool keyPressed(const key::KeyType key) {
	return pWindow->getKeyPressed(key);
}

bool keyDown(const std::string& key) {
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
	pWindow->SetMouse(boost::math::iround((position.x + getScreenWidth() / 2) * getScaleFactor()),
	                  boost::math::iround((position.y + getScreenHeight() / 2) * getScaleFactor()));
}

void setRelativeMouseMode(const bool relative) {
	return pWindow->SetRelativeMouseMode(relative);
}

void setMouseVisible(const bool visible) {
	return pWindow->SetMouseVisible(visible);
}

bool isMouseVisible() {
	return pWindow->getMouseVisible();
}

bool getRelativeMouseMode() {
	return pWindow->getRelativeMouseMode();
}

void setTitle(const std::string& title) {
	return pWindow->SetTitle(title);
}

double getTextWidth(const std::string& text) {
	return pWindow->GetTextWidth(text);
}

int getLineHeight() {
	return pWindow->getLineHeight();
}

void setLineHeight(int h) {
	pWindow->setLineHeight(h);
}

void print(const std::string& text, const jngl::Vec2 position) {
	pWindow->print(text, boost::math::iround(position.x), boost::math::iround(position.y));
}

void print(const std::string& text, const int xposition, const int yposition) {
	pWindow->print(text, xposition, yposition);
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
	pWindow->SetIcon(pathPrefix + filename);
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

void reset() {
	boost::qvm::set_identity(opengl::modelview);
}

void rotate(const double degree) {
	boost::qvm::rotate_z(opengl::modelview, degree * M_PI / 180.);
}

void translate(const double x, const double y) {
	opengl::translate(x * getScaleFactor(), y * getScaleFactor());
}

void scale(const double factor) {
	opengl::scale(factor, factor);
}

void scale(const double xfactor, const double yfactor) {
	opengl::scale(xfactor, yfactor);
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
	pWindow->drawRect({ xposition, yposition }, { width, height });
}

void drawRect(const Vec2 position, const Vec2 size) {
	pWindow->drawRect(position, size);
}

void drawTriangle(const Vec2 a, const Vec2 b, const Vec2 c) {
	pWindow->drawTriangle(a, b, c);
}

void drawTriangle(const double A_x, const double A_y, const double B_x, const double B_y,
                  const double C_x, const double C_y) {
	pWindow->drawTriangle({ A_x, A_y}, { B_x, B_y }, { C_x, C_y });
}

void setLineWidth(const float width) {
	glLineWidth(width);
}

void drawLine(const double xstart, const double ystart, const double xend, const double yend) {
	pWindow->drawLine({ xstart, ystart }, { xend, yend });
}

void drawLine(const Vec2 start, const Vec2 end) {
	pWindow->drawLine(start, end);
}

void drawPoint(const double x, const double y) {
	pWindow->drawEllipse({x, y}, {1, 1}, 0);
}

void readPixel(const int x, const int y, unsigned char& red, unsigned char& green,
               unsigned char& blue) {
	unsigned char data[3];
	data[0] = static_cast<unsigned char>(bgRed * 255.0f);
	data[1] = static_cast<unsigned char>(bgGreen * 255.0f);
	data[2] = static_cast<unsigned char>(bgBlue * 255.0f);
	glReadPixels(x, getWindowHeight() - y - 1, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, data);
	red = data[0];
	green = data[1];
	blue = data[2];
}

int getWindowWidth() {
	return pWindow->getCanvasWidth();
}

int getWindowHeight() {
	return pWindow->getCanvasHeight();
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
	debug("WARNING: anti aliasing not available!\n");
#endif
}

bool getAntiAliasing() {
	return antiAliasingEnabled;
}

void loadSound(const std::string&); // definied in audio.cpp

Finally load(const std::string& filename) {
	if (filename.length() >= 4 && filename.substr(filename.length() - 4) == ".ogg") {
		loadSound(filename);
		return Finally(nullptr);
	}
	return loadSprite(filename);
}

#ifdef _WIN32
#include <epoxy/wgl.h>

void setVerticalSync(bool enabled) {
	if (wglSwapIntervalEXT && wglSwapIntervalEXT(enabled)) {
		vsyncEnabled = enabled;
	}
#else
void setVerticalSync(bool) {
#endif
#ifdef __APPLE__
	// OSX always enables V-SYNC
	vsyncEnabled = true;
#endif
}

bool getVerticalSync() {
	return vsyncEnabled;
}

void setWork(std::shared_ptr<Work> work) {
	return pWindow->setWork(std::move(work));
}

void resetFrameLimiter() {
	pWindow->resetFrameLimiter();
}

void setWork(Work* w) {
	return pWindow->setWork(std::shared_ptr<Work>(w));
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
}

std::string getConfigPath() {
	if (configPath) { return *configPath; }
#ifndef IOS
	std::stringstream path;
#if defined(__APPLE__) || defined(_WIN32)
	path << getSystemConfigPath() << "/" << App::instance().getDisplayName() << "/";
#else
	path << getenv("HOME") << "/.config/" << App::instance().getDisplayName() << "/";
#endif
	return *(configPath = path.str());
#endif
	throw std::runtime_error("Couldn't get config path. Has the app been started?");
}

void setArgs(std::vector<std::string> args) {
	jngl::args = std::move(args);
}

std::vector<std::string> getArgs() {
	return args;
}

std::stringstream readAsset(const std::string& filename) {
	if (!filename.empty() && filename[0] == '/') {
		throw std::runtime_error("Do not pass absolute paths to jngl::readAsset.");
	}
	std::stringstream sstream;
#ifdef IOS
	FILE* const f = fopen((pathPrefix + filename).c_str(), "rb");
#else
	FILE* const f = fopen(filename.c_str(), "rb");
#endif
	if (!f) {
		sstream.setstate(std::ios::failbit);
		return sstream;
	}
	Finally closeFile([f]() { fclose(f); });
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

#if !defined(__APPLE__) || !TARGET_OS_IPHONE
std::string readConfig(const std::string& key) {
	if (!key.empty() && key[0] == '/') {
		throw std::runtime_error("Do not pass absolute paths as keys to jngl::readConfig.");
	}

#ifdef _WIN32
	std::filesystem::path p = std::filesystem::u8path(jngl::getConfigPath() + key);
	std::ifstream fin(p, std::ios::binary);
#else
	std::ifstream fin(jngl::getConfigPath() + key, std::ios::binary);
#endif

	std::string out;
	constexpr size_t READ_SIZE = 4096;
	std::string buf(READ_SIZE, '\0');
	while (fin.read(&buf[0], READ_SIZE)) {
		out.append(buf, 0, fin.gcount());
	}
	out.append(buf, 0, fin.gcount());
	return out;
}

void writeConfig(const std::string& key, const std::string& value) {
	if (!key.empty() && key[0] == '/') {
		throw std::runtime_error("Do not pass absolute paths as keys to jngl::readConfig.");
	}
#ifdef _WIN32
	std::filesystem::path p = std::filesystem::u8path(jngl::getConfigPath() + key);
	std::ofstream fout(p, std::ios::binary);
#else
	std::ofstream fout(jngl::getConfigPath() + key, std::ios::binary);
#endif
	fout.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fout << value;
}
#endif

ShaderProgram::Context useSimpleShaderProgram() {
	auto context = jngl::simpleShaderProgram->use();
	glUniform4f(simpleColorUniform, float(colorRed) / 255.0f, float(colorGreen) / 255.0f,
	            float(colorBlue) / 255.0f, float(colorAlpha) / 255.0f);
	glUniformMatrix3fv(simpleModelviewUniform, 1, GL_TRUE, &opengl::modelview.a[0][0]);

	assert(simpleShaderProgram->getAttribLocation("position") == 0);
	glEnableVertexAttribArray(0);

	return context;
}

} // namespace jngl
