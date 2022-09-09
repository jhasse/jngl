// Copyright 2007-2022 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "jngl/Pixels.hpp"
#include "texture.hpp"

#include <ft2build.h> // NOLINT
#include FT_FREETYPE_H

#include <map>

namespace jngl {

constexpr double LINE_HEIGHT_FACOTR = 1. / .63;
extern unsigned char fontColorRed, fontColorGreen, fontColorBlue, fontColorAlpha;

class Character {
public:
	Character(char32_t ch, unsigned int fontHeight, FT_Face);
	Character(const Character&) = delete;
	Character& operator=(const Character&) = delete;
	Character(Character&&) = delete;
	Character& operator=(Character&&) = delete;
	~Character();
	void draw(Mat3& modelview) const;
	Pixels getWidth() const;

private:
	Texture* texture_ = nullptr;
	Pixels width_{0};
	Pixels left_{0};
	Pixels top_{0};
};

class FontImpl {
public:
	FontImpl(const std::string& relativeFilename, unsigned int height);
	FontImpl(const FontImpl&) = delete;
	FontImpl& operator=(const FontImpl&) = delete;
	FontImpl(FontImpl&&) = delete;
	FontImpl& operator=(FontImpl&&) = delete;
	~FontImpl();
	void print(Mat3 modelview, const std::string& text);
	void print(double x, double y, const std::string& text);
	Pixels getTextWidth(const std::string& text);
	int getLineHeight() const;
	void setLineHeight(int);

private:
	Character& GetCharacter(std::string::iterator& it, std::string::iterator end);

	static int instanceCounter;
	static FT_Library library;
	FT_Face face = nullptr;
	std::unique_ptr<Finally> freeFace; // Frees face_ if necessary
	unsigned int height_;
	int lineHeight;
	std::map<char32_t, std::shared_ptr<Character>> characters_;
	std::shared_ptr<std::vector<FT_Byte>> bytes;

	static std::map<std::string, std::weak_ptr<std::vector<FT_Byte>>> fileCaches;
};

} // namespace jngl
