// Copyright 2011-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "FrameBuffer.hpp"

#include "../App.hpp"
#include "../main.hpp"
#include "../spriteimpl.hpp"
#include "../windowptr.hpp"
#include "ScaleablePixels.hpp"
#include "matrix.hpp"
#include "screen.hpp"

#ifdef JNGL_VULKAN
#include "../Renderer.hpp"
#include "../log.hpp"
#include "../opengl.hpp"
#include "../vulkan/VulkanRenderer.hpp"
#include "Rgba.hpp"
#else
#include "../ShaderCache.hpp"
#include "../texture.hpp"
#endif

#include <array>

namespace jngl {

namespace {

struct Active {
	std::function<void()> activate;
	int width;
	int height;
};

/// If this is not empty, there's a FrameBuffer in use: the top, with the function that activated
/// it
std::stack<Active> activateStack;

#ifdef JNGL_VULKAN
VulkanRenderer& vulkanRenderer() {
	return static_cast<VulkanRenderer&>(getRenderer());
}

/// Draws the framebuffer \a fb's image as a textured quad transformed by \a modelview. The quad is
/// (0, 0)-(width, height) in screen pixels with (0, 0)-(1, 1) texture coordinates, matching what
/// jngl::Texture builds.
void drawFramebuffer(const VulkanFramebuffer& fb, const Mat3& modelview, Rgba color) {
	const auto w = static_cast<float>(fb.width / getScaleFactor());
	const auto h = static_cast<float>(fb.height / getScaleFactor());
	const std::array<float, 16> vertexes = {
		0.f, 0.f, 0.f, 0.f, // x, y, u, v
		0.f, h,   0.f, 1.f, //
		w,   h,   1.f, 1.f, //
		w,   0.f, 1.f, 0.f, //
	};
	vulkanRenderer().drawSprite(fb.color, vertexes.data(), 4, PrimitiveType::TriangleFan, modelview,
	                            color);
}
#endif

} // namespace

optional<std::array<int, 2>> activeFrameBufferSize() {
	if (activateStack.empty()) {
		return std::nullopt;
	}
	return std::array{ activateStack.top().width, activateStack.top().height };
}

void applyBlendMode() {
#ifdef JNGL_VULKAN
	// TODO: The Vulkan backend has no separate alpha blend function for FrameBuffers yet
	vulkanRenderer().setSpriteBlend(gBlendMode == BlendMode::Premultiplied
	                                    ? VulkanRenderer::SpriteBlend::Premultiplied
	                                    : VulkanRenderer::SpriteBlend::Composite);
#else
	if (gBlendMode) {
		switch (*gBlendMode) {
		case BlendMode::Composite:
			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE,
			                    GL_ONE_MINUS_SRC_ALPHA);
			break;
		case BlendMode::Premultiplied:
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			break;
		}
	} else if (activateStack.empty()) {
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	} else {
		// Alpha adds up, so that what's drawn into the FrameBuffer is as opaque as the most opaque
		// of it when it's drawn again
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
	}
#endif
}

struct FrameBuffer::Impl {
#ifdef JNGL_VULKAN
	Impl(int width, int height, bool /*hdr*/)
	: width(width), height(height), vkFramebuffer(vulkanRenderer().createFramebuffer(width, height)) {
	}
#else
	Impl(int width, int height, bool hdr)
	: width(width), height(height),
	  texture(static_cast<float>(width), static_cast<float>(height), width, height, nullptr,
	          GL_RGBA, nullptr, hdr ? GL_HALF_FLOAT : GL_UNSIGNED_BYTE) {
	}
#endif
	Impl(const Impl&) = delete;
	Impl& operator=(const Impl&) = delete;
	Impl(Impl&&) = delete;
	Impl& operator=(Impl&&) = delete;

	~Impl() {
#ifdef JNGL_VULKAN
		if (vkFramebuffer) {
			if (auto* renderer = getRendererIfExists()) {
				static_cast<VulkanRenderer*>(renderer)->destroyFramebuffer(*vkFramebuffer);
			}
		}
#else
		if (pWindow) { // Don't bother deleting OpenGL objects when the OpenGL context has already
			           // been destroyed
			glDeleteFramebuffers(1, &fbo);
			glDeleteRenderbuffers(1, &buffer);
		}
#endif
	}

