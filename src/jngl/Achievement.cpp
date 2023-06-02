// Copyright 2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "Achievement.hpp"

#include "../AchievementLayer.hpp"

#include <cassert>

namespace jngl {

Achievement::Achievement(std::string id, std::string name, std::string description,
                         std::string icon, int initialvalue, int maxValue)
: id(std::move(id)), name(std::move(name)), description(std::move(description)),
  icon(std::move(icon)), maxValue(maxValue), value(initialvalue) {
}

int Achievement::getValue() const {
	return value;
}

void Achievement::increaseValue(int by) {
	assert(by >= 0);
	if ((value < maxValue && by > 0) || (value >= maxValue && by >= 0)) {
		AchievementLayer::handle().notify(*this, value, value + by);
	}
	value += by;
}

} // namespace jngl
