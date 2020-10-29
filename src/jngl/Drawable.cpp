// Copyright 2012-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "Drawable.hpp"

#include "Color.hpp"
#include "screen.hpp"
#include "shapes.hpp"

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

Vec2 Drawable::getSize() const {
	return { double(width) / getScaleFactor(), double(height) / getScaleFactor() };
}

float Drawable::getWidth() const {
	return width / static_cast<float>(getScaleFactor());
}

float Drawable::getHeight() const {
	return height / static_cast<float>(getScaleFactor());
}

void Drawable::drawBoundingBox() const {
	setColor(Color(255, 0, 0));
	const double LINE_WIDTH = 2;
	drawRect(getX() - LINE_WIDTH / 2, getY() - LINE_WIDTH / 2, LINE_WIDTH + getWidth(), LINE_WIDTH);
	drawRect(getX() - LINE_WIDTH / 2, getY() - LINE_WIDTH / 2, LINE_WIDTH,
	         LINE_WIDTH + getHeight());
	drawRect(getX() - LINE_WIDTH / 2, getY() - LINE_WIDTH / 2 + getHeight(),
	         LINE_WIDTH + getWidth(), LINE_WIDTH);
	drawRect(getX() - LINE_WIDTH / 2 + getWidth(), getY() - LINE_WIDTH / 2, LINE_WIDTH,
	         LINE_WIDTH + getHeight());
}

bool Drawable::contains(const jngl::Vec2 point) const {
	return (x - getWidth() / 2 <= point.x && point.x < (x + getWidth() / 2) &&
	        y - getHeight() / 2 <= point.y && point.y < (y + getHeight() / 2));
}

} // namespace jngl
