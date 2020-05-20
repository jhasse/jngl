// Copyright 2011-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "framebuffer.hpp"

#include "../main.hpp"
#include "../spriteimpl.hpp"
#include "../texture.hpp"
#include "matrix.hpp"
#include "screen.hpp"

namespace jngl {

struct FrameBuffer::Impl {
	Impl(int width, int height)
	: height(height),
	  texture(static_cast<float>(width), static_cast<float>(height), width, height, nullptr),
	  letterboxing(glIsEnabled(GL_SCISSOR_TEST)) {
	}
	Impl(const Impl&) = delete;
	Impl& operator=(const Impl&) = delete;
	Impl(Impl&&) = delete;
	Impl& operator=(Impl&&) = delete;

	~Impl() {
		glDeleteFramebuffers(1, &fbo);
		glDeleteRenderbuffers(1, &buffer);
	}

	GLuint fbo = 0;
	GLuint buffer = 0;
	const int height;
	Texture texture;
	bool letterboxing;
	GLuint systemFbo;
	GLuint systemBuffer;
#if !defined(GL_VIEWPORT_BIT) || defined(__APPLE__)
	GLint viewport[4];
#endif
};

FrameBuffer::FrameBuffer(const int width, const int height)
: impl(std::make_unique<Impl>(width, height)) {
	GLint tmp;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &tmp);
	impl->systemFbo = tmp;
	glGetIntegerv(GL_RENDERBUFFER_BINDING, &tmp);
	impl->systemBuffer = tmp;

	glGenRenderbuffers(1, &impl->buffer);
	glBindRenderbuffer(GL_RENDERBUFFER, impl->buffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, width, height);

	glGenFramebuffers(1, &impl->fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, impl->fbo);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, impl->buffer);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
	                       impl->texture.getID(), 0);

	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	clear();

	glBindFramebuffer(GL_FRAMEBUFFER, impl->systemFbo);
	glBindRenderbuffer(GL_RENDERBUFFER, impl->systemBuffer);
}

FrameBuffer::~FrameBuffer() = default;

void FrameBuffer::draw(const double x, const double y) const {
	return draw({ x, y });
}

void FrameBuffer::draw(const Vec2 position, const ShaderProgram* const shaderProgram) const {
	pushMatrix();
	jngl::translate(position);
	opengl::scale(1, -1);
	jngl::translate(0, -impl->height / getScaleFactor());
	impl->texture.draw(float(spriteColorRed) / 255.0f, float(spriteColorGreen) / 255.0f,
	                   float(spriteColorBlue) / 255.0f, float(spriteColorAlpha) / 255.0f,
	                   shaderProgram);
	popMatrix();
}

void FrameBuffer::drawMesh(const std::vector<Vertex>& vertexes,
                           const ShaderProgram* const shaderProgram) const {
	pushMatrix();
	scale(getScaleFactor());
	impl->texture.drawMesh(vertexes, float(spriteColorRed) / 255.0f,
	                       float(spriteColorGreen) / 255.0f, float(spriteColorBlue) / 255.0f,
	                       float(spriteColorAlpha) / 255.0f, shaderProgram);
	popMatrix();
}

Finally FrameBuffer::use() {
	glBindFramebuffer(GL_FRAMEBUFFER, impl->fbo);
	glBindRenderbuffer(GL_RENDERBUFFER, impl->buffer);
	pushMatrix();
#if defined(GL_VIEWPORT_BIT) && !defined(__APPLE__)
	glPushAttrib(GL_VIEWPORT_BIT);
#else
	glGetIntegerv(GL_VIEWPORT, impl->viewport);
#endif
	glViewport((pWindow->getCanvasWidth() - pWindow->getWidth()) / 2,
	           -((pWindow->getCanvasHeight() + pWindow->getHeight()) / 2 - impl->height),
	           pWindow->getWidth(), pWindow->getHeight());
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
	if (impl->letterboxing) {
		glDisable(GL_SCISSOR_TEST);
	}
	return Finally([this]() {
		if (impl->letterboxing) {
			glEnable(GL_SCISSOR_TEST);
		}
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		popMatrix();
#if defined(GL_VIEWPORT_BIT) && !defined(__APPLE__)
		glPopAttrib();
#else
		glViewport(impl->viewport[0], impl->viewport[1], impl->viewport[2], impl->viewport[3]);
#endif
		glBindFramebuffer(GL_FRAMEBUFFER, impl->systemFbo);
		glBindRenderbuffer(GL_RENDERBUFFER, impl->systemBuffer);
		clearBackgroundColor();
	});
}

void FrameBuffer::clear() {
	glClearColor(1, 1, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	clearBackgroundColor();
}

Vec2 FrameBuffer::getSize() const {
	return { impl->texture.getPreciseWidth() / getScaleFactor(),
		     impl->texture.getPreciseHeight() / getScaleFactor() };
}

GLuint FrameBuffer::getTextureID() const {
	return impl->texture.getID();
}

} // namespace jngl