	const int width;
	const int height;
#ifdef JNGL_VULKAN
	std::unique_ptr<VulkanFramebuffer> vkFramebuffer;
#else
	GLuint fbo = 0;
	GLuint buffer = 0;
	Texture texture;
#if !defined(GL_VIEWPORT_BIT) || defined(__APPLE__)
	GLint viewport[4]{};
#endif
#endif

	/// The projection matrix of the window, saved when the outermost FrameBuffer gets activated.
	/// FrameBuffers used inside of it scale this one instead of their outer FrameBuffer's.
	static Mat4 screenProjection;
};

Mat4 FrameBuffer::Impl::screenProjection;

FrameBuffer::FrameBuffer(const Pixels width, const Pixels height, const bool hdr)
: impl(std::make_unique<Impl>(static_cast<int>(width), static_cast<int>(height), hdr)) {
#ifdef JNGL_VULKAN
	// createFramebuffer already cleared the image to transparent.
	(void)hdr; // HDR framebuffers aren't supported on Vulkan yet.
#else
	glGenRenderbuffers(1, &impl->buffer);
	glBindRenderbuffer(GL_RENDERBUFFER, impl->buffer);
	glRenderbufferStorage(GL_RENDERBUFFER, hdr ? GL_RGBA16F : GL_RGBA8, static_cast<int>(width),
	                      static_cast<int>(height));

	glGenFramebuffers(1, &impl->fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, impl->fbo);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, impl->buffer);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
	                       impl->texture.getID(), 0);

	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	// All of it, whatever's clipped on the window by its letterboxing or a jngl::scissor()
	const bool scissorTest = glIsEnabled(GL_SCISSOR_TEST);
	glDisable(GL_SCISSOR_TEST);
	glClearColor(1, 1, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	clearBackgroundColor();
	if (scissorTest) {
		glEnable(GL_SCISSOR_TEST);
	}

	if (activateStack.empty()) {
		pWindow->bindSystemFramebufferAndRenderbuffer();
	} else {
		activateStack.top().activate(); // back to the FrameBuffer that's in use
	}
#endif
}

FrameBuffer::FrameBuffer(ScaleablePixels width, ScaleablePixels height, const bool hdr)
: FrameBuffer(static_cast<Pixels>(width), static_cast<Pixels>(height), hdr) {
}

FrameBuffer::FrameBuffer(std::array<Pixels, 2> size, const bool hdr)
: FrameBuffer(size[0], size[1], hdr) {
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
#ifdef JNGL_VULKAN
	if (shaderProgram) {
		const auto context = shaderProgram->use();
		drawFramebuffer(*impl->vkFramebuffer, opengl::modelview, gSpriteColor);
	} else {
		drawFramebuffer(*impl->vkFramebuffer, opengl::modelview, gSpriteColor);
	}
#else
	auto context =
	    shaderProgram ? shaderProgram->use() : ShaderCache::handle().textureShaderProgram->use();
	if (shaderProgram) {
		glUniformMatrix3fv(shaderProgram->getUniformLocation("modelview"), 1, GL_FALSE,
		                   opengl::modelview.data);
	} else {
		ShaderCache::handle().setTextureColorUniforms(gSpriteColor);
		glUniformMatrix3fv(ShaderCache::handle().modelviewUniform, 1, GL_FALSE,
		                   opengl::modelview.data);
	}
	impl->texture.draw();
#endif
	popMatrix();
}

