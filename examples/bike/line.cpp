#include "line.hpp"

#include "wheel.hpp"

Line::Line(jngl::Vec2 start, jngl::Vec2 end) : start_(start), end_(end) {
}

void Line::HandleCollision(Wheel& wheel) const
{
	jngl::Vec2 connection = end_ - start_;
	const double positionOnLine =
	    boost::qvm::dot((wheel.position_ - start_), connection) / boost::qvm::mag(connection);
	if (0 < positionOnLine && positionOnLine < boost::qvm::mag(connection)) {
		boost::qvm::normalize(connection);
		jngl::Vec2 collisionPoint = start_ + (connection * positionOnLine);
		double distanceSq = boost::qvm::mag_sqr(collisionPoint - wheel.position_);
		if(distanceSq < Wheel::radius_ * Wheel::radius_)
		{
			wheel.CollisionWith(collisionPoint);
			return;
		}
	}
	if (boost::qvm::mag_sqr(start_ - wheel.position_) < Wheel::radius_ * Wheel::radius_) {
		wheel.CollisionWith(start_);
	}
	if (boost::qvm::mag_sqr(end_ - wheel.position_) < Wheel::radius_ * Wheel::radius_) {
		wheel.CollisionWith(end_);
	}
}
