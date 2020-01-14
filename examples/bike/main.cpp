#include "base.hpp"
#include "constants.hpp"

#include <boost/math/special_functions/round.hpp>
#include <cmath>
#include <jngl.hpp>
#include <sstream>
#include <thread>

JNGL_MAIN_BEGIN {
	jngl::setScaleFactor(
	    std::floor(std::min(jngl::getDesktopWidth() / 800, jngl::getDesktopHeight() / 600)));
	jngl::showWindow("Bike", boost::math::iround(screenWidth * jngl::getScaleFactor()),
	                 boost::math::iround(screenHeight * jngl::getScaleFactor()));
	Base base;
	double oldTime = jngl::getTime();
	bool needDraw = true;
	while (jngl::running()) {
		if (jngl::getTime() - oldTime > timePerFrame) {
			// This stuff needs to be done 100 times per second
			oldTime += timePerFrame;
			needDraw = true;
			base.DoFrame();
		} else {
			if (needDraw) {
				needDraw = false;
				// This needs to be done when "needDraw" is true
				jngl::updateInput();
				jngl::translate(-jngl::getScreenWidth() / 2.0, -jngl::getScreenHeight() / 2.0);
				base.Draw();
				std::stringstream sstream;
				sstream << "FPS: " << static_cast<int>(jngl::getFPS());
				jngl::print(sstream.str(), 10, 10);
				jngl::swapBuffers();
			} else {
				// Nothing to do? Okay let's Sleep.
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
		}
	}
} JNGL_MAIN_END
