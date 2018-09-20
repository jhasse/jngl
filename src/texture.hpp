// Copyright 2010-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "jngl/ShaderProgram.hpp"
#include "opengl.hpp"

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

namespace jngl {

class Texture {
public:
	Texture(float preciseWidth, float preciseHeight,
	        const GLubyte* const* rowPointers, // data as row pointers ...
	        GLenum format = GL_RGBA, const GLubyte* data = nullptr /* ... or as one pointer */);
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;
	~Texture();
	void Bind() const;
	void draw(float red, float green, float blue, float alpha, const ShaderProgram* = nullptr) const;
	void drawClipped(float xstart, float xend, float ystart, float yend) const;
	GLuint getID() const;
	int getWidth() const;
	int getHeight() const;
	static void unloadShader();

private:
	static ShaderProgram* textureShaderProgram;
	static int shaderSpriteColorUniform;
	static int modelviewUniform;
	GLuint texture_ = 0;
	GLuint vertexBuffer_ = 0;
	GLuint vao = 0;
	int width;
	int height;
};

extern std::unordered_map<std::string, std::shared_ptr<Texture>> textures;

} // namespace jngl
