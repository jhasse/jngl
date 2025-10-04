// Copyright 2007-2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#define _LIBCPP_DISABLE_DEPRECATION_WARNINGS // NOLINT
#include "freetype.hpp"

#include "ShaderCache.hpp"
#include "helper.hpp"
#include "jngl/Finally.hpp"
#include "jngl/ScaleablePixels.hpp"
#include "jngl/matrix.hpp"
#include "jngl/screen.hpp"
#include "log.hpp"
#include "main.hpp"

#ifdef ANDROID
#include "android/fopen.hpp"
#endif

#include FT_GLYPH_H

#include <cassert>
#include <codecvt>
#include <locale>
#include <memory>

namespace jngl {

Character::Character(const char32_t ch, const unsigned int fontHeight, FT_Face face,
                     FT_Stroker stroker) {
	const auto flags = FT_LOAD_TARGET_LIGHT | FT_LOAD_DEFAULT;
	if (FT_Load_Char(face, ch, flags)) {
		const std::string msg =
		    std::string("FT_Load_Glyph failed. Character: ") + std::to_string(ch);
		// Load a question mark instead
		if (FT_Load_Glyph(face, FT_Get_Char_Index(face, '?'), flags)) {
			throw std::runtime_error(msg);
		}
		internal::error(msg);
	}
	FT_Glyph glyph;
	if (FT_Get_Glyph(face->glyph, &glyph)) {
		throw std::runtime_error("FT_Get_Glyph failed");
	}
	if (stroker) {
		FT_Glyph_StrokeBorder(&glyph, stroker, 0, 1);
	}
	FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, nullptr, 1);
	Finally freeGlyph([&]() { FT_Done_Glyph(glyph); });
	const auto bitmap_glyph = reinterpret_cast<FT_BitmapGlyph>(glyph); // NOLINT
	const FT_Bitmap& bitmap = bitmap_glyph->bitmap;

	const auto width = static_cast<ptrdiff_t>(bitmap.width);
	const int height = static_cast<int>(bitmap.rows);
	width_ = Pixels(static_cast<int32_t>(face->glyph->advance.x >> 6));

	if (height == 0) {
		return;
	}

	std::vector<GLubyte*> data(height);

