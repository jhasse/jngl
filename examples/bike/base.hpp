#pragma once

#include "bike.hpp"
#include "line.hpp"

#include <jngl/work.hpp>

class Base : public jngl::Work {
public:
	Base();
	void step() override;
	void draw() const override;

private:
	std::vector<Line> lines_;
	Bike bike_;
};
