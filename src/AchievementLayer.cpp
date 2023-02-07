// Copyright 2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "AchievementLayer.hpp"

#include "freetype.hpp"
#include "jngl/Achievement.hpp"
#include "jngl/Vec2.hpp"
#include "jngl/font.hpp"
#include "jngl/matrix.hpp"
#include "jngl/screen.hpp"
#include "jngl/shapes.hpp"
#include "windowptr.hpp"

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
}

void AchievementLayer::draw() const {
	const jngl::Vec2 BOX(300, 60);
	auto mv = jngl::modelview();
	mv.translate(jngl::getScreenSize() / 2. - BOX);
	jngl::drawRect(mv, BOX, Color(150, 150, 150));
	mv.translate({ 20, 20 }); // padding
	pWindow->getFontImpl()->print(mv, achievement);
}

void AchievementLayer::notify(const Achievement& achievement) {
	this->achievement = achievement.description;
}

} // namespace jngl
