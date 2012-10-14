/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#include "types.hpp"
#include "drawable.hpp"

#include <memory>
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>

#pragma GCC visibility push(default)
namespace jngl {
	class Texture;

	class Sprite : public Drawable {
	public:
		Sprite(const std::string& filename, bool halfLoad = false);
		int getWidth() const;
		int getHeight() const;
		void step() override;
		void draw() const override;
		void drawScaled(float factor) const;
		void drawScaled(float xfactor, float yfactor) const;
		void drawClipped(float xstart, float xend, float ystart, float yend) const;
	private:
		static void CleanUpRowPointers(std::vector<unsigned char*>& buf);
		void LoadTexture(const std::string& filename, int channels, bool halfLoad, unsigned int format,
		                 unsigned char** rowPointers, unsigned char* data = nullptr);
		void LoadPNG(const std::string& filename, FILE* const fp, const bool halfLoad);
		struct BMPHeader {
			unsigned int dataOffset;
			unsigned int headerSize;
			int width;
			int height;
			unsigned short planes;
			unsigned short bpp;
			unsigned int compression;
			unsigned int dataSize;
		};
 		void LoadBMP(const std::string& filename, FILE* const fp, const bool halfLoad);
#ifndef NOJPEG
		void LoadJPG(const std::string& filename, FILE* file, const bool halfLoad);
#endif
#ifndef NOWEBP
		void LoadWebP(const std::string& filename, FILE* file, const bool halfLoad);
#endif

		boost::shared_ptr<Texture> texture;
	};

	void setSpriteColor(unsigned char red,
	                    unsigned char green,
	                    unsigned char blue,
	                    unsigned char alpha);

	void setSpriteColor(unsigned char red,
	                    unsigned char green,
	                    unsigned char alpha);

	void setSpriteAlpha(unsigned char alpha);

	void pushSpriteAlpha(unsigned char alpha = 255);

	void popSpriteAlpha();
}
#pragma GCC visibility pop