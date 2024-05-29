#include "base.hpp"
#include "constants.hpp"

#include <jngl/init.hpp>

jngl::AppParameters jnglInit() {
	jngl::AppParameters params;
	params.start = []() {
		jngl::setStepsPerSecond(std::lround(1. / timePerFrame));
		jngl::setPrefix("../examples/bike");
		return std::make_shared<Base>();
	};
	params.displayName = "Bike";
	params.screenSize = { screenWidth, screenHeight };
	return params;
}
