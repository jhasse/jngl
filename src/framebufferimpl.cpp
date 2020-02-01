// Copyright 2011-2019 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "framebufferimpl.hpp"

#include "jngl/matrix.hpp"
#include "jngl/screen.hpp"
#include "main.hpp"
#include "spriteimpl.hpp"

#include <cassert>

namespace jngl {

FrameBufferImpl::FrameBufferImpl(int width, int height)
: height(height),
  texture(static_cast<float>(width), static_cast<float>(height), width, height, nullptr),
  letterboxing(glIsEnabled(GL_SCISSOR_TEST)) {
	GLint tmp;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &tmp);
	systemFbo = tmp;
	glGetIntegerv(GL_RENDERBUFFER_BINDING, &tmp);
	systemBuffer = tmp;

	glGenRenderbuffers(1, &buffer);
	glBindRenderbuffer(GL_RENDERBUFFER, buffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, width, height);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, buffer);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.getID(), 0);

	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	clear();

	glBindFramebuffer(GL_FRAMEBUFFER, systemFbo);
	glBindRenderbuffer(GL_RENDERBUFFER, systemBuffer);
}

FrameBufferImpl::~FrameBufferImpl() {
	glDeleteFramebuffers(1, &fbo);
	glDeleteRenderbuffers(1, &buffer);
}

void FrameBufferImpl::BeginDraw() {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glBindRenderbuffer(GL_RENDERBUFFER, buffer);
	pushMatrix();
#ifdef GL_VIEWPORT_BIT
	glPushAttrib(GL_VIEWPORT_BIT);
#else
	glGetIntegerv(GL_VIEWPORT, viewport);
#endif
	glViewport((pWindow->getCanvasWidth() - pWindow->getWidth()) / 2,
	           -((pWindow->getCanvasHeight() + pWindow->getHeight()) / 2 - height),
	           pWindow->getWidth(), pWindow->getHeight());
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	if (letterboxing) {
		glDisable(GL_SCISSOR_TEST);
	}
}

void FrameBufferImpl::clear() {
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	clearBackgroundColor();
}

Vec2 FrameBufferImpl::getSize() const {
	return { texture.getPreciseWidth() / getScaleFactor(),
		     texture.getPreciseHeight() / getScaleFactor() };
}

void FrameBufferImpl::EndDraw() {
	if (letterboxing) {
		glEnable(GL_SCISSOR_TEST);
	}
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	popMatrix();
#ifdef GL_VIEWPORT_BIT
	glPopAttrib();
#else
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
#endif
	glBindFramebuffer(GL_FRAMEBUFFER, systemFbo);
	glBindRenderbuffer(GL_RENDERBUFFER, systemBuffer);
	clearBackgroundColor();
}

void FrameBufferImpl::Draw(const double x, const double y) const {
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	pushMatrix();
	jngl::translate(x, y);
	opengl::scale(1, -1);
	jngl::translate(0, -height / getScaleFactor());
	texture.draw(float(spriteColorRed) / 255.0f, float(spriteColorGreen) / 255.0f,
	             float(spriteColorBlue) / 255.0f, float(spriteColorAlpha) / 255.0f);
	popMatrix();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

} // namespace jngl
