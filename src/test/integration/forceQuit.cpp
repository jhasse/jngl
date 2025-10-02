// Copyright 2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

/// Test for jngl::forceQuit

#include <jngl.hpp>
#include <jngl/init.hpp>

struct MyScene : public jngl::Scene {
	void draw() const override {}
	void step() override {
		jngl::forceQuit(std::stoi(jngl::getArgs().at(0)));
	}
	void onQuitEvent() override {
		jngl::error("onQuitEvent should not be called when using jngl::forceQuit");
		std::abort();
	}
};

jngl::AppParameters jnglInit() {
	jngl::AppParameters params;
	params.start = []() { return std::make_shared<MyScene>(); };
	return params;
}
