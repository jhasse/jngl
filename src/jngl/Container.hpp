// Copyright 2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
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
	void removeWidget(Widget*);

private:
	std::vector<std::unique_ptr<Widget>> widgets;
	std::set<Widget*> needToRemove;
};

} // namespace jngl
