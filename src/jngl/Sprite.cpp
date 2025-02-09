// Copyright 2012-2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#ifndef NOPNG
#include <png.h> // Include first, see https://bugs.launchpad.net/ubuntu/+source/libpng/+bug/218409
#endif

#include "Sprite.hpp"

#include "../ShaderCache.hpp"
#include "../TextureCache.hpp"
#include "../helper.hpp"
#include "../log.hpp"
#include "../main.hpp"
#include "../spriteimpl.hpp"
#include "../texture.hpp"
#include "../windowptr.hpp"
#include "Alpha.hpp"
#include "matrix.hpp"
#include "screen.hpp"

#ifdef _WIN32
#include "../win32/unicode.hpp"
#endif

#ifdef ANDROID
#include "../android/fopen.hpp"
#endif

#if __cplusplus < 202002L
#include <boost/algorithm/string/predicate.hpp>
#endif
#include <cstddef>
#include <cstring>
#include <sstream>
#ifndef NOWEBP
#include "../ImageDataWebP.hpp"
#endif

namespace jngl {

Sprite::Sprite(const ImageData& imageData, double scale, std::optional<std::string_view> filename) {
	if (!pWindow) {
		throw std::runtime_error("Window hasn't been created yet.");
	}
	width = scale * imageData.getWidth();
	height = scale * imageData.getHeight();
	texture = filename ? TextureCache::handle().get(*filename) : nullptr;
	if (!texture) {
		texture = std::make_shared<Texture>(
		    static_cast<int>(std::lround(width)), static_cast<int>(std::lround(height)),
		    imageData.getWidth(), imageData.getHeight(), nullptr, GL_RGBA, imageData.pixels());
		setCenter(0, 0);
		if (filename) {
			TextureCache::handle().insert(*filename, texture);
		}
	}
}

Sprite::Sprite(const uint8_t* const bytes, const size_t width, const size_t height) {
	if (!pWindow) {
		throw std::runtime_error("Window hasn't been created yet.");
	}
	texture = std::make_shared<Texture>(width, height, static_cast<int>(width),
	                                    static_cast<int>(height), nullptr, GL_RGBA, bytes);
	Drawable::width = static_cast<float>(width);
	Drawable::height = static_cast<float>(height);
	setCenter(0, 0);
}

Sprite::Sprite(const std::string& filename, LoadType loadType)
: texture(TextureCache::handle().get(filename)) {
	if (texture) {
		width = texture->getPreciseWidth();
		height = texture->getPreciseHeight();
		setCenter(0, 0);
		return;
	}
	const bool halfLoad = (loadType == LoadType::HALF);
	if (!halfLoad) {
		internal::debug("Creating sprite {}...", filename);
	}
	auto fullFilename = pathPrefix + filename;
	const char* extensions[] = {
#ifndef NOWEBP
		".webp",
#endif
#ifndef NOPNG
		".png",
#endif
		".bmp"
	};
	std::function<Finally(Sprite*, std::string, FILE*, bool)> functions[] = {
#ifndef NOWEBP
		&Sprite::LoadWebP,
#endif
#ifndef NOPNG
		&Sprite::LoadPNG,
#endif
		&Sprite::LoadBMP
	};
	const size_t size = sizeof(extensions) / sizeof(extensions[0]);
	std::function<Finally(Sprite*, std::string, FILE*, bool)> loadFunction;
	for (size_t i = 0; i < size; ++i) {
#if __cplusplus < 202002L
		if (boost::algorithm::ends_with(fullFilename, extensions[i])) {
#else
		if (fullFilename.ends_with(extensions[i])) {
#endif
			loadFunction = functions[i];
			break;
		}
		std::string tmp = fullFilename + extensions[i];
		if (fileExists(tmp)) {
			fullFilename += extensions[i];
			loadFunction = functions[i];
			break;
		}
	}
	if (!loadFunction) {
		std::ostringstream message;
		message << "No suitable image file found for: " << fullFilename
		        << "\nSupported file extensions: ";
		for (size_t i = 0; i < size; ++i) {
			if (i != 0) {
				message << ", ";
			}
			message << extensions[i];
		}
		throw std::runtime_error(message.str());
	}
#ifdef _WIN32
	FILE* pFile = _wfopen(utf8ToUtf16(fullFilename).c_str(), L"rb");
#else
	FILE* pFile = fopen(fullFilename.c_str(), "rb");
#endif
	if (pFile == nullptr) {
		throw std::runtime_error(std::string("File not found: " + fullFilename));
	}
	auto loadTexture = std::make_shared<Finally>(loadFunction(this, filename, pFile, halfLoad));
	loader = std::make_shared<Finally>([pFile, loadTexture, this]() mutable {
		loadTexture.reset(); // call ~Finally
		if (fclose(pFile) != 0) {
			internal::warn("Error closing file: {}", strerror(errno));
		}
		setCenter(0, 0);
	});
	if (loadType != LoadType::THREADED) {
		loader.reset();
	}
}

void Sprite::step() {
}

void Sprite::draw() const {
	pushMatrix();
	opengl::translate(static_cast<float>(position.x), static_cast<float>(position.y));
	auto context = ShaderCache::handle().textureShaderProgram->use();
	glUniform4f(ShaderCache::handle().shaderSpriteColorUniform, gSpriteColor.getRed(),
	            gSpriteColor.getGreen(), gSpriteColor.getBlue(), gSpriteColor.getAlpha());
	glUniformMatrix3fv(ShaderCache::handle().modelviewUniform, 1, GL_FALSE, opengl::modelview.data);
	texture->draw();
	popMatrix();
}

void Sprite::draw(Mat3 modelview, const ShaderProgram* const shaderProgram) const {
	draw(modelview, Alpha(gSpriteColor.getAlpha()), shaderProgram);
}

void Sprite::draw(Mat3 modelview, Rgba color) const {
	modelview *=
	    boost::qvm::translation_mat(boost::qvm::vec<double, 2>({ -width / 2., -height / 2. }));
	auto context = ShaderCache::handle().textureShaderProgram->use();
	glUniform4f(ShaderCache::handle().shaderSpriteColorUniform, color.getRed(), color.getGreen(),
	            color.getBlue(), color.getAlpha());
	glUniformMatrix3fv(ShaderCache::handle().modelviewUniform, 1, GL_FALSE, modelview.data);
	texture->draw();
}

void Sprite::draw(Mat3 modelview, Alpha alpha, const ShaderProgram* const shaderProgram) const {
	modelview *=
	    boost::qvm::translation_mat(boost::qvm::vec<double, 2>({ -width / 2., -height / 2. }));
	auto context =
	    shaderProgram ? shaderProgram->use() : ShaderCache::handle().textureShaderProgram->use();
	if (shaderProgram) {
		glUniformMatrix3fv(shaderProgram->getUniformLocation("modelview"), 1, GL_FALSE,
		                   modelview.data);
	} else {
		glUniform4f(ShaderCache::handle().shaderSpriteColorUniform, gSpriteColor.getRed(),
		            gSpriteColor.getGreen(), gSpriteColor.getBlue(), alpha.getAlpha());
		glUniformMatrix3fv(ShaderCache::handle().modelviewUniform, 1, GL_FALSE, modelview.data);
	}
	texture->draw();
}

void Sprite::draw(const ShaderProgram* const shaderProgram) const {
	pushMatrix();
	opengl::translate(static_cast<float>(position.x), static_cast<float>(position.y));
	auto context =
	    shaderProgram ? shaderProgram->use() : ShaderCache::handle().textureShaderProgram->use();
	if (shaderProgram) {
		glUniformMatrix3fv(shaderProgram->getUniformLocation("modelview"), 1, GL_FALSE,
			opengl::modelview.data);
	}
	else {
		glUniform4f(ShaderCache::handle().shaderSpriteColorUniform, gSpriteColor.getRed(),
		            gSpriteColor.getGreen(), gSpriteColor.getBlue(), gSpriteColor.getAlpha());
		glUniformMatrix3fv(ShaderCache::handle().modelviewUniform, 1, GL_FALSE,
		                   opengl::modelview.data);
	}
	texture->draw();
	popMatrix();
}

struct Sprite::Batch::Impl {
	ShaderProgram::Context context;
	jngl::Mat3 translation;
	int modelviewUniform;
};

Sprite::Batch::Batch(std::unique_ptr<Impl> impl) : impl(std::move(impl)) {
}

Sprite::Batch::~Batch() = default;

void Sprite::Batch::draw(Mat3 modelview) const {
	modelview *= impl->translation;
	glUniformMatrix3fv(impl->modelviewUniform, 1, GL_FALSE, modelview.data);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // see Texture::draw()
}

auto Sprite::batch(const ShaderProgram* const shaderProgram) const -> Batch {
	auto context =
	    shaderProgram ? shaderProgram->use() : ShaderCache::handle().textureShaderProgram->use();
	if (!shaderProgram) {
		glUniform4f(ShaderCache::handle().shaderSpriteColorUniform, gSpriteColor.getRed(),
		            gSpriteColor.getGreen(), gSpriteColor.getBlue(), gSpriteColor.getAlpha());
	}
	texture->bind();
	return Batch{ std::make_unique<Batch::Impl>(Batch::Impl{
		std::move(context),
		boost::qvm::translation_mat(boost::qvm::vec<double, 2>({ -width / 2., -height / 2. })),
		shaderProgram ? shaderProgram->getUniformLocation("modelview")
		              : ShaderCache::handle().modelviewUniform }) };
}

void Sprite::drawScaled(float xfactor, float yfactor,
                        const ShaderProgram* const shaderProgram) const {
	pushMatrix();
	opengl::translate(static_cast<float>(position.x), static_cast<float>(position.y));
	opengl::scale(xfactor, yfactor);
	auto context =
	    shaderProgram ? shaderProgram->use() : ShaderCache::handle().textureShaderProgram->use();
	if (shaderProgram) {
		glUniformMatrix3fv(shaderProgram->getUniformLocation("modelview"), 1, GL_FALSE,
			opengl::modelview.data);
	}
	else {
		glUniform4f(ShaderCache::handle().shaderSpriteColorUniform, gSpriteColor.getRed(),
		            gSpriteColor.getGreen(), gSpriteColor.getBlue(), gSpriteColor.getAlpha());
		glUniformMatrix3fv(ShaderCache::handle().modelviewUniform, 1, GL_FALSE,
		                   opengl::modelview.data);
	}
	texture->draw();
	popMatrix();
}

void Sprite::drawClipped(float xstart, float xend, float ystart, float yend) const {
	pushMatrix();
	opengl::translate(xstart * width, ystart * height);
	drawClipped({ xstart, ystart }, { xend, yend });
	popMatrix();
}

void Sprite::drawClipped(const Vec2 start, const Vec2 end) const {
	pushMatrix();
	opengl::translate(static_cast<float>(position.x), static_cast<float>(position.y));
	texture->drawClipped(static_cast<float>(start.x), static_cast<float>(end.x),
	                     static_cast<float>(start.y), static_cast<float>(end.y),
	                     gSpriteColor.getRed(), gSpriteColor.getGreen(), gSpriteColor.getBlue(),
	                     gSpriteColor.getAlpha());
	popMatrix();
}

void Sprite::drawMesh(Mat3 modelview, const std::vector<Vertex>& vertexes,
                      const ShaderProgram* const shaderProgram) const {
	if (vertexes.empty()) {
		return;
	}
	modelview.scale(getScaleFactor());
	auto context =
	    shaderProgram ? shaderProgram->use() : ShaderCache::handle().textureShaderProgram->use();
	if (shaderProgram) {
		glUniformMatrix3fv(shaderProgram->getUniformLocation("modelview"), 1, GL_FALSE,
		                   modelview.data);
	} else {
		glUniform4f(ShaderCache::handle().shaderSpriteColorUniform, gSpriteColor.getRed(),
		            gSpriteColor.getGreen(), gSpriteColor.getBlue(), gSpriteColor.getAlpha());
		glUniformMatrix3fv(ShaderCache::handle().modelviewUniform, 1, GL_FALSE, modelview.data);
	}
	texture->drawMesh(vertexes);
}

void Sprite::drawMesh(const std::vector<Vertex>& vertexes,
                      const ShaderProgram* const shaderProgram) const {
	if (vertexes.empty()) {
		return;
	}
	pushMatrix();
	opengl::translate(static_cast<float>(position.x), static_cast<float>(position.y));
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
	texture->drawMesh(vertexes);
	popMatrix();
}

void Sprite::setBytes(const unsigned char* const bytes) {
	texture->setBytes(bytes, static_cast<int>(std::lround(width)),
	                  static_cast<int>(std::lround(height)));
}

const Shader& Sprite::vertexShader() {
	return *ShaderCache::handle().textureVertexShader;
}

#ifndef NOPNG
Finally Sprite::LoadPNG(const std::string& filename, FILE* const fp, const bool halfLoad) {
	const unsigned int PNG_BYTES_TO_CHECK = 4;
	png_byte buf[PNG_BYTES_TO_CHECK];

	// Read in some of the signature bytes
	if (fread(buf, 1, PNG_BYTES_TO_CHECK, fp) != PNG_BYTES_TO_CHECK ||
	    png_sig_cmp(buf, static_cast<png_size_t>(0), PNG_BYTES_TO_CHECK) != 0) {
		throw std::runtime_error(std::string("Error reading signature bytes. (" + filename + ")"));
	}

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	if (!png_ptr) {
		throw std::runtime_error(std::string("libpng error while reading (" + filename + ")"));
	}

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		throw std::runtime_error(std::string("libpng error while reading (" + filename + ")"));
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		// Free all of the memory associated with the png_ptr and info_ptr
		png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
		throw std::runtime_error(std::string("Error reading file. (" + filename + ")"));
	}
	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, PNG_BYTES_TO_CHECK);
	if (const int colorType = png_get_color_type(png_ptr, info_ptr);
	    colorType == PNG_COLOR_TYPE_GRAY || colorType == PNG_COLOR_TYPE_GRAY_ALPHA) {
		png_set_gray_to_rgb(png_ptr);
	}
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_STRIP_16, nullptr);

	GLenum format;
	switch (png_get_channels(png_ptr, info_ptr)) {
		case 1: // gray or palette, we can set format to RGB because we passed PNG_TRANSFORM_EXPAND
		        // to png_read_png
		case 3:
			format = GL_RGB;
			break;
		case 2: // gray + alpha
		case 4:
			format = GL_RGBA;
			break;
		default:
			throw std::runtime_error(
			    std::string("Unsupported number of channels. (" + filename + ")"));
	}

	Finally freePng([&png_ptr, &info_ptr]() {
		png_destroy_read_struct(&png_ptr, &info_ptr, static_cast<png_infop*>(nullptr));
	});
	unsigned char** rowPointers = png_get_rows(png_ptr, info_ptr);
	const auto scaledWidth = static_cast<int>(png_get_image_width(png_ptr, info_ptr));
	const auto scaledHeight = static_cast<int>(png_get_image_height(png_ptr, info_ptr));
	width = static_cast<float>(scaledWidth * getScaleFactor());
	height = static_cast<float>(scaledHeight * getScaleFactor());
	loadTexture(scaledWidth, scaledHeight, filename, halfLoad, format, rowPointers);
	return Finally(nullptr);
}
#endif

void Sprite::cleanUpRowPointers(std::vector<unsigned char*>& buf) {
	for (const auto p : buf) {
		delete[] p;
	}
}

Finally Sprite::LoadBMP(const std::string& filename, FILE* const fp, const bool halfLoad) {
	if (fseek(fp, 10, SEEK_SET) != 0) {
		throw std::runtime_error(std::string("Error seeking file. (" + filename + ")"));
	}
	BMPHeader header{};
	if (!fread(&header, sizeof(header), 1, fp)) {
		throw std::runtime_error(std::string("Error reading file. (" + filename + ")"));
	}
	if (header.headerSize != 40) {
		throw std::runtime_error(std::string("Unsupported header size. (" + filename + ")"));
	}
	if (header.bpp != 24) {
		throw std::runtime_error(std::string("Bpp not supported. (" + filename + ")"));
	}
	if (header.compression != 0) {
		throw std::runtime_error(std::string("Compression not supported. (" + filename + ")"));
	}
	if (header.dataSize == 0) {
		header.dataSize = header.width * header.height * 3;
	}
	std::vector<unsigned char*> buf(header.height);
	for (auto& row : buf) {
		row = new unsigned char[static_cast<unsigned long>(header.width * 3)];
	}
	Finally cleanUp([&buf]() { cleanUpRowPointers(buf); });

	if (header.height < 0) {
		header.height = -header.height;
		for (int i = 0; i < header.height; ++i) {
			if (fseek(fp, header.dataOffset + static_cast<long>(i) * header.width * 3, SEEK_SET) !=
			    0) {
				throw std::runtime_error(std::string("Error reading file. (" + filename + ")"));
			}
			if (!fread(buf[i], static_cast<size_t>(header.width) * 3, 1, fp)) {
				throw std::runtime_error(std::string("Error reading data. (" + filename + ")"));
			}
		}
	} else { // "bottom-up"-Bitmap
		for (int i = header.height - 1; i >= 0; --i) {
			if (fseek(fp, header.dataOffset + static_cast<long>(i) * header.width * 3, SEEK_SET) !=
			    0) {
				throw std::runtime_error(std::string("Error reading file. (" + filename + ")"));
			}
			if (!fread(buf[(header.height - 1) - i], static_cast<size_t>(header.width) * 3, 1,
			           fp)) {
				throw std::runtime_error(std::string("Error reading data. (" + filename + ")"));
			}
		}
	}
	width = static_cast<float>(header.width * getScaleFactor());
	height = static_cast<float>(header.height * getScaleFactor());
	loadTexture(header.width, header.height, filename, halfLoad, GL_BGR, buf.data());
	return Finally(nullptr);
}
#ifndef NOWEBP
Finally Sprite::LoadWebP(const std::string& filename, FILE* file, const bool halfLoad) {
	auto imageData = std::make_shared<ImageDataWebP>(filename, file, getScaleFactor());
	width = static_cast<float>(imageData->getImageWidth() * getScaleFactor());
	height = static_cast<float>(imageData->getImageHeight() * getScaleFactor());
	return Finally([imageData = std::move(imageData), filename, halfLoad, this]() mutable {
		loadTexture(imageData->getWidth(), imageData->getHeight(), filename, halfLoad, GL_RGBA,
		            nullptr, imageData->pixels());
	});
}
#endif

void Sprite::loadTexture(const int scaledWidth, const int scaledHeight, const std::string& filename,
                         const bool halfLoad, const unsigned int format,
                         const unsigned char* const* const rowPointers,
                         const unsigned char* const data) {
	if (!pWindow) {
		if (halfLoad) {
			return;
		}
		throw std::runtime_error(std::string("Window hasn't been created yet. (" + filename + ")"));
	}
	texture = std::make_shared<Texture>(width, height, scaledWidth, scaledHeight, rowPointers,
	                                    format, data);
	TextureCache::handle().insert(filename, texture);
}

Finally disableBlending() {
	if (!glIsEnabled(GL_BLEND)) {
		return Finally(nullptr);
	}
	glDisable(GL_BLEND);
	return Finally([]() {
		glEnable(GL_BLEND);
	});
}

} // namespace jngl
