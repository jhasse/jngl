// Copyright 2021 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "ImageData.hpp"

#include "../helper.hpp"
#include "../main.hpp"

#ifdef ANDROID
#include "../android/fopen.hpp"
#endif
#ifndef NOPNG
#include "../png/ImageDataPNG.hpp"
#endif
#ifndef NOWEBP
#include "../ImageDataWebP.hpp"
#endif

#if __cplusplus < 202002L
#include <boost/algorithm/string/predicate.hpp>
#endif
#include <functional>
#include <sstream>

namespace jngl {

std::unique_ptr<ImageData> ImageData::load(const std::string& filename) {
	auto fullFilename = pathPrefix + filename;
	const char* extensions[] = {
#ifndef NOPNG
		".png",
#endif
#ifndef NOWEBP
		".webp",
#endif
	};
	std::function<std::unique_ptr<ImageData>(std::string, FILE*)> functions[] = {
#ifndef NOPNG
		[](const std::string& filename, FILE* file) {
		    return std::make_unique<ImageDataPNG>(filename, file);
		},
#endif
#ifndef NOWEBP
		[](std::string filename, FILE* file) {
		    return std::make_unique<ImageDataWebP>(std::move(filename), file, 1);
		},
#endif
	};
	const size_t size = sizeof(extensions) / sizeof(extensions[0]);
	std::function<std::unique_ptr<ImageData>(std::string, FILE*)> loadFunction;
	for (size_t i = 0; i < size; ++i) {
#if __cplusplus < 202002L
		if (boost::algorithm::ends_with(fullFilename, extensions[i])) {
#else
		if (fullFilename.ends_with(extensions[i])) {
#endif
			loadFunction = functions[i];
			break;
		}
		std::string tmp = fullFilename + extensions[i];
		if (fileExists(tmp)) {
			fullFilename += extensions[i];
			loadFunction = functions[i];
			break;
		}
	}
	if (!loadFunction) {
		std::ostringstream message;
		message << "No suitable image file found for: " << fullFilename
		        << "\nSupported file extensions: ";
		for (size_t i = 0; i < size; ++i) {
			if (i != 0) {
				message << ", ";
			}
			message << extensions[i];
		}
		throw std::runtime_error(message.str());
	}
	FILE* pFile = fopen(fullFilename.c_str(), "rb");
	if (pFile == nullptr) {
		throw std::runtime_error(std::string("File not found: " + fullFilename));
	}
	Finally _([pFile]() { fclose(pFile); });
	return loadFunction(filename, pFile);
}

} // namespace jngl
