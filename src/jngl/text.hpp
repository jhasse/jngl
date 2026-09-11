// Copyright 2012-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// @file

#pragma once

#include "Drawable.hpp"
#include "Mat3.hpp"

#include <memory>
#include <string>
#include <vector>

namespace jngl {

/// How multiple lines should be aligned in a text block
enum class Alignment : uint8_t { LEFT, RIGHT, CENTER };

class Font;
class FontInterface;

/// Rectangle shaped text block
///
/// This class represents a drawable text block that supports multi-line text with automatic word
/// wrapping. Text can contain newline characters (`\n`) for manual line breaks, or use automatic
/// word wrapping based on a maximum width.
///
/// @note Set the font using setFont() before calling setText() with a maxWidth parameter for
/// correct text wrapping behavior.
///
/// Example usage:
/// @code
/// jngl::Text text;
/// text.setFont(myFont);
/// text.setText("Hello World!\nThis is a very long line.", 150);
/// text.setAlign(jngl::Alignment::CENTER);
/// text.draw();
/// @endcode
class Text : public Drawable {
public:
	/// Constructor, \a text may contain `\n` newlines
	explicit Text(const std::string& text = "");

	/// The text to display (may contain `\n` newlines)
	///
	/// \param maxWidth Maximum width of the text block in pixels. If the text exceeds this width,
	///                 it will be wrapped at word boundaries to fit within the specified width.
	///                 Defaults to no limit (std::numeric_limits<double>::max()).
	///
	/// \note In order for this to work correctly, call this after setFont.
	void setText(const std::string&, double maxWidth = std::numeric_limits<double>::max());

	/// Font family
	void setFont(Font&);

	void setFont(const std::shared_ptr<FontInterface>&);

	/// Alignment of this text block
	void setAlign(Alignment);

	/// Does nothing
	void step() override;

	/// Simply draws the Text object
	void draw() const override;
	void draw(Mat3 modelview) const;

private:
	/// Adds multiple line by breaking on words if the line would be longer than \a maxWidth
	void addLine(const std::string& text, double maxWidth);

	class Line;
	std::vector<std::shared_ptr<Line>> lines;
	std::shared_ptr<FontInterface> font;
	Alignment align = Alignment::LEFT;
};

} // namespace jngl
