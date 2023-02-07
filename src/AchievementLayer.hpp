// Copyright 2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#pragma once

#include "jngl/job.hpp"

#include <string>

namespace jngl {

struct Achievement;

class AchievementLayer : public Job {
public:
	static AchievementLayer& handle();

	void step() override;
	void draw() const override;

	void notify(const Achievement&);

private:
	std::string achievement;
};

} // namespace jngl
