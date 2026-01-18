// Copyright 2024-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "OutlinedFont.hpp"

#include "../FontImpl.hpp"
#include "Mat3.hpp"

namespace jngl {

OutlinedFont::OutlinedFont(const std::string& filename, unsigned int size, float strokePercentage)
: inner(filename, size), outer(filename, size, strokePercentage) {
}

void OutlinedFont::print(const Mat3& modelview, const std::string& text, Rgba innerColor,
                         Rgba outerColor) const {
	outer.print(modelview, text, outerColor);
	inner.print(modelview, text, innerColor);
}

void OutlinedFont::printCentered(Mat3 modelview, const std::string& text, Rgba innerColor,
                                 Rgba outerColor) const {
	modelview.translate({ -getTextWidth(text) / 2, 0 });
	outer.print(modelview, text, outerColor);
	inner.print(modelview, text, innerColor);
}

double OutlinedFont::getTextWidth(std::string_view text) const {
	return inner.getTextWidth(text);
}

class BakedFont : public FontInterface {
public:
	BakedFont(std::shared_ptr<const FontImpl> innerFontImpl,
	          std::shared_ptr<const FontImpl> outerFontImpl, jngl::Rgba innerColor,
	          jngl::Rgba outerColor)
	: innerFontImpl(std::move(innerFontImpl)), outerFontImpl(std::move(outerFontImpl)),
	  innerColor(innerColor), outerColor(outerColor) {
	}

	void print(const Mat3& modelview, std::string_view text) const override {
		outerFontImpl->print(modelview, std::string(text), outerColor);
		innerFontImpl->print(modelview, std::string(text), innerColor);
	}

	double getTextWidth(std::string_view text) const override {
		return innerFontImpl->getTextWidth(text);
	}

	double getLineHeight() const override {
		return innerFontImpl->getLineHeight();
	}

private:
	std::shared_ptr<const FontImpl> innerFontImpl;
	std::shared_ptr<const FontImpl> outerFontImpl;
	jngl::Rgba innerColor;
	jngl::Rgba outerColor;
};

std::shared_ptr<FontInterface> OutlinedFont::bake(jngl::Rgba innerColor,
                                                  jngl::Rgba outerColor) const {
	return std::make_shared<BakedFont>(inner.getImpl(), outer.getImpl(), innerColor, outerColor);
}

} // namespace jngl
