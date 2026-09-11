// Copyright 2012-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "text.hpp"

#include "../FontImpl.hpp"
#include "../helper.hpp"
#include "../windowptr.hpp"
#include "FontInterface.hpp"
#include "font.hpp"
#include "matrix.hpp"
#include "screen.hpp"

namespace jngl {
class Text::Line {
public:
	Line(std::string text, FontInterface& font) : text(std::move(text)) {
		setFont(font);
	}
	void setFont(FontInterface& font) {
		width = font.getTextWidth(text);
		height = font.getLineHeight();
		this->font = &font;
	}
	void setText(std::string text) {
		if (std::ranges::find(text, '\n') != text.end()) {
			throw std::runtime_error("Line must not contain \\n");
		}
		width = font->getTextWidth(text);
		this->text = std::move(text);
	}
	void draw(Mat3 modelview) const {
		font->print(modelview.translate(position), text);
	}
	double getWidth() const {
		assert(!std::isnan(width));
		return width;
	}
	double getHeight() const {
		assert(!std::isnan(height));
		return height;
	}
	void setX(double x) {
		position.x = x;
	}
	void setY(double y) {
		position.y = y;
	}

private:
	std::string text;
	const FontInterface* font = nullptr;
	Vec2 position;
	double width = std::numeric_limits<double>::quiet_NaN();
	double height = std::numeric_limits<double>::quiet_NaN();
};

Text::Text(const std::string& text) : font(text.empty() ? nullptr : pWindow->getFontImpl()) {
	if (!text.empty()) {
		setText(text);
	}
}

void Text::setText(const std::string& text, double maxWidth) {
	if (!font) {
		font = pWindow->getFontImpl();
	}
	lines.clear();
	for (const auto& lineText : splitlines(text)) {
		addLine(lineText, maxWidth);
	}
	setAlign(align);
}

void Text::setFont(Font& f) {
	setFont(f.getImpl());
}

void Text::setFont(const std::shared_ptr<FontInterface>& f) {
	font = f;
	for (auto& line : lines) {
		line->setFont(*font);
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

void Text::addLine(const std::string& text, double maxWidth) {
	auto newLine = std::make_shared<Line>(text, *font);
	if (newLine->getWidth() > maxWidth) {
		auto currentPos = text.begin();
		while (true) {
			auto spacePos = std::find(currentPos, text.end(), ' ');
			bool undo = (spacePos == text.end());
			if (!undo) {
				newLine->setText(std::string(text.begin(), spacePos));
				undo = (newLine->getWidth() > maxWidth);
			}
			if (undo) {
				if (currentPos == text.begin()) {
					// the line starts with a very long word. Just add it anyway:
					lines.emplace_back(std::move(newLine));
					return;
				}
				// we went too far, undo
				newLine->setText(std::string(text.begin(), currentPos - 1));
				break;
			}
			currentPos = spacePos + 1;
		}
		lines.emplace_back(std::move(newLine));
		addLine(std::string(currentPos, text.end()), maxWidth);
	} else {
		lines.emplace_back(std::move(newLine));
	}
}

} // namespace jngl
