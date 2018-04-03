// Copyright 2012-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include <string>
#include <memory>

#include "color.hpp"
#include "dll.hpp"

namespace jngl {
	class FontImpl;

	class Font {
	public:
		JNGLDLL_API Font(const std::string& filename, unsigned int size);
		std::shared_ptr<FontImpl> JNGLDLL_API getImpl();
		void JNGLDLL_API print(const std::string&, int x, int y);
	private:
		std::shared_ptr<FontImpl> impl;
	};

	void JNGLDLL_API print(const std::string& text,
	           int xposition,
	           int yposition);

	int JNGLDLL_API getFontSize();

	void JNGLDLL_API setFontSize(int size);

	std::string JNGLDLL_API getFont();

	void JNGLDLL_API setFont(const std::string& filename);

	void JNGLDLL_API setFontByName(const std::string& name);

	void JNGLDLL_API setFontColor(jngl::Color);

	void JNGLDLL_API setFontColor(unsigned char red,
	                              unsigned char green,
	                              unsigned char blue,
	                              unsigned char alpha = 255);

	void JNGLDLL_API pushFontColor(unsigned char red, unsigned char green, unsigned char blue);

	void JNGLDLL_API popFontColor();

	int JNGLDLL_API getLineHeight();

	void JNGLDLL_API setLineHeight(int);

	double JNGLDLL_API getTextWidth(const std::string& text);
}
