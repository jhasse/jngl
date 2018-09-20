// Copyright 2010-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "texture.hpp"

#include "jngl/Shader.hpp"

#include <boost/math/special_functions/relative_difference.hpp>
#include <cassert>
#include <fstream>

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

namespace jngl {

std::unordered_map<std::string, std::shared_ptr<Texture>> textures;

ShaderProgram* Texture::textureShaderProgram = nullptr;
int Texture::shaderSpriteColorUniform = -1;

Texture::Texture(const float preciseWidth, const float preciseHeight,
                 const GLubyte* const* const rowPointers, GLenum format, const GLubyte* const data)
: width(std::lround(preciseWidth)), height(std::lround(preciseHeight)) {
#ifdef EPOXY_PUBLIC
	static bool first = true;
	if (first && !epoxy_has_gl_extension("GL_ARB_vertex_buffer_object")) {
		throw std::runtime_error("VBOs not supported\n");
	}
	first = false;
#endif
	if (!textureShaderProgram) {
		static Shader* vertexShader;
		static Shader* fragmentShader;
		{
			vertexShader = new Shader(R"(#version 130
				in mediump vec2 position;
				in mediump vec2 inTexCoord;
				out mediump vec2 texCoord;

				void main() {
					gl_Position = gl_ModelViewProjectionMatrix * vec4(position, 0.0, 1.0);
					texCoord = inTexCoord;
				})", Shader::Type::VERTEX
			);
		}
		{
			std::ifstream fin("data/texture.frag");
			std::stringstream buffer;
			buffer << fin.rdbuf();
			fragmentShader = new Shader(R"(#version 130
				uniform sampler2D tex;
				uniform lowp vec4 spriteColor = vec4(1.0, 1.0, 1.0, 1.0);

				in mediump vec2 texCoord;

				out lowp vec4 outColor;

				void main() {
					outColor = texture2D(tex, texCoord) * spriteColor;
				})", Shader::Type::FRAGMENT
			);
		}
		textureShaderProgram = new ShaderProgram(*vertexShader, *fragmentShader);
		shaderSpriteColorUniform = textureShaderProgram->getUniformLocation("spriteColor");
	}
	glGenTextures(1, &texture_);
	glBindTexture(GL_TEXTURE_2D, texture_);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
	                GL_CLAMP_TO_EDGE); // preventing wrapping artifacts
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	GLfloat vertexes[] = {
		0, 0,
		0, 0, // texture coordinates
		0, preciseHeight,
		0, 1, // texture coordinates
		preciseWidth, preciseHeight,
		1, 1, // texture coordinates
		preciseWidth, 0,
		1, 0 // texture coordinates
	};
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vertexBuffer_);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
	glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(GLfloat), vertexes, GL_STATIC_DRAW);

	const GLint posAttrib = textureShaderProgram->getAttribLocation("position");
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glEnableVertexAttribArray(posAttrib);

	const GLint texCoordAttrib = textureShaderProgram->getAttribLocation("inTexCoord");
	glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
	                      reinterpret_cast<void*>(2 * sizeof(float)));
	glEnableVertexAttribArray(texCoordAttrib);

	if (rowPointers) {
		assert(!data);
		for (int i = 0; i < height; ++i) {
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, i, width, 1, format, GL_UNSIGNED_BYTE,
			                rowPointers[i]);
		}
	}
	if (data) {
		assert(!rowPointers);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);
	}

	glBindVertexArray(0);
}

Texture::~Texture() {
	glDeleteTextures(1, &texture_);
	glDeleteBuffers(1, &vertexBuffer_);
	glDeleteVertexArrays(1, &vao);
}

void Texture::draw(const float red, const float green, const float blue, const float alpha,
                   const ShaderProgram* const shaderProgram) const {
	auto _ = shaderProgram ? shaderProgram->use() : textureShaderProgram->use();
	if (!shaderProgram) {
		glUniform4f(shaderSpriteColorUniform, red, green, blue, alpha);
	}
	glBindVertexArray(vao);
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, texture_);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glDisable(GL_TEXTURE_2D);
	glBindVertexArray(0);
}

void Texture::drawClipped(const float /*xstart*/, const float /*xend*/, const float /*ystart*/,
                          const float /*yend*/) const {
	// TODO
}

GLuint Texture::getID() const {
	return texture_;
}

int Texture::getWidth() const {
	return width;
}

int Texture::getHeight() const {
	return height;
}

void Texture::unloadShader() {
	delete textureShaderProgram;
	textureShaderProgram = nullptr;
}

} // namespace jngl
