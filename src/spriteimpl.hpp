/*
Copyright 2009-2015 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#include "opengl.hpp"
#include "windowptr.hpp"
#include "texture.hpp"
#include "jngl/sprite.hpp"

#include <string>
#include <boost/noncopyable.hpp>

namespace jngl {
	void loadSprite(const std::string&);
	Sprite& GetSprite(const std::string& filename, const bool halfLoad = false);

	extern unsigned char spriteColorRed, spriteColorGreen, spriteColorBlue, spriteColorAlpha,
	                     colorRed,       colorGreen,       colorBlue,       colorAlpha;
}
