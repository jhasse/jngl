// Copyright 2020-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "Label.hpp"

#include "font.hpp"

namespace jngl {

Label::Label(const std::string& str, Font& font, const Rgb color, const jngl::Vec2 position)
: Widget(position), text(font, str), color(color) {
	text.setCenter(Vec2(0, 0));
}

void Label::drawSelf(jngl::Mat3 modelview) const {
	jngl::setFontColor(color, alpha);
	text.draw(modelview);
}

void Label::setAlpha(float alpha) {
	this->alpha = alpha;
}

void Label::setAlign(Alignment align) {
	switch (align) {
	case Alignment::LEFT:
		text.setX(0);
		break;
	case Alignment::RIGHT:
		text.setX(-text.getWidth());
		break;
	case Alignment::CENTER:
		text.setCenter(Vec2(0, 0));
		break;
	}
}

float Label::getWidth() const {
	return static_cast<float>(text.getWidth());
}

} // namespace jngl
