// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "GpuTimer.hpp"

#include "../opengl.hpp"
#include "../windowptr.hpp"

#include <deque>
#include <vector>

namespace jngl {

#if defined(IOS) || defined(ANDROID) || defined(__EMSCRIPTEN__)

struct GpuTimer::Impl {};

bool GpuTimer::isSupported() {
	return false;
}

void GpuTimer::start() {
}

void GpuTimer::stop() {
}

std::optional<std::chrono::nanoseconds> GpuTimer::poll() {
	return std::nullopt;
}

#else

struct GpuTimer::Impl {
	~Impl() {
		if (pWindow) { // Don't bother deleting OpenGL objects when the OpenGL context has already
			           // been destroyed
			for (const auto query : pending) {
				glDeleteQueries(1, &query);
			}
			if (!unused.empty()) {
				glDeleteQueries(static_cast<GLsizei>(unused.size()), unused.data());
			}
		}
	}

	std::deque<GLuint> pending; ///< stopped, oldest first, waiting for their results
	std::vector<GLuint> unused; ///< to be used again, rather than generating new ones
	std::optional<GLuint> running;
};

bool GpuTimer::isSupported() {
	return GLAD_GL_VERSION_3_3 != 0;
}

void GpuTimer::start() {
	if (!isSupported() || impl->running) {
		return;
	}
	GLuint query = 0;
	if (impl->unused.empty()) {
		glGenQueries(1, &query);
	} else {
		query = impl->unused.back();
		impl->unused.pop_back();
	}
	glBeginQuery(GL_TIME_ELAPSED, query);
	impl->running = query;
}

void GpuTimer::stop() {
	if (!impl->running) {
		return;
	}
	glEndQuery(GL_TIME_ELAPSED);
	impl->pending.push_back(*impl->running);
	impl->running.reset();
}

std::optional<std::chrono::nanoseconds> GpuTimer::poll() {
	if (impl->pending.empty()) {
		return std::nullopt;
	}
	const auto query = impl->pending.front();
	GLint available = GL_FALSE;
	glGetQueryObjectiv(query, GL_QUERY_RESULT_AVAILABLE, &available);
	if (available == GL_FALSE) {
		return std::nullopt;
	}
	GLuint64 elapsed = 0;
	glGetQueryObjectui64v(query, GL_QUERY_RESULT, &elapsed);
	impl->pending.pop_front();
	impl->unused.push_back(query);
	return std::chrono::nanoseconds(elapsed);
}

#endif

GpuTimer::GpuTimer() : impl(std::make_unique<Impl>()) {
}

GpuTimer::~GpuTimer() = default;
GpuTimer::GpuTimer(GpuTimer&&) noexcept = default;
GpuTimer& GpuTimer::operator=(GpuTimer&&) noexcept = default;

} // namespace jngl
