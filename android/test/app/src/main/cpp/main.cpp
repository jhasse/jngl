#include <jngl.hpp>
#include <sstream>

JNGL_MAIN_BEGIN {
	try {
		jngl::showWindow("Android Test", 1920, 1080);
		float x = 0;
		bool first = true;
		while (jngl::running()) {
			jngl::updateInput();
			jngl::setBackgroundColor(133, 133, 133);
			jngl::setFont("Arial.ttf");
			jngl::setFontSize(100);
			if (first) {
				first = false;
				jngl::play("test.ogg");
			}
			const auto mouse = jngl::getMousePos();
			jngl::setColor(200, jngl::mouseDown(jngl::mouse::Left) ? 255 : 54, 45);
			jngl::drawRect(mouse.x, mouse.y, 100, 100);
			x += 0.4;
			// jngl::setSpriteColor(255, 255, 255, 120);
			jngl::draw("jngl.webp", mouse.x, mouse.y);
			if (jngl::mouseDown(jngl::mouse::Left)) {
				jngl::print("hello world!", 0, 0);
			}

			std::stringstream sstream;
			sstream << "mouse.x: " << mouse.x << "  mouse.y: " << mouse.y;
			jngl::debugLn(sstream.str());

			jngl::setColor(255, 0, 0);
			jngl::drawCircle({-800, -400}, 30); // top left

			jngl::setColor(255, 255, 0);
			jngl::drawCircle({800, -400}, 30); // top right

			jngl::setColor(120, 255, 120);
			jngl::drawCircle({-800, 400}, 30); // bottom left

			jngl::setColor(0, 0, 255);
			jngl::drawCircle({800, 400}, 30); // bottom right

			jngl::swapBuffers();
		}
	} catch (std::exception& e) {
		jngl::errorMessage(e.what());
	}
}
JNGL_MAIN_END
