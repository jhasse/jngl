#pragma once

#include <jngl/Vec2.hpp>

class Wheel;

class Line
{
public:
	Line(jngl::Vec2 start, jngl::Vec2 end);
	void HandleCollision(Wheel&) const;
	jngl::Vec2 start_;
	jngl::Vec2 end_;
};
