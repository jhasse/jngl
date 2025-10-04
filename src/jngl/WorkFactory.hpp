// Copyright 2024-2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// @file
#pragma once

#include "Scene.hpp"

namespace jngl {

class WorkFactory : public jngl::Scene {
public:
	explicit WorkFactory(std::function<std::shared_ptr<Scene>()>);

private:
	void step() override;
	void draw() const override;
	void onLoad() override;

	std::shared_ptr<Scene> work;
	std::function<std::shared_ptr<Scene>()> factory;
};

} // namespace jngl
