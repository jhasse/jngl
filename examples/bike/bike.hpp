#ifndef BIKE_HPP
#define BIKE_HPP

#include "wheel.hpp"
#include "line.hpp"

#include <vector>

class Bike
{
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

#endif
