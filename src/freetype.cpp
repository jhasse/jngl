/*
Copyright 2007-2012 Jan Niklas Hasse <jhasse@gmail.com>

Most of this code is based on http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=43

For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "freetype.hpp"
#include "jngl/font.hpp"
#include "jngl/debug.hpp"
#include "jngl/screen.hpp"
#include "ConvertUTF.h"
#include "main.hpp"

#include <functional>
#include <boost/lexical_cast.hpp>
#include <fstream>

namespace jngl {

	Character::Character(const unsigned long ch, const unsigned int fontHeight, FT_Face face) : texture_(0) {
		if (FT_Load_Glyph(face, FT_Get_Char_Index(face, ch) , FT_LOAD_TARGET_LIGHT)) {
			std::string msg = std::string("FT_Load_Glyph failed. Character: ") + boost::lexical_cast<std::string>(ch);
			debug(msg);
			// Load a question mark instead
			if (FT_Load_Glyph(face, FT_Get_Char_Index(face, '?') , FT_LOAD_TARGET_LIGHT)) {
				throw std::runtime_error(msg);
			}
		}
		FT_Glyph glyph;
		if (FT_Get_Glyph(face->glyph, &glyph)) {
			throw std::runtime_error("FT_Get_Glyph failed");
		}
		FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
		FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;
		FT_Bitmap& bitmap = bitmap_glyph->bitmap;

		int width = opengl::NextPowerOf2(bitmap.width);
		int height = opengl::NextPowerOf2(bitmap.rows);

		std::vector<GLubyte*> data(height);

		for (int y = 0; y < height; y++) {
			data[y] = new GLubyte[width * 4];
			for (int x = 0; x < width; x++) {
				data[y][x * 4    ] = 255;
				data[y][x * 4 + 1] = 255;
				data[y][x * 4 + 2] = 255;
				unsigned char alpha = 0;
				if (x < bitmap.width && y < bitmap.rows) { // Are we in the padding zone? (see next_p2)
					if (bitmap.pixel_mode == FT_PIXEL_MODE_MONO) {
						if (bitmap.buffer[y * bitmap.pitch + x / 8] & (0x80 >> (x % 8))) {
							alpha = 255;
						} else {
							alpha = 0;
						}
					} else if(bitmap.pixel_mode == FT_PIXEL_MODE_GRAY) {
						alpha = bitmap.buffer[x + bitmap.width * y];
					} else {
						throw std::runtime_error("Unsupported pixel mode\n");
					}
				}
				data[y][x*4 + 3] = alpha;
			}
		}

		texture_ = new Texture(width, height, &data[0]);
		for (auto d : data) {
			delete[] d;
		}

		top_ = fontHeight - bitmap_glyph->top;
		width_ = face->glyph->advance.x >> 6;
		left_ = bitmap_glyph->left;
	}

	void Character::Draw() const
	{
		glPushMatrix();
		opengl::translate(left_, top_);

		texture_->draw();

		glPopMatrix();
		opengl::translate(width_, 0);
	}

	int Character::getWidth() const {
		return width_;
	}

	Character::~Character()
	{
		delete texture_;
	}

	Character& FontImpl::GetCharacter(std::string::iterator& it, const std::string::iterator end)
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
				debug(" ERROR - " + boost::lexical_cast<std::string>(result));
			}
		}
		if(characters_[unicodeCharacter] == 0)
		{
			characters_[unicodeCharacter].reset(new Character(unicodeCharacter, height_, face_));
		}
		return *(characters_[unicodeCharacter]);
	}

	FontImpl::FontImpl()
	{
		throw std::runtime_error("Attempting to use an unitialized FontImpl object");
	}

	FontImpl::FontImpl(const std::string& relativeFilename, unsigned int height)
	: height_(static_cast<unsigned int>(height * getScaleFactor())), lineHeight(int(height_ / .63)) {
		auto filename = pathPrefix + relativeFilename;
		if (!std::ifstream(filename.c_str())) {
			throw std::runtime_error(std::string("Font file not found: ") + filename);
		}
		if(++instanceCounter_ == 1)
		{
			if(FT_Init_FreeType(&library_))
			{
				--instanceCounter_;
				throw std::runtime_error("FT_Init_FreeType failed");
			}
		}
		debug("Loading font "); debug(filename); debug("... ");
		if (FT_New_Face(library_, filename.c_str(), 0, &face_)) {
			throw std::runtime_error("FT_New_Face failed");
		}
		debug("OK\n");
		freeFace_.reset(new Finally(std::bind(FT_Done_Face, face_))); // Finally will call FT_Done_Face when the Font class is destroyed

		// For some twisted reason, Freetype measures font size
		// in terms of 1/64ths of pixels.  Thus, to make a font
		// h pixels high, we need to request a size of h*64.
		FT_Set_Char_Size(face_, height_ * 64, height_ * 64, 96, 96);
	}

	FontImpl::~FontImpl()
	{
		freeFace_.reset((Finally*)0); // free face_ with FT_Done_Face
		if(--instanceCounter_ == 0)
		{
			FT_Done_FreeType(library_);
		}
	}

	std::vector<std::string> FontImpl::ParseString(const std::string& text) {
		std::vector<std::string> lines;
		const char* start_line = text.c_str();
		const char* c;
		for (c = text.c_str(); *c; c++) {
			if (*c == '\n') {
				std::string line;
				for (auto *n = start_line; n < c; ++n) {
					line.append(1, *n);
				}
				lines.push_back(line);
				start_line = c + 1;
			}
		}
		if (start_line) {
			std::string line;
			for (auto n = start_line; n < c; ++n) {
				line.append(1, *n);
			}
			lines.push_back(line);
		}
		return lines;
	}

	int FontImpl::getTextWidth(const std::string& text) {
		int maxWidth = 0;
		std::vector<std::string> lines(ParseString(text));

		auto lineEnd = lines.end();
		for (auto lineIter = lines.begin(); lineIter != lineEnd; ++lineIter) {
			int lineWidth = 0;
			auto charEnd = lineIter->end();
			for (auto charIter = lineIter->begin(); charIter != charEnd; ++charIter) {
				lineWidth += GetCharacter(charIter, charEnd).getWidth();
			}
			if (lineWidth > maxWidth) {
				maxWidth = lineWidth;
			}
		}
		return int(maxWidth / getScaleFactor());
	}

	int FontImpl::getLineHeight() const {
		return lineHeight;
	}

	void FontImpl::setLineHeight(int h) {
		lineHeight = h;
	}

	void FontImpl::print(int x, int y, const std::string& text) {
		x = int(x * getScaleFactor());
		y = int(y * getScaleFactor());
		std::vector<std::string> lines(ParseString(text));

		glColor4ub(fontColorRed, fontColorGreen, fontColorBlue, fontColorAlpha);
		glEnable(GL_TEXTURE_2D);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		auto lineEnd = lines.end();
		int lineNr = 0;
		for (auto lineIter = lines.begin(); lineIter != lineEnd; ++lineIter) {
			glPushMatrix();
			opengl::translate(x, y + lineHeight * lineNr);
			++lineNr;

			auto charEnd = lineIter->end();
			for (auto charIter = lineIter->begin(); charIter != charEnd; ++charIter) {
				GetCharacter(charIter, charEnd).Draw();
			}

			glPopMatrix();
		}
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisable(GL_TEXTURE_2D);
	}

	FT_Library FontImpl::library_;
	int FontImpl::instanceCounter_ = 0;
}