	for (int y = 0; y < height; ++y) {
		data[y] = new GLubyte[width * 4];
		for (ptrdiff_t x = 0; x < width; ++x) {
			data[y][x * 4    ] = 255;
			data[y][x * 4 + 1] = 255;
			data[y][x * 4 + 2] = 255;
			unsigned char alpha = 0;
			if (bitmap.pixel_mode == FT_PIXEL_MODE_MONO) {
				if (bitmap.buffer[static_cast<ptrdiff_t>(y * bitmap.pitch) + x / 8] &
				    (0x80 >> (x % 8))) {
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

	texture_ = new Texture(static_cast<float>(width), static_cast<float>(height),
	                       static_cast<int>(width), height, data.data());
	for (auto d : data) {
		delete[] d;
	}

	top_ = Pixels(static_cast<int>(fontHeight) - bitmap_glyph->top);
	left_ = Pixels(bitmap_glyph->left);
}

void Character::draw(Mat3& modelview) const {
	if (texture_) {
		glUniformMatrix3fv(ShaderCache::handle().modelviewUniform, 1, GL_FALSE,
		                   Mat3(modelview).translate(left_, top_).data);
		texture_->draw();
	}
	modelview.translate(width_, 0_px);
}

Pixels Character::getWidth() const {
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
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
	static std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
#pragma GCC diagnostic pop
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
			if (++it == end) {
				throw std::runtime_error(ERROR_MSG);
			}
			++sourceEnd;
			if (ch & 0x10) { // fourth bit
				if (++it == end) {
					throw std::runtime_error(ERROR_MSG);
				}
				++sourceEnd;
			}
		}
		unicodeCharacter = cvt.from_bytes(&ch, sourceEnd)[0];
	}
	if (characters_[unicodeCharacter] == nullptr) {
		characters_[unicodeCharacter] =
		    std::make_shared<Character>(unicodeCharacter, height_, face, stroker);
	}
	return *(characters_[unicodeCharacter]);
}

FontImpl::FontImpl(const std::string& relativeFilename, unsigned int height, float strokePercentage)
: height_(static_cast<unsigned int>(height * getScaleFactor())),
  lineHeight(static_cast<int>(height_ * LINE_HEIGHT_FACTOR)) {
	auto filename = pathPrefix + relativeFilename;
	if (!fileExists(filename)) {
		if (!fileExists(relativeFilename)) {
			if (relativeFilename.empty()) {
				throw std::runtime_error("No font file set. Use jngl::setFont.");
			}
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
		internal::debug("Reusing font buffer for {}... ", filename);
	} else {
		internal::debug("Loading font {}...", filename);

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

	if (FT_New_Memory_Face(library, bytes->data(), static_cast<FT_Long>(bytes->size()), 0, &face) !=
	    0) {
		throw std::runtime_error("FT_New_Memory_Face failed");
	}
	// Finally will call FT_Done_Face when the Font class is destroyed:
	freeFace = std::make_unique<Finally>([this]() { return FT_Done_Face(face); });

	// For some twisted reason, Freetype measures font size
	// in terms of 1/64ths of pixels.  Thus, to make a font
	// h pixels high, we need to request a size of h*64.
	FT_Set_Char_Size(face, static_cast<FT_F26Dot6>(height_) * 64,
	                 static_cast<FT_F26Dot6>(height_) * 64, 96, 96);

	FT_Fixed strokeWidth =
	    std::lround(strokePercentage * static_cast<double>(height) * getScaleFactor() * 0.64);
	if (strokeWidth != 0) {
		FT_Stroker_New(library, &stroker);
		FT_Stroker_Set(stroker, strokeWidth, FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND,
		               0);
	}
}

FontImpl::~FontImpl() {
	freeFace.reset(); // free face_ with FT_Done_Face
	if (stroker) {
		FT_Stroker_Done(stroker);
	}
	if (--instanceCounter == 0) {
		FT_Done_FreeType(library);
	}
}

Pixels FontImpl::getTextWidth(const std::string& text) {
	auto maxWidth = 0_px;
	std::vector<std::string> lines(splitlines(text));

	auto lineEnd = lines.end();
	for (auto lineIter = lines.begin(); lineIter != lineEnd; ++lineIter) {
		auto lineWidth = 0_px;
		auto charEnd = lineIter->end();
		for (auto charIter = lineIter->begin(); charIter != charEnd; ++charIter) {
			lineWidth += GetCharacter(charIter, charEnd).getWidth();
		}
		if (lineWidth > maxWidth) {
			maxWidth = lineWidth;
		}
	}
	return maxWidth;
}

Pixels FontImpl::getLineHeight() const {
	return Pixels(lineHeight);
}

void FontImpl::setLineHeight(Pixels h) {
	lineHeight = static_cast<int>(h);
}

void FontImpl::print(Mat3 modelview, const std::string& text, Rgba color) {
	auto context = ShaderCache::handle().textureShaderProgram->use();
	glUniform4f(ShaderCache::handle().shaderSpriteColorUniform, color.getRed(), color.getGreen(),
	            color.getBlue(), color.getAlpha());
	std::vector<std::string> lines(splitlines(text));

	auto lineEnd = lines.end();
	auto lineIter = lines.begin();
	while (true) {
		auto charEnd = lineIter->end();
		Mat3 modelviewCopy = modelview;
		for (auto charIter = lineIter->begin(); charIter != charEnd; ++charIter) {
			GetCharacter(charIter, charEnd).draw(modelviewCopy);
		}
		if (++lineIter == lineEnd) {
			break;
		}
		modelview.translate(Pixels(0), Pixels(lineHeight));
	}
}

void FontImpl::print(const ScaleablePixels x, const ScaleablePixels y, const std::string& text) {
	auto context = ShaderCache::handle().textureShaderProgram->use();
	glUniform4f(ShaderCache::handle().shaderSpriteColorUniform, gFontColor.getRed(),
	            gFontColor.getGreen(), gFontColor.getBlue(), gFontColor.getAlpha());
	const int xRounded = static_cast<int>(std::lround(static_cast<double>(Pixels{ x })));
	const int yRounded = static_cast<int>(std::lround(static_cast<double>(Pixels{ y })));
	std::vector<std::string> lines(splitlines(text));

	auto lineEnd = lines.end();
	int lineNr = 0;
	for (auto lineIter = lines.begin(); lineIter != lineEnd; ++lineIter) {
		auto modelview =
		    jngl::modelview().translate(Pixels(xRounded), Pixels(yRounded + lineHeight * lineNr));
		++lineNr;

		auto charEnd = lineIter->end();
		for (auto charIter = lineIter->begin(); charIter != charEnd; ++charIter) {
			GetCharacter(charIter, charEnd).draw(modelview);
		}
	}
}

FT_Library FontImpl::library;
int FontImpl::instanceCounter = 0;
decltype(FontImpl::fileCaches) FontImpl::fileCaches;

} // namespace jngl
