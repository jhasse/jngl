#include "Label.hpp"

#include "font.hpp"

namespace jngl {

Label::Label(const std::string& str, Font& font, const Color color, const jngl::Vec2 position) : Widget(position), text(str), color(color) {
	text.setFont(font);
	text.setCenter(position);
}

void Label::drawSelf() const {
	jngl::setFontColor(color);
	text.draw();
}

} // namespace jngl
