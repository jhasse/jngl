/*
Copyright 2011-2012 Jan Niklas Hasse <jhasse@gmail.com>

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

#include "framebufferimpl.hpp"
#include "windowptr.hpp"
#include "main.hpp"

namespace jngl {

	FrameBufferImpl::FrameBufferImpl(int width, int height) : width(width), height(height), texture(width, height, 0) {
		if(!GLEW_EXT_framebuffer_object) {
			throw std::runtime_error("OpenGL Frame Buffer Object not supported!");
		}
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

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.GetID(), 0);

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
		glPushMatrix();
#ifdef GL_VIEWPORT_BIT
		glPushAttrib(GL_VIEWPORT_BIT);
#else
		glGetIntegerv(GL_VIEWPORT, viewport);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		#define f2x(x) ((int)((x) * 65536))
		glOrthox(f2x(-pWindow->GetWidth()/2), f2x(pWindow->GetWidth()/2), f2x(pWindow->GetHeight()/2), f2x(-pWindow->GetHeight()/2), f2x(-1), f2x(1));
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
#endif
		glViewport(0, -(pWindow->GetHeight() - height), pWindow->GetWidth(), pWindow->GetHeight());
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
	}
	
	void FrameBufferImpl::Clear() {
		glClearColor(1, 1, 1, 0);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void FrameBufferImpl::EndDraw() {
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glPopMatrix();
#ifdef GL_VIEWPORT_BIT
		glPopAttrib();
#else
		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
		
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
#endif
		glBindFramebuffer(GL_FRAMEBUFFER, systemFbo);
		glBindRenderbuffer(GL_RENDERBUFFER, systemBuffer);
		ClearBackgroundColor();
	}

	void FrameBufferImpl::Draw(const int x, const int y) const {
		glPushMatrix();
		jngl::Translate(x, y);
		glScalef(1.0f, -1.0f, 1.0f);
		jngl::Translate(0, -height);
		texture.Draw();
		glPopMatrix();
	}
}
