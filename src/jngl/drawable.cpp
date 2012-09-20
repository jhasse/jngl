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

	Float Drawable::getLeft() const {
		return x;
	}

	void Drawable::setLeft(Float x) {
		this->x = x;
	}

	Float Drawable::getTop() const {
		return y;
	}

	void Drawable::setTop(Float y) {
		this->y = y;
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

	int Drawable::getWidth() const {
		return width;
	}

	int Drawable::getHeight() const {
		return height;
	}
}