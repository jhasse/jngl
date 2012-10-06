/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "drawable.hpp"
#include "screen.hpp"

namespace jngl {
	Drawable::Drawable() : x(0), y(0), width(0), height(0) {
	}

	Drawable::~Drawable() {
	}

	void Drawable::setPos(Float x, Float y) {
		this->x = x;
		this->y = y;
	}

	void Drawable::setCenter(Float x, Float y) {
		this->x = x - width/2;
		this->y = y - height/2;
	}

	Float Drawable::getLeft() const {
		return x + getScreenWidth() / 2;
	}

	void Drawable::setLeft(Float x) {
		this->x = x - getScreenWidth() / 2;
	}

	Float Drawable::getTop() const {
		return y + getScreenHeight() / 2;
	}

	void Drawable::setTop(Float y) {
		this->y = y - getScreenHeight() / 2;
	}

	Float Drawable::getBottom() const {
		return getScreenHeight()/2 - y - height;
	}

	void Drawable::setBottom(Float y) {
		this->y = getScreenHeight()/2 - y - height;
	}

	Float Drawable::getRight() const {
		return getScreenWidth()/2 - x - width;
	}

	void Drawable::setRight(Float x) {
		this->x = getScreenWidth()/2 - x - width;
	}

	Float Drawable::getX() const {
		return x;
	}

	void Drawable::setX(Float x) {
		this->x = x;
	}

	Float Drawable::getY() const {
		return y;
	}

	void Drawable::setY(Float y) {
		this->y = y;
	}

	int Drawable::getWidth() const {
		return width;
	}

	int Drawable::getHeight() const {
		return height;
	}
}