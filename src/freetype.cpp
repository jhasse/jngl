// Copyright 2007-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "freetype.hpp"

#include "helper.hpp"
#include "jngl/debug.hpp"
#include "jngl/matrix.hpp"
#include "jngl/screen.hpp"
#include "main.hpp"
#include "spriteimpl.hpp"

#ifdef ANDROID
#include "android/fopen.hpp"
#endif

#include FT_GLYPH_H

#include <cassert>
#include <codecvt>
#include <locale>

namespace jngl {

	Character::Character(const char32_t ch, const unsigned int fontHeight, FT_Face face) {
		const auto flags = FT_LOAD_TARGET_LIGHT | FT_LOAD_RENDER;
		if (FT_Load_Char(face, ch, flags)) {
		    const std::string msg =
		        std::string("FT_Load_Glyph failed. Character: ") + std::to_string(ch);
		    debugLn(msg);
			// Load a question mark instead
			if (FT_Load_Glyph(face, FT_Get_Char_Index(face, '?') , flags)) {
				throw std::runtime_error(msg);
			}
		}
		FT_Glyph glyph;
		if (FT_Get_Glyph(face->glyph, &glyph)) {
			throw std::runtime_error("FT_Get_Glyph failed");
		}
		const auto bitmap_glyph = reinterpret_cast<FT_BitmapGlyph>(glyph); // NOLINT
		const FT_Bitmap& bitmap = bitmap_glyph->bitmap;

		int width = bitmap.width;
		int height = bitmap.rows;
		width_ = int(face->glyph->advance.x >> 6);

		if (height == 0) {
			return;
		}

		std::vector<GLubyte*> data(height);

		for (int y = 0; y < height; ++y) {
			data[y] = new GLubyte[width * 4];
			for (int x = 0; x < width; ++x) {
				data[y][x * 4    ] = 255;
				data[y][x * 4 + 1] = 255;
				data[y][x * 4 + 2] = 255;
				unsigned char alpha = 0;
				if (bitmap.pixel_mode == FT_PIXEL_MODE_MONO) {
					if (bitmap.buffer[y * bitmap.pitch + x / 8] & (0x80 >> (x % 8))) {
						alpha = 255;
					} else {
						alpha = 0;
					}
				} else if (bitmap.pixel_mode == FT_PIXEL_MODE_GRAY) {
					alpha = bitmap.buffer[x + width * y];
				} else {
					throw std::runtime_error("Unsupported pixel mode");
				}
				data[y][x * 4 + 3] = alpha;
			}
		}

		texture_ = new Texture(width, height, &data[0]);
		for (auto d : data) {
			delete[] d;
		}

		top_ = fontHeight - bitmap_glyph->top;
		left_ = bitmap_glyph->left;
	}

	void Character::Draw() const {
		if (texture_) {
			pushMatrix();
			opengl::translate(left_, top_);
			texture_->draw(float(fontColorRed) / 255.0f, float(fontColorGreen) / 255.0f,
			               float(fontColorBlue) / 255.0f, float(fontColorAlpha) / 255.0f);
			popMatrix();
		}
		opengl::translate(width_, 0);
	}

	int Character::getWidth() const {
		return width_;
	}

	Character::~Character() {
		delete texture_;
	}

