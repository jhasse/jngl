// Copyright 2018-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/other.hpp"

#include "unicode.hpp"

#include <shlobj.h>
#include <stdexcept>
#include <windows.h>

#ifdef JNGL_UWP
#include <windows.storage.h>
#include <wrl.h>
#endif

namespace jngl {

std::string getBinaryPath() {
	wchar_t filename[MAX_PATH];
	GetModuleFileName(nullptr, filename, MAX_PATH);
	std::string tmp = utf16ToUtf8(filename);
	return tmp.substr(0, tmp.find_last_of("\\/") + 1);
}

std::string getSystemConfigPath() {
#if JNGL_UWP
	// https://stackoverflow.com/a/39825568/647898
	using namespace ABI::Windows::Storage;
	using namespace Microsoft::WRL;
	using namespace Microsoft::WRL::Wrappers;

	ComPtr<IApplicationDataStatics> appDataStatics;
	auto hr = RoGetActivationFactory(HStringReference(L"Windows.Storage.ApplicationData").Get(),
	                                 __uuidof(appDataStatics), &appDataStatics);
	if (FAILED(hr)) {
		throw std::runtime_error("Failed to retrieve application data statics");
	}
	ComPtr<IApplicationData> appData;
	hr = appDataStatics->get_Current(&appData);
	if (FAILED(hr)) {
		throw std::runtime_error("Failed to retrieve current application data");
	}
	ComPtr<IStorageFolder> roamingFolder;
	hr = appData->get_RoamingFolder(&roamingFolder);
	if (FAILED(hr)) {
		throw std::runtime_error("Failed to retrieve roaming folder");
	}
	ComPtr<IStorageItem> folderItem;
	hr = roamingFolder.As(&folderItem);
	if (FAILED(hr)) {
		throw std::runtime_error("Failed to cast roaming folder to IStorageItem");
	}
	HString roamingPathHString;
	hr = folderItem->get_Path(roamingPathHString.GetAddressOf());
	if (FAILED(hr)) {
		throw std::runtime_error("Failed to retrieve roaming folder path");
	}
	uint32_t pathLength;
	auto roamingPathCStr = roamingPathHString.GetRawBuffer(&pathLength);
	return utf16ToUtf8(std::wstring(roamingPathCStr, pathLength));
#else
	PWSTR pPath;
	if (SHGetKnownFolderPath(FOLDERID_RoamingAppData, CSIDL_FLAG_CREATE, nullptr, &pPath) != S_OK) {
		throw std::runtime_error("Couldn't get %AppData% location!");
	}
	std::string tmp(utf16ToUtf8(pPath));
	CoTaskMemFree(pPath);
	return tmp;
#endif
}

void openURL(const std::string& url) {
	ShellExecute(0, 0, utf8ToUtf16(url).c_str(), 0, 0, SW_SHOW);
}

} // namespace jngl
