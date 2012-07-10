/*
Copyright 2009 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#include <string>

namespace jngl
{
	class Sprite;

	void LoadSprite(const std::string&);
	Sprite& GetSprite(const std::string& filename, const bool halfLoad = false);

	extern unsigned char spriteColorRed, spriteColorGreen, spriteColorBlue, spriteColorAlpha,
						 colorRed,       colorGreen,       colorBlue,       colorAlpha;
};
