// Copyright 2020-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "Container.hpp"

#include "Finally.hpp"
#include "Widget.hpp"

#include <algorithm>

namespace jngl {

void Container::step() {
	for (auto& widget : needToAdd) {
		widgets.emplace_back(std::move(widget));
	}
	needToAdd.clear();
	++iterating;
	{
		Finally _([this]() { --iterating; });
		for (const auto& widget : widgets) {
			switch (widget->step()) {
			case Widget::Action::NONE:
				break;
			case Widget::Action::REMOVE:
				needToRemove.insert(widget.get());
				break;
			}
		}
	}
	for (const auto widget : needToRemove) {
		widgets.erase(std::remove_if(
		                  widgets.begin(), widgets.end(),
		                  [widget](const std::unique_ptr<Widget>& p) { return p.get() == widget; }),
		              widgets.end());
	}
	needToRemove.clear();
}

void Container::draw() const {
	for (const auto& widget : widgets) {
		widget->draw();
	}
}

Widget* Container::addWidget(std::unique_ptr<Widget> w) {
	if (iterating == 0) {
		widgets.emplace_back(std::move(w));
		return widgets.back().get();
	}
	needToAdd.emplace_back(std::move(w));
	return needToAdd.back().get();
}

void Container::removeWidget(Widget* const widget) {
	if (iterating == 0) {
		widgets.erase(std::remove_if(
		                  widgets.begin(), widgets.end(),
		                  [widget](const std::unique_ptr<Widget>& p) { return p.get() == widget; }),
		              widgets.end());
	}
	needToRemove.insert(widget);
}

} // namespace jngl
