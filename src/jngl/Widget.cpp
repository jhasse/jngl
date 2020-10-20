// Copyright 2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "Widget.hpp"

#include "effects.hpp"
#include "matrix.hpp"

#include <algorithm>

namespace jngl {

Widget::Widget(const jngl::Vec2 position) : position(position) {
}

Widget::~Widget() = default;

Widget::Action Widget::step() {
	for (const auto& effect : effects) {
		switch (effect->step()) {
			case jngl::Effect::Action::NONE:
				break;
			case jngl::Effect::Action::REMOVE_EFFECT:
				removeEffect(effect.get());
				break;
			case jngl::Effect::Action::REMOVE_WIDGET:
				return Action::REMOVE;
		}
	}
	if (!needToRemove.empty()) {
		for (const auto& effect : needToRemove) {
			const auto it = std::find_if(effects.begin(), effects.end(),
			                             [effect](const auto& p) { return p.get() == effect; });
			if (it != effects.end()) {
				effects.erase(it);
			}
		}
		needToRemove.clear();
	}
	return Action::NONE;
}

void Widget::draw() const {
	jngl::pushMatrix();
	jngl::translate(position);
	for (const auto& effect : effects) {
		effect->beginDraw();
	}
	drawSelf();
	for (const auto& effect : effects) {
		effect->endDraw();
	}
	jngl::popMatrix();
}

void Widget::addEffect(std::unique_ptr<Effect> e) {
	effects.emplace_back(std::move(e));
}

void Widget::removeEffect(Effect* e) {
	needToRemove.insert(e);
}

void Widget::removeEffects() {
	effects.clear();
}

} // namespace jngl
