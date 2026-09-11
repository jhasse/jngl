// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#pragma once

#include "job.hpp"
#include "Vec2.hpp"

namespace jngl {

class VirtualMouseCursor : public jngl::Job {
public:
	virtual Vec2 getPosition() const = 0;
	virtual bool pressed() const = 0;
	virtual bool down() const = 0;
};

} // namespace jngl
