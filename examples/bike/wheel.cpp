#include "wheel.hpp"

#include <jngl.hpp>

void Wheel::CollisionWith(const Vector2d& collisionPoint)
{
	Vector2d perpendicular = position_ - collisionPoint;
	perpendicular.Normalize();
	double temp2 = -1.5 * (perpendicular * speed_);
	Vector2d temp = temp2 * perpendicular;
	speed_ += temp;

	double distance = (collisionPoint - position_).Length();
	Vector2d correction = perpendicular * (radius_ - distance + 0.1);
	position_ += correction;
	otherWheel_->position_ += correction;
}

void Wheel::Move()
{
	speed_ += Vector2d(0, 0.04); // Schwerkraft
	speed_ *= 0.99; // Luftreibung
}

void Wheel::Draw()
{
	jngl::draw("wheel", static_cast<int>(position_.X() - radius_),
	           static_cast<int>(position_.Y() - radius_));
}

Wheel::Wheel(const double x, const double y) : position_(x, y)
{
}

const int Wheel::radius_ = 32;
