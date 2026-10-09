// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/Scene.hpp"
#include "../jngl/matrix.hpp"
#include "../jngl/screen.hpp"
#include "../jngl/shapes.hpp"
#include "../jngl/window.hpp"
#include "../window.hpp"
#include "../windowptr.hpp"
#include "Fixture.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <cmath>
#include <functional>
#include <vector>

namespace {

class RotatableScene : public jngl::Scene {
public:
	void step() override {
	}
	void draw() const override {
	}
	bool supportsScreenSize(const jngl::Vec2 screenSize) const override {
		asked.emplace_back(screenSize);
		return supports(screenSize);
	}
	void onScreenSizeChanged() override {
		++changed;
	}

	std::function<bool(jngl::Vec2)> supports = [](jngl::Vec2) { return false; };
	mutable std::vector<jngl::Vec2> asked;
	int changed = 0;
};

} // namespace

TEST_CASE("supportsScreenSize") {
	using Catch::Matchers::WithinAbs;

	Fixture f(1.f); // 320x70, which is also the only aspect ratio allowed
	auto scene = std::make_shared<RotatableScene>();
	jngl::setScene(scene);

	jngl::pWindow->updateScreenSize();
	REQUIRE(scene->asked.empty()); // the canvas already fills the window

	// actual pixels, which differ from screen pixels on HiDPI displays
	const auto px = [](const double screenPixels) {
		return static_cast<int>(std::lround(screenPixels * jngl::getScaleFactor()));
	};

	// Rotate the window, the Scene doesn't support it yet, so letter-box the canvas:
	jngl::pWindow->setActualSize(px(70), px(320));
	jngl::pWindow->updateScreenSize();
	// Letter-boxing 70x320 to 32:7 would be 70x15, so it gets asked for that, too:
	REQUIRE(scene->asked.size() == 2);
	REQUIRE(scene->asked[0].x == 70);
	REQUIRE(scene->asked[0].y == 320);
	REQUIRE_THAT(scene->asked[1].x, WithinAbs(70, 1));
	REQUIRE_THAT(scene->asked[1].y, WithinAbs(15, 1));
	REQUIRE(jngl::getScreenSize().x == 320);
	REQUIRE(jngl::getScreenSize().y == 70);
	REQUIRE(scene->changed == 0);
	// The center of the actual window is still the center of the canvas:
	const auto center = jngl::pWindow->toWindowCoordinates(px(70) / 2.f, px(320) / 2.f);
	REQUIRE_THAT(center.x, WithinAbs(px(320) / 2., 1));
	REQUIRE_THAT(center.y, WithinAbs(px(70) / 2., 1));

	scene->supports = [](jngl::Vec2) { return true; };
	jngl::pWindow->updateScreenSize();
	REQUIRE(jngl::getScreenSize().x == 70);
	REQUIRE(jngl::getScreenSize().y == 320);
	REQUIRE(scene->changed == 1);
	// Not letter-boxed anymore:
	const auto topLeft = jngl::pWindow->toWindowCoordinates(0, 0);
	REQUIRE_THAT(topLeft.x, WithinAbs(0, 1e-6));
	REQUIRE_THAT(topLeft.y, WithinAbs(0, 1e-6));
	jngl::pWindow->updateScreenSize();
	REQUIRE(scene->changed == 1); // nothing changed

	// e.g. another Scene became active which doesn't support it:
	scene->supports = [](jngl::Vec2) { return false; };
	jngl::pWindow->updateScreenSize();
	REQUIRE(jngl::getScreenSize().x == 320);
	REQUIRE(jngl::getScreenSize().y == 70);
	REQUIRE(scene->changed == 2);

	// Twice as wide as high, but the Scene only supports the aspect ratio of 32:7, so it gets
	// letter-boxed to that instead of using the original canvas:
	scene->supports = [](jngl::Vec2 size) { return std::abs(size.x / size.y - 32. / 7) < 0.1; };
	jngl::pWindow->setActualSize(px(640), px(320));
	jngl::pWindow->updateScreenSize();
	REQUIRE_THAT(jngl::getScreenSize().x, WithinAbs(640, 1));
	REQUIRE_THAT(jngl::getScreenSize().y, WithinAbs(140, 1));
	REQUIRE(scene->changed == 3);

	// Back to the original size, there's no need to ask the Scene:
	scene->asked.clear();
	jngl::pWindow->setActualSize(px(320), px(70));
	jngl::pWindow->updateScreenSize();
	REQUIRE(scene->asked.empty());
	REQUIRE(jngl::getScreenSize().x == 320);
	REQUIRE(jngl::getScreenSize().y == 70);
	REQUIRE(scene->changed == 4);
}

TEST_CASE("readPixels of a zoomed canvas") {
	Fixture f(1.f); // 320x70
	const auto draw = []() {
		jngl::drawRect(jngl::modelview().translate({ -60, -10 }), { 40, 20 }, 0x000000_rgb);
	};
	draw();
	const auto expected = f.getAsciiArt();

	// The window is only half as big now, but the canvas keeps its size (as there's no Scene which
	// would support it) and gets zoomed out. readPixels() should still return the whole canvas.
	jngl::pWindow->setActualSize(160, 35);
	jngl::pWindow->updateScreenSize();
	REQUIRE(jngl::getWindowWidth() == 320);
	REQUIRE(jngl::getWindowHeight() == 70);
	Fixture::reset(); // the frame has been drawn before resizing
	draw();
	REQUIRE(f.getAsciiArt() == expected);
}
