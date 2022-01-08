// Copyright 2020-2021 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::Label class
/// @file
#pragma once

#include "Color.hpp"
#include "TextLine.hpp"
#include "Widget.hpp"

namespace jngl {

/// Simple Widget displaying a line of text
class Label : public Widget {
public:
	explicit Label(const std::string& str, Font&, Color, Vec2 position);

	void drawSelf() const override;

	/// Set font color alpha from 0...1
	void setAlpha(float alpha);

private:
	jngl::TextLine text;
	jngl::Color color;
	float alpha = 1.f;
};

} // namespace jngl
