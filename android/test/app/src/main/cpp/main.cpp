#include <jngl.hpp>

JNGL_MAIN_BEGIN {
    try {
        jngl::showWindow("Android Test", 1920, 1080);
        float x = 0;
        while (jngl::running()) {
            jngl::updateInput();
            jngl::setBackgroundColor(0, 255, 0);
            jngl::setColor(0, 0, 255);
            jngl::drawRect(-jngl::getWindowWidth() / 2, -jngl::getWindowHeight() / 2, jngl::getWindowWidth() - jngl::getMouseX(), jngl::getMouseY());
            x += 0.4;
            jngl::swapBuffers();
        }
    } catch(std::exception& e) {
        jngl::errorMessage(e.what());
    }
} JNGL_MAIN_END
