/*
Copyright 2009-2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#include "opengl.hpp"
#include "windowptr.hpp"
#include "texture.hpp"
#include "jngl/sprite.hpp"

#include <string>
#include <boost/noncopyable.hpp>

namespace jngl
{
	class SpriteImpl : boost::noncopyable {
	public:
		SpriteImpl(const std::string& filename, const bool halfLoad);
		~SpriteImpl();
		void LoadTexture(const std::string& filename, int channels, bool halfLoad, GLenum format,
						 GLubyte** rowPointers, GLubyte* data = nullptr);
		void LoadPNG(const std::string& filename, FILE* const fp, const bool halfLoad);
		struct BMPHeader
		{
			unsigned int dataOffset;
			unsigned int headerSize;
			int width;
			int height;
			unsigned short planes;
			unsigned short bpp;
			unsigned int compression;
			unsigned int dataSize;
		};
		static void CleanUpRowPointers(std::vector<unsigned char*>& buf);
 		void LoadBMP(const std::string& filename, FILE* const fp, const bool halfLoad);
#ifndef NOJPEG
		void LoadJPG(const std::string& filename, FILE* file, const bool halfLoad);
#endif
#ifndef NOWEBP
		void LoadWebP(const std::string& filename, FILE* file, const bool halfLoad);
#endif
		int Width();
		int Height();
		template<class T>
		void Draw(const T xposition, const T yposition) {
			glPushMatrix();
			opengl::translate(xposition, yposition);
			texture_->Draw();
			glPopMatrix();
		}
		template<class T>
		void DrawScaled(const T xposition, const T yposition, const float xfactor, const float yfactor) {
			glPushMatrix();
			opengl::translate(xposition, yposition);
			opengl::scale(xfactor, yfactor);
			texture_->Draw();
			glPopMatrix();
		}
		template<class T>
		void DrawClipped(const T x, const T y,
		                 const float xstart, const float xend,
		                 const float ystart, const float yend) {
			glPushMatrix();
			opengl::translate(x, y);
			texture_->drawClipped(xstart, xend, ystart, yend);
			glPopMatrix();
		}
	private:
		Texture* texture_;
		int width_, height_;
		const static unsigned int PNG_BYTES_TO_CHECK = 4;
	};

	void loadSprite(const std::string&);
	Sprite& GetSprite(const std::string& filename, const bool halfLoad = false);

	extern unsigned char spriteColorRed, spriteColorGreen, spriteColorBlue, spriteColorAlpha,
						 colorRed,       colorGreen,       colorBlue,       colorAlpha;
};
