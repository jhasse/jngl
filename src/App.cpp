// Copyright 2019-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "App.hpp"

#include "jngl/AppParameters.hpp"
#include "jngl/Scene.hpp"
#include "jngl/ShaderProgram.hpp"
#include "jngl/matrix.hpp"
#include "jngl/other.hpp"
#include "jngl/screen.hpp"
#include "jngl/window.hpp"
#include "log.hpp"
#include "main.hpp"
#include "windowptr.hpp"

#include <cmath>
#include <memory>
#include <set>
#include <stdexcept>

namespace jngl {

/// Restarts the program via Steam if needed
void initSteamAchievements();

/// Initializes the Steamworks SDK (if JNGL_STEAMWORKS has been set via CMake)
void initSteam(uint32_t steamAppId);

App* App::self = nullptr;

struct App::Impl {
	std::string displayName;
	bool pixelArt = false;
	std::optional<uint32_t> steamAppId;
	std::set<ShaderProgram*> shaderPrograms;
	std::function<double(int, int)> scaleFactor;
};

App::App() {
	if (self) {
		throw std::runtime_error("You may only create one instance of jngl::App.");
	}
	self = this;
}

App::~App() {
	self = nullptr;
}

App& App::instance() {
	if (!self) {
		static App dummy; // e.g. for unit tests
		self = &dummy;
	}
	return *self;
}

Finally App::init(AppParameters params) {
	assert(impl == nullptr);
	impl = std::make_unique<App::Impl>(App::Impl{ .displayName = std::move(params.displayName),
	                                              .pixelArt = params.pixelArt,
	                                              .steamAppId = params.steamAppId,
	                                              .shaderPrograms = {},
	                                              .scaleFactor = std::move(params.scaleFactor) });
	return Finally{ [this]() { impl.reset(); } };
}

void App::atExit(std::function<void()> f) {
	callAtExit.emplace_back(std::move(f));
}

void App::callAtExitFunctions() {
	auto tmp = std::move(callAtExit);
	assert(callAtExit.empty());

	// destroy Singletons in the reverse order that they were created:
	const auto end = tmp.rend();
	for (auto it = tmp.rbegin(); it != end; ++it) {
		(*it)();
	}

	if (!callAtExit.empty()) {
		internal::warn("The destructor of a Singleton caused the creation of another Singleton. "
		               "Use handleIfAlive inside of destructors of Singletons.");
	}
	callAtExit.clear();
}

namespace {
#if defined(GL_DEBUG_OUTPUT) && !defined(NDEBUG)
#ifdef _WIN32
void __stdcall
#else
void
#endif
debugCallback(GLenum /*source*/, GLenum /*type*/, GLuint /*id*/, GLenum severity,
              GLsizei /*length*/, const GLchar* message, const void* /*userParam*/) {
	if (severity == GL_DEBUG_SEVERITY_HIGH) {
		internal::error(message);
	} else if (severity == GL_DEBUG_SEVERITY_MEDIUM) {
		internal::warn(message);
	} else if (severity != GL_DEBUG_SEVERITY_NOTIFICATION) {
		internal::info(message);
	} else {
		// NVIDIA driver will spam us, see https://stackoverflow.com/q/46771287/647898
		// internal::trace(message);
	}
}
#endif
} // namespace

void App::initGl(int width, int height, int canvasWidth, int canvasHeight) {
#if defined(GL_DEBUG_OUTPUT) && !defined(NDEBUG)
#ifdef GLAD_GL
	if (GLAD_GL_VERSION_4_3 != 0 || GLAD_GL_KHR_debug != 0) {
#endif
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(reinterpret_cast<GLDEBUGPROC>(debugCallback), nullptr); // NOLINT
#ifdef GLAD_GL
	}
#endif
#endif

	if (impl && impl->scaleFactor) {
		setScaleFactor(impl->scaleFactor(canvasWidth, canvasHeight));
	}
	updateProjection(width, height, static_cast<float>(width), static_cast<float>(height));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	updateViewportAndLetterboxing(width, height, canvasWidth, canvasHeight);

	reset();
	modelviewStack = {};

	clearBackgroundColor();
	glClear(GL_COLOR_BUFFER_BIT);

	glFlush();
	setVerticalSync(true);
}

std::string App::getDisplayName() const {
	return impl ? impl->displayName : "";
}

void App::setDisplayName(const std::string& displayName) {
	if (!impl) { // e.g. Android when using showWindow without jnglInit
		static Finally dummy(init({}));
	}
	impl->displayName = displayName;
}

uint8_t App::mainLoop() {
	if (impl->steamAppId) {
		initSteamAchievements();
	}
	internal::trace("Starting main loop for '{}'.", impl->displayName);
	return pWindow->mainLoop();
}

bool App::isPixelArt() {
	return self ? self->impl->pixelArt : false;
}

void App::setPixelArt(const bool pixelArt) {
	impl->pixelArt = pixelArt;
}

void App::registerShaderProgram(ShaderProgram* shaderProgram) {
	if (!impl) { // unit tests
		static Finally dummy(init({}));
	}
	impl->shaderPrograms.insert(shaderProgram);
}

void App::unregisterShaderProgram(ShaderProgram* shaderProgram) {
	impl->shaderPrograms.erase(shaderProgram);
}

void App::updateProjectionMatrix() const {
	for (const auto shaderProgram : impl->shaderPrograms) {
		const auto context = shaderProgram->use();
		glUniformMatrix4fv(shaderProgram->getUniformLocation("projection"), 1, GL_FALSE,
		                   opengl::projection.data);
	}
}

#if !defined(__APPLE__) || !defined(TARGET_OS_IPHONE) || !TARGET_OS_IPHONE // iOS
namespace internal {

uint8_t mainLoop(AppParameters params) {
	auto context = App::instance().init(params);
	if (auto id = params.steamAppId) {
		jngl::initSteam(*id);
	}

	bool fullscreen = false;
#if (!defined(__EMSCRIPTEN__) && defined(NDEBUG)) || defined(__ANDROID__)
	fullscreen = true;
#endif
	if (params.fullscreen) {
		fullscreen = *params.fullscreen;
	}
	std::pair<int, int> minAspectRatio{ 1, 3 };
	std::pair<int, int> maxAspectRatio{ 3, 1 };
	if (params.screenSize) {
		maxAspectRatio = minAspectRatio = std::pair<int, int>(std::lround(params.screenSize->x),
		                                                      std::lround(params.screenSize->y));
	} else {
		params.screenSize = { static_cast<double>(getDesktopWidth()),
			                  static_cast<double>(getDesktopHeight()) };
	}
	if (!fullscreen) {
		// Make window as big as possible
		const double scaleFactor = std::min((getDesktopWidth() * 0.85) / params.screenSize->x,
		                                    (getDesktopHeight() * 0.85) / params.screenSize->y);
		if (scaleFactor > 1) {
			setScaleFactor(std::floor(scaleFactor));
		} else {
			setScaleFactor(scaleFactor);
		}
	}
	int windowPixelWidth = static_cast<int>(std::lround(params.screenSize->x * getScaleFactor()));
	int windowPixelHeight = static_cast<int>(std::lround(params.screenSize->y * getScaleFactor()));
#ifdef JNGL_RECORD
	if (!fullscreen && (windowPixelWidth % 2 != 0 || windowPixelHeight % 2 != 0)) {
		auto oldWindowPixelWidth = windowPixelWidth;
		auto oldWindowPixelHeight = windowPixelHeight;
		if (windowPixelWidth % 2 != 0) {
			--windowPixelWidth; // make sure width is even for recording in H.264
		}
		if (windowPixelHeight % 2 != 0) {
			--windowPixelHeight; // make sure height is even for recording in H.264
		}
		const double newScaleFactor =
		    std::min(static_cast<double>(windowPixelWidth) / params.screenSize->x,
		             static_cast<double>(windowPixelHeight) / params.screenSize->y);
		auto scaleFactor = getScaleFactor();
		setScaleFactor(newScaleFactor);
		internal::debug(
		    "Adjusted scale factor from {} to {} to get even window dimensions ({}x{} -> {}x{}).",
		    scaleFactor, newScaleFactor, oldWindowPixelWidth, oldWindowPixelHeight,
		    windowPixelWidth, windowPixelHeight);
	}
#endif
	showWindow(params.displayName, windowPixelWidth, windowPixelHeight, fullscreen,
	           params.minAspectRatio ? *params.minAspectRatio : minAspectRatio,
	           params.maxAspectRatio ? *params.maxAspectRatio : maxAspectRatio);
	setScene(params.start());
	uint8_t exitcode = App::instance().mainLoop();
	hideWindow();
	return exitcode;
}

} // namespace internal
#endif

} // namespace jngl
