// Copyright 2012-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "drawable.hpp"
#include "screen.hpp"

namespace jngl {

Drawable::Drawable() : x(0), y(0), width(0), height(0) {
}

Drawable::~Drawable() = default;

void Drawable::setPos(Float x, Float y) {
	setX(x);
	setY(y);
}

void Drawable::setCenter(Float x, Float y) {
	setX(x - getWidth() / 2);
	setY(y - getHeight() / 2);
}

Float Drawable::getLeft() const {
	return getX() + getScreenWidth() / 2;
}

void Drawable::setLeft(Float x) {
	setX(x - getScreenWidth() / 2);
}

Float Drawable::getTop() const {
	return getY() + getScreenHeight() / 2;
}

void Drawable::setTop(Float y) {
	setY(y - getScreenHeight() / 2);
}

Float Drawable::getBottom() const {
	return getScreenHeight() / 2 - getY() - getHeight();
}

void Drawable::setBottom(Float y) {
	setY(getScreenHeight() / 2 - y - getHeight());
}

Float Drawable::getRight() const {
	return getScreenWidth() / 2 - getX() - getWidth();
}

void Drawable::setRight(Float x) {
	setX(getScreenWidth() / 2 - x - getWidth());
}

Float Drawable::getX() const {
	return x / getScaleFactor();
}

void Drawable::setX(Float x) {
	this->x = x * getScaleFactor();
}

Float Drawable::getY() const {
	return y / getScaleFactor();
}

void Drawable::setY(Float y) {
	this->y = y * getScaleFactor();
}

float Drawable::getWidth() const {
	return width / getScaleFactor();
}

float Drawable::getHeight() const {
	return height / getScaleFactor();
}

} // namespace jngl
