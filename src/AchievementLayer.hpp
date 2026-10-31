// Copyright 2023-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#pragma once

#include "jngl/Job.hpp"

#include <functional>
#include <string>
#include <vector>

namespace jngl {

struct Achievement;

class AchievementLayer : public Job {
public:
	static AchievementLayer& handle();

	void step() override;
	void draw() const override;

	virtual void notify(const Achievement&, int oldValue, int newValue);

	/// see jngl::setAchievementProgressSeparator
	void setProgressSeparator(std::string);

private:
	const Achievement* achievement = nullptr;
	float fadeIn = 2.f;

	float value = 0;
	int targetValue = 0;
	int maxValue = 0;
	int stepsPassed = 0;
	float colorFade = 0;

	/// between the current and the maximum value, e.g. "5 of 20"
	std::string progressSeparator = " of ";

	std::vector<std::pair<const Achievement*, std::function<void()>>> queue;
};

} // namespace jngl