void FrameBuffer::draw(Mat3 modelview, const ShaderProgram* const shaderProgram) const {
	modelview.scale(1, -1).translate(
	    { -impl->width / getScaleFactor() / 2, -impl->height / getScaleFactor() / 2 });
#ifdef JNGL_VULKAN
	if (shaderProgram) {
		const auto context = shaderProgram->use();
		drawFramebuffer(*impl->vkFramebuffer, modelview, gSpriteColor);
	} else {
		drawFramebuffer(*impl->vkFramebuffer, modelview, gSpriteColor);
	}
#else
	auto context =
	    shaderProgram ? shaderProgram->use() : ShaderCache::handle().textureShaderProgram->use();
	if (shaderProgram) {
		glUniformMatrix3fv(shaderProgram->getUniformLocation("modelview"), 1, GL_FALSE,
		                   modelview.data);
	} else {
		ShaderCache::handle().setTextureColorUniforms(gSpriteColor);
		glUniformMatrix3fv(ShaderCache::handle().modelviewUniform, 1, GL_FALSE, modelview.data);
	}
	impl->texture.draw();
#endif
}

void FrameBuffer::draw(Mat3 modelview, const TextureFilter textureFilter,
                       const ShaderProgram* const shaderProgram) const {
#ifdef JNGL_VULKAN
	// The Vulkan backend bakes the filter into the framebuffer's sampler at creation, so the
	// per-draw textureFilter is ignored for now.
	(void)textureFilter;
	modelview.scale(1, -1).translate(
	    { -impl->width / getScaleFactor() / 2, -impl->height / getScaleFactor() / 2 });
	if (shaderProgram) {
		const auto context = shaderProgram->use();
		drawFramebuffer(*impl->vkFramebuffer, modelview, gSpriteColor);
	} else {
		drawFramebuffer(*impl->vkFramebuffer, modelview, gSpriteColor);
	}
#else
	impl->texture.bind();
	int oldFilter;
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &oldFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
	                textureFilter == TextureFilter::NearestNeighbor ? GL_NEAREST : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
	                textureFilter == TextureFilter::NearestNeighbor ? GL_NEAREST : GL_LINEAR);
	auto context =
	    shaderProgram ? shaderProgram->use() : ShaderCache::handle().textureShaderProgram->use();
	if (shaderProgram) {
		glUniformMatrix3fv(shaderProgram->getUniformLocation("modelview"), 1, GL_FALSE,
		                   modelview.scale(1, -1)
		                       .translate({ -impl->width / getScaleFactor() / 2,
		                                    -impl->height / getScaleFactor() / 2 })
		                       .data);
	} else {
		ShaderCache::handle().setTextureColorUniforms(gSpriteColor);
		glUniformMatrix3fv(ShaderCache::handle().modelviewUniform, 1, GL_FALSE,
		                   modelview.scale(1, -1)
		                       .translate({ -impl->width / getScaleFactor() / 2,
		                                    -impl->height / getScaleFactor() / 2 })
		                       .data);
	}
	glDrawArrays(GL_TRIANGLE_FAN, 0,
	             4); // no need to bind again, that's why we don't call impl->texture.draw() here
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, oldFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, oldFilter);
#endif
}

void FrameBuffer::drawMesh(const Mat3& modelview, const std::vector<Vertex>& vertexes,
                           const ShaderProgram* const shaderProgram) const {
	drawMesh(modelview, vertexes, gSpriteColor, shaderProgram);
}

void FrameBuffer::drawMesh(const Mat3& modelview, const std::vector<Vertex>& vertexes,
                           const Rgba color, const ShaderProgram* const shaderProgram) const {
	if (vertexes.empty()) {
		return;
	}
#ifdef JNGL_VULKAN
	std::optional<ShaderProgram::Context> context;
	if (shaderProgram) {
		context.emplace(shaderProgram->use());
	}
	// jngl::Vertex is a tightly packed { x, y, u, v }, matching what drawSprite expects.
	vulkanRenderer().drawSprite(impl->vkFramebuffer->color,
	                            reinterpret_cast<const float*>(vertexes.data()), vertexes.size(),
	                            PrimitiveType::Triangles, modelview, color);
#else
	auto context =
	    shaderProgram ? shaderProgram->use() : ShaderCache::handle().textureShaderProgram->use();
	if (shaderProgram) {
		glUniformMatrix3fv(shaderProgram->getUniformLocation("modelview"), 1, GL_FALSE,
		                   modelview.data);
	} else {
		ShaderCache::handle().setTextureColorUniforms(color);
		glUniformMatrix3fv(ShaderCache::handle().modelviewUniform, 1, GL_FALSE, modelview.data);
	}
	impl->texture.drawMesh(vertexes);
#endif
}

