// Copyright 2020-2022 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::TextLine class
/// @file
#pragma once

#include "Drawable.hpp"
#include "Mat3.hpp"

#include <memory>

namespace jngl {

class Font;
class FontImpl;

/// Rectangle shaped text (in contrast to jngl::Text this only represents one line)
class TextLine {
public:
	TextLine(Font&, std::string text);
	void step();
	void draw() const;
	void draw(Mat3 modelview) const;
	void setText(std::string);

	Vec2 getSize() const;
	double getWidth() const;
	double getHeight() const;
	void setCenter(double x, double y);
	void setCenter(Vec2);
	double getX() const;
	void setX(double);
	double getY() const;
	void setY(double);
	void setPos(double, double);

private:
	std::string text;
	std::shared_ptr<FontImpl> fontImpl;
	Vec2 position;
};

} // namespace jngl
