// Copyright 2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::TextLine class
/// @file
#pragma once

#include "Drawable.hpp"

#include <memory>

namespace jngl {

class Font;
class FontImpl;

/// Rectangle shaped text (in contrast to jngl::Text this only represents one line)
class TextLine : public Drawable {
public:
	TextLine(Font&, std::string text);
	void step() override;
	void draw() const override;

private:
	std::string text;
	std::shared_ptr<FontImpl> font;
};

} // namespace jngl
