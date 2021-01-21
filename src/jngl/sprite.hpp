// Copyright 2012-2021 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::Sprite class and related functions
/// \file
#pragma once

#include "Drawable.hpp"
#include "ShaderProgram.hpp"
#include "Vec2.hpp"

#include <vector>

namespace jngl {

class Mat3;
class Texture;
struct Vertex;

/// Higher-level representation of an image
class Sprite : public Drawable {
public:
	enum class LoadType {
		NORMAL,
		HALF,
		THREADED,
	};

	Sprite(const unsigned char* bytes, size_t width, size_t height);
	explicit Sprite(const std::string& filename, LoadType loadType = LoadType::NORMAL);
	void step() override;
	void draw() const override;

	/// Draws the image centered using \a modelview
	///
	/// \param shaderProgram Passing `nullptr` uses the default.
	void draw(Mat3 modelview, const ShaderProgram* = nullptr) const;

	void draw(const ShaderProgram* shaderProgram) const;

	/// Draws the image scaled by `xfactor` and `yfactor`
	///
	/// \param xfactor Scale width by this factor
	/// \param yfactor Scale height by this factor
	/// \param shaderProgram Passing `nullptr` uses the default.
	///
	/// `drawScaled(1, 1)` would draw it normally. You can pass negative values to flip the image.
	/// For example `drawScaled(-1, 1)` would draw the image horizontally flipped.
	///
	/// \deprecated Scale the modelview matrix instead using jngl::Mat3::scale
	[[deprecated("Scale the modelview matrix instead using jngl::Mat3::scale")]]
	void drawScaled(float xfactor, float yfactor,
	                const ShaderProgram* shaderProgram = nullptr) const;

	/// \deprecated Use new drawClipped(Vec2, Vec2) method instead
	[[deprecated("Use new drawClipped(Vec2, Vec2) method instead")]] void
	drawClipped(float xstart, float xend, float ystart, float yend) const;

	/// Draw a cutout of the sprite. drawClipped({0, 0}, {1, 1}) would draw it normally.
	void drawClipped(Vec2 start, Vec2 end) const;

	/// Draws a list of triangles with the sprite's texture on it
	void drawMesh(const std::vector<Vertex>& vertexes, const ShaderProgram* = nullptr) const;

	void setBytes(const unsigned char*);

	/// Returns a reference to JNGL's default vertex shader used to draw textures
	static const Shader& vertexShader();

	/// Function which actually loads the sprite
	std::shared_ptr<Finally> loader;

private:
	static void cleanUpRowPointers(std::vector<unsigned char*>& buf);
	void loadTexture(int scaledWidth, int scaledHeight, const std::string& filename, bool halfLoad,
	                 unsigned int format, const unsigned char* const* rowPointers,
	                 const unsigned char* data = nullptr);
	Finally LoadPNG(const std::string& filename, FILE* fp, bool halfLoad);
	struct BMPHeader {
		unsigned int dataOffset;
		unsigned int headerSize;
		int width;
		int height;
		unsigned short planes;
		unsigned short bpp;
		unsigned int compression;
		unsigned int dataSize;
	};
	Finally LoadBMP(const std::string& filename, FILE* fp, bool halfLoad);
#ifndef NOJPEG
	Finally LoadJPG(const std::string& filename, FILE* file, bool halfLoad);
#endif
#ifndef NOWEBP
	Finally LoadWebP(const std::string& filename, FILE* file, bool halfLoad);
#endif

	std::shared_ptr<Texture> texture;
};

void draw(const std::string& filename, double x, double y);

template <class Vect> void draw(const std::string& filename, Vect pos) {
	draw(filename, pos.x, pos.y);
}

/// Starts a thread to load \a filename and returns a Finally which will join it
///
/// \param filename Name of an image file (extension is optional) or a .ogg sound file.
Finally load(const std::string& filename);

void unload(const std::string& filename);

void unloadAll();

void drawClipped(const std::string& filename, double xposition, double yposition, float xstart,
                 float xend, float ystart, float yend);

void setSpriteColor(unsigned char red, unsigned char green, unsigned char blue,
                    unsigned char alpha);

void setSpriteColor(unsigned char red, unsigned char green, unsigned char blue);

void setSpriteAlpha(unsigned char alpha);

void pushSpriteAlpha(unsigned char alpha = 255);

void popSpriteAlpha();

int getWidth(const std::string& filename);

int getHeight(const std::string& filename);

#if __cplusplus >= 201703L
[[nodiscard]]
#endif
Finally
disableBlending();

} // namespace jngl
