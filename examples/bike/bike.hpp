#pragma once

#include "wheel.hpp"

#include <vector>

class Line;

class Bike {
public:
	Bike();
	void draw() const;
	void DoFrame();
	jngl::Vec2 WheelConnection();
	void HandleCollision(const Line&);

private:
	std::vector<Wheel> wheels_;
	jngl::Vec2 position_;
};
