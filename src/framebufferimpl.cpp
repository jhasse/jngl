// Copyright 2011-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "framebufferimpl.hpp"

#include "jngl/matrix.hpp"
#include "jngl/screen.hpp"
#include "main.hpp"
#include "spriteimpl.hpp"
#include "windowptr.hpp"

#include <cassert>

namespace jngl {

	FrameBufferImpl::FrameBufferImpl(int width, int height)
	: height(height), texture(width, height, nullptr) {
#ifdef EPOXY_PUBLIC
		static bool first = true;
		if (first && !epoxy_has_gl_extension("GL_EXT_framebuffer_object")) {
			throw std::runtime_error("OpenGL Frame Buffer Object not supported!");
		}
		first = false;
#endif
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

		Clear();

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
		glViewport(0, -(pWindow->getHeight() - height), pWindow->getWidth(), pWindow->getHeight());
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
	}

	void FrameBufferImpl::Clear() {
		glClearColor(1, 1, 1, 0);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void FrameBufferImpl::EndDraw() {
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
		pushMatrix();
		jngl::translate(x, y);
		opengl::scale(1, -1);
		jngl::translate(0, -height / getScaleFactor());
		texture.draw(float(spriteColorRed) / 255.0f, float(spriteColorGreen) / 255.0f,
		             float(spriteColorBlue) / 255.0f, float(spriteColorAlpha) / 255.0f);
		popMatrix();
	}
} // namespace jngl
