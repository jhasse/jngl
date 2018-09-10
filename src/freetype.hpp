// Copyright 2007-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "jngl/Finally.hpp"
#include "texture.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace jngl {

extern unsigned char fontColorRed, fontColorGreen, fontColorBlue, fontColorAlpha;

class Character {
public:
	Character(char32_t ch, unsigned int fontHeight, FT_Face);
	Character(const Character&) = delete;
	Character& operator=(const Character&) = delete;
	~Character();
	void Draw() const;
	int getWidth() const;

private:
	Texture* texture_ = nullptr;
	int width_;
	int left_ = 0;
	int top_ = 0;
};

class FontImpl {
public:
	FontImpl(const std::string& relativeFilename, unsigned int height);
	FontImpl(const FontImpl&) = delete;
	FontImpl& operator=(const FontImpl&) = delete;
	~FontImpl();
	void print(int x, int y, const std::string& text);
	int getTextWidth(const std::string& text);
	int getLineHeight() const;
	void setLineHeight(int);

private:
	Character& GetCharacter(std::string::iterator& it, const std::string::iterator end);

	static int instanceCounter;
	static FT_Library library;
	FT_Face face;
	std::unique_ptr<Finally> freeFace; // Frees face_ if necessary
	unsigned int height_;
	int lineHeight;
	std::map<char32_t, std::shared_ptr<Character>> characters_;
	std::shared_ptr<std::vector<FT_Byte>> bytes;

	static std::map<std::string, std::weak_ptr<std::vector<FT_Byte>>> fileCaches;
};

} // namespace jngl
