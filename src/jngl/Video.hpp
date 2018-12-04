// Copyright 2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "sprite.hpp"

#include <memory>
#include <string>

namespace jngl {

class Video {
public:
	Video(const std::string& filename);
	~Video();

	void draw() const;
	int getWidth() const;
	int getHeight() const;

private:
	class Impl;

	std::unique_ptr<Impl> impl;
};

} // namespace jngl
