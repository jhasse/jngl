// Copyright 2011-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "FrameBuffer.hpp"

#include "../ShaderCache.hpp"
#include "../main.hpp"
#include "../spriteimpl.hpp"
#include "../texture.hpp"
#include "../windowptr.hpp"
#include "ScaleablePixels.hpp"
#include "matrix.hpp"
#include "screen.hpp"

namespace jngl {

struct FrameBuffer::Impl {
	Impl(int width, int height)
	: width(width), height(height),
	  texture(static_cast<float>(width), static_cast<float>(height), width, height, nullptr),
	  letterboxing(glIsEnabled(GL_SCISSOR_TEST)) {
	}
	Impl(const Impl&) = delete;
	Impl& operator=(const Impl&) = delete;
	Impl(Impl&&) = delete;
	Impl& operator=(Impl&&) = delete;

	~Impl() {
		if (pWindow) { // Don't bother deleting OpenGL objects when the OpenGL context has already
			           // been destroyed
			glDeleteFramebuffers(1, &fbo);
			glDeleteRenderbuffers(1, &buffer);
		}
	}

	GLuint fbo = 0;
	GLuint buffer = 0;
	const int width;
	const int height;
	Texture texture;
	bool letterboxing;
#if !defined(GL_VIEWPORT_BIT) || defined(__APPLE__)
	GLint viewport[4]{};
#endif

	/// If this is not empty, there's a FrameBuffer in use and this was the function that activated
	/// it.
	static std::stack<std::function<void()>> activate;
};

std::stack<std::function<void()>> FrameBuffer::Impl::activate;

FrameBuffer::FrameBuffer(const Pixels width, const Pixels height)
: impl(std::make_unique<Impl>(static_cast<int>(width), static_cast<int>(height))) {
	glGenRenderbuffers(1, &impl->buffer);
	glBindRenderbuffer(GL_RENDERBUFFER, impl->buffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, static_cast<int>(width),
	                      static_cast<int>(height));

	glGenFramebuffers(1, &impl->fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, impl->fbo);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, impl->buffer);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
	                       impl->texture.getID(), 0);

	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	if (impl->letterboxing) {
		glDisable(GL_SCISSOR_TEST);
	}
	glClearColor(1, 1, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	clearBackgroundColor();
	if (impl->letterboxing) {
		glEnable(GL_SCISSOR_TEST);
	}

	pWindow->bindSystemFramebufferAndRenderbuffer();
}

FrameBuffer::FrameBuffer(ScaleablePixels width, ScaleablePixels height)
: FrameBuffer(static_cast<Pixels>(width), static_cast<Pixels>(height)) {
}

FrameBuffer::FrameBuffer(std::array<Pixels, 2> size) : FrameBuffer(size[0], size[1]) {
}

FrameBuffer::FrameBuffer(FrameBuffer&&) noexcept = default;
FrameBuffer& FrameBuffer::operator=(FrameBuffer&&) noexcept = default;
FrameBuffer::~FrameBuffer() = default;

void FrameBuffer::draw(const double x, const double y) const {
	draw(Vec2{ x, y });
}

void FrameBuffer::draw(const Vec2 position, const ShaderProgram* const shaderProgram) const {
	pushMatrix();
	jngl::translate(position);
	opengl::scale(1, -1);
	jngl::translate(0, -impl->height / getScaleFactor());
	auto context =
	    shaderProgram ? shaderProgram->use() : ShaderCache::handle().textureShaderProgram->use();
	if (shaderProgram) {
		glUniformMatrix3fv(shaderProgram->getUniformLocation("modelview"), 1, GL_FALSE,
		                   opengl::modelview.data);
	} else {
		glUniform4f(ShaderCache::handle().shaderSpriteColorUniform, gSpriteColor.getRed(),
		            gSpriteColor.getGreen(), gSpriteColor.getBlue(), gSpriteColor.getAlpha());
		glUniformMatrix3fv(ShaderCache::handle().modelviewUniform, 1, GL_FALSE,
		                   opengl::modelview.data);
	}
	impl->texture.draw();
	popMatrix();
}

void FrameBuffer::draw(Mat3 modelview, const ShaderProgram* const shaderProgram) const {
	auto context =
	    shaderProgram ? shaderProgram->use() : ShaderCache::handle().textureShaderProgram->use();
	if (shaderProgram) {
		glUniformMatrix3fv(shaderProgram->getUniformLocation("modelview"), 1, GL_FALSE,
		                   modelview.scale(1, -1)
		                       .translate({ -impl->width / getScaleFactor() / 2,
		                                    -impl->height / getScaleFactor() / 2 })
		                       .data);
	} else {
		glUniform4f(ShaderCache::handle().shaderSpriteColorUniform, gSpriteColor.getRed(),
		            gSpriteColor.getGreen(), gSpriteColor.getBlue(), gSpriteColor.getAlpha());
		glUniformMatrix3fv(ShaderCache::handle().modelviewUniform, 1, GL_FALSE,
		                   modelview.scale(1, -1)
		                       .translate({ -impl->width / getScaleFactor() / 2,
		                                    -impl->height / getScaleFactor() / 2 })
		                       .data);
	}
	impl->texture.draw();
}