void FrameBuffer::drawMesh(const std::vector<Vertex>& vertexes,
                           const ShaderProgram* const shaderProgram) const {
#ifdef JNGL_VULKAN
	if (!vertexes.empty()) {
		// jngl::Vertex is a tightly packed { x, y, u, v }, matching what drawSprite expects.
		if (shaderProgram) {
			const auto context = shaderProgram->use();
			vulkanRenderer().drawSprite(
			    impl->vkFramebuffer->color, reinterpret_cast<const float*>(vertexes.data()),
			    vertexes.size(), PrimitiveType::Triangles, opengl::modelview, gSpriteColor);
		} else {
			vulkanRenderer().drawSprite(
			    impl->vkFramebuffer->color, reinterpret_cast<const float*>(vertexes.data()),
			    vertexes.size(), PrimitiveType::Triangles, opengl::modelview, gSpriteColor);
		}
	}
#else
	pushMatrix();
	auto context =
	    shaderProgram ? shaderProgram->use() : ShaderCache::handle().textureShaderProgram->use();
	if (shaderProgram) {
		glUniformMatrix3fv(shaderProgram->getUniformLocation("modelview"), 1, GL_FALSE,
		                   opengl::modelview.data);
	} else {
		ShaderCache::handle().setTextureColorUniforms(gSpriteColor);
		glUniformMatrix3fv(ShaderCache::handle().modelviewUniform, 1, GL_FALSE,
		                   opengl::modelview.data);
	}
	impl->texture.drawMesh(vertexes);
	popMatrix();
#endif
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
#ifdef JNGL_VULKAN
	vulkanRenderer().clearCurrentRenderTarget(Rgba(1, 1, 1, 0));
#else
	glClearColor(1, 1, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT);
#endif
}

void FrameBuffer::Context::clear(const Rgb color) {
	assert(resetCallback);
#ifdef JNGL_VULKAN
	vulkanRenderer().clearCurrentRenderTarget(
	    Rgba(color.getRed(), color.getGreen(), color.getBlue(), 1.f));
#else
	glClearColor(color.getRed(), color.getGreen(), color.getBlue(), 1);
	glClear(GL_COLOR_BUFFER_BIT);
#endif
}

