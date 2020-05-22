// Copyright 2012-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "Drawable.hpp"
#include "screen.hpp"

namespace jngl {

Drawable::Drawable() = default;

Drawable::~Drawable() = default;

void Drawable::setPos(const double x, const double y) {
	setX(x);
	setY(y);
}

void Drawable::setCenter(const double x, const double y) {
	setX(x - getWidth() / 2);
	setY(y - getHeight() / 2);
}

double Drawable::getLeft() const {
	return getX() + getScreenWidth() / 2;
}

void Drawable::setLeft(const double x) {
	setX(x - getScreenWidth() / 2);
}

double Drawable::getTop() const {
	return getY() + getScreenHeight() / 2;
}

void Drawable::setTop(const double y) {
	setY(y - getScreenHeight() / 2);
}

double Drawable::getBottom() const {
	return getScreenHeight() / 2 - getY() - getHeight();
}

void Drawable::setBottom(const double y) {
	setY(getScreenHeight() / 2 - y - getHeight());
}

double Drawable::getRight() const {
	return getScreenWidth() / 2 - getX() - getWidth();
}

void Drawable::setRight(const double x) {
	setX(getScreenWidth() / 2 - x - getWidth());
}

double Drawable::getX() const {
	return x / getScaleFactor();
}

void Drawable::setX(const double x) {
	this->x = x * getScaleFactor();
}

double Drawable::getY() const {
	return y / getScaleFactor();
}

void Drawable::setY(const double y) {
	this->y = y * getScaleFactor();
}

float Drawable::getWidth() const {
	return width / static_cast<float>(getScaleFactor());
}

float Drawable::getHeight() const {
	return height / static_cast<float>(getScaleFactor());
}

} // namespace jngl
