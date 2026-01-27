// Copyright 2020-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::Container class
/// @file
#pragma once

#include <memory>
#include <set>
#include <vector>

namespace jngl {

class Widget;

/// Helper class to handle multiple instances of Widget
class Container {
public:
	/// Calls Widget::step of every widget and removes the ones which request it
	void step();

	/// Calls Widget::draw of every widget
	void draw() const;

	/// Adds a Widget to the container (safe to call during Container::step)
	///
	/// \return Pointer to the added Widget
	Widget* addWidget(std::unique_ptr<Widget>);

	/// The same as addWidget(std::unique_ptr<Widget>) but creates the Widget for you
	template <class T, class... Args> T* addWidget(Args&&... args) {
		return dynamic_cast<T*>(addWidget(std::make_unique<T>(std::forward<Args>(args)...)));
	}

	/// Removes a Widget from the Container (safe to call during Container::step)
	void removeWidget(Widget*);

private:
	std::vector<std::unique_ptr<Widget>> widgets;
	std::vector<std::unique_ptr<Widget>> needToAdd;
	std::set<Widget*> needToRemove;
	uint8_t iterating = 0;
};

} // namespace jngl
