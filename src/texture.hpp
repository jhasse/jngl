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

struct Vertex;

class Texture {
public:
	Texture(float preciseWidth, float preciseHeight, int width, int height,
	        const GLubyte* const* rowPointers, // data as row pointers ...
	        GLenum format = GL_RGBA, const GLubyte* data = nullptr /* ... or as one pointer */);
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;
	~Texture();
	void Bind() const;
	void draw(float red, float green, float blue, float alpha, const ShaderProgram* = nullptr) const;
	void drawClipped(float xstart, float xend, float ystart, float yend, float red, float green,
	                 float blue, float alpha) const;
	void drawMesh(const std::vector<Vertex>& vertexes, float red, float green, float blue,
	              float alpha) const;
	GLuint getID() const;
	float getPreciseWidth() const;
	float getPreciseHeight() const;
	static void unloadShader();
	void setBytes(const unsigned char*, int width, int height);

	static const Shader& vertexShader();

private:
	static ShaderProgram* textureShaderProgram;
	static Shader* textureVertexShader;
	static int shaderSpriteColorUniform;
	static int modelviewUniform;
	GLuint texture_ = 0;
	GLuint vertexBuffer_ = 0;
	GLuint vao = 0;
	std::vector<GLfloat> vertexes;
};

extern std::unordered_map<std::string, std::shared_ptr<Texture>> textures;

} // namespace jngl
