#pragma once

#include "wheel.hpp"

#include <vector>

class Line;

class Bike {
public:
	Bike();
	void Draw();
	void DoFrame();
	Vector2d WheelConnection();
	void HandleCollision(const Line&);

private:
	std::vector<Wheel> wheels_;
	Vector2d position_;
};
