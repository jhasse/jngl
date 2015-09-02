#include <jngl.hpp>

JNGL_MAIN_BEGIN {
    try {
        jngl::debugLn("Start");
        jngl::showWindow("Android Test", 1920, 1080);
        float x = 0;
        while (jngl::running()) {
            jngl::updateInput();
            jngl::setBackgroundColor(0, 255, 0);
            jngl::setColor(0, 0, 255);
            jngl::drawRect(0, 0, 10 + x, 10);
            x += 0.4;
            jngl::swapBuffers();
        }
    } catch(std::exception& e) {
        jngl::errorMessage(e.what());
    }
} JNGL_MAIN_END
