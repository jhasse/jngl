// Copyright 2021-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::ImageData class
/// \file
#pragma once

#include <cstddef>
#include <memory>
#include <optional>
#include <string>

namespace jngl {

/// Containing the pixel data of an image file
class ImageData {
public:
	/// Passing a filename will load the specified \a filename
	///
	/// PNG and WebP files are supported.
	/// If \a scale isn't set, jngl::getScaleFactor() gets used instead.
	static std::unique_ptr<ImageData> load(const std::string& filename,
	                                       std::optional<double> scale = std::nullopt);

	virtual ~ImageData() = default;

	/// Returns the width of the image in pixels
	virtual int getWidth() const = 0;

	/// Returns the height of the image in pixels
	virtual int getHeight() const = 0;

	/// If the image has been scaled by getScaleFactor() this will return the original image width
	virtual int getImageWidth() const = 0;
	/// If the image has been scaled by getScaleFactor() this will return the original image height
	virtual int getImageHeight() const = 0;

	/// RGBA values ordered row-major
	virtual const uint8_t* pixels() const = 0;
};

} // namespace jngl
