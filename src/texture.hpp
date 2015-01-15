/*
Copyright 2010-2015 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#include "opengl.hpp"

#include <vector>
#include <boost/noncopyable.hpp>
#include <unordered_map>
#include <string>
#include <memory>

namespace jngl {
	class Texture : boost::noncopyable {
	public:
		Texture(int imgWidth, int imgHeight,
		        GLubyte** rowPointers, // data as row pointers ...
		        GLenum format = GL_RGBA, int channels = 4,
		        GLubyte* data = nullptr /* ... or as one pointer */ );
		~Texture();
		void Bind() const;
		void draw() const;
		void drawClipped(float xstart, float xend, float ystart, float yend) const;
		GLuint getID() const;
		int getWidth() const;
		int getHeight() const;
	private:
		GLuint texture_;
		GLuint vertexBuffer_;
		std::vector<opengl::CoordType> texCoords_;
		std::vector<GLfloat> vertexes_;
		int width;
		int height;
	};

	extern std::unordered_map<std::string, std::shared_ptr<Texture>> textures;
}
