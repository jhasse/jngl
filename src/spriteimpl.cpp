// Copyright 2007-2017 Jan Niklas Hasse <jhasse@gmail.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include <png.h> // We need to include it first, I don't know why

#include "spriteimpl.hpp"
#include "window.hpp"
#include "jngl/Finally.hpp"
#include "windowptr.hpp"
#include "texture.hpp"
#include "main.hpp"

#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/function.hpp>
#include <stack>
#include <stdexcept>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#ifdef _WIN32
	// These defines are needed to prevent conflicting types declarations in jpeglib.h:
	#define XMD_H
	#define HAVE_BOOLEAN
#endif
#ifndef NOJPEG
	extern "C" {
		#include <jpeglib.h>
	}
#endif
#ifndef NOWEBP
	#include <webp/decode.h>
#endif

namespace jngl
{
	unsigned char spriteColorRed = 255, spriteColorGreen = 255, spriteColorBlue = 255, spriteColorAlpha = 255;

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

	void setSpriteColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha) {
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

	boost::unordered_map<std::string, std::shared_ptr<Sprite>> sprites_;

	// halfLoad is used, if we only want to find out the width or height of an image. Load won't throw an exception then
	Sprite& GetSprite(const std::string& filename, const bool halfLoad) {
		auto it = sprites_.find(filename);
		if (it == sprites_.end()) { // texture hasn't been loaded yet?
			if (!halfLoad) {
				pWindow.ThrowIfNull();
			}
			auto s = new Sprite(filename, halfLoad);
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

	void drawClipped(const std::string& filename, double x, double y, float xstart, float xend, float ystart, float yend) {
		auto& s = GetSprite(filename);
		s.setPos(x, y);
		s.drawClipped(xstart, xend, ystart, yend);
	}

	Finally loadSprite(const std::string& filename) {
		return std::move(*GetSprite(filename).loader);
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
		const int width = GetSprite(filename, true).getWidth();
		if (!pWindow) {
			unload(filename);
		}
		return width;
	}

	int getHeight(const std::string& filename) {
		const int height = GetSprite(filename, true).getHeight();
		if (!pWindow) {
			unload(filename);
		}
		return height;
	}
}
