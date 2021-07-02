// Copyright 2021 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::ImageData class
/// \file
#pragma once

#include <cstddef>
#include <string>

namespace jngl {

class ImageData {
public:
	/// Passing a filename will load the specified \a filename
	///
	/// Currently only WebP files are supported (WIP).
	static std::unique_ptr<ImageData> load(const std::string& filename);

	virtual ~ImageData() = default;

	virtual int getWidth() const = 0;
	virtual int getHeight() const = 0;

	/// RGBA values ordered row-major
	virtual const uint8_t* pixels() const = 0;
};

} // namespace jngl
