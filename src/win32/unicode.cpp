// Copyright 2020-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "unicode.hpp"

#include <codecvt>
#include <locale>

namespace jngl {

std::wstring utf8ToUtf16(std::string_view s) {
	std::wstring wstr =
	    std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(std::string(s).c_str());
#if defined(__MINGW32__) && __GNUC__ < 8
	// Workaround for https://sourceforge.net/p/mingw-w64/bugs/538/
	for (wchar_t& wch : wstr) {
		wch = int(wch) / 256;
	}
#endif
	return wstr;
}

std::string utf16ToUtf8(std::wstring_view wstr) {
	return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().to_bytes(
	    std::wstring(wstr).c_str());
}

} // namespace jngl