void FrameBuffer::drawMesh(const std::vector<Vertex>& vertexes,
                           const ShaderProgram* const shaderProgram) const {
	pushMatrix();
	scale(getScaleFactor());
	auto context =
	    shaderProgram ? shaderProgram->use() : ShaderCache::handle().textureShaderProgram->use();
	if (shaderProgram) {
		glUniformMatrix3fv(shaderProgram->getUniformLocation("modelview"), 1, GL_FALSE,
		                   opengl::modelview.data);
	} else {
		glUniform4f(ShaderCache::handle().shaderSpriteColorUniform, gSpriteColor.getRed(),
		            gSpriteColor.getGreen(), gSpriteColor.getBlue(), gSpriteColor.getAlpha());
		glUniformMatrix3fv(ShaderCache::handle().modelviewUniform, 1, GL_FALSE,
		                   opengl::modelview.data);
	}
	impl->texture.drawMesh(vertexes);
	popMatrix();
}

FrameBuffer::Context::Context(std::function<void()> resetCallback)
: resetCallback(std::move(resetCallback)) {
}

FrameBuffer::Context::Context(Context&& other) noexcept
: resetCallback(std::move(other.resetCallback)) {
	other.resetCallback = nullptr;
}

FrameBuffer::Context& FrameBuffer::Context::operator=(Context&& other) noexcept {
	resetCallback = other.resetCallback;
	other.resetCallback = nullptr;
	return *this;
}

FrameBuffer::Context::~Context() {
	if (resetCallback) {
		resetCallback();
	}
}

void FrameBuffer::Context::clear() {
	assert(resetCallback);
	glClearColor(1, 1, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT);
}

void FrameBuffer::Context::clear(const Rgb color) {
	assert(resetCallback);
	glClearColor(color.getRed(), color.getGreen(), color.getBlue(), 1);
	glClear(GL_COLOR_BUFFER_BIT);
}

FrameBuffer::Context FrameBuffer::use() const {
	auto activate = [this]() {
		glBindFramebuffer(GL_FRAMEBUFFER, impl->fbo);
		glBindRenderbuffer(GL_RENDERBUFFER, impl->buffer);
		glViewport(0, 0, impl->width, impl->height);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);

		// each time we active we have to check again, because the window might have been resized
		if (!impl->letterboxing) {
			impl->letterboxing = glIsEnabled(GL_SCISSOR_TEST);
		}
		if (impl->letterboxing) {
			glDisable(GL_SCISSOR_TEST);
		}
	};
	pushMatrix();
	reset();
	opengl::scale(static_cast<float>(pWindow->getWidth()) / static_cast<float>(impl->width) *
	                  pWindow->getResizedWindowScalingX(),
	              static_cast<float>(pWindow->getHeight()) / static_cast<float>(impl->height) *
	                  pWindow->getResizedWindowScalingY());
#if defined(GL_VIEWPORT_BIT) && !defined(__APPLE__)
	glPushAttrib(GL_VIEWPORT_BIT);
#else
	glGetIntegerv(GL_VIEWPORT, impl->viewport);
#endif
	activate();
	Impl::activate.emplace(std::move(activate));
	return Context([this]() {
		Impl::activate.pop();
		popMatrix();
#if defined(GL_VIEWPORT_BIT) && !defined(__APPLE__)
		glPopAttrib();
#else
		glViewport(impl->viewport[0], impl->viewport[1], impl->viewport[2], impl->viewport[3]);
#endif
		if (!Impl::activate.empty()) {
			Impl::activate.top()(); // Restore the FrameBuffer that was previously active
			return;
		}
		if (impl->letterboxing) {
			glEnable(GL_SCISSOR_TEST);
		}
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		pWindow->bindSystemFramebufferAndRenderbuffer();
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

Pixels FrameBuffer::getPixelWidth() const {
	return static_cast<Pixels>(impl->width);
}

Pixels FrameBuffer::getPixelHeight() const {
	return static_cast<Pixels>(impl->height);
}

GLuint FrameBuffer::getTextureID() const {
	return impl->texture.getID();
}

} // namespace jngl
