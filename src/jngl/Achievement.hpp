// Copyright 2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::Achievement class
/// @file
#pragma once

#include <future>
#include <string>

namespace jngl {

class Sprite;

struct Achievement {
	Achievement(std::string id, std::string name, std::string description, std::string icon,
	            int initialValue = 0, int maxValue = 1);

	/// API Name
	std::string id;

	std::string name;
	std::string description;

	/// filename for achievement icon, will be loaded when the achievement is displayed
	std::string icon;

	int maxValue;

	int getValue() const;
	void increaseValue(int by);

private:
	int value;
};

} // namespace jngl
