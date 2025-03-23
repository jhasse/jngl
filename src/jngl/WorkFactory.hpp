// Copyright 2024-2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// @file
#pragma once

#include "work.hpp"

namespace jngl {

class WorkFactory : public jngl::Work {
public:
	explicit WorkFactory(std::function<std::shared_ptr<Work>()>);

private:
	void step() override;
	void draw() const override;
	void onLoad() override;

	std::shared_ptr<Work> work;
	std::function<std::shared_ptr<Work>()> factory;
};

} // namespace jngl
