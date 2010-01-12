/*
Copyright 2007-2009 Jan Niklas Hasse <jhasse@gmail.com>

This file is part of JNGL.

JNGL is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

JNGL is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with JNGL.  If not, see <http://www.gnu.org/licenses/>.
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
		Character(unsigned long ch, float height, FT_Face);
		~Character();
		void Draw() const;
		double GetWidth() const;
	private:
		Texture* texture_;
		int width_;
		int left_;
		float top_;
	};

	class Font : boost::noncopyable
	{
	public:
		Font();
		Font(const char* filename, unsigned int height);
		~Font();
		void Print(int x, int y, const std::string& text);
		double GetTextWidth(const std::string& text);
	private:
		Character& GetCharacter(std::string::iterator& it, const std::string::iterator end);
		std::vector<std::string> ParseString(const std::string&);

		static int instanceCounter_;
		static FT_Library library_;
		FT_Face face_;
		boost::shared_ptr<Finally> freeFace_; // Frees face_ if necessary
		float height_;
		std::map<unsigned long, boost::shared_ptr<Character> > characters_;
	};
}
