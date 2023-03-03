// Copyright 2021 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#pragma once

#include "../jngl/ImageData.hpp"

#include <cstdint>
#include <vector>

namespace jngl {

class ImageDataPNG : public ImageData {
public:
    ImageDataPNG(const std::string& filename, FILE*);
    ~ImageDataPNG() override;
    ImageDataPNG(const ImageDataPNG&) = delete;
    ImageDataPNG& operator=(const ImageDataPNG&) = delete;
    ImageDataPNG(ImageDataPNG&&) noexcept;
    ImageDataPNG& operator=(ImageDataPNG&&) noexcept;

    const uint8_t* pixels() const override;
    int getWidth() const override;
    int getHeight() const override;
    int getImageWidth() const override;
    int getImageHeight() const override;

private:
	const static unsigned int PNG_BYTES_TO_CHECK = 4;
    std::vector<uint8_t> imageData;
    size_t x, y;
};

} // namespace jngl
