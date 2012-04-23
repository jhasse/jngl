/*
Copyright 2007-2009 Jan Niklas Hasse <jhasse@gmail.com>
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
#include <boost/shared_ptr.hpp>

namespace jngl
{
	class Character : boost::noncopyable
	{
	public:
		Character(unsigned long ch, unsigned int height, FT_Face);
		~Character();
		void Draw() const;
		double GetWidth() const;
	private:
		Texture* texture_;
		int width_;
		int left_;
		int top_;
	};

	class Font : boost::noncopyable
	{
	public:
		Font();
		Font(const char* filename, unsigned int height);
		~Font();
		void Print(int x, int y, const std::string& text);
		double GetTextWidth(const std::string& text);
		int GetLineHeight() const;
	private:
		Character& GetCharacter(std::string::iterator& it, const std::string::iterator end);
		std::vector<std::string> ParseString(const std::string&);

		static int instanceCounter_;
		static FT_Library library_;
		FT_Face face_;
		boost::shared_ptr<Finally> freeFace_; // Frees face_ if necessary
		unsigned int height_;
		std::map<unsigned long, boost::shared_ptr<Character> > characters_;
	};
}
