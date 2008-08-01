#include "base.hpp"
#include "constants.hpp"

#include <jngl.hpp>

Base::Base()
{
	lines_.push_back(Line(Vector2d(0, 0), Vector2d(screenWidth, 0)));
	lines_.push_back(Line(Vector2d(0, screenHeight), Vector2d(screenWidth, screenHeight)));
	lines_.push_back(Line(Vector2d(screenWidth, 0), Vector2d(screenWidth, screenHeight)));
	lines_.push_back(Line(Vector2d(0, 0), Vector2d(0, screenHeight)));
	lines_.push_back(Line(Vector2d(118, 444), Vector2d(213, 250)));
	lines_.push_back(Line(Vector2d(213, 250), Vector2d(368, 228)));
	lines_.push_back(Line(Vector2d(368, 228), Vector2d(452, 295)));
	lines_.push_back(Line(Vector2d(452, 295), Vector2d(216, 448)));
}

void Base::DoFrame()
{
	for(std::vector<Line>::iterator it = lines_.begin(); it != lines_.end(); ++it)
	{
		bike_.HandleCollision(*it);
	}
	bike_.DoFrame();
}

void Base::Draw()
{
	jngl::Draw("bg.png", 0, 0);
	for(std::vector<Line>::iterator it = lines_.begin(); it != lines_.end(); ++it)
	{
		it->Draw();
	}
	bike_.Draw();
}
