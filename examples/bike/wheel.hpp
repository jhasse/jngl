#ifndef WHEEL_HPP
#define WHEEL_HPP

#include "vector2d.hpp"
#include "line.hpp"

class Wheel
{
public:
	Wheel(double x, double y);
	void Draw();
	void Move();
	void CollisionWith(const Vector2d&);
	Vector2d position_;
	Vector2d speed_;
	const static int radius_;
	Wheel* otherWheel_;
};

#endif
