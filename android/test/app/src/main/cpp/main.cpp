#include <jngl.hpp>

JNGL_MAIN_BEGIN {
	try {
		jngl::showWindow("Android Test", 2560, 1440);
		float x = 0;
		while (jngl::running()) {
			jngl::updateInput();
			jngl::setBackgroundColor(0, 255, 0);
			jngl::setColor(0, 0, 255);
			const auto mouse = jngl::getMousePos();
			jngl::drawRect(-jngl::getScreenWidth() / 2, -jngl::getScreenHeight() / 2,
			               jngl::getScreenWidth() / 2 - mouse.x,
			               jngl::getScreenHeight() / 2 + mouse.y);
			x += 0.4;
			jngl::swapBuffers();
		}
	} catch (std::exception& e) {
		jngl::errorMessage(e.what());
	}
}
JNGL_MAIN_END
