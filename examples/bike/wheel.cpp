#include "wheel.hpp"

#include <jngl.hpp>

void Wheel::CollisionWith(const jngl::Vec2& collisionPoint) {
	jngl::Vec2 perpendicular = position_ - collisionPoint;
	boost::qvm::normalize(perpendicular);
	double temp2 = -1.5 * boost::qvm::dot(perpendicular, speed_);
	jngl::Vec2 temp = temp2 * perpendicular;
	speed_ += temp;

	const double distance = boost::qvm::mag(collisionPoint - position_);
	jngl::Vec2 correction = perpendicular * (radius_ - distance + 0.1);
	position_ += correction;
	otherWheel_->position_ += correction;
}

void Wheel::Move()
{
	speed_ += jngl::Vec2(0, 0.04); // Schwerkraft
	speed_ *= 0.99; // Luftreibung
}

void Wheel::draw() const {
	jngl::draw("wheel", static_cast<int>(position_.x - radius_),
	           static_cast<int>(position_.y - radius_));
}

Wheel::Wheel(const double x, const double y) : position_(x, y)
{
}

const int Wheel::radius_ = 32;
