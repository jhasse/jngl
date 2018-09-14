#pragma once

#include "vector2d.hpp"

class Wheel {
public:
	Wheel(double x, double y);
	void Draw();
	void Move();
	void CollisionWith(const Vector2d&);
	Vector2d position_;
	Vector2d speed_;
	const static int radius_;
	Wheel* otherWheel_ = nullptr;
};
