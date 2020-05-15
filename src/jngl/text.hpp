// Copyright 2012-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// @file

#pragma once

#include "Drawable.hpp"

#include <memory>
#include <string>
#include <vector>

namespace jngl {

/// How multiple lines should be aligned in a text block
enum class Alignment { LEFT, RIGHT, CENTER };

class Font;
class FontImpl;
class Line;

/// Rectangle shaped text block
class Text : public Drawable {
public:
	/// Constructor, \a text may contain `\n` newlines
	explicit Text(const std::string& text = "");

	/// The text to display (may contain `\n` newlines)
	void setText(const std::string&);

	/// Font family
	void setFont(Font&);

	/// Alignment of this text block
	void setAlign(Alignment);

	/// Does nothing
	void step() override;

	/// Simply draws the Text object
	void draw() const override;

private:
	std::vector<std::shared_ptr<Line>> lines;
	std::shared_ptr<FontImpl> font;
	Alignment align = Alignment::LEFT;
};

} // namespace jngl
