#include "base.hpp"
#include "constants.hpp"

#include <jngl/init.hpp>

jngl::AppParameters jnglInit() {
	return {
		[]() {
		jngl::setStepsPerSecond(1. / timePerFrame);
		jngl::setPrefix("../examples/bike");
		return std::make_shared<Base>();
		},
		"Bike",
		jngl::Vec2(screenWidth, screenHeight),
	    };
}
