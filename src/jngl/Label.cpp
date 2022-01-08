// Copyright 2020-2021 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "Label.hpp"

#include "font.hpp"

namespace jngl {

Label::Label(const std::string& str, Font& font, const Color color, const jngl::Vec2 position)
: Widget(position), text(font, str), color(color) {
	text.setCenter(Vec2(0, 0));
}

void Label::drawSelf() const {
	jngl::setFontColor(color, alpha);
	text.draw();
}

void Label::setAlpha(float alpha) {
	this->alpha = alpha;
}

} // namespace jngl
