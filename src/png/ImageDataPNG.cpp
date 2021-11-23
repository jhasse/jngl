// Copyright 2021 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "ImageDataPNG.hpp"

#include "../jngl/Finally.hpp"

#include <cassert>
#include <cstring>
#include <png.h>
#include <stdexcept>

namespace jngl {

ImageDataPNG::ImageDataPNG(const std::string& filename, FILE* fp) {
	if (!fp) {
		throw std::runtime_error(std::string("File not found: ") + filename);
	}
	Finally _([&fp]() { fclose(fp); });
	png_byte buf[PNG_BYTES_TO_CHECK];
	assert(PNG_BYTES_TO_CHECK >= sizeof(unsigned short));

	// Read in some of the signature bytes
	if (fread(buf, 1, PNG_BYTES_TO_CHECK, fp) != PNG_BYTES_TO_CHECK) {
		throw std::runtime_error(std::string("Error reading signature bytes."));
	}

	assert(png_sig_cmp(buf, (png_size_t)0, PNG_BYTES_TO_CHECK) == 0);
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	if (!png_ptr) {
		throw std::runtime_error("libpng error while reading");
	}

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		throw std::runtime_error("libpng error while reading");
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		// Free all of the memory associated with the png_ptr and info_ptr
		png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
		throw std::runtime_error("Error reading file.");
	}
	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, PNG_BYTES_TO_CHECK);
	int colorType = png_get_color_type(png_ptr, info_ptr);
	if (colorType == PNG_COLOR_TYPE_GRAY || colorType == PNG_COLOR_TYPE_GRAY_ALPHA) {
		png_set_gray_to_rgb(png_ptr);
	}
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_STRIP_16, nullptr);
	Finally destroyRead(
	    [&png_ptr, &info_ptr]() { png_destroy_read_struct(&png_ptr, &info_ptr, nullptr); });

	x = png_get_image_width(png_ptr, info_ptr);
	y = png_get_image_height(png_ptr, info_ptr);
	const int channels = png_get_channels(png_ptr, info_ptr);
	if (channels != 4) {
		throw std::runtime_error(filename + " has " + std::to_string(channels) +
		                         " channels. Only PNGs with 4 channels (RGBA) are supported.");
	}

	const auto row_pointers = png_get_rows(png_ptr, info_ptr);
	imageData.resize(x * y * channels);
	for (size_t i = 0; i < y; ++i) {
		memcpy(&imageData[i * x * channels], row_pointers[i], x * channels);
	}
}

ImageDataPNG::~ImageDataPNG() = default;

const uint8_t* ImageDataPNG::pixels() const {
	return imageData.data();
}

int ImageDataPNG::getWidth() const {
	return static_cast<int>(x);
}

int ImageDataPNG::getHeight() const {
	return static_cast<int>(y);
}

} // namespace jngl
