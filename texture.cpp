/*
Copyright 2007-2009  Jan Niklas Hasse <jhasse@gmail.com>

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

#include <map>
#include <iostream>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <GL/gl.h>
#include <GL/glu.h>
#include <vector>
#include <stdexcept>
#include <png.h>
#include <cmath>

#include "texture.hpp"
#include "window.hpp"
#include "jngl.hpp"
#include "finally.hpp"

#ifndef linux
#include <gl/glext.h>
#endif

namespace jngl
{
	struct RawTexture
	{
		GLuint textureid;
		int width;
		int height;
	};
	class Texture
	{
	public:
		Texture()
		{
		}
		~Texture()
		{
			if(pWindow)
			{
				for(std::vector<std::vector<RawTexture> >::iterator i = parts_.begin(); i != parts_.end(); ++i)
				{
					for(std::vector<RawTexture>::iterator j = i->begin(); j != i->end(); ++j)
					{
						glDeleteTextures(1, &j->textureid);
					}
				}
			}
		}
		template<class ArrayType>
		void loadTexture(const std::string& filename,
		                 const int imgWidth,
		                 const int imgHeight,
		                 ArrayType& rowPointers,
		                 int channels,
		                 const bool halfLoad,
		                 GLenum format)
		{
			int yoffset = 0, height = 0, img_height = imgHeight;
			do
			{
				height = static_cast<int>(pow(2, floor(log(img_height) / log(2))));
				int xoffset = 0, width = 0, img_width = imgWidth;
				std::vector<RawTexture> temp;
				do
				{
					width = static_cast<int>(pow(2, floor(log(img_width) / log(2))));
					RawTexture rawTexture;
					rawTexture.width = width;
					rawTexture.height = height;

					if(pWindow)
					{
						glEnable(GL_TEXTURE_2D);
						glGenTextures(1, &rawTexture.textureid);
						glBindTexture(GL_TEXTURE_2D, rawTexture.textureid);
						glTexImage2D(GL_TEXTURE_2D, 0, channels, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // preventing wrapping artifacts
						glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
						for(int i = 0; i < height; ++i)
						{
							glTexSubImage2D(GL_TEXTURE_2D, 0, 0, i, width, 1, format, GL_UNSIGNED_BYTE, rowPointers[i + yoffset] + xoffset * channels);
						}
						glDisable(GL_TEXTURE_2D);
					}
					else if(!halfLoad)
						throw std::runtime_error(std::string("Window hasn't been created yet. (" + filename + ")"));

					temp.push_back(rawTexture);
					xoffset += width;
				} while(img_width -= width);
				yoffset += height;
				parts_.push_back(temp);
			} while(img_height -= height);
		}
		void loadPNG(const std::string& filename, FILE* const fp, const bool halfLoad)
		{
			png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, png_voidp_NULL, png_error_ptr_NULL, png_error_ptr_NULL);
		    if (!png_ptr)
		    	throw std::runtime_error(std::string("libpng error while reading (" + filename + ")"));

			png_infop info_ptr = png_create_info_struct(png_ptr);
			if(!info_ptr)
				throw std::runtime_error(std::string("libpng error while reading (" + filename + ")"));

			if(setjmp(png_jmpbuf(png_ptr)))
			{
				// Free all of the memory associated with the png_ptr and info_ptr
				png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
				throw std::runtime_error(std::string("Error reading file. (" + filename + ")"));
			}
		    png_init_io(png_ptr, fp);
		    png_set_sig_bytes(png_ptr, PNG_BYTES_TO_CHECK);
			png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, png_voidp_NULL);

			png_ptr->num_rows = png_ptr->height; // Make sure this is set correctly

			GLenum format;
			switch(png_ptr->channels)
			{
				case 1: // palette, we can set channels to 3 because we passed PNG_TRANSFORM_EXPAND to png_read_png
					png_ptr->channels = 3; // we need this value when creating the texture for OpenGL
				case 3:
					format = GL_RGB;
				break;
				case 4:
					format = GL_RGBA;
				break;
				default:
					throw std::runtime_error(std::string("Unsupported number of channels. (" + filename + ")"));
			}

			Finally freePng(boost::bind(png_destroy_read_struct, &png_ptr, &info_ptr, png_infopp_NULL));
			loadTexture(filename, png_ptr->width, png_ptr->height, info_ptr->row_pointers, png_ptr->channels, halfLoad, format);
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
		static void cleanUpRowPointers(std::vector<char*>& buf)
		{
			for(std::vector<char*>::iterator i = buf.begin(); i != buf.end(); ++i)
			{
				delete[] *i;
			}
		}
 		void loadBMP(const std::string& filename, FILE* const fp, const bool halfLoad)
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
			Finally cleanUp(boost::bind(cleanUpRowPointers, boost::ref(buf)));

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
			loadTexture(filename, header.width, header.height, buf, header.bpp / 8, halfLoad, GL_BGR);
		}
		Texture& load(const std::string& filename, const bool halfLoad)
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
				loadPNG(filename, pFile, halfLoad);
			else if(*reinterpret_cast<unsigned short*>(buf) == 19778)
				loadBMP(filename, pFile, halfLoad);
			else
				throw std::runtime_error(std::string("Neither a PNG nor a BMP file. (" + filename + ")"));

			return *this;
		}
		int Width()
		{
			int width = 0;
			for(std::vector<RawTexture>::iterator j = parts_.begin()->begin(); j != parts_.begin()->end(); ++j)
			{
				width += j->width;
			}
			return width;
		}
		int Height()
		{
			int height = 0;
			for(std::vector<std::vector<RawTexture> >::iterator i = parts_.begin(); i != parts_.end(); ++i)
			{
				height += i->begin()->height;
			}
			return height;
		}
		void Draw(const double xposition, const double yposition)
		{
			glPushMatrix();
			glTranslated(xposition, yposition, 0);
			glEnable(GL_TEXTURE_2D);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			for(std::vector<std::vector<RawTexture> >::iterator i = parts_.begin(); i != parts_.end(); ++i)
			{
				glPushMatrix();
				for(std::vector<RawTexture>::iterator j = i->begin(); j != i->end(); ++j)
				{
					glBindTexture(GL_TEXTURE_2D, j->textureid);
					GLint vertexes[] = { 0, 0, j->width, 0, j->width, j->height, 0, j->height };
					glVertexPointer(2, GL_INT, 0, vertexes);
					GLint texCoords[] = { 0, 0, 1, 0, 1, 1, 0, 1 };
					glTexCoordPointer(2, GL_INT, 0, texCoords);
					glDrawArrays(GL_QUADS, 0, 4);
					glTranslatef(j->width, 0, 0);
				}
				glPopMatrix();
				glTranslatef(0, i->begin()->height, 0);
			}
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glDisable(GL_TEXTURE_2D);
			glPopMatrix();
		}
		void DrawScaled(const double xposition, const double yposition, const float xfactor, const float yfactor)
		{
			glPushMatrix();
			glTranslatef(xposition, yposition, 0);
			glEnable(GL_TEXTURE_2D);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			for(std::vector<std::vector<RawTexture> >::iterator i = parts_.begin(); i != parts_.end(); ++i)
			{
				glPushMatrix();
				for(std::vector<RawTexture>::iterator j = i->begin(); j != i->end(); ++j)
				{
					glBindTexture(GL_TEXTURE_2D, j->textureid);
					GLdouble vertexes[] = { 0,                  0,
					                        j->width * xfactor, 0,
					                        j->width * xfactor, j->height * yfactor,
					                        0,                  j->height * yfactor };
					glVertexPointer(2, GL_DOUBLE, 0, vertexes);
					GLdouble texCoords[] = { 0, 0, 1, 0, 1, 1, 0, 1 };
					glTexCoordPointer(2, GL_DOUBLE, 0, texCoords);
					glDrawArrays(GL_QUADS, 0, 4);
					glTranslatef(j->width * xfactor, 0, 0);
				}
				glPopMatrix();
				glTranslatef(0, i->begin()->height * yfactor, 0);
			}
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glDisable(GL_TEXTURE_2D);
			glPopMatrix();
		}
	private:
		std::vector<std::vector<RawTexture> > parts_;
		const static unsigned int PNG_BYTES_TO_CHECK = 4;
	};

	std::map<std::string, Texture> textures_;

	// halfLoad is used, if we only want to find out the width or height of an image. Load won't throw an exception then
	Texture& GetTexture(const std::string& filename, const bool halfLoad = false)
	{
		std::map<std::string, Texture>::iterator i;
		if((i = textures_.find(filename)) == textures_.end()) // texture hasn't been loaded yet?
			return textures_[filename].load(filename, halfLoad);
		return i->second;
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

	void Load(const std::string& filename)
	{
		GetTexture(filename);
	}

	void Unload(const std::string& filename)
	{
		std::map<std::string, Texture>::iterator i;
		if((i = textures_.find(filename)) != textures_.end())
			textures_.erase(i);
	}

	void UnloadAll()
	{
		textures_.clear();
	}

	int Width(const std::string& filename)
	{
		const int width = GetTexture(filename, true).Width();
		if(!pWindow)
			Unload(filename);
		return width;
	}

	int Height(const std::string& filename)
	{
		const int height = GetTexture(filename, true).Height();
		if(!pWindow)
			Unload(filename);
		return height;
	}

	bool Button(const std::string& texture, const double xposition, const double yposition, const std::string& mouseover)
	{
		if(xposition <= MouseX() && MouseX() < (xposition + Width(texture)) &&
		   yposition <= MouseY() && MouseY() < (yposition + Height(texture)))
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
