/*
Copyright 2007-2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include <png.h> // We need to include it first, I don't know why

#include "spriteimpl.hpp"
#include "window.hpp"
#include "jngl.hpp"
#include "finally.hpp"
#include "windowptr.hpp"
#include "texture.hpp"
#include "main.hpp"
#include "debug.hpp"

#include <map>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <vector>
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

	void setSpriteColor(const unsigned char red, const unsigned char green, const unsigned char blue, const unsigned char alpha) {
		spriteColorRed = red;
		spriteColorGreen = green;
		spriteColorBlue = blue;
		spriteColorAlpha = alpha;
		glColor4ub(spriteColorRed, spriteColorGreen, spriteColorBlue, spriteColorAlpha);
	}

	void setSpriteAlpha(const unsigned char alpha) {
		setSpriteColor(spriteColorRed, spriteColorGreen, spriteColorBlue, alpha);
	}

#ifndef NOJPEG
	struct JpegErrorMgr {
		struct jpeg_error_mgr pub;
		jmp_buf setjmp_buffer;	// for return to caller
	};
	METHODDEF(void)	JpegErrorExit(j_common_ptr info) {
		longjmp(reinterpret_cast<JpegErrorMgr*>(info->err)->setjmp_buffer, 1); // Return control to the setjmp point
	}
#endif

	SpriteImpl::SpriteImpl(const std::string& filename, const bool halfLoad) : texture_(0) {
		FILE* pFile = fopen(filename.c_str(), "rb");
		if (!pFile) {
			throw std::runtime_error(std::string("File not found: " + filename));
		}

		Finally closeFile(boost::bind(fclose, pFile));

		if (boost::algorithm::ends_with(filename, ".webp")) {
#ifdef NOWEBP
			throw std::runtime_error(std::string("Sorry, WebP files not supported in this build of JNGL. (" + filename + ")"
#else
			LoadWebP(filename, pFile, halfLoad);
			return;
#endif
		}

		png_byte buf[PNG_BYTES_TO_CHECK];
		assert(PNG_BYTES_TO_CHECK >= sizeof(unsigned short));

		// Read in some of the signature bytes
		if (fread(buf, 1, PNG_BYTES_TO_CHECK, pFile) != PNG_BYTES_TO_CHECK)
			throw std::runtime_error(std::string("Error reading signature bytes. (" + filename + ")"));

		if (png_sig_cmp(buf, (png_size_t)0, PNG_BYTES_TO_CHECK) == 0)
			LoadPNG(filename, pFile, halfLoad);
		else if (*reinterpret_cast<unsigned short*>(buf) == 19778)
			LoadBMP(filename, pFile, halfLoad);
		else if (*reinterpret_cast<unsigned short*>(buf) == 55551) {
		#ifdef NOJPEG
			throw std::runtime_error(std::string("Sorry, JPEG files not supported in this build of JNGL. (" + filename + ")"));
		#else
			LoadJPG(filename, pFile, halfLoad);
		#endif
		}
		else
			throw std::runtime_error(std::string("Not a PNG, WebP, JPEG or BMP file. (" + filename + ")"));
	}

	SpriteImpl::~SpriteImpl() {
		delete texture_;
	}

	void SpriteImpl::LoadPNG(const std::string& filename, FILE* const fp,
	                         const bool halfLoad) {
		png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (!png_ptr) {
			throw std::runtime_error(std::string("libpng error while reading (" + filename + ")"));
		}

		png_infop info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr) {
			throw std::runtime_error(std::string("libpng error while reading (" + filename + ")"));
		}

		if (setjmp(png_jmpbuf(png_ptr))) {
			// Free all of the memory associated with the png_ptr and info_ptr
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			throw std::runtime_error(std::string("Error reading file. (" + filename + ")"));
		}
		png_init_io(png_ptr, fp);
		png_set_sig_bytes(png_ptr, PNG_BYTES_TO_CHECK);
		int colorType = png_get_color_type(png_ptr, info_ptr);
		if (colorType == PNG_COLOR_TYPE_GRAY || colorType == PNG_COLOR_TYPE_GRAY_ALPHA) {
			png_set_gray_to_rgb(png_ptr);
		}
		png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_STRIP_16, NULL);

		GLenum format;
		int channels = png_get_channels(png_ptr, info_ptr);
		switch (channels) {
			case 1: // gray or palette, we can set channels to 3 because we passed PNG_TRANSFORM_EXPAND to png_read_png
				channels = 3; // we need this value when creating the texture for OpenGL
			case 3:
				format = GL_RGB;
			break;
			case 2: // gray + alpha
				channels = 4;
			case 4:
				format = GL_RGBA;
			break;
			default:
				throw std::runtime_error(std::string("Unsupported number of channels. (" + filename + ")"));
		}

		Finally freePng(boost::bind(png_destroy_read_struct, &png_ptr, &info_ptr, (png_infop*)NULL));
		unsigned char** rowPointers = png_get_rows(png_ptr, info_ptr);
		width_ = static_cast<int>(png_get_image_width(png_ptr, info_ptr));
		height_ = static_cast<int>(png_get_image_height(png_ptr, info_ptr));
		LoadTexture(filename, channels, halfLoad, format, rowPointers);
	}
	void SpriteImpl::CleanUpRowPointers(std::vector<unsigned char*>& buf) {
		for (auto i = buf.begin(); i != buf.end(); ++i) {
			delete[] *i;
		}
	}
	void SpriteImpl::LoadBMP(const std::string& filename, FILE* const fp,
	                         const bool halfLoad) {
		fseek(fp, 10, SEEK_SET);
		BMPHeader header;
		if (!fread(&header, sizeof(header), 1, fp))
			throw std::runtime_error(std::string("Error reading file. (" + filename + ")"));

		if (header.headerSize != 40)
			throw std::runtime_error(std::string("Unsupported header size. (" + filename + ")"));

		if (header.bpp != 24)
			throw std::runtime_error(std::string("Bpp not supported. (" + filename + ")"));

		if (header.compression != 0)
			throw std::runtime_error(std::string("Compression not supported. (" + filename + ")"));

		if (header.dataSize == 0)
			header.dataSize = header.width * header.height * 3;

		std::vector<unsigned char*> buf(header.height);
		for (auto i = buf.begin(); i != buf.end(); ++i) {
			*i = new unsigned char[header.width * 3];
		}
		Finally cleanUp(boost::bind(CleanUpRowPointers, boost::ref(buf)));

		if (header.height < 0) {
			header.height = !header.height;
			for (int i = 0; i < header.height; ++i) {
				if(fseek(fp, header.dataOffset + i * header.width * 3, SEEK_SET) != 0)
					throw std::runtime_error(std::string("Error reading file. (" + filename + ")"));
				if(!fread(buf[i], header.width * 3, 1, fp))
					throw std::runtime_error(std::string("Error reading data. (" + filename + ")"));
			}
		} else { // "bottom-up"-Bitmap
			for (int i = header.height - 1; i >= 0; --i) {
				if (fseek(fp, header.dataOffset + i * header.width * 3, SEEK_SET) != 0)
					throw std::runtime_error(std::string("Error reading file. (" + filename + ")"));
				if (!fread(buf[(header.height - 1) - i], header.width * 3, 1, fp))
					throw std::runtime_error(std::string("Error reading data. (" + filename + ")"));
			}
		}
		LoadTexture(filename, header.bpp / 8, halfLoad, GL_BGR, &buf[0]);
	}
#ifndef NOJPEG
	void SpriteImpl::LoadJPG(const std::string& filename, FILE* file,
	                         const bool halfLoad) {
		fseek(file, 0, SEEK_SET); // Seek to the beginning

		jpeg_decompress_struct info;
		JpegErrorMgr err;
		info.err = jpeg_std_error(&err.pub);
		err.pub.error_exit = JpegErrorExit;

		// Establish the setjmp return context for my_error_exit to use.
		if (setjmp(err.setjmp_buffer)) {
			// If we get here, the JPEG code has signaled an error.
			char buf[JMSG_LENGTH_MAX];
			info.err->format_message(reinterpret_cast<jpeg_common_struct*>(&info), buf);
			jpeg_destroy_decompress(&info);
			throw std::runtime_error(buf);
		}

		jpeg_create_decompress(&info);
		jpeg_stdio_src(&info, file);
		jpeg_read_header(&info, TRUE);
		jpeg_start_decompress(&info);

		width_ = info.output_width;
		height_ = info.output_height;
		int channels = info.num_components;

		GLenum format = GL_RGB;
		if (channels == 4) {
			format = GL_RGBA;
		}

		assert(sizeof(JSAMPLE) == sizeof(char));
		std::vector<unsigned char*> buf(height_);
		for (auto i = buf.begin(); i != buf.end(); ++i) {
			*i = new unsigned char[width_ * channels];
		}
		Finally cleanUp(boost::bind(CleanUpRowPointers, boost::ref(buf)));

		while (info.output_scanline < info.output_height) {
			jpeg_read_scanlines(&info, reinterpret_cast<JSAMPLE**>(&buf[info.output_scanline]), 1);
		}

		jpeg_finish_decompress(&info);

		LoadTexture(filename, channels, halfLoad, format, &buf[0]);
	}
#endif
#ifndef NOWEBP
	void SpriteImpl::LoadWebP(const std::string& filename, FILE* file,
	                         const bool halfLoad) {
		fseek(file, 0, SEEK_END);
		auto filesize = ftell(file);
		fseek(file, 0, SEEK_SET);

		std::vector<uint8_t> buf(filesize);
		if (!fread(&buf[0], filesize, 1, file)) {
			throw std::runtime_error(std::string("Couldn't open WebP file. (" + filename + ")"));
		}

		if (!WebPGetInfo(&buf[0], filesize, &width_, &height_)) {
			throw std::runtime_error(std::string("Invalid WebP file. (" + filename + ")"));
		}

		auto data = WebPDecodeRGBA(&buf[0], filesize, &width_, &height_);
		LoadTexture(filename, 4, halfLoad, GL_RGBA, nullptr, data);
	}
#endif

	void SpriteImpl::LoadTexture(const std::string& filename,
								 int channels,
								 const bool halfLoad,
								 GLenum format,
								 GLubyte** rowPointers,
								 GLubyte* data) {
		if (!pWindow) {
			if (halfLoad) {
				return;
			}
			throw std::runtime_error(std::string("Window hasn't been created yet. (" + filename + ")"));
		}
		texture_ = new Texture(width_, height_, reinterpret_cast<GLubyte**>(&rowPointers[0]),
							   format, channels, data);
	}

	int SpriteImpl::Width() {
		return width_;
	}
	int SpriteImpl::Height() {
		return height_;
	}

	std::map<std::string, boost::shared_ptr<Sprite>> sprites_;

	// halfLoad is used, if we only want to find out the width or height of an image. Load won't throw an exception then
	Sprite& GetSprite(const std::string& filename, const bool halfLoad) {
		std::map<std::string, boost::shared_ptr<Sprite> >::iterator i;
		if ((i = sprites_.find(filename)) == sprites_.end()) { // texture hasn't been loaded yet?
			if (!halfLoad) {
				pWindow.ThrowIfNull();
				Debug("Loading "); Debug(filename); Debug(" ...\n");
			}
			auto s = new Sprite(pathPrefix + filename, halfLoad);
			sprites_[filename].reset(s);
			return *s;
		}
		return *(i->second);
	}

	void draw(const std::string& filename, const double xposition, const double yposition) {
		GetSprite(filename).draw(xposition, yposition);
	}

	void drawScaled(const std::string& filename, const double xposition, const double yposition,
	                const float xfactor, const float yfactor) {
		GetSprite(filename).drawScaled(xposition, yposition, xfactor, yfactor);
	}

	void drawScaled(const std::string& filename, const double xposition, const double yposition,
	                const float factor) {
		GetSprite(filename).drawScaled(xposition, yposition, factor, factor);
	}

	void drawClipped(const std::string& filename, double xposition, double yposition, float xstart, float xend, float ystart, float yend) {
		GetSprite(filename).drawClipped(xposition, yposition, xstart, xend, ystart, yend);
	}

	void loadSprite(const std::string& filename) {
		GetSprite(filename);
	}

	void unload(const std::string& filename) {
		std::map<std::string, boost::shared_ptr<Sprite> >::iterator i;
		if((i = sprites_.find(filename)) != sprites_.end())
			sprites_.erase(i);
	}

	void unloadAll() {
		sprites_.clear();
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

	bool drawButton(const std::string& sprite, const double xposition, const double yposition, const std::string& mouseover) {
		if (xposition <= getMouseX() && getMouseX() < (xposition + getWidth(sprite)) &&
		    yposition <= getMouseY() && getMouseY() < (yposition + getHeight(sprite))) {

			GetSprite(mouseover).draw(xposition, yposition);
			if (mousePressed()) {
				return true;
			}
		} else {
			GetSprite(sprite).draw(xposition, yposition);
		}
		return false;
	}
}
