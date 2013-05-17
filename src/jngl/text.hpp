/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#include "drawable.hpp"

#include <string>
#include <memory>

#ifndef _MSC_VER
#pragma GCC visibility push(default)
#endif
namespace jngl {
	enum Alignment {
		LEFT,
		RIGHT,
		CENTER
	};

	class Font;
	class FontImpl;

	class Text : public Drawable {
	public:
		Text(const std::string& text = "");
		void setText(const std::string&);
		void setFont(std::shared_ptr<Font>);
		void setAlign(Alignment);
		void step();
		void draw() const;
	private:
		std::string text;
		std::shared_ptr<FontImpl> font;
		Alignment align;
	};
}
#ifndef _MSC_VER
#pragma GCC visibility pop
#endif