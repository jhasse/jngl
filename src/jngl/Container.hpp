// Copyright 2020-2021 Jan Niklas Hasse <jhasse@bixense.com>
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
	/// Calls Widget::step of every widget and removes the once which request it
	void step();

	/// Calls Widget::draw of every widget
	void draw() const;

	/// Immediately adds a Widget to the container
	void addWidget(std::unique_ptr<Widget>);

	/// The same as addWidget(std::unique_ptr<Widget>) but creates the Widget for you
	template <class T, class... Args>
	void addWidget(Args&&... args) {
		addWidget(std::make_unique<T>(std::forward<Args>(args)...));
	}

	/// Marks a Widget to be removed by Container::step
	void removeWidget(Widget*);

private:
	std::vector<std::unique_ptr<Widget>> widgets;
	std::set<Widget*> needToRemove;
};

} // namespace jngl
