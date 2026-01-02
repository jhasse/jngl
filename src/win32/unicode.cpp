// Copyright 2020-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "unicode.hpp"

#include <stdexcept>
#include <windows.h>

namespace jngl {

std::wstring utf8ToUtf16(std::string_view s) {
	if (s.empty()) {
		return {};
	}

	const int requiredSize = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, s.data(),
	                                             static_cast<int>(s.size()), nullptr, 0);
	if (requiredSize == 0) {
		throw std::runtime_error("utf8ToUtf16: MultiByteToWideChar size query failed");
	}

	std::wstring result(static_cast<size_t>(requiredSize), L'\0');
	const int written =
	    MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, s.data(), static_cast<int>(s.size()),
	                        result.data(), requiredSize);
	if (written == 0) {
		throw std::runtime_error("utf8ToUtf16: MultiByteToWideChar conversion failed");
	}

	return result;
}

std::string utf16ToUtf8(std::wstring_view wstr) {
	if (wstr.empty()) {
		return {};
	}

	const int requiredSize =
	    WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wstr.data(),
	                        static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr);
	if (requiredSize == 0) {
		throw std::runtime_error("utf16ToUtf8: WideCharToMultiByte size query failed");
	}

	std::string result(static_cast<size_t>(requiredSize), '\0');
	const int written = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wstr.data(),
	                                        static_cast<int>(wstr.size()), result.data(),
	                                        requiredSize, nullptr, nullptr);
	if (written == 0) {
		throw std::runtime_error("utf16ToUtf8: WideCharToMultiByte conversion failed");
	}

	return result;
}

} // namespace jngl
