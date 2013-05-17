/*
Copyright 2007-2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#include "finally.hpp"
#include "texture.hpp"

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

#include <vector>
#include <string>
#include <stdexcept>
#include <map>

#include <boost/noncopyable.hpp>

namespace jngl {
	extern unsigned char fontColorRed, fontColorGreen, fontColorBlue, fontColorAlpha;

	class Character : boost::noncopyable
	{
	public:
		Character(unsigned long ch, unsigned int height, FT_Face);
		~Character();
		void Draw() const;
		int getWidth() const;
	private:
		Texture* texture_;
		int width_;
		int left_;
		int top_;
	};

	class FontImpl : boost::noncopyable {
	public:
		FontImpl();
		FontImpl(const std::string& filename, unsigned int height);
		~FontImpl();
		void print(int x, int y, const std::string& text);
		int getTextWidth(const std::string& text);
		int getLineHeight() const;
		void setLineHeight(int);
	private:
		Character& GetCharacter(std::string::iterator& it, const std::string::iterator end);
		std::vector<std::string> ParseString(const std::string&);

		static int instanceCounter_;
		static FT_Library library_;
		FT_Face face_;
		std::shared_ptr<Finally> freeFace_; // Frees face_ if necessary
		unsigned int height_;
		int lineHeight;
		std::map<unsigned long, std::shared_ptr<Character>> characters_;
	};
}
