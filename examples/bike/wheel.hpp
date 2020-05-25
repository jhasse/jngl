#pragma once

#include <jngl/Vec2.hpp>

class Wheel {
public:
	Wheel(double x, double y);
	void draw() const;
	void Move();
	void CollisionWith(const jngl::Vec2&);
	jngl::Vec2 position_;
	jngl::Vec2 speed_;
	const static int radius_;
	Wheel* otherWheel_ = nullptr;
};
