/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include <png.h> // We need to include it first, I don't know why

#include "sprite.hpp"

#include "screen.hpp"
#include "../texture.hpp"
#include "../finally.hpp"
#include "../windowptr.hpp"

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string/predicate.hpp>
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

namespace jngl {
	const unsigned int PNG_BYTES_TO_CHECK = 4;

#ifndef NOJPEG
	struct JpegErrorMgr {
		struct jpeg_error_mgr pub;
		jmp_buf setjmp_buffer;	// for return to caller
	};
	METHODDEF(void)	JpegErrorExit(j_common_ptr info) {
		longjmp(reinterpret_cast<JpegErrorMgr*>(info->err)->setjmp_buffer, 1); // Return control to the setjmp point
	}
#endif

	Sprite::Sprite(const std::string& filename, bool halfLoad) : texture(0) {
		FILE* pFile = fopen(filename.c_str(), "rb");
		if (!pFile) {
			throw std::runtime_error(std::string("File not found: " + filename));
		}

		Finally closeFile(boost::bind(fclose, pFile));

		if (boost::algorithm::ends_with(filename, ".webp")) {
		#ifdef NOWEBP
			throw std::runtime_error(std::string("Sorry, WebP files not supported in this build of JNGL. (" + filename + ")"));
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

#ifdef NOPNG
		if (false)
#else
		if (png_sig_cmp(buf, (png_size_t)0, PNG_BYTES_TO_CHECK) == 0)
#endif
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

	Sprite::~Sprite() {
		delete texture;
	}

	int Sprite::getWidth() const {
		return width;
	}

	int Sprite::getHeight() const {
		return height;
	}

	void Sprite::draw(Float x, Float y) const {
		glPushMatrix();
		opengl::translate(x, y);
		texture->draw();
		glPopMatrix();
	}

	void Sprite::drawScaled(Float x, Float y, float factor) const {
		drawScaled(x, y, factor, factor);
	}

	void Sprite::drawScaled(Float x, Float y,
	                        float xfactor, float yfactor) const {
		glPushMatrix();
		opengl::translate(x, y);
		opengl::scale(xfactor, yfactor);
		texture->draw();
		glPopMatrix();
	}

	void Sprite::drawClipped(Float x, Float y,
	                 float xstart, float xend,
	                 float ystart, float yend) const {
		glPushMatrix();
		opengl::translate(x, y);
		texture->drawClipped(xstart, xend, ystart, yend);
		glPopMatrix();
	}

#ifndef NOPNG
	void Sprite::LoadPNG(const std::string& filename, FILE* const fp,
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
		width = static_cast<int>(png_get_image_width(png_ptr, info_ptr));
		height = static_cast<int>(png_get_image_height(png_ptr, info_ptr));
		LoadTexture(filename, channels, halfLoad, format, rowPointers);
	}
#endif

	void Sprite::CleanUpRowPointers(std::vector<unsigned char*>& buf) {
		for (auto i = buf.begin(); i != buf.end(); ++i) {
			delete[] *i;
		}
	}
	void Sprite::LoadBMP(const std::string& filename, FILE* const fp,
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
	void Sprite::LoadJPG(const std::string& filename, FILE* file,
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

		width = info.output_width;
		height = info.output_height;
		int channels = info.num_components;

		GLenum format = GL_RGB;
		if (channels == 4) {
			format = GL_RGBA;
		}

		assert(sizeof(JSAMPLE) == sizeof(char));
		std::vector<unsigned char*> buf(height);
		for (auto i = buf.begin(); i != buf.end(); ++i) {
			*i = new unsigned char[width * channels];
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
	void Sprite::LoadWebP(const std::string& filename, FILE* file,
	                         const bool halfLoad) {
		fseek(file, 0, SEEK_END);
		auto filesize = ftell(file);
		fseek(file, 0, SEEK_SET);

		std::vector<uint8_t> buf(filesize);
		if (!fread(&buf[0], filesize, 1, file)) {
			throw std::runtime_error(std::string("Couldn't open WebP file. (" + filename + ")"));
		}

		if (!WebPGetInfo(&buf[0], filesize, &width, &height)) {
			throw std::runtime_error(std::string("Invalid WebP file. (" + filename + ")"));
		}

		WebPDecoderConfig config;
		WebPInitDecoderConfig(&config);
		if (getScaleFactor() != 1) {
			config.options.use_scaling = true;
			width *= getScaleFactor();
			height *= getScaleFactor();
			if (width < 1) width = 1;
			if (height < 1) height = 1;
			config.options.scaled_width = width;
			config.options.scaled_height = height;
		}
		config.output.colorspace = MODE_RGBA;
		if (WebPDecode(&buf[0], filesize, &config) != VP8_STATUS_OK) {
			throw std::runtime_error(std::string("Can't decode WebP file. (" + filename + ")"));
		}
		Finally _([&]() {
			WebPFreeDecBuffer(&config.output);
		});
		LoadTexture(filename, 4, halfLoad, GL_RGBA, nullptr, config.output.u.RGBA.rgba);
	}
#endif

	void Sprite::LoadTexture(const std::string& filename,
	                         int channels,
	                         const bool halfLoad,
	                         unsigned int format,
	                         unsigned char** rowPointers,
	                         unsigned char* data) {
		if (!pWindow) {
			if (halfLoad) {
				return;
			}
			throw std::runtime_error(std::string("Window hasn't been created yet. (" + filename + ")"));
		}
		texture = new Texture(width, height, reinterpret_cast<unsigned char**>(&rowPointers[0]),
							   format, channels, data);
	}

}