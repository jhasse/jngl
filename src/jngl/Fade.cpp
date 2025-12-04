// Copyright 2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "Fade.hpp"

#include "Alpha.hpp"
#include "WorkFactory.hpp"
#include "matrix.hpp"
#include "other.hpp"
#include "screen.hpp"
#include "shapes.hpp"

namespace jngl {

Fade::Fade(std::shared_ptr<Work> work, int speed)
: work(std::move(work)), oldWork(getWork()), fadeCount(0), speed(speed) {
}

Fade::Fade(std::function<std::shared_ptr<Scene>()> factory, int speed)
: Fade(std::make_shared<WorkFactory>(std::move(factory)), speed) {
}

Fade::~Fade() {
	if (quit) {
		jngl::quit();
	}
}

void Fade::step() {
	std::shared_ptr<Scene> self = getActiveScene();
	if (self.get() != this) {
		throw std::runtime_error("Only call jngl::Fade::step() if it's the active scene.");
	}
	const int maxAlpha = 255;
	bool wasHalfTime = fadeCount >= maxAlpha;
	fadeCount += speed;
	if (quit || fadeCount >= 2 * maxAlpha) { // Finished?
		setScene(work);
		return;
	}
	if (!wasHalfTime && fadeCount >= maxAlpha) {
		fadeCount = maxAlpha; // draw at least one completely black frame
	}
	if (fadeCount == maxAlpha) {
		oldWork.reset();
		assert(!getNextScene());
		while (dynamic_cast<WorkFactory*>(work.get())) {
			work->onLoad();
			if (auto correctScene = getNextScene()) {
				work = std::move(correctScene);
				setScene(std::move(self));
			} else {
				break;
			}
		}
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
	drawSquare(modelview().scale(getScreenSize()), Rgba{ getBackgroundColor(), Alpha::u8(alpha) });
}

void Fade::onQuitEvent() {
	quit = true;
	cancelQuit();
}

} // namespace jngl
