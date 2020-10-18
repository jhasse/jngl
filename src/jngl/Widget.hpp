#pragma once

#include "Vec2.hpp"

namespace jngl {

class Effect;

class Widget {
public:
	/// Creates a Widget centered at \a position
	explicit Widget(jngl::Vec2 position);

	virtual ~Widget();

	virtual void step() = 0;
	virtual void draw() const = 0;

protected:
	/// Center
	jngl::Vec2 position;
};

} // namespace jngl
