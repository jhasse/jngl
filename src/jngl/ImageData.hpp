// Copyright 2021-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::ImageData class
/// \file
#pragma once

#include <memory>
#include <string>

namespace jngl {

/// Containing the pixel data of an image file
class ImageData {
public:
	/// Passing a filename will load the specified \a filename
	///
	/// PNG and WebP files are supported.
	static std::unique_ptr<ImageData> load(const std::string& filename);

	virtual ~ImageData() = default;

	/// Returns the width of the image in pixels
	virtual int getWidth() const = 0;

	/// Returns the height of the image in pixels
	virtual int getHeight() const = 0;

	/// RGBA values ordered row-major
	///
	/// To access the x=5, y=7 pixel's green value:
	/// `img.pixels()[5 * 4 + 7 * img.getWidth() * 4 + 1]`
	///
	/// This function doesn't create the underlying data or loads the image (that has already
	/// happened in ImageData::load), so you may call this as often as you like.
	///
	/// Example:
	/// \code
	/// size_t x = 5;
	/// size_t y = 7;
	/// uint8_t r = img.pixels()[x * 4 + y * img.getWidth() * 4];
	/// uint8_t g = img.pixels()[x * 4 + y * img.getWidth() * 4 + 1];
	/// uint8_t b = img.pixels()[x * 4 + y * img.getWidth() * 4 + 2];
	/// uint8_t a = img.pixels()[x * 4 + y * img.getWidth() * 4 + 3];
	/// \endcode
	virtual const uint8_t* pixels() const = 0;
};

} // namespace jngl
