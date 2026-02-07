// Copyright 2007-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#pragma once

#include "jngl/FontInterface.hpp"
#include "jngl/Finally.hpp"
#include "jngl/Mat3.hpp"
#include "jngl/Rgba.hpp"
#include "jngl/ScaleablePixels.hpp"

#include <map>
#include <memory>
#include <string>

#include <ft2build.h> // NOLINT
#include FT_FREETYPE_H
#include FT_STROKER_H

namespace jngl {

class Character;

class FontImpl : public FontInterface {
public:
	FontImpl(const std::string& relativeFilename, unsigned int height, float strokePercentage);
	FontImpl(const FontImpl&) = delete;
	FontImpl& operator=(const FontImpl&) = delete;
	FontImpl(FontImpl&&) = delete;
	FontImpl& operator=(FontImpl&&) = delete;
	~FontImpl() override;
	void print(const Mat3& modelview, std::string_view text) const override;
	void print(Mat3 modelview, const std::string& text, Rgba color) const;
	void print(ScaleablePixels x, ScaleablePixels y, const std::string& text);
	double getTextWidth(std::string_view text) const override;
	double getLineHeight() const override;
	void setLineHeight(Pixels);

private:
	Character& getCharacter(std::string::iterator& it, std::string::iterator end) const;

	static int instanceCounter;
	static FT_Library library;
	FT_Face face = nullptr;
	FT_Stroker stroker = nullptr;
	std::unique_ptr<Finally> freeFace; // Frees face_ if necessary
	unsigned int height_;
	int lineHeight;
	mutable std::map<char32_t, std::shared_ptr<Character>> characters_;
	std::shared_ptr<std::vector<FT_Byte>> bytes;

	static std::map<std::string, std::weak_ptr<std::vector<FT_Byte>>> fileCaches;
};

} // namespace jngl
