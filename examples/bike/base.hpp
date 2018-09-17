#pragma once

#include "bike.hpp"
#include "line.hpp"

class Base {
public:
	Base();
	void DoFrame();
	void Draw();

private:
	std::vector<Line> lines_;
	Bike bike_;
};
