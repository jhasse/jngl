// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::FontInterface class
/// @file
#pragma once

#include <string_view>

namespace jngl {

class Mat3;

/// Common interface for fonts, used internally by jngl::Font and jngl::OutlinedFont::bake
class FontInterface {
public:
	virtual ~FontInterface() = default;
	virtual void print(const Mat3& modelview, std::string_view text) const = 0;
	virtual double getTextWidth(std::string_view text) const = 0;
	virtual double getLineHeight() const = 0;
};

} // namespace jngl
