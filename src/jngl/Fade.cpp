// Copyright 2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "Fade.hpp"

#include "matrix.hpp"
#include "other.hpp"
#include "screen.hpp"
#include "shapes.hpp"

namespace jngl {

Fade::Fade(std::shared_ptr<Work> work, int speed)
: work(std::move(work)), oldWork(jngl::getWork()), fadeCount(0), speed(speed) {
}

Fade::~Fade() {
	if (quit) {
		jngl::quit();
	}
}

void Fade::step() {
	const int maxAlpha = 255;
	fadeCount += speed;
	if (quit || fadeCount >= 2 * maxAlpha) { // Finished?
		jngl::setWork(work);
	}
}

void Fade::draw() const {
	const int maxAlpha = 255;
	if (fadeCount < maxAlpha) {
		if (!dynamic_cast<Fade*>(oldWork.get())) {
			oldWork->draw();
		}
	} else {
		work->draw();
	}
	const int alpha = static_cast<int>(fadeCount > maxAlpha ? 2 * maxAlpha - fadeCount : fadeCount);
	jngl::drawSquare(jngl::modelview().scale(jngl::getScreenSize()),
	                 jngl::Rgba::u8(0, 0, 0, alpha));
}

void Fade::onQuitEvent() {
	quit = true;
	cancelQuit();
}

} // namespace jngl
