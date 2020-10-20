#pragma once

#include "Color.hpp"
#include "Widget.hpp"
#include "text.hpp"

namespace jngl {

class Label : public Widget {
public:
	explicit Label(const std::string& str, Font&, Color, Vec2 position);

	void drawSelf() const override;

private:
	jngl::Text text;
	jngl::Color color;
};

} // namespace jngl
