#pragma once

#include "line.hpp"
#include "bike.hpp"

#include <vector>

class Base
{
public:
	Base();
	void DoFrame();
	void Draw();
private:
	std::vector<Line> lines_;
	Bike bike_;
};
