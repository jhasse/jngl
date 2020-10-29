// Copyright 2012-2019 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "text.hpp"

#include "../freetype.hpp"
#include "../helper.hpp"
#include "../spriteimpl.hpp"
#include "font.hpp"
#include "matrix.hpp"
#include "screen.hpp"

namespace jngl {
class Line : public Drawable {
public:
	Line(std::string text, std::shared_ptr<FontImpl> font) : text(std::move(text)) {
		setFont(std::move(font));
	}
	void setFont(std::shared_ptr<FontImpl> font) {
		width = font->getTextWidth(text) * getScaleFactor();
		height = static_cast<float>(font->getLineHeight());
		this->font = std::move(font);
	}
	void step() override {
	}
	void draw() const override {
		font->print(x / getScaleFactor(), y / getScaleFactor(), text);
	}

private:
	std::string text;
	std::shared_ptr<FontImpl> font;
};

Text::Text(const std::string& text) : font(pWindow->getFontImpl()) {
	setText(text);
}

void Text::setText(const std::string& text) {
	lines.clear();
	for (const auto& lineText : splitlines(text)) {
		auto newLine = std::make_shared<Line>(lineText, font);
		lines.emplace_back(std::move(newLine));
	}
	setAlign(align);
}

void Text::setFont(Font& f) {
	font = f.getImpl();
	for (auto& line : lines) {
		line->setFont(font);
	}
	setAlign(align);
}

void Text::setAlign(Alignment a) {
	align = a;
	// Recalculate max width
	width = 0;
	for (const auto& line : lines) {
		if (line->getWidth() > width) {
			width = line->getWidth();
		}
	}
	height = 0;
	for (auto& line : lines) {
		switch (align) {
		case Alignment::LEFT:
			line->setX(0);
			break;
		case Alignment::CENTER:
			line->setX((width - line->getWidth()) / 2.);
			break;
		case Alignment::RIGHT:
			line->setX(width - line->getWidth());
			break;
		}
		line->setY(height);
		height += line->getHeight();
	}
	width *= getScaleFactor();
	height *= getScaleFactor();
}

void Text::step() {
}

void Text::draw() const {
	jngl::pushMatrix();
	jngl::translate(int(x / getScaleFactor()), int(y / getScaleFactor()));
	for (auto& line : lines) {
		line->draw();
	}
	jngl::popMatrix();
}
} // namespace jngl
