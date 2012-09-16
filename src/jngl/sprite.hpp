/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#include "types.hpp"

#include <memory>
#include <boost/noncopyable.hpp>
#include <string>
#include <vector>

namespace jngl {
	class Texture;

	class Sprite : boost::noncopyable {
	public:
		Sprite(const std::string& filename, bool halfLoad);
		~Sprite();
		int getWidth() const;
		int getHeight() const;
		void draw(Float x, Float y) const;
		void drawScaled(Float x, Float y, float factor) const;
		void drawScaled(Float x, Float y, float xfactor, float yfactor) const;
		void drawClipped(Float x, Float y,
		                 float xstart, float xend,
		                 float ystart, float yend) const;
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

		Texture* texture;
		int width;
		int height;
	};

	void setSpriteColor(unsigned char red,
	                    unsigned char green,
	                    unsigned char blue,
	                    unsigned char alpha);

	void setSpriteColor(unsigned char red,
	                    unsigned char green,
	                    unsigned char alpha);

	void setSpriteAlpha(unsigned char alpha);
}