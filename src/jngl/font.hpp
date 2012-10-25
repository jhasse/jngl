/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#include <string>
#include <boost/shared_ptr.hpp>

#ifndef _MSC_VER
#pragma GCC visibility push(default)
#endif
namespace jngl {
	class FontImpl;

	class Font {
	public:
		Font(const std::string& filename, unsigned int size);
		boost::shared_ptr<FontImpl> getImpl();
		void print(const std::string&, int x, int y);
	private:
		boost::shared_ptr<FontImpl> impl;
	};

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