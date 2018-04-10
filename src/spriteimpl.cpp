// Copyright 2007-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "spriteimpl.hpp"

#include "texture.hpp"

namespace jngl {

unsigned char spriteColorRed = 255, spriteColorGreen = 255, spriteColorBlue = 255,
              spriteColorAlpha = 255;

void setSpriteColor(unsigned char red, unsigned char green, unsigned char blue) {
	spriteColorRed = red;
	spriteColorGreen = green;
	spriteColorBlue = blue;
	glColor4ub(red, green, blue, spriteColorAlpha);
}

std::stack<unsigned char> spriteAlphas;

void pushSpriteAlpha(unsigned char alpha) {
	spriteAlphas.push(spriteColorAlpha);
	setSpriteAlpha(spriteColorAlpha * alpha / 255);
}

void popSpriteAlpha() {
	setSpriteAlpha(spriteAlphas.top());
	spriteAlphas.pop();
}

void setSpriteColor(unsigned char red, unsigned char green, unsigned char blue,
                    unsigned char alpha) {
	spriteColorRed = red;
	spriteColorGreen = green;
	spriteColorBlue = blue;
	spriteColorAlpha = alpha;
	glColor4ub(red, green, blue, alpha);
}

void setSpriteAlpha(unsigned char alpha) {
	spriteColorAlpha = alpha;
	setSpriteColor(spriteColorRed, spriteColorGreen, spriteColorBlue, alpha);
}

std::unordered_map<std::string, std::shared_ptr<Sprite>> sprites_;

// halfLoad is used, if we only want to find out the width or height of an image. Load won't throw
// an exception then
Sprite& GetSprite(const std::string& filename, const Sprite::LoadType loadType) {
	auto it = sprites_.find(filename);
	if (it == sprites_.end()) { // texture hasn't been loaded yet?
		if (loadType == Sprite::LoadType::HALF) {
			pWindow.ThrowIfNull();
		}
		auto s = new Sprite(filename, loadType);
		sprites_[filename].reset(s);
		return *s;
	}
	return *(it->second);
}

void draw(const std::string& filename, double x, double y) {
	auto& s = GetSprite(filename);
	s.setPos(x, y);
	s.draw();
}

void drawScaled(const std::string& filename, double x, double y, float xfactor, float yfactor) {
	auto& s = GetSprite(filename);
	s.setPos(x, y);
	s.drawScaled(xfactor, yfactor);
}

void drawScaled(const std::string& filename, double x, double y, float factor) {
	auto& s = GetSprite(filename);
	s.setPos(x, y);
	s.drawScaled(factor);
}

void drawClipped(const std::string& filename, double x, double y, float xstart, float xend,
                 float ystart, float yend) {
	auto& s = GetSprite(filename);
	s.setPos(x, y);
	s.drawClipped(xstart, xend, ystart, yend);
}

Finally loadSprite(const std::string& filename) {
	return std::move(*GetSprite(filename, Sprite::LoadType::THREADED).loader);
}

void unload(const std::string& filename) {
	auto it = sprites_.find(filename);
	if (it != sprites_.end()) {
		sprites_.erase(it);
	}
	auto it2 = textures.find(filename);
	if (it2 != textures.end()) {
		textures.erase(it2);
	}
}

void unloadAll() {
	sprites_.clear();
	textures.clear();
}

int getWidth(const std::string& filename) {
	const int width = GetSprite(filename, Sprite::LoadType::HALF).getWidth();
	if (!pWindow) {
		unload(filename);
	}
	return width;
}

int getHeight(const std::string& filename) {
	const int height = GetSprite(filename, Sprite::LoadType::HALF).getHeight();
	if (!pWindow) {
		unload(filename);
	}
	return height;
}

} // namespace jngl
