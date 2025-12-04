// Copyright 2008-2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#pragma once

#include "bike.hpp"
#include "line.hpp"

#include <jngl/Scene.hpp>

class Base : public jngl::Scene {
public:
	Base();
	void step() override;
	void draw() const override;

private:
	std::vector<Line> lines_;
	Bike bike_;
};
