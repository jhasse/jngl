// Copyright 2012-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "Finally.hpp"
#include "drawable.hpp"

#include <memory>
#include <string>
#include <vector>

#include "dll.hpp"

namespace jngl {
	class Texture;

	class Sprite : public Drawable {
	public:
		enum class LoadType {
			NORMAL,
			HALF,
			THREADED,
		};

		JNGLDLL_API Sprite(const char* bytes, size_t width, size_t height);
		JNGLDLL_API Sprite(const std::string& filename, LoadType loadType = LoadType::NORMAL);
		void JNGLDLL_API step();
		void JNGLDLL_API draw() const;
		void JNGLDLL_API drawScaled(float factor) const;
		void JNGLDLL_API drawScaled(float xfactor, float yfactor) const;
		void JNGLDLL_API drawClipped(float xstart, float xend, float ystart, float yend) const;

		std::shared_ptr<Finally> loader;

	private:
		static void cleanUpRowPointers(std::vector<unsigned char*>& buf);
		void loadTexture(const std::string& filename, bool halfLoad, unsigned int format,
		                 const unsigned char* const* rowPointers,
		                 const unsigned char* data = nullptr);
		Finally LoadPNG(const std::string& filename, FILE* const fp, const bool halfLoad);
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
 		Finally LoadBMP(const std::string& filename, FILE* const fp, const bool halfLoad);
#ifndef NOJPEG
		Finally LoadJPG(const std::string& filename, FILE* file, const bool halfLoad);
#endif
#ifndef NOWEBP
		Finally LoadWebP(const std::string& filename, FILE* file, const bool halfLoad);
#endif

		std::shared_ptr<Texture> texture;
	};

	void JNGLDLL_API draw(const std::string& filename, double x, double y);

	template<class Vect>
	void draw(const std::string& filename, Vect pos) {
		draw(filename, pos.x, pos.y);
	}

	Finally JNGLDLL_API load(const std::string& filename);

	void JNGLDLL_API unload(const std::string& filename);

	void JNGLDLL_API unloadAll();

	void JNGLDLL_API drawScaled(const std::string& filename, double xposition, double yposition,
	                            float xfactor, float yfactor);

	void JNGLDLL_API drawScaled(const std::string& filename, double xposition, double yposition,
	                            float factor);

	void JNGLDLL_API drawClipped(const std::string& filename, double xposition, double yposition,
	                             float xstart, float xend, float ystart, float yend);

	void JNGLDLL_API setSpriteColor(unsigned char red, unsigned char green, unsigned char blue,
	                                unsigned char alpha);

	void JNGLDLL_API setSpriteColor(unsigned char red, unsigned char green, unsigned char blue);

	void JNGLDLL_API setSpriteAlpha(unsigned char alpha);

	void JNGLDLL_API pushSpriteAlpha(unsigned char alpha = 255);

	void JNGLDLL_API popSpriteAlpha();

	int JNGLDLL_API getWidth(const std::string& filename);

	int JNGLDLL_API getHeight(const std::string& filename);

	void JNGLDLL_API setMasking(bool enabled);
}
