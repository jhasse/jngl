// Copyright 2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
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
