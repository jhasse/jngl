// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "ScrollArea.hpp"

#include "Mat3.hpp"
#include "MouseInfo.hpp"
#include "input.hpp"

namespace jngl {

ScrollArea::ScrollArea(const float height) : height(height) {
}

void ScrollArea::stepAndTransformInput() {
	const auto mouse = jngl::getMousePos();
	auto clampToBounds = [this](float& y) {
		if (y > height) {
			y -= (y - height) * 0.5f;
		}
		if (y < 0) {
			y -= y * 0.5f;
		}
	};
	if (jngl::mousePressed()) {
		targetScrollY = scrollY; // when touching the screen lock current position
		active = true;
		notSureIfHorizontalOrVeriticalScroll = true;
		startMouseX = mouse.x;
		startMouseY = mouse.y;
		previousMouseY = mouse.y;
	}
	if (active) {
		if (notSureIfHorizontalOrVeriticalScroll) {
			const double diffX = std::abs(mouse.x - startMouseX);
			const double diffY = std::abs(mouse.y - startMouseY);
			if (diffX > diffY) {
				if (diffX > 20) {
					active = false;
				}
			} else {
				if (diffY > 20) {
					notSureIfHorizontalOrVeriticalScroll = false;
				}
			}
		}
		if (!jngl::mouseDown()) {
			active = false;
			// as we might have clamped scrollY, set targetScrollY to scrollY to avoid flickering
			// when releasing mouse when scrolling out of bounds:
			targetScrollY = scrollY;
		}
	}
	if (active) {
		targetScrollY += previousMouseY - mouse.y;
		speed = speed * 0.75f + 0.25f * (previousMouseY - mouse.y);
		previousMouseY = mouse.y;

		scrollY = targetScrollY;
		clampToBounds(scrollY);
	} else {
		targetScrollY += speed;
		scrollY += speed;
		speed *= 0.95f;

		// Handle mouse wheel:
		targetScrollY -= jngl::getMouseWheel() * 50;
		clampToBounds(targetScrollY);
		scrollY += (targetScrollY - scrollY) * 0.2f;
	}

	jngl::Mat3 transform;
	transform.translate({ 0, -scrollY });
	jngl::input().transform(transform);
}

void ScrollArea::updateModelview(Mat3& modelview) const {
	modelview.translate({ 0, -scrollY });
}

} // namespace jngl
