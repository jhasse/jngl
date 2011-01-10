/*
Copyright 2007-2009 Jan Niklas Hasse <jhasse@gmail.com>

Most of this code is based on http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=43

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

#include "freetype.hpp"
#include "jngl.hpp"
#include "debug.hpp"

#include "ConvertUTF.h"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

namespace jngl
{
	unsigned char fontColorRed = 0, fontColorGreen = 0, fontColorBlue = 0, fontColorAlpha = 255;

	void SetFontColor(const unsigned char red, const unsigned char green, const unsigned char blue, const unsigned char alpha)
	{
		fontColorRed = red;
		fontColorGreen = green;
		fontColorBlue = blue;
		fontColorAlpha = alpha;
	}

	Character::Character(const unsigned long ch, const float fontHeight, FT_Face face) : texture_(0)
	{
		if(FT_Load_Glyph(face, FT_Get_Char_Index(face, ch) , FT_LOAD_TARGET_LIGHT))
		{
			std::string msg = std::string("FT_Load_Glyph failed. Character: ") + boost::lexical_cast<std::string>(ch);
			Debug(msg);
			// Load a question mark instead
			if(FT_Load_Glyph(face, FT_Get_Char_Index(face, '?') , FT_LOAD_TARGET_LIGHT))
			{
				throw std::runtime_error(msg);
			}
		}
		FT_Glyph glyph;
		if(FT_Get_Glyph(face->glyph, &glyph))
		{
			throw std::runtime_error("FT_Get_Glyph failed");
		}
		FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
		FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;
		FT_Bitmap& bitmap = bitmap_glyph->bitmap;

		int width = opengl::NextPowerOf2(bitmap.width);
		int height = opengl::NextPowerOf2(bitmap.rows);

		std::vector<GLubyte> data(width * height * 4);

		for(int y = 0; y < height; y++)
		{
			for(int x = 0; x < width; x++)
			{
				data[x * 4 + y * 4 * width    ] = 255;
				data[x * 4 + y * 4 * width + 1] = 255;
				data[x * 4 + y * 4 * width + 2] = 255;
				unsigned char alpha = 0;
				if(x < bitmap.width && y < bitmap.rows) // Are we in the padding zone? (see next_p2)
				{
					if(bitmap.pixel_mode == FT_PIXEL_MODE_MONO)
					{
						if(bitmap.buffer[y * bitmap.pitch + x / 8] & (0x80 >> (x % 8)))
						{
							alpha = 255;
						}
						else
						{
							alpha = 0;
						}
					}
					else if(bitmap.pixel_mode == FT_PIXEL_MODE_GRAY)
					{
						alpha = bitmap.buffer[x + bitmap.width * y];
					}
					else
					{
						throw std::runtime_error("Unsupported pixel mode\n");
					}
				}
				data[x*4 + y*4*width + 3] = alpha;
			}
		}

		texture_ = new Texture(width, height);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);

		top_ = fontHeight - bitmap_glyph->top;
		width_ = face->glyph->advance.x >> 6;
		left_ = bitmap_glyph->left;
	}

	void Character::Draw() const
	{
		glPushMatrix();
		opengl::Translate(left_, top_);

		texture_->Draw();

		glPopMatrix();
		opengl::Translate(width_, 0);
	}

	double Character::GetWidth() const
	{
		return width_;
	}

	Character::~Character()
	{
		delete texture_;
	}

	Character& Font::GetCharacter(std::string::iterator& it, const std::string::iterator end)
	{
		const char& ch = (*it); // Just to have less code
		unsigned long unicodeCharacter = ch;
		if(ch & 0x80) // first bit (Check if this is an Unicode character)
		{
			const UTF8* sourceEnd = (const unsigned char*)&ch + 2; // sourceEnd has to be the next character after the utf-8 sequence
			const std::runtime_error unicodeError("Invalid UTF-8 string!");
			if(++it == end)
			{
				throw unicodeError;
			}
			if(ch & 0x20) // third bit
			{
				if(++it == end)
				{
					throw unicodeError;
				}
				++sourceEnd;
				if(ch & 0x10) // fourth bit
				{
					if(++it == end)
					{
						throw unicodeError;
					}
					++sourceEnd;
				}
			}
			const UTF8* temp = (const unsigned char*)&ch;
			const UTF8** sourceStart = &temp;
			UTF32 unicode;
			UTF32* temp2 = &unicode;
			UTF32** targetStart = &temp2;
			UTF32* targetEnd = &unicode + sizeof(unsigned long);
			ConversionResult result = ConvertUTF8toUTF32(sourceStart, sourceEnd, targetStart, targetEnd, lenientConversion);
			if(result == conversionOK)
			{
				unicodeCharacter = unicode;
			}
			else
			{
				Debug(" ERROR - " + boost::lexical_cast<std::string>(result));
			}
		}
		if(characters_[unicodeCharacter] == 0)
		{
			characters_[unicodeCharacter].reset(new Character(unicodeCharacter, height_, face_));
		}
		return *(characters_[unicodeCharacter]);
	}

	Font::Font()
	{
		throw std::runtime_error("Attempting to use an unitialized Font object");
	}

	Font::Font(const char* filename, unsigned int height) : height_(height)
	{
		if(++instanceCounter_ == 1)
		{
			if(FT_Init_FreeType(&library_))
			{
				--instanceCounter_;
				throw std::runtime_error("FT_Init_FreeType failed");
			}
		}
		Debug("Loading font "); Debug(filename); Debug("... ");
		if(FT_New_Face(library_, filename, 0, &face_))
		{
			throw std::runtime_error("FT_New_Face failed");
		}
		Debug("OK\n");
		freeFace_.reset(new Finally(boost::bind(FT_Done_Face, face_))); // Finally will call FT_Done_Face when the Font class is destroyed

		// For some twisted reason, Freetype measures font size
		// in terms of 1/64ths of pixels.  Thus, to make a font
		// h pixels high, we need to request a size of h*64.
		FT_Set_Char_Size(face_, height * 64, height * 64, 96, 96);
	}

	Font::~Font()
	{
		freeFace_.reset((Finally*)0); // free face_ with FT_Done_Face
		if(--instanceCounter_ == 0)
		{
			FT_Done_FreeType(library_);
		}
	}

	std::vector<std::string> Font::ParseString(const std::string& text)
	{
		std::vector<std::string> lines;
		const char* start_line = text.c_str();
		const char* c;
		for(c = text.c_str(); *c; c++)
		{
			if(*c == '\n')
			{
				std::string line;
				for(const char *n = start_line; n < c; ++n)
				{
					line.append(1, *n);
				}
				lines.push_back(line);
				start_line = c + 1;
			}
		}
		if(start_line)
		{
			std::string line;
			for(const char* n = start_line; n < c; ++n)
			{
				line.append(1, *n);
			}
			lines.push_back(line);
		}
		return lines;
	}

	double Font::GetTextWidth(const std::string& text)
	{
		double maxWidth = 0;
		std::vector<std::string> lines(ParseString(text));

		std::vector<std::string>::iterator lineEnd = lines.end();
		for(std::vector<std::string>::iterator lineIter = lines.begin(); lineIter != lineEnd; ++lineIter)
		{
			double lineWidth = 0;
			std::string::iterator charEnd = lineIter->end();
			for(std::string::iterator charIter = lineIter->begin(); charIter != charEnd; ++charIter)
			{
				lineWidth += GetCharacter(charIter, charEnd).GetWidth();
			}
			if(lineWidth > maxWidth)
			{
				maxWidth = lineWidth;
			}
		}
		return maxWidth;
	}
	
	int Font::GetLineHeight() const {
		return int(height_ / .63);
	}

	void Font::Print(const int x, const int y, const std::string& text)
	{
		const int h = GetLineHeight();

		std::vector<std::string> lines(ParseString(text));

		glColor4ub(fontColorRed, fontColorGreen, fontColorBlue, fontColorAlpha);
		glEnable(GL_TEXTURE_2D);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		std::vector<std::string>::iterator lineEnd = lines.end();
		int lineNr = 0;
		for(std::vector<std::string>::iterator lineIter = lines.begin(); lineIter != lineEnd; ++lineIter)
		{
			glPushMatrix();
			opengl::Translate(x, y + h * lineNr);
			++lineNr;

			std::string::iterator charEnd = lineIter->end();
			for(std::string::iterator charIter = lineIter->begin(); charIter != charEnd; ++charIter)
			{
				GetCharacter(charIter, charEnd).Draw();
			}

			glPopMatrix();
		}
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisable(GL_TEXTURE_2D);
	}

	FT_Library Font::library_;
	int Font::instanceCounter_ = 0;
}