void FrameBuffer::Context::clear(const Rgba color) {
	assert(resetCallback);
#ifdef JNGL_VULKAN
	vulkanRenderer().clearCurrentRenderTarget(color);
#else
	glClearColor(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
	glClear(GL_COLOR_BUFFER_BIT);
#endif
}

FrameBuffer::Context FrameBuffer::use() const {
#ifdef JNGL_VULKAN
	pushMatrix();
	auto savedProjection = opengl::projection;
	if (activateStack.empty()) {
		Impl::screenProjection = opengl::projection;
	} else {
		opengl::projection = Impl::screenProjection;
	}
	const float sx = static_cast<float>(pWindow->getWidth()) / static_cast<float>(impl->width) *
	                 pWindow->getResizedWindowScalingX();
	const float sy = static_cast<float>(pWindow->getHeight()) / static_cast<float>(impl->height) *
	                 pWindow->getResizedWindowScalingY();
	for (int i = 0; i < 4; ++i) {
		opengl::projection.data[i] *= sx;
		opengl::projection.data[i + 4] *= sy;
	}
	vulkanRenderer().setProjection(opengl::projection);
	vulkanRenderer().pushRenderTarget(*impl->vkFramebuffer);
	// The render target has already been pushed, which is all there is to switch on Vulkan
	auto activate = []() {
		applyBlendMode();
		applyScissor();
	};
	activateStack.emplace(Active{ std::move(activate), impl->width, impl->height });
	activateStack.top().activate();
	return Context([savedProjection]() {
		activateStack.pop();
		vulkanRenderer().popRenderTarget();
		popMatrix();
		opengl::projection = savedProjection;
		vulkanRenderer().setProjection(opengl::projection);
		applyBlendMode();
		applyScissor();
	});
#else
	auto activate = [this]() {
		glBindFramebuffer(GL_FRAMEBUFFER, impl->fbo);
		glBindRenderbuffer(GL_RENDERBUFFER, impl->buffer);
		glViewport(0, 0, impl->width, impl->height);
		// This FrameBuffer's on activateStack by now: the window's letterboxing doesn't apply here,
		// a jngl::scissor() does
		applyBlendMode();
		applyScissor();
	};
	pushMatrix();
	auto savedProjection = opengl::projection;
	if (activateStack.empty()) {
		Impl::screenProjection = opengl::projection;
	} else {
		// Scaling the outer FrameBuffer's projection again would only be right if its factors were
		// 1, which isn't the case e.g. with letterboxing after the window has been resized
		opengl::projection = Impl::screenProjection;
	}
	const float sx = static_cast<float>(pWindow->getWidth()) / static_cast<float>(impl->width) *
	                 pWindow->getResizedWindowScalingX();
	const float sy = static_cast<float>(pWindow->getHeight()) / static_cast<float>(impl->height) *
	                 pWindow->getResizedWindowScalingY();
	// Scale the projection matrix (right-multiply by diag(sx, sy, 1, 1)) so that drawing inside the
	// framebuffer works with letterboxing. Unlike modifying the modelview, this isn't undone when
	// the user calls jngl::reset() or uses jngl::Mat3().
	opengl::projection.data[0] *= sx;
	opengl::projection.data[1] *= sx;
	opengl::projection.data[2] *= sx;
	opengl::projection.data[3] *= sx;
	opengl::projection.data[4] *= sy;
	opengl::projection.data[5] *= sy;
	opengl::projection.data[6] *= sy;
	opengl::projection.data[7] *= sy;
	App::instance().updateProjectionMatrix();
#if defined(GL_VIEWPORT_BIT) && !defined(__APPLE__)
	glPushAttrib(GL_VIEWPORT_BIT);
#else
	glGetIntegerv(GL_VIEWPORT, impl->viewport);
#endif
	activateStack.emplace(Active{ std::move(activate), impl->width, impl->height });
	activateStack.top().activate();
	return Context([this, savedProjection]() {
		activateStack.pop();
		popMatrix();
#if defined(GL_VIEWPORT_BIT) && !defined(__APPLE__)
		glPopAttrib();
#else
		glViewport(impl->viewport[0], impl->viewport[1], impl->viewport[2], impl->viewport[3]);
#endif
		opengl::projection = savedProjection;
		App::instance().updateProjectionMatrix();
		if (!activateStack.empty()) {
			activateStack.top().activate(); // Restore the FrameBuffer that was previously active
			return;
		}
		applyBlendMode();
		applyScissor(); // the window's letterboxing again, with a jngl::scissor() inside it
		pWindow->bindSystemFramebufferAndRenderbuffer();
		clearBackgroundColor();
	});
#endif
}

void FrameBuffer::clear() {
#ifdef JNGL_VULKAN
	vulkanRenderer().clearCurrentRenderTarget(Rgba(1, 1, 1, 0));
#else
	glClearColor(1, 1, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	clearBackgroundColor();
#endif
}

Vec2 FrameBuffer::getSize() const {
#ifdef JNGL_VULKAN
	return { impl->width / getScaleFactor(), impl->height / getScaleFactor() };
#else
	return { impl->texture.getPreciseWidth() / getScaleFactor(),
		     impl->texture.getPreciseHeight() / getScaleFactor() };
#endif
}

Pixels FrameBuffer::getPixelWidth() const {
	return static_cast<Pixels>(impl->width);
}

Pixels FrameBuffer::getPixelHeight() const {
	return static_cast<Pixels>(impl->height);
}

GLuint FrameBuffer::getTextureID() const {
#ifdef JNGL_VULKAN
	return 0; // no equivalent on Vulkan
#else
	return impl->texture.getID();
#endif
}

} // namespace jngl
