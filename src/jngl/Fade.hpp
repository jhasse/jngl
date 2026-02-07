// Copyright 2025-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::Fade class
/// @file
#pragma once

#include "Scene.hpp"

#include <optional>

namespace jngl {

/// Fades between two scenes, first fading to jngl::getBackgroundColor() and then fading to the new
/// scene
class Fade : public Scene {
public:
	struct Options {
		int speed = 20;

		/// If true, only fades out and then immediately switches to the new scene
		bool fadeOutOnly = false;
	};

	explicit Fade(std::shared_ptr<Scene>, std::optional<Options> options = {});

	/// Uses factory() to get the new Scene when the screen has faded to black (or the background
	/// color). This is useful when the ctor of your new Scene might take a long time (e.g. loading
	/// assets) and you don't want the lag to be noticable.
	explicit Fade(std::function<std::shared_ptr<Scene>()> factory,
	              std::optional<Options> options = {});

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
	bool fadeOutOnly;
};

} // namespace jngl
