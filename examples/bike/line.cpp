#include "line.hpp"
#include "wheel.hpp"

Line::Line(Vector2d start, Vector2d end) : start_(start), end_(end)
{
}

void Line::HandleCollision(Wheel& wheel) const
{
	Vector2d connection = end_ - start_;
	double positionOnLine = ((wheel.position_ - start_) * connection) / connection.Length();
	if(0 < positionOnLine && positionOnLine < connection.Length())
	{
		connection.Normalize();
		Vector2d collisionPoint = start_ + (connection * positionOnLine);
		double distanceSq = (collisionPoint - wheel.position_).LengthSq();
		if(distanceSq < Wheel::radius_ * Wheel::radius_)
		{
			wheel.CollisionWith(collisionPoint);
			return;
		}
	}
	if((start_ - wheel.position_).LengthSq() < Wheel::radius_ * Wheel::radius_)
	{
		wheel.CollisionWith(start_);
	}
	if((end_ - wheel.position_).LengthSq() < Wheel::radius_ * Wheel::radius_)
	{
		wheel.CollisionWith(end_);
	}
}
