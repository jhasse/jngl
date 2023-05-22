// Copyright 2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#pragma once

#include "jngl/job.hpp"

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

	void notify(const Achievement&, int oldValue, int newValue);

private:
	const Achievement* achievement = nullptr;
	float fadeIn = 2.f;

	float value = 0;
	int targetValue = 0;
	int maxValue = 0;
	int stepsPassed = 0;
	float colorFade = 0;

	std::vector<std::pair<const Achievement*, std::function<void()>>> queue;
};

} // namespace jngl
