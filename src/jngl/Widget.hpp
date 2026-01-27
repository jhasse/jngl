// Copyright 2020-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::Container class
/// @file
#pragma once

#include "Mat3.hpp"
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

	enum class Action : uint8_t {
		NONE,
		REMOVE,
		REQUEST_FOCUS,
	};

	/// Steps all Effects
	///
	/// When Action::REMOVE is returned, the Widget wants to be removed from its container.
	[[nodiscard]] virtual Action step();

	/// Draws the widget with all effects applied
	virtual void draw(Mat3 modelview) const;

	/// Override this function to draw the widget
	virtual void drawSelf(Mat3 modelview) const = 0;

	void addEffect(std::unique_ptr<Effect>);

	template<class T, class... Args>
	void addEffect(Args&&... args) {
		return addEffect(std::make_unique<T>(std::forward<Args>(args)...));
	}

	void removeEffect(Effect*);

	/// Removes all effects
	virtual void removeEffects();

	/// Returns the center
	jngl::Vec2 getPosition() const;

protected:
	/// Center
	jngl::Vec2 position;

private:
	std::vector<std::unique_ptr<Effect>> effects;
	std::set<Effect*> needToRemove;
};

} // namespace jngl
