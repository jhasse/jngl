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

class Container {
public:
	void step();
	void draw() const;
	void addWidget(std::unique_ptr<Widget>);

	/// The same as addWidget(std::shared_ptr<Widget>) but creates the Widget for you
	template <class T, class... Args>
	void addWidget(Args&&... args) {
		addWidget(std::make_unique<T>(std::forward<Args>(args)...));
	}

	void removeWidget(Widget*);

private:
	std::vector<std::unique_ptr<Widget>> widgets;
	std::set<Widget*> needToRemove;
};

} // namespace jngl
