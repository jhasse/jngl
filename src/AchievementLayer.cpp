// Copyright 2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "AchievementLayer.hpp"

#include "freetype.hpp"
#include "jngl/Achievement.hpp"
#include "jngl/Vec2.hpp"
#include "jngl/font.hpp"
#include "jngl/matrix.hpp"
#include "jngl/other.hpp"
#include "jngl/screen.hpp"
#include "jngl/shapes.hpp"
#include "windowptr.hpp"

#include <sstream>

namespace jngl {

AchievementLayer& AchievementLayer::handle() {
	static std::weak_ptr<AchievementLayer> instance;
	if (auto shared = instance.lock()) {
		return *shared;
	}
	auto shared = std::make_shared<AchievementLayer>();
	instance = shared;
	addJob(shared);
	return *shared;
}

void AchievementLayer::step() {
	if (++stepsPassed > getStepsPerSecond() * 3) {
		fadeIn += 0.05f;
	} else {
		fadeIn *= 0.85f;
	}
	if (fadeIn < 0.015f) {
		value += (static_cast<float>(targetValue) - value) * 0.05f;
		if (fadeIn < 0.001f && targetValue >= maxValue) {
			colorFade += (1 - colorFade) * 0.05f;
		}
	}
	if (fadeIn > 1.f) {
		achievement = nullptr;
		if (!queue.empty()) {
			do {
				queue.front().second();
				assert(achievement != nullptr);
				queue.pop_front();
			} while (!queue.empty() && achievement == queue.front().first);
		}
	}
}

void AchievementLayer::draw() const {
	if (!achievement) {
		return;
	}
	const Vec2 BOX(650, 140);
	auto mv = modelview();
	mv.translate({0., fadeIn * 140});
	mv.translate(getScreenSize() / 2. - BOX);
	jngl::setAlpha(180);
	drawRect(mv, BOX, Color(50, 50, 50));
	const Vec2 padding{ 20, 20 };
	mv.translate(padding);

	setFontColor(0xffffff_rgb, 1.f);
	setFontSize(37);
	pWindow->getFontImpl()->print(mv, achievement->name);

	mv.translate({0, 50});
	setFontSize(28);
	std::ostringstream tmp;
	if (achievement->customDisplayValue) {
		tmp << achievement->customDisplayValue(static_cast<int>(std::lround(value)));
	} else {
		tmp << std::lround(value);
	}
	tmp << " of ";
	if (achievement->customDisplayValue) {
		tmp << achievement->customDisplayValue(maxValue);
	} else {
		tmp << maxValue;
	}
	pWindow->getFontImpl()->print(mv, tmp.str());

	Vec2 bar(BOX.x - padding.x * 2, 10);
	mv.translate({0, 40}); // below text
	const float percentage = std::min(1.f, value / static_cast<float>(maxValue));
	bar.x *= percentage;
	drawRect(mv, bar, interpolate(Color(255, 255, 255), 0xe2b007_rgb /* gold */, colorFade));
	mv.translate({ bar.x, 0 });
	drawRect(mv, Vec2((1.f - percentage) * (BOX.x - padding.x * 2), bar.y), Color(90, 90, 90));
	jngl::setAlpha(255);
}

void AchievementLayer::notify(const Achievement& achievement, int oldValue, int newValue) {
	auto start = [this, &achievement, oldValue, newValue]() {
		targetValue = newValue;
		stepsPassed = 0;
		if (this->achievement == nullptr) {
			// only start animations, if we aren't re-use an existing popup
			fadeIn = 1.f;
			value = static_cast<float>(oldValue);
			maxValue = achievement.maxValue;
			colorFade = 0;
		} else {
			assert(&achievement == this->achievement);
		}
		this->achievement = &achievement;
	};
	if (this->achievement == nullptr || this->achievement == &achievement /* re-use existing popup */) {
		start();
	} else {
		queue.emplace_back(&achievement, start);
	}
}

} // namespace jngl
