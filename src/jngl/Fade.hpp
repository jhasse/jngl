// Copyright 2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::Fade class
/// @file
#pragma once

#include "Scene.hpp"

namespace jngl {

/// Fades between two scenes, first fading to black and then fading to the new scene
class Fade : public Scene {
public:
	explicit Fade(std::shared_ptr<Scene>, int speed = 20);
	~Fade() override;
	void step() override;
	void draw() const override;

private:
	void onQuitEvent() override;

	std::shared_ptr<Scene> work;
	std::shared_ptr<Scene> oldWork;
	double fadeCount;
	int speed;
	bool quit = false;
};

} // namespace jngl
