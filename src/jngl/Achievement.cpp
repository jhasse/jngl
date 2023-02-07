// Copyright 2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "Achievement.hpp"

#include "../AchievementLayer.hpp"

namespace jngl {

Achievement::Achievement(std::string id, std::string name, std::string description,
                         std::string icon, int minValue, int maxValue)
: id(std::move(id)), name(std::move(name)), description(std::move(description)), icon(std::move(icon)),
  minValue(minValue), maxValue(maxValue), value(minValue) {
}

void Achievement::setValue(int value) {
	if (this->value < maxValue && value >= maxValue) {
		AchievementLayer::handle().notify(*this);
	}
	this->value = value;
}

}
