/*
Copyright 2007-2009 Jan Niklas Hasse <jhasse@gmail.com>

This file is part of JNGL.

JNGL is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

JNGL is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with JNGL.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <png.h> // We need to include it first, I don't know why

#include "texture.hpp"
#include "window.hpp"
#include "jngl.hpp"
#include "finally.hpp"
#include "windowptr.hpp"
#include "opengl.hpp"

#include <map>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#ifndef linux
// These defines are needed to prevent conflicting types declarations in jpeglib.h:
#define XMD_H
#define HAVE_BOOLEAN
#endif
#ifndef NOJPEG
extern "C" {
#include <jpeglib.h>
}
#endif

namespace jngl
{
	unsigned char spriteColorRed = 255, spriteColorGreen = 255, spriteColorBlue = 255, spriteColorAlpha = 255;

	void SetSpriteColor(const unsigned char red, const unsigned char green, const unsigned char blue, const unsigned char alpha)
	{
		spriteColorRed = red;
		spriteColorGreen = green;
		spriteColorBlue = blue;
		spriteColorAlpha = alpha;
		glColor4ub(spriteColorRed, spriteColorGreen, spriteColorBlue, spriteColorAlpha);
	}
#ifndef NOJPEG
	struct JpegErrorMgr
	{
		struct jpeg_error_mgr pub;
		jmp_buf setjmp_buffer;	// for return to caller
	};
	METHODDEF(void)	JpegErrorExit(j_common_ptr info)
	{
		longjmp(reinterpret_cast<JpegErrorMgr*>(info->err)->setjmp_buffer, 1); // Return control to the setjmp point
	}
#endif

	class Texture : boost::noncopyable {
	public:
		Texture(const std::string& filename, const bool halfLoad) : vertexBuffer_(0)
		{
			FILE* pFile = fopen(filename.c_str(), "rb");
			if(!pFile)
				throw std::runtime_error(std::string("File not found: " + filename));

			Finally closeFile(boost::bind(fclose, pFile));

			png_byte buf[PNG_BYTES_TO_CHECK];
			assert(PNG_BYTES_TO_CHECK >= sizeof(unsigned short));

			// Read in some of the signature bytes
			if (fread(buf, 1, PNG_BYTES_TO_CHECK, pFile) != PNG_BYTES_TO_CHECK)
				throw std::runtime_error(std::string("Error reading signature bytes. (" + filename + ")"));

			if(png_sig_cmp(buf, (png_size_t)0, PNG_BYTES_TO_CHECK) == 0)
				LoadPNG(filename, pFile, halfLoad);
			else if(*reinterpret_cast<unsigned short*>(buf) == 19778)
				LoadBMP(filename, pFile, halfLoad);
			else if(*reinterpret_cast<unsigned short*>(buf) == 55551)
			{
			#ifdef NOJPEG
				throw std::runtime_error(std::string("Sorry, JPEG files not supported in this build of JNGL. (" + filename + ")"));
			#else
				LoadJPG(filename, pFile, halfLoad);
			#endif
			}
			else
				throw std::runtime_error(std::string("Not a PNG, JPEG or BMP file. (" + filename + ")"));
		}
		~Texture()
		{
			if(pWindow)
			{
				glDeleteTextures(1, &texture_);
				glDeleteBuffers(1, &vertexBuffer_);
			}
		}
		void DrawTexture()
		{
			glEnable(GL_TEXTURE_2D);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

			glBindTexture(GL_TEXTURE_2D, texture_);
			opengl::BindArrayBuffer(vertexBuffer_);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glDisable(GL_TEXTURE_2D);
		}
		template<class ArrayType>
		void LoadTexture(const std::string& filename,
		                 const int imgWidth,
		                 const int imgHeight,
		                 ArrayType& rowPointers,
		                 int channels,
		                 const bool halfLoad,
		                 GLenum format)
		{
			int width = opengl::NextPowerOf2(imgWidth);
			int height = opengl::NextPowerOf2(imgHeight);
			width_ = imgWidth;
			height_ = imgHeight;
			if(!pWindow)
			{
				if(halfLoad)
				{
					return;
				}
				throw std::runtime_error(std::string("Window hasn't been created yet. (" + filename + ")"));
			}
			glGenTextures(1, &texture_);
			glBindTexture(GL_TEXTURE_2D, texture_);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // preventing wrapping artifacts
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			for(int i = 0; i < imgHeight; ++i)
			{
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, i, imgWidth, 1, format, GL_UNSIGNED_BYTE, rowPointers[i]);
				std::vector<unsigned char> empty((width - imgWidth) * channels, 0);
				glTexSubImage2D(GL_TEXTURE_2D, 0, imgWidth, i, width - imgWidth, 1, format, GL_UNSIGNED_BYTE, &empty[0]);
			}
			for(int i = imgHeight; i < height; ++i)
			{
				std::vector<unsigned char> empty(width * channels, 0);
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, i, width, 1, format, GL_UNSIGNED_BYTE, &empty[0]);
			}

			const GLfloat x = static_cast<GLfloat>(imgWidth) / static_cast<GLfloat>(width);
			const GLfloat y = static_cast<GLfloat>(imgHeight)  / static_cast<GLfloat>(height);
			GLfloat vertexes[] = {
			                       0, 0, 0, y, x, y, x, 0, // texture coordinates
			                       0, 0, 0, imgHeight, imgWidth, imgHeight, imgWidth, 0
			                     };
			glGenBuffers(1, &vertexBuffer_);
			opengl::BindArrayBuffer(vertexBuffer_);
			glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(GLfloat), vertexes, GL_STATIC_DRAW);
		}
		void LoadPNG(const std::string& filename, FILE* const fp, const bool halfLoad)
		{
			png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, png_voidp_NULL, png_error_ptr_NULL, png_error_ptr_NULL);
			if(!png_ptr)
			{
				throw std::runtime_error(std::string("libpng error while reading (" + filename + ")"));
			}

			png_infop info_ptr = png_create_info_struct(png_ptr);
			if(!info_ptr)
			{
				throw std::runtime_error(std::string("libpng error while reading (" + filename + ")"));
			}

			if(setjmp(png_jmpbuf(png_ptr)))
			{
				// Free all of the memory associated with the png_ptr and info_ptr
				png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
				throw std::runtime_error(std::string("Error reading file. (" + filename + ")"));
			}
			png_init_io(png_ptr, fp);
			png_set_sig_bytes(png_ptr, PNG_BYTES_TO_CHECK);
			int colorType = png_get_color_type(png_ptr, info_ptr);
			if(colorType == PNG_COLOR_TYPE_GRAY || colorType == PNG_COLOR_TYPE_GRAY_ALPHA)
			{
				png_set_gray_to_rgb(png_ptr);
			}
			png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_STRIP_16, png_voidp_NULL);

			png_ptr->num_rows = png_ptr->height; // Make sure this is set correctly

			GLenum format;
			switch(png_ptr->channels)
			{
				case 1: // gray or palette, we can set channels to 3 because we passed PNG_TRANSFORM_EXPAND to png_read_png
					png_ptr->channels = 3; // we need this value when creating the texture for OpenGL
				case 3:
					format = GL_RGB;
				break;
				case 2: // gray + alpha
					png_ptr->channels = 4;
				case 4:
					format = GL_RGBA;
				break;
				default:
					throw std::runtime_error(std::string("Unsupported number of channels. (" + filename + ")"));
			}

			Finally freePng(boost::bind(png_destroy_read_struct, &png_ptr, &info_ptr, png_infopp_NULL));
			LoadTexture(filename, png_ptr->width, png_ptr->height, info_ptr->row_pointers, png_ptr->channels, halfLoad, format);
		}
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
		static void CleanUpRowPointers(std::vector<char*>& buf)
		{
			for(std::vector<char*>::iterator i = buf.begin(); i != buf.end(); ++i)
			{
				delete[] *i;
			}
		}
 		void LoadBMP(const std::string& filename, FILE* const fp, const bool halfLoad)
		{
			fseek(fp, 10, SEEK_SET);
			BMPHeader header;
			if(!fread(&header, sizeof(header), 1, fp))
				throw std::runtime_error(std::string("Error reading file. (" + filename + ")"));

			if(header.headerSize != 40)
				throw std::runtime_error(std::string("Unsupported header size. (" + filename + ")"));

			if(header.bpp != 24)
				throw std::runtime_error(std::string("Bpp not supported. (" + filename + ")"));

			if(header.compression != 0)
				throw std::runtime_error(std::string("Compression not supported. (" + filename + ")"));

			if(header.dataSize == 0)
				header.dataSize = header.width * header.height * 3;

			std::vector<char*> buf(header.height);
			for(std::vector<char*>::iterator i = buf.begin(); i != buf.end(); ++i)
			{
				*i = new char[header.width * 3];
			}
			Finally cleanUp(boost::bind(CleanUpRowPointers, boost::ref(buf)));

			if(header.height < 0)
			{
				header.height = !header.height;
				for(int i = 0; i < header.height; ++i)
				{
					if(fseek(fp, header.dataOffset + i * header.width * 3, SEEK_SET) != 0)
						throw std::runtime_error(std::string("Error reading file. (" + filename + ")"));
					if(!fread(buf[i], header.width * 3, 1, fp))
						throw std::runtime_error(std::string("Error reading data. (" + filename + ")"));
				}
			}
			else // "bottom-up"-Bitmap
			{
				for(int i = header.height - 1; i >= 0; --i)
				{
					if(fseek(fp, header.dataOffset + i * header.width * 3, SEEK_SET) != 0)
						throw std::runtime_error(std::string("Error reading file. (" + filename + ")"));
					if(!fread(buf[(header.height - 1) - i], header.width * 3, 1, fp))
						throw std::runtime_error(std::string("Error reading data. (" + filename + ")"));
				}
			}
			LoadTexture(filename, header.width, header.height, buf, header.bpp / 8, halfLoad, GL_BGR);
		}
#ifndef NOJPEG
		void LoadJPG(const std::string& filename, FILE* file, const bool halfLoad)
		{
			fseek(file, 0, SEEK_SET); // Seek to the beginning

			jpeg_decompress_struct info;
			JpegErrorMgr err;
			info.err = jpeg_std_error(&err.pub);
			err.pub.error_exit = JpegErrorExit;

			// Establish the setjmp return context for my_error_exit to use.
			if(setjmp(err.setjmp_buffer))
			{
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

			int x = info.output_width;
			int y = info.output_height;
			int channels = info.num_components;

			GLenum format = GL_RGB;
			if(channels == 4)
			{
				format = GL_RGBA;
			}

			assert(sizeof(JSAMPLE) == sizeof(char));
			std::vector<char*> buf(y);
			for(std::vector<char*>::iterator i = buf.begin(); i != buf.end(); ++i)
			{
				*i = new char[x * channels];
			}
			Finally cleanUp(boost::bind(CleanUpRowPointers, boost::ref(buf)));

			while(info.output_scanline < info.output_height)
			{
				jpeg_read_scanlines(&info, reinterpret_cast<JSAMPLE**>(&buf[info.output_scanline]), 1);
			}

			jpeg_finish_decompress(&info);

			LoadTexture(filename, x, y, buf, channels, halfLoad, format);
		}
#endif
		int Width()
		{
			return width_;
		}
		int Height()
		{
			return height_;
		}
		template<class T>
		void Draw(const T xposition, const T yposition)
		{
			glPushMatrix();
			opengl::Translate(xposition, yposition);
			DrawTexture();
			glPopMatrix();
		}
		template<class T>
		void DrawScaled(const T xposition, const T yposition, const float xfactor, const float yfactor)
		{
			glPushMatrix();
			opengl::Translate(xposition, yposition);
			opengl::Scale(xfactor, yfactor);
			DrawTexture();
			glPopMatrix();
		}
		GLuint GetID()
		{
			return texture_;
		}
	private:
		GLuint texture_;
		GLuint vertexBuffer_;
		int width_, height_;
		const static unsigned int PNG_BYTES_TO_CHECK = 4;
	};

	std::map<std::string, boost::shared_ptr<Texture> > textures_;

	// halfLoad is used, if we only want to find out the width or height of an image. Load won't throw an exception then
	Texture& GetTexture(const std::string& filename, const bool halfLoad = false)
	{
		std::map<std::string, boost::shared_ptr<Texture> >::iterator i;
		if((i = textures_.find(filename)) == textures_.end()) // texture hasn't been loaded yet?
		{
			if(!halfLoad)
			{
				pWindow.ThrowIfNull();
			}
			textures_[filename].reset(new Texture(filename, halfLoad));
			return *(textures_[filename]);
		}
		return *(i->second);
	}

	void Draw(const std::string& filename, const double xposition, const double yposition)
	{
		GetTexture(filename).Draw(xposition, yposition);
	}

	void DrawScaled(const std::string& filename, const double xposition, const double yposition,
	                const float xfactor, const float yfactor)
	{
		GetTexture(filename).DrawScaled(xposition, yposition, xfactor, yfactor);
	}

	void DrawScaled(const std::string& filename, const double xposition, const double yposition,
	                const float factor)
	{
		GetTexture(filename).DrawScaled(xposition, yposition, factor, factor);
	}

	GLuint Load(const std::string& filename)
	{
		return GetTexture(filename).GetID();
	}

	void Unload(const std::string& filename)
	{
		std::map<std::string, boost::shared_ptr<Texture> >::iterator i;
		if((i = textures_.find(filename)) != textures_.end())
			textures_.erase(i);
	}

	void UnloadAll()
	{
		textures_.clear();
	}

	int GetWidth(const std::string& filename)
	{
		const int width = GetTexture(filename, true).Width();
		if(!pWindow)
			Unload(filename);
		return width;
	}

	int GetHeight(const std::string& filename)
	{
		const int height = GetTexture(filename, true).Height();
		if(!pWindow)
			Unload(filename);
		return height;
	}

	bool DrawButton(const std::string& texture, const double xposition, const double yposition, const std::string& mouseover)
	{
		if(xposition <= GetMouseX() && GetMouseX() < (xposition + GetWidth(texture)) &&
		   yposition <= GetMouseY() && GetMouseY() < (yposition + GetHeight(texture)))
		{
			GetTexture(mouseover).Draw(xposition, yposition);
			if(MousePressed())
			{
				return true;
			}
		}
		else
		{
			GetTexture(texture).Draw(xposition, yposition);
		}
		return false;
	}
}
