// Copyright 2007-2022 Jan Niklas Hasse <jhasse@bixense.com>
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
}

void setSpriteColor(Color color) {
	spriteColorRed = color.getRed();
	spriteColorGreen = color.getGreen();
	spriteColorBlue = color.getBlue();
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
}

void setSpriteAlpha(unsigned char alpha) {
	spriteColorAlpha = alpha;
	setSpriteColor(spriteColorRed, spriteColorGreen, spriteColorBlue, alpha);
}

std::unordered_map<std::string_view, std::shared_ptr<Sprite>> sprites_;

// halfLoad is used, if we only want to find out the width or height of an image. Load won't throw
// an exception then
Sprite& GetSprite(std::string_view filename, const Sprite::LoadType loadType) {
	auto it = sprites_.find(filename);
	if (it == sprites_.end()) { // texture hasn't been loaded yet?
		if (loadType != Sprite::LoadType::HALF) {
			pWindow.ThrowIfNull();
		}
		return *sprites_.emplace(filename, std::make_shared<Sprite>(filename, loadType))
		            .first->second;
	}
	return *(it->second);
}

void draw(std::string_view filename, double x, double y) {
	auto& s = GetSprite(filename);
	s.setPos(x, y);
	s.draw();
}

void drawClipped(const std::string& filename, const double xposition, const double yposition,
                 const float xstart, const float xend, const float ystart, const float yend) {
	auto& s = GetSprite(filename);
	s.setPos(xposition + xstart * s.getWidth(), yposition + ystart * s.getHeight());
	s.drawClipped({ xstart, ystart }, { xend, yend });
}

Finally loadSprite(const std::string& filename) {
	auto& loader = GetSprite(filename, Sprite::LoadType::THREADED).loader;
	if (loader) {
		auto rtn = std::move(*loader);
		loader = nullptr;
		return rtn;
	}
	return Finally(nullptr);
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
	Texture::unloadShader();
}

int getWidth(const std::string& filename) {
	const int width = int(std::lround(GetSprite(filename, Sprite::LoadType::HALF).getWidth()));
	if (!pWindow) {
		unload(filename);
	}
	return width;
}

int getHeight(const std::string& filename) {
	const int height = int(std::lround(GetSprite(filename, Sprite::LoadType::HALF).getHeight()));
	if (!pWindow) {
		unload(filename);
	}
	return height;
}

} // namespace jngl
