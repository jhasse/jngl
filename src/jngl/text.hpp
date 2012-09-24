/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#include "drawable.hpp"

#include <string>
#include <boost/shared_ptr.hpp>

namespace jngl {
	class Font;
	class FontImpl;

	class Text : public Drawable {
	public:
		Text(const std::string& text = "");
		void setText(const std::string&);
		void setFont(boost::shared_ptr<Font>);
		void step() override;
		void draw() const override;
	private:
		std::string text;
		boost::shared_ptr<FontImpl> font;
	};
}