// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "GpuTimer.hpp"

#include "../opengl.hpp"
#include "../windowptr.hpp"

#ifdef JNGL_VULKAN
#include "../Renderer.hpp"
#include "../vulkan/VulkanRenderer.hpp"
#endif

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

#elif defined(JNGL_VULKAN)

namespace {
VulkanRenderer* vulkanRenderer() {
	return static_cast<VulkanRenderer*>(getRendererIfExists());
}
} // namespace

struct GpuTimer::Impl {
	/// Two timestamp queries in \a pool, for start() and stop()
	struct Queries {
		VkQueryPool pool;
		uint32_t first;
	};

	~Impl() {
		if (auto* const renderer = vulkanRenderer()) {
			for (const auto pool : pools) {
				renderer->destroyTimestampPool(pool);
			}
		}
	}

	std::vector<VkQueryPool> pools;
	std::deque<Queries> pending; ///< stopped, oldest first, waiting for their results
	std::vector<Queries> unused; ///< reset, to be used again
	std::optional<Queries> running;
};

bool GpuTimer::isSupported() {
	const auto* const renderer = vulkanRenderer();
	return renderer && renderer->areTimestampsSupported();
}

void GpuTimer::start() {
	if (!isSupported() || impl->running) {
		return;
	}
	if (impl->unused.empty()) {
		constexpr uint32_t poolSize = 32;
		const VkQueryPool pool = vulkanRenderer()->createTimestampPool(poolSize);
		impl->pools.push_back(pool);
		for (uint32_t first = 0; first < poolSize; first += 2) {
			impl->unused.push_back({ pool, first });
		}
	}
	const auto queries = impl->unused.back();
	if (vulkanRenderer()->writeTimestamp(queries.pool, queries.first)) {
		impl->unused.pop_back();
		impl->running = queries;
	}
}

void GpuTimer::stop() {
	if (!impl->running || !vulkanRenderer()) {
		return;
	}
	// Otherwise the frame has been dropped since start(). Its query won't be used again, as it
	// can't be reset before the GPU is done with it.
	if (vulkanRenderer()->writeTimestamp(impl->running->pool, impl->running->first + 1)) {
		impl->pending.push_back(*impl->running);
	}
	impl->running.reset();
}

std::optional<std::chrono::nanoseconds> GpuTimer::poll() {
	if (impl->pending.empty() || !vulkanRenderer()) {
		return std::nullopt;
	}
	const auto queries = impl->pending.front();
	const auto elapsed = vulkanRenderer()->takeElapsedTime(queries.pool, queries.first);
	if (elapsed) {
		impl->pending.pop_front();
		impl->unused.push_back(queries);
	}
	return elapsed;
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
