#include "bike.hpp"

#include <jngl.hpp>
#include <iostream>

void Bike::HandleCollision(const Line& line)
{
	for(std::vector<Wheel>::iterator it = wheels_.begin(); it != wheels_.end(); ++it)
	{
		line.HandleCollision(*it);
	}
}

void DisplayVector2d(Vector2d vec)
{
	std::cout << "X: " << vec.X() << "   Y: " << vec.Y() << "   Length: " << vec.Length() << std::endl;
}

void Bike::DoFrame()
{
	for (auto& wheel : wheels_) {
		wheel.Move();
	}
	Vector2d connection = wheels_[1].position_ - wheels_[0].position_;
//  	std::cout << connection.Length() << std::endl;
	connection.Normalize();
	connection.Set(connection.Y() * 0.1, connection.X() * 0.1); // Um 90 Grad drehen und 50% Intensität
	if (jngl::keyDown(jngl::key::Right)) {
		for (auto& wheel : wheels_) {
			wheel.speed_ += Vector2d(0.4, 0);
		}
		wheels_[0].speed_ += connection; // Dann hoch damit!
		wheels_[1].speed_ -= connection; // Dann hoch damit!
	}
	if (jngl::keyDown(jngl::key::Left)) {
		for (auto& wheel : wheels_) {
			wheel.speed_ -= Vector2d(0.4, 0);
		}
		wheels_[0].speed_ -= connection; // Dann hoch damit!
		wheels_[1].speed_ += connection; // Dann hoch damit!
	}
	std::vector<Vector2d> correction(2);
	for (size_t i = 0; i < wheels_.size(); ++i) {
		Vector2d connection = wheels_[i].position_ - wheels_[1-i].position_;
		connection.Normalize();
		double projection = connection * wheels_[i].speed_;
		correction[i] = (connection * projection);
// 		std::cout << wheels_[i].speed_.X() << " " << projection << " " << correction[i].X() << "  "; DisplayVector2d(connection);
	}
	for (size_t i = 0; i < wheels_.size(); ++i) {
// 		DisplayVector2d(wheels_[i].speed_);
 		wheels_[i].speed_ -= correction[i];
		wheels_[i].speed_ += (correction[i] + correction[1-i]) / 2;
//  		DisplayVector2d(wheels_[i].speed_);
	}
// 	std::cout << "\n" << std::endl;
	for (size_t i = 0; i < wheels_.size(); ++i) {
		wheels_[i].position_ += wheels_[i].speed_; // Bewegen
	}
	connection = wheels_[1].position_ - wheels_[0].position_;
	connection.Normalize();
	wheels_[1].position_ =  wheels_[0].position_ + connection * 128;
}

Bike::Bike()
{
	wheels_.push_back(Wheel(100, 100));
	wheels_.push_back(Wheel(228, 100));
	wheels_[0].otherWheel_ = &wheels_[1];
	wheels_[1].otherWheel_ = &wheels_[0];
}

void Bike::Draw()
{
	for (auto& wheel : wheels_) {
		wheel.Draw();
	}
}
