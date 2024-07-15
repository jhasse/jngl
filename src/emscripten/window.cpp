// Copyright 2021-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../window.hpp"

#include "../jngl/message.hpp"

namespace jngl {

std::string Window::GetFontFileByName(const std::string&) {
	return "Arial.ttf"; // Not implemented on Web. Always use Arial.ttf as that's what jngl-test
	                    // uses.
}

std::string getBinaryPath() {
	return "";
}

std::function<void()> g_jnglMainLoop;

void jnglMainLoop() {
	try {
		g_jnglMainLoop();
	} catch (std::exception& e) {
		errorMessage(e.what());
		throw;
	}
}

} // namespace jngl
