// Copyright 2012-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// @file
#pragma once

#include "Color.hpp"
#include "Vec2.hpp"

#include <memory>

namespace jngl {

class FontImpl;

/// Font loaded from a TTF or OTF file
class Font {
public:
	Font(const std::string& filename, unsigned int size);
	std::shared_ptr<FontImpl> getImpl();
	void print(const std::string&, int x, int y);
	void print(const std::string&, Vec2 position);

private:
	std::shared_ptr<FontImpl> impl;
};

/// Print \a text at \a position
void print(const std::string& text, jngl::Vec2 position);

/// Print \a text at { \a xposition, \a yposition }
void print(const std::string& text, int xposition, int yposition);

/// Get the font size used by print()
int getFontSize();

/// Change the font size used by print()
void setFontSize(int size);

std::string getFont();

void setFont(const std::string& filename);

void setFontByName(const std::string& name);

void setFontColor(jngl::Color);

void setFontColor(unsigned char red, unsigned char green, unsigned char blue,
                  unsigned char alpha = 255);

void pushFontColor(unsigned char red, unsigned char green, unsigned char blue);

void popFontColor();

/// Get line height used py print() in pixel
int getLineHeight();

/// Set line height used by print() in pixel
void setLineHeight(int);

double getTextWidth(const std::string& text);

} // namespace jngl
