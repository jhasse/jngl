// Copyright 2020-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include <jngl.hpp>
#include <sstream>

JNGL_MAIN_BEGIN {
	try {
		jngl::showWindow("Android Test", 1920, 1080);

		jngl::Video video("verysmall.ogv");
		while (jngl::running() && !video.finished()) {
			jngl::updateInput();
			video.draw();
			jngl::swapBuffers();
		}

		bool first = true;
		std::string text;
		jngl::Sprite sprite("././/././jngl.webp");
		while (jngl::running()) {
			jngl::updateInput();
			jngl::setBackgroundColor(jngl::Rgb::u8(133, 133, 133));
			jngl::setFont("Arial.ttf");
			jngl::setFontSize(100);
			if (first) {
				first = false;
				jngl::play("test.ogg");
			}
			sprite.drawClipped({0.3, 0.3}, {0.7, 0.7});
			jngl::setColor(200, jngl::mouseDown(jngl::mouse::Left) ? 255 : 54, 45);
			for (const auto mouse : jngl::getTouchPositions()) {
				jngl::drawRect({ mouse.x, mouse.y }, { 300, 300 });
			}
			// jngl::setSpriteColor(255, 255, 255, 120);
			const auto mouse = jngl::getMousePos();
			jngl::draw("jngl.webp", mouse.x, mouse.y);
			if (jngl::mouseDown(jngl::mouse::Left)) {
				jngl::setFontColor(0x000000_rgb);
				jngl::print(jngl::getPreferredLanguage() +
				                ", touches: " + std::to_string(jngl::getTouchPositions().size()),
				            -100, 0);
			}
			if (jngl::mousePressed(jngl::mouse::Left) and jngl::isMultitouch()) {
				jngl::setKeyboardVisible(true);
			}
			text += jngl::getTextInput();
			jngl::print(text, -100, -200);

			jngl::setColor(255, 0, 0);
			jngl::drawCircle(jngl::Vec2(-800, -400), 30); // top left

			jngl::setColor(255, 255, 0);
			jngl::drawCircle(jngl::Vec2(800, -400), 30); // top right

			jngl::setColor(120, 255, 120);
			jngl::drawCircle(jngl::Vec2(-800, 400), 30); // bottom left

			jngl::setColor(0, 0, 255);
			jngl::drawCircle(jngl::Vec2(800, 400), 30); // bottom right

			jngl::pushMatrix();
			jngl::translate(-300, -200);
			jngl::scale(2);
			jngl::setFontSize(10);
			int controllerNr = 1;
			for (const auto& controller : jngl::getConnectedControllers()) {
				std::stringstream sstream;
				sstream << "Controller " << controllerNr << " connected." << std::endl
						<< "Trigger: " << controller->state(jngl::controller::LeftTrigger) << " " << controller->state(jngl::controller::RightTrigger)
						<< "\nA: " << controller->down(jngl::controller::A)
						<< " B: " << controller->down(jngl::controller::B)
						<< " X: " << controller->down(jngl::controller::X)
						<< " Y: " << controller->down(jngl::controller::Y)
						<< "\nLB: " << controller->down(jngl::controller::LeftButton)
						<< " RB: " << controller->down(jngl::controller::RightButton)
						<< " LT: " << controller->down(jngl::controller::LeftTrigger)
						<< " RT: " << controller->down(jngl::controller::RightTrigger)
						<< "\nLS: " << controller->down(jngl::controller::LeftStick)
						<< " RS: " << controller->down(jngl::controller::RightStick)
						<< " Start: " << controller->down(jngl::controller::Start)
						<< " Back: " << controller->down(jngl::controller::Back)
						<< "\n↑: " << controller->down(jngl::controller::DpadUp)
						<< " ↓: " << controller->down(jngl::controller::DpadDown)
						<< " ←: " << controller->down(jngl::controller::DpadLeft)
						<< " →: " << controller->down(jngl::controller::DpadRight);
				static uint8_t pressedFade = 150;
				if (controller->pressed(jngl::controller::A)) {
					assert(controller->pressed(jngl::controller::A));
					using namespace std::chrono_literals;
					controller->rumble(0.5f, 200ms);
					pressedFade = 0;
				} else {
					if (pressedFade < 150) {
						pressedFade += 10;
					}
				}
				jngl::setFontColor(pressedFade, pressedFade, pressedFade);

				jngl::pushMatrix();
				for (const jngl::Vec2 stick :
						{ jngl::Vec2(controller->state(jngl::controller::LeftStickX),
									 -controller->state(jngl::controller::LeftStickY)),
						  jngl::Vec2(controller->state(jngl::controller::RightStickX),
									 -controller->state(jngl::controller::RightStickY)) }) {
					const float circleRadius = 20;
					const auto circlePos = jngl::Vec2(530, double(-40 + controllerNr * 110));
					jngl::setColor(100, 100, 100, 255);
					jngl::drawEllipse(circlePos, circleRadius, circleRadius);
					jngl::setColor(255, 255, 255, 255);
					jngl::drawCircle(circlePos + circleRadius * stick, 4);
					jngl::translate(0, 2 * circleRadius + 10);
				}
				jngl::popMatrix();

				jngl::setColor(255, 255, 255, 150);
				jngl::drawRect({500, 40. + double(controllerNr - 1) * 110.}, {300, 120});
				jngl::print(sstream.str(), 558, 50 + (controllerNr - 1) * 110);
				++controllerNr;
			}
			jngl::popMatrix();

			jngl::swapBuffers();
		}
	} catch (std::exception& e) {
		jngl::errorMessage(e.what());
	}
}
JNGL_MAIN_END
