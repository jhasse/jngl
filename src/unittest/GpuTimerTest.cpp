// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/GpuTimer.hpp"
#include "../jngl/matrix.hpp"
#include "../jngl/shapes.hpp"
#include "../jngl/window.hpp"
#include "../opengl.hpp"
#include "Fixture.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("GpuTimer") {
	Fixture f(1.f);
	if (!jngl::GpuTimer::isSupported()) {
		SKIP("No timer queries");
	}
	jngl::GpuTimer timer;
	CHECK(!timer.poll()); // nothing measured yet
	for (int i = 0; i < 2; ++i) {
		timer.start();
		jngl::drawRect(jngl::modelview(), { 10, 10 }, 0x000000_rgb);
		timer.stop();
	}
#ifdef JNGL_VULKAN
	jngl::readPixels(); // submits what's been drawn and waits for it, so that both have come in
#else
	glFinish(); // so that both have come in
#endif
	CHECK(timer.poll());
	CHECK(timer.poll());
	CHECK(!timer.poll());

	timer.stop(); // without start(), which does nothing
	CHECK(!timer.poll());
}
