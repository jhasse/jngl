#include <jngl.hpp>

JNGL_MAIN_BEGIN {
	const auto filename = jngl::getArgs().at(0);
	jngl::Video video(filename);
	jngl::showWindow(filename, 1280, 720);
	while (jngl::running()) {
		jngl::updateInput();
		video.draw();
		jngl::swapBuffers();
	}
} JNGL_MAIN_END
