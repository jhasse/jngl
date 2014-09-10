/*
Copyright 2012-2014 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#include "drawable.hpp"

#include <string>
#include <memory>

#include "dll.hpp"

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
		JNGLDLL_API Text(const std::string& text = "");
		void JNGLDLL_API setText(const std::string&);
		void JNGLDLL_API setFont(std::shared_ptr<Font>);
		void JNGLDLL_API setAlign(Alignment);
		void JNGLDLL_API step();
		void JNGLDLL_API draw() const;
	private:
		std::string text;
		std::shared_ptr<FontImpl> font;
		Alignment align;
	};
}
#ifndef _MSC_VER
#pragma GCC visibility pop
#endif
