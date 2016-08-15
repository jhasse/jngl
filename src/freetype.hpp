/*
Copyright 2007-2016 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#include "finally.hpp"
#include "texture.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

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
		Texture* texture_ = nullptr;
		int width_;
		int left_ = 0;
		int top_ = 0;
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

		static int instanceCounter;
		static FT_Library library;
		FT_Face face_;
		std::shared_ptr<Finally> freeFace_; // Frees face_ if necessary
		unsigned int height_;
		int lineHeight;
		std::map<unsigned long, std::shared_ptr<Character>> characters_;
	};
}
