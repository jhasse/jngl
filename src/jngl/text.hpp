/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#include "drawable.hpp"

#include <string>

namespace jngl {
	class Text : public Drawable {
	public:
		Text(const std::string& text);
		void step() override;
		void draw() const override;
	private:
		std::string text;
	};
}