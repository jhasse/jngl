// Copyright 2012-2022 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#ifndef NOPNG
#include <png.h> // Include first, see https://bugs.launchpad.net/ubuntu/+source/libpng/+bug/218409
#endif

#include "sprite.hpp"

#include "../freetype.hpp"
#include "../helper.hpp"
#include "../main.hpp"
#include "../spriteimpl.hpp"
#include "debug.hpp"
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
#include <sstream>
#include <thread>
#ifndef NOWEBP
#include "../ImageDataWebP.hpp"
#endif

namespace jngl {

std::shared_ptr<Texture> getTexture(const std::string& filename) {
	auto it = textures.find(filename);
	if (it == textures.end()) {
		return nullptr;
	}
	return it->second;
}

Sprite::Sprite(const std::string& filename, const ImageData& imageData) {
	if (!pWindow) {
		throw std::runtime_error("Window hasn't been created yet.");
	}
	width = imageData.getImageWidth();
	height = imageData.getImageHeight();
	auto it = textures.find(filename);
	if (it == textures.end()) {
		textures[filename] = texture =
		    std::make_shared<Texture>(imageData.getImageWidth(), imageData.getImageHeight(), width,
		                              height, nullptr, GL_RGBA, imageData.pixels());
	} else {
		texture = it->second;
	}
	setCenter(0, 0);
}

Sprite::Sprite(const std::string& filename, LoadType loadType) : texture(getTexture(filename)) {
	if (texture) {
		width = texture->getPreciseWidth();
		height = texture->getPreciseHeight();
		setCenter(0, 0);
		return;
	}
	const bool halfLoad = (loadType == LoadType::HALF);
	if (!halfLoad) {
		jngl::debug("Creating sprite ");
		jngl::debug(filename);
		jngl::debug("... ");
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
	loader = std::make_shared<Finally>([pFile, loadTexture, halfLoad, this]() mutable {
		loadTexture.reset(); // call ~Finally
		fclose(pFile);
		setCenter(0, 0);
		if (!halfLoad) {
			jngl::debugLn("OK");
		}
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
	auto context = Texture::textureShaderProgram->use();
	glUniform4f(Texture::shaderSpriteColorUniform, float(spriteColorRed) / 255.0f,
	            float(spriteColorGreen) / 255.0f, float(spriteColorBlue) / 255.0f,
	            float(spriteColorAlpha) / 255.0f);
	glUniformMatrix3fv(Texture::modelviewUniform, 1, GL_FALSE, opengl::modelview.data);
	texture->draw();
	popMatrix();
}

void Sprite::draw(Mat3 modelview, const ShaderProgram* const shaderProgram) const {
	modelview *=
	    boost::qvm::translation_mat(boost::qvm::vec<double, 2>({ -width / 2., -height / 2. }));
	auto context = shaderProgram ? shaderProgram->use() : Texture::textureShaderProgram->use();
	if (shaderProgram) {
		glUniformMatrix3fv(shaderProgram->getUniformLocation("modelview"), 1, GL_FALSE,
		                   modelview.data);
	} else {
		glUniform4f(Texture::shaderSpriteColorUniform, float(spriteColorRed) / 255.0f,
		            float(spriteColorGreen) / 255.0f, float(spriteColorBlue) / 255.0f,
		            float(spriteColorAlpha) / 255.0f);
		glUniformMatrix3fv(Texture::modelviewUniform, 1, GL_FALSE, modelview.data);
	}
	texture->draw();
}

void Sprite::draw(const ShaderProgram* const shaderProgram) const {
	pushMatrix();
	opengl::translate(static_cast<float>(position.x), static_cast<float>(position.y));
	auto context = shaderProgram ? shaderProgram->use() : Texture::textureShaderProgram->use();
	if (shaderProgram) {
		glUniformMatrix3fv(shaderProgram->getUniformLocation("modelview"), 1, GL_FALSE,
			opengl::modelview.data);
	}
	else {
		glUniform4f(Texture::shaderSpriteColorUniform, float(spriteColorRed) / 255.0f, float(spriteColorGreen) / 255.0f,
			float(spriteColorBlue) / 255.0f, float(spriteColorAlpha) / 255.0f);
		glUniformMatrix3fv(Texture::modelviewUniform, 1, GL_FALSE, opengl::modelview.data);
	}
	texture->draw();
	popMatrix();
}

void Sprite::drawScaled(float xfactor, float yfactor,
                        const ShaderProgram* const shaderProgram) const {
	pushMatrix();
	opengl::translate(static_cast<float>(position.x), static_cast<float>(position.y));
	opengl::scale(xfactor, yfactor);
		auto context = shaderProgram ? shaderProgram->use() : Texture::textureShaderProgram->use();
	if (shaderProgram) {
		glUniformMatrix3fv(shaderProgram->getUniformLocation("modelview"), 1, GL_FALSE,
			opengl::modelview.data);
	}
	else {
		glUniform4f(Texture::shaderSpriteColorUniform, float(spriteColorRed) / 255.0f, float(spriteColorGreen) / 255.0f,
			float(spriteColorBlue) / 255.0f, float(spriteColorAlpha) / 255.0f);
		glUniformMatrix3fv(Texture::modelviewUniform, 1, GL_FALSE, opengl::modelview.data);
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
	texture->drawClipped(float(start.x), float(end.x), float(start.y), float(end.y),
	                     float(spriteColorRed) / 255.0f, float(spriteColorGreen) / 255.0f,
	                     float(spriteColorBlue) / 255.0f, float(spriteColorAlpha) / 255.0f);
	popMatrix();
}

void Sprite::drawMesh(const std::vector<Vertex>& vertexes,
                      const ShaderProgram* const shaderProgram) const {
	if (vertexes.empty()) {
		return;
	}
	pushMatrix();
	opengl::translate(static_cast<float>(position.x), static_cast<float>(position.y));
	scale(getScaleFactor());
	texture->drawMesh(vertexes, float(spriteColorRed) / 255.0f, float(spriteColorGreen) / 255.0f,
	                  float(spriteColorBlue) / 255.0f, float(spriteColorAlpha) / 255.0f,
	                  shaderProgram);
	popMatrix();
}

void Sprite::setBytes(const unsigned char* const bytes) {
	texture->setBytes(bytes, int(std::lround(width)), int(std::lround(height)));
}

const Shader& Sprite::vertexShader() {
	return Texture::vertexShader();
}

#ifndef NOPNG
Finally Sprite::LoadPNG(const std::string& filename, FILE* const fp, const bool halfLoad) {
	const unsigned int PNG_BYTES_TO_CHECK = 4;
	png_byte buf[PNG_BYTES_TO_CHECK];

	// Read in some of the signature bytes
	if (fread(buf, 1, PNG_BYTES_TO_CHECK, fp) != PNG_BYTES_TO_CHECK ||
	    png_sig_cmp(buf, png_size_t(0), PNG_BYTES_TO_CHECK) != 0) {
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
	fseek(fp, 10, SEEK_SET);
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
		row = new unsigned char[header.width * 3];
	}
	Finally cleanUp([&buf]() { cleanUpRowPointers(buf); });

	if (header.height < 0) {
		header.height = -header.height;
		for (int i = 0; i < header.height; ++i) {
			if (fseek(fp, header.dataOffset + i * header.width * 3, SEEK_SET) != 0) {
				throw std::runtime_error(std::string("Error reading file. (" + filename + ")"));
			}
			if (!fread(buf[i], header.width * 3, 1, fp)) {
				throw std::runtime_error(std::string("Error reading data. (" + filename + ")"));
			}
		}
	} else { // "bottom-up"-Bitmap
		for (int i = header.height - 1; i >= 0; --i) {
			if (fseek(fp, header.dataOffset + i * header.width * 3, SEEK_SET) != 0) {
				throw std::runtime_error(std::string("Error reading file. (" + filename + ")"));
			}
			if (!fread(buf[(header.height - 1) - i], header.width * 3, 1, fp)) {
				throw std::runtime_error(std::string("Error reading data. (" + filename + ")"));
			}
		}
	}
	loadTexture(header.width, header.height, filename, halfLoad, GL_BGR, &buf[0]);
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
	textures[filename] = texture;
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
