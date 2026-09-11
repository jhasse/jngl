#include <jngl.hpp>

JNGL_MAIN_BEGIN {
	const auto args = jngl::getArgs();
	std::string filename;
	if (args.empty()) {
		filename = jngl::getBinaryPath() + "verysmall.ogv";
	} else {
		filename = args[0];
	}
	jngl::Video video(filename);
	jngl::setScaleFactor(2);
	jngl::showWindow(filename, video.getWidth() * jngl::getScaleFactor(),
	                 video.getHeight() * jngl::getScaleFactor());
	while (jngl::running()) {
		jngl::updateInput();
		video.draw();
		if (video.finished()) {
			jngl::setTitle(filename + " [FINISHED]");
		}
		jngl::swapBuffers();
	}
	jngl::trace("quit");
}
JNGL_MAIN_END
