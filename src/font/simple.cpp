// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Stub implementation of GetFontFileByName for platforms without fontconfig (Android and Linux if
/// configured with JNGL_FONTCONFIG=OFF)
#include "../window.hpp"

namespace jngl {

std::string Window::GetFontFileByName(const std::string& fontname) {
	std::string tmp = fontname;
	if (fontname == "sans-serif") {
		tmp = "Arial";
	}
	return tmp + ".ttf";
}

} // namespace jngl
