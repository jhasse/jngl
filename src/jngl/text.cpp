// Copyright 2012-2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "text.hpp"

#include "../FontImpl.hpp"
#include "../freetype.hpp"
#include "../helper.hpp"
#include "../windowptr.hpp"
#include "Pixels.hpp"
#include "ScaleablePixels.hpp"
#include "font.hpp"
#include "matrix.hpp"
#include "screen.hpp"

namespace jngl {
class Text::Line {
public:
	Line(std::string text, std::shared_ptr<FontImpl> font) : text(std::move(text)) {
		setFont(std::move(font));
	}
	void setFont(std::shared_ptr<FontImpl> font) {
		width = font->getTextWidth(text);
		height = font->getLineHeight();
		this->font = std::move(font);
	}
	void draw(Mat3 modelview) const {
		font->print(modelview.translate(position), text, gFontColor);
	}
	double getWidth() const {
		return static_cast<double>(static_cast<ScaleablePixels>(width));
	}
	double getHeight() const {
		return static_cast<double>(static_cast<ScaleablePixels>(height));
	}
	void setX(double x) {
		position.x = x;
	}
	void setY(double y) {
		position.y = y;
	}

private:
	std::string text;
	std::shared_ptr<FontImpl> font;
	Vec2 position;
	Pixels width{ -1 };
	Pixels height{ -1 };
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
	width *= static_cast<float>(getScaleFactor());
	height *= static_cast<float>(getScaleFactor());
}

void Text::step() {
}

void Text::draw() const {
	draw(modelview());
}

void Text::draw(Mat3 modelview) const {
	auto mv = modelview.translate({ static_cast<double>(static_cast<int>(getX())),
	                                static_cast<double>(static_cast<int>(getY())) });
	for (auto& line : lines) {
		line->draw(mv);
	}
}

} // namespace jngl
