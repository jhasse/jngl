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
	virtual ~ImageData() = default;

	virtual int getWidth() const = 0;
	virtual int getHeight() const = 0;

	virtual const uint8_t* pixels() const = 0;
};

} // namespace jngl
