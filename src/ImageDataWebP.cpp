// Copyright 2021 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#ifndef NOWEBP
#include "ImageDataWebP.hpp"

#include <boost/math/special_functions/round.hpp>
#include <future>
#include <thread>
#include <vector>

namespace jngl {

ImageDataWebP::ImageDataWebP(std::string filename, FILE* file, double scaleFactor)
: filename(std::move(filename)) {
	fseek(file, 0, SEEK_END);
	auto filesize = ftell(file);
	fseek(file, 0, SEEK_SET);

	std::vector<uint8_t> buf(filesize);
	if (!fread(&buf[0], filesize, 1, file)) {
		throw std::runtime_error(std::string("Couldn't open WebP file. (" + this->filename + ")"));
	}

	if (!WebPGetInfo(&buf[0], filesize, &imgWidth, &imgHeight)) {
		throw std::runtime_error(std::string("Invalid WebP file. (" + this->filename + ")"));
	}

	WebPInitDecoderConfig(&config);
	config.options.use_threads = 1;
	scaledWidth = imgWidth;
	scaledHeight = imgHeight;
	if (scaleFactor + 1e-9 < 1) {
		config.options.use_scaling = 1;
		config.options.scaled_width = scaledWidth =
		    std::max(1, boost::math::iround(imgWidth * scaleFactor));
		config.options.scaled_height = scaledHeight =
		    std::max(1, boost::math::iround(imgHeight * scaleFactor));
	}
	config.output.colorspace = MODE_RGBA;
#ifndef __EMSCRIPTEN__
	thread = std::make_unique<std::thread>([this, buf{ std::move(buf) }, filesize]() mutable {
#endif
		result = WebPDecode(&buf[0], filesize, &config);
#ifndef __EMSCRIPTEN__
	});
#endif
}

ImageDataWebP::~ImageDataWebP() {
#ifndef __EMSCRIPTEN__
	if (thread && thread->joinable()) {
		thread->join();
	}
#endif
	WebPFreeDecBuffer(&config.output);
}

const uint8_t* ImageDataWebP::pixels() const {
#ifndef __EMSCRIPTEN__
	if (thread->joinable()) {
		thread->join();
	}
#endif
	if (result != VP8_STATUS_OK) {
		throw std::runtime_error(std::string("Can't decode WebP file. (" + filename + ")"));
	}
	return config.output.u.RGBA.rgba; // NOLINT
}

int ImageDataWebP::getImageWidth() const {
	return imgWidth;
}

int ImageDataWebP::getImageHeight() const {
	return imgHeight;
}

int ImageDataWebP::getWidth() const {
	return scaledWidth;
}

int ImageDataWebP::getHeight() const {
	return scaledHeight;
}

} // namespace jngl
#endif
