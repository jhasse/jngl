#include <chrono>
#include <jngl.hpp>

JNGL_MAIN_BEGIN {
	const auto args = jngl::getArgs();
	std::string filename;
	if (args.empty()) {
		filename = jngl::getBinaryPath() + "test.ogg";
	} else {
		filename = args[0];
	}
	jngl::showWindow(filename, 200, 80);
	jngl::SoundFile soundFile(filename);
	soundFile.play();
	while (jngl::running()) {
		auto played = soundFile.progress() * soundFile.length();
		jngl::updateInput();
		std::ostringstream tmp;
		auto secondsPlayed = std::chrono::duration_cast<std::chrono::seconds>(played);
		tmp << secondsPlayed << " " << std::lround((played - secondsPlayed).count() / 10)
		    << "cs\nof " << std::chrono::duration_cast<std::chrono::seconds>(soundFile.length());
		jngl::print(tmp.str(), jngl::Vec2{ -50, -20 });
		if (!soundFile.isPlaying()) {
			jngl::setTitle(filename + " [FINISHED]");
		}
		jngl::swapBuffers();
	}
}
JNGL_MAIN_END
