/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#include <string>
#include <memory>

#ifndef _MSC_VER
#pragma GCC visibility push(default)
#endif
namespace jngl {
	class FontImpl;

	class Font {
	public:
		Font(const std::string& filename, unsigned int size);
		std::shared_ptr<FontImpl> getImpl();
		void print(const std::string&, int x, int y);
	private:
		std::shared_ptr<FontImpl> impl;
	};

	void print(const std::string& text,
	           int xposition,
	           int yposition);

	int getFontSize();

	void setFontSize(int size);

	std::string getFont();

	void setFont(const std::string& filename);

	void setFontByName(const std::string& name);

	void setFontColor(unsigned char red,
	                  unsigned char green,
	                  unsigned char blue,
	                  unsigned char alpha = 255);

	void pushFontColor(unsigned char red, unsigned char green, unsigned char blue);

	void popFontColor();

	int getLineHeight();

	void setLineHeight(int);

	double getTextWidth(const std::string& text);
}
#ifndef _MSC_VER
#pragma GCC visibility pop
#endif