// Copyright 2020-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include <string_view>
#include <string>

namespace jngl {

std::wstring utf8ToUtf16(std::string_view);

std::string utf16ToUtf8(std::wstring_view);

} // namespace jngl
