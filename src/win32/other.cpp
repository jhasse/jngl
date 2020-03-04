// Copyright 2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/other.hpp"

#include "unicode.hpp"

#include <shlobj.h>
#include <stdexcept>
#include <windows.h>

namespace jngl {

std::string getBinaryPath() {
	wchar_t filename[MAX_PATH];
	GetModuleFileName(nullptr, filename, MAX_PATH);
	std::string tmp = utf16ToUtf8(filename);
	return tmp.substr(0, tmp.find_last_of("\\/") + 1);
}

std::string getSystemConfigPath() {
	PWSTR pPath;
	if (SHGetKnownFolderPath(FOLDERID_RoamingAppData, CSIDL_FLAG_CREATE, nullptr, &pPath) != S_OK) {
		throw std::runtime_error("Couldn't get %AppData% location!");
	}
	std::string tmp(utf16ToUtf8(pPath));
	CoTaskMemFree(pPath);
	return tmp;
}

} // namespace jngl
