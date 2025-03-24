// Copyright 2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::Fade class
/// @file
#pragma once

#include "work.hpp"

namespace jngl {

/// Fades between two scenes, first fading to black and then fading to the new scene
class Fade : public jngl::Work {
public:
	explicit Fade(std::shared_ptr<jngl::Work>, int speed = 20);
	~Fade() override;
	void step() override;
	void draw() const override;

private:
	void onQuitEvent() override;

	std::shared_ptr<Work> work;
	std::shared_ptr<jngl::Work> oldWork;
	double fadeCount;
	int speed;
	bool quit = false;
};

} // namespace jngl
