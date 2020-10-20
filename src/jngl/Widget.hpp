// Copyright 2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "Vec2.hpp"

#include <memory>
#include <set>
#include <vector>

namespace jngl {

class Effect;

class Widget {
public:
	/// Creates a Widget centered at \a position
	explicit Widget(jngl::Vec2 position);

	virtual ~Widget();

	enum class Action {
		NONE,
		REMOVE,
	};

	/// Steps all Effects
	///
	/// When Action::REMOVE is returned, the Widget wants to be removed from its container.
	[[nodiscard]] virtual Action step();

	/// Draws the widget with all effects applied
	virtual void draw() const;

	/// Override this function to draw the widget
	virtual void drawSelf() const = 0;

	void addEffect(std::unique_ptr<Effect>);
	void removeEffect(Effect*);

	/// Removes all effects
	virtual void removeEffects();

protected:
	/// Center
	jngl::Vec2 position;

private:
	std::vector<std::unique_ptr<Effect>> effects;
	std::set<Effect*> needToRemove;
};

} // namespace jngl