	Character& FontImpl::GetCharacter(std::string::iterator& it, const std::string::iterator end) {
#ifdef _MSC_VER
		// https://stackoverflow.com/questions/32055357/visual-studio-c-2015-stdcodecvt-with-char16-t-or-char32-t
		static std::wstring_convert<std::codecvt_utf8<int32_t>, int32_t> cvt;
#else
		static std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
#endif
		const char& ch = (*it); // Just to have less code
		char32_t unicodeCharacter = ch;
		if (ch & 0x80) { // first bit (Check if this is an Unicode character)
			const char* sourceEnd = &ch + 2;
			// sourceEnd has to be the next character after the utf-8 sequence
			const static auto ERROR_MSG = "Invalid UTF-8 string!";
			if (++it == end) {
				throw std::runtime_error(ERROR_MSG);
			}
			if (ch & 0x20) { // third bit
				if(++it == end) {
					throw std::runtime_error(ERROR_MSG);
				}
				++sourceEnd;
				if(ch & 0x10) // fourth bit
				{
					if(++it == end) {
						throw std::runtime_error(ERROR_MSG);
					}
					++sourceEnd;
				}
			}
			unicodeCharacter = cvt.from_bytes(&ch, sourceEnd)[0];
		}
		if (characters_[unicodeCharacter] == nullptr) {
			characters_[unicodeCharacter].reset(new Character(unicodeCharacter, height_, face));
		}
		return *(characters_[unicodeCharacter]);
	}

	FontImpl::FontImpl(const std::string& relativeFilename, unsigned int height)
	: height_(static_cast<unsigned int>(height * getScaleFactor())), lineHeight(int(height_ / .63)) {
		auto filename = pathPrefix + relativeFilename;
		if (!fileExists(filename)) {
			if (!fileExists(relativeFilename)) {
				throw std::runtime_error(std::string("Font file not found: ") + filename);
			}
			filename = relativeFilename;
		}
		if (++instanceCounter == 1) {
			if (FT_Init_FreeType(&library)) {
				--instanceCounter;
				throw std::runtime_error("FT_Init_FreeType failed");
			}
		}
		auto& fileCache = fileCaches[filename];
		bytes = fileCache.lock();
		if (bytes) {
			debug("Reusing font buffer for "); debug(filename); debug("... ");
		} else {
			debug("Loading font "); debug(filename); debug("... ");

			FILE* const f = fopen(filename.c_str(), "rb");
			assert(f);
			fseek(f, 0, SEEK_END);
			const size_t fsize = ftell(f);
			fseek(f, 0, SEEK_SET);
			bytes = std::make_shared<std::vector<FT_Byte>>(fsize);
			const auto result = fread(bytes->data(), 1, fsize, f);
			if (result != fsize) {
				throw std::runtime_error("fread failed");
			}
			fclose(f);
			fileCache = bytes;
		}

		if (FT_New_Memory_Face(library, bytes->data(), bytes->size(), 0, &face) != 0) {
			throw std::runtime_error("FT_New_Memory_Face failed");
		}
		debug("OK\n");
		// Finally will call FT_Done_Face when the Font class is destroyed:
		freeFace = std::make_unique<Finally>([this]() { return FT_Done_Face(face); });

		// For some twisted reason, Freetype measures font size
		// in terms of 1/64ths of pixels.  Thus, to make a font
		// h pixels high, we need to request a size of h*64.
		FT_Set_Char_Size(face, height_ * 64, height_ * 64, 96, 96);
	}

	FontImpl::~FontImpl()
	{
		freeFace.reset(); // free face_ with FT_Done_Face
		if(--instanceCounter == 0)
		{
			FT_Done_FreeType(library);
		}
	}

	int FontImpl::getTextWidth(const std::string& text) {
		int maxWidth = 0;
		std::vector<std::string> lines(splitlines(text));

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
		std::vector<std::string> lines(splitlines(text));

		glEnable(GL_TEXTURE_2D);

		auto lineEnd = lines.end();
		int lineNr = 0;
		for (auto lineIter = lines.begin(); lineIter != lineEnd; ++lineIter) {
			pushMatrix();
			opengl::translate(x, y + lineHeight * lineNr);
			++lineNr;

			auto charEnd = lineIter->end();
			for (auto charIter = lineIter->begin(); charIter != charEnd; ++charIter) {
				GetCharacter(charIter, charEnd).Draw();
			}

			popMatrix();
		}
		glDisable(GL_TEXTURE_2D);
	}

	FT_Library FontImpl::library;
	int FontImpl::instanceCounter = 0;
	decltype(FontImpl::fileCaches) FontImpl::fileCaches;
} // namespace jngl
