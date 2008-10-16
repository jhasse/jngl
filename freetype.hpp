/*
Copyright 2007-2008  Jan Niklas Hasse <jhasse@gmail.com>

This file is part of jngl.

jngl is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

jngl is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with jngl.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __FREETYPE_HPP__
#define __FREETYPE_HPP__

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include <vector>
#include <string>
#include <stdexcept>
#include <map>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include "finally.hpp"

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

namespace jngl
{
	class UnicodeCharacter
	{
	public:
		~UnicodeCharacter();
		void Init(unsigned long ch, float height, FT_Face);
		void Draw();
	private:
		GLuint displayList_;
		GLuint texture_;
	};

	class Font : boost::noncopyable
	{
	public:
		Font();
		Font(const char* filename, unsigned int height);
		~Font();
		void Print(double, double, const std::string& text);
	private:
		void MakeDisplayList(unsigned long);

		static bool initialized_;
		static FT_Library library_;
		FT_Face face_;
		boost::shared_ptr<Finally> freeFace_; // Frees face_ if necessary
		float height_;
		GLuint textures_[128];
		GLuint listBase_;
		std::map<unsigned long, UnicodeCharacter> characters_;
	};
}

#endif // __FREETYPE_HPP__
