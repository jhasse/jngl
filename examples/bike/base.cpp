#include "base.hpp"
#include "constants.hpp"

#include <jngl.hpp>

Base::Base() {
	lines_.emplace_back(jngl::Vec2(0, 0), jngl::Vec2(screenWidth, 0));
	lines_.emplace_back(jngl::Vec2(0, screenHeight), jngl::Vec2(screenWidth, screenHeight));
	lines_.emplace_back(jngl::Vec2(screenWidth, 0), jngl::Vec2(screenWidth, screenHeight));
	lines_.emplace_back(jngl::Vec2(0, 0), jngl::Vec2(0, screenHeight));
	lines_.emplace_back(jngl::Vec2(118, 444), jngl::Vec2(213, 250));
	lines_.emplace_back(jngl::Vec2(213, 250), jngl::Vec2(368, 228));
	lines_.emplace_back(jngl::Vec2(368, 228), jngl::Vec2(452, 295));
	lines_.emplace_back(jngl::Vec2(452, 295), jngl::Vec2(216, 448));
}

void Base::step() {
	for (auto& line : lines_) {
		bike_.HandleCollision(line);
	}
	bike_.DoFrame();
}

void Base::draw() const {
	jngl::translate(-jngl::getScreenWidth() / 2.0, -jngl::getScreenHeight() / 2.0);
	jngl::draw("bg", 0, 0);
	bike_.draw();
}
