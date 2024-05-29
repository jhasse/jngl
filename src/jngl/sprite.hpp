// Copyright 2012-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::Sprite class and related functions
/// \file
#pragma once

#include "Drawable.hpp"
#include "Rgb.hpp"
#include "ShaderProgram.hpp"
#include "Vec2.hpp"

#include <future>
#include <vector>

namespace jngl {

class ImageData;
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

	/// Creates a Sprite from ImageData and scales it by \a scale
	///
	/// When you want to draw an ImageData that hasn't been scaled, make sure to pass JNGL's scale
	/// factor so that it appears as the same size independent of the resolution of the user.
	///
	/// Example:
	/// \code
	/// auto img = jngl::ImageData::load("foo.png");
	/// auto sprite1 = std::make_unique<jngl::Sprite>(*img, jngl::getScaleFactor());
	///
	/// auto sprite2 = std::make_unique<jngl::Sprite>("foo.png");
	///
	/// assert(std::lround(sprite1.getWidth()) == std::lround(sprite2.getWidth()));
	/// \endcode
	explicit Sprite(const ImageData&, double scale);

	/// \deprecated Use Loader instead
	explicit Sprite(const std::string& filename, LoadType loadType = LoadType::NORMAL);

	/// Does nothing
	void step() override;

	/// Draws the Sprite, centered by default
	void draw() const override;

	/// Use this class to load a Sprite asynchronously
	///
	/// Example:
	/// \code
	/// class MyGame : public jngl::Work {
	///     void step() override {}
	///     void draw() const override {
	///         if (mySprite) {
	///             mySprite->draw();
	///         } else {
	///             jngl::print("loading...", jngl::Vec2(0, 0));
	///         }
	///     }
	///
	///     jngl::Sprite::Loader mySprite{ "foo.webp" };
	/// };
	/// \endcode
	class Loader {
	public:
		/// Starts a thread to load \a filename and returns instantly
		///
		/// Note that if the file couldn't be found this will not throw. Instead the exception will
		/// be thrown on first use by shared() or operator->().
		explicit Loader(std::string filename) noexcept;

		/// Blocks until the Sprite has been loaded
		///
		/// While you won't have access to std::shared_ptr returned by shared(), the Sprite will
		/// still be loaded into JNGL's cache and the next time you create a Sprite from this
		/// filename, you'll get the same std::shared_ptr.
		///
		/// If shared() or operator->() haven't been called yet and the file wasn't found the
		/// destructor won't throw but use errorMessage(const std::string&).
		~Loader() noexcept;

		/// Blocks until the Sprite has been loaded and returns a non-nullptr std::shared_ptr
		///
		/// \throws std::runtime_error on decoding errors or file not found
		std::shared_ptr<Sprite> shared() const;

		/// Returns whether the sprite has been loaded, does NOT block
		///
		/// After this returned true all other methods won't block any more.
		operator bool() const; // NOLINT

		/// Blocks until the Sprite has been loaded
		///
		/// \throws std::runtime_error on decoding errors or file not found
		Sprite* operator->() const;

	private:
		mutable std::future<std::unique_ptr<ImageData>> imageDataFuture;
		std::string filename;
	};

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

	/// Draws a list of triangles with the sprite's texture on it using the global modelview from
	/// jngl::modelview()
	void drawMesh(const std::vector<Vertex>& vertexes, const ShaderProgram* = nullptr) const;

	/// Draws a list of triangles with the sprite's texture on it, ignores the Sprite's position
	void drawMesh(Mat3 modelview, const std::vector<Vertex>& vertexes,
	              const ShaderProgram* = nullptr) const;

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

/// Sets the global color used for drawing Sprites, leaves the alpha value untouched
void setSpriteColor(Rgb);

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
