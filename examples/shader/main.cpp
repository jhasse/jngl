// Copyright 2019-2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include <jngl.hpp>
#include <jngl/init.hpp>

#ifdef JNGL_D3D12
#include "ExampleShaders.hpp" // generated, see CMakeLists.txt
#endif

class ShaderExample : public jngl::Scene {
public:
	ShaderExample() {
#ifdef JNGL_D3D12
		// Direct3D 12 can't compile them at runtime, see jngl_compile_shaders() in CMakeLists.txt
		blurProgram = std::make_unique<jngl::ShaderProgram>(example_shaders::blur);
		texturedProgram = std::make_unique<jngl::ShaderProgram>(example_shaders::textured);
		fadeProgram = std::make_unique<jngl::ShaderProgram>(example_shaders::fade);
#else
		blurFragment = std::make_unique<jngl::Shader>(jngl::readAsset("blur.frag"),
		                                              jngl::Shader::Type::FRAGMENT);
		blurProgram =
		    std::make_unique<jngl::ShaderProgram>(jngl::Sprite::vertexShader(), *blurFragment);
		texturedFragment = std::make_unique<jngl::Shader>(jngl::readAsset("textured.frag"),
		                                                  jngl::Shader::Type::FRAGMENT);
		texturedProgram =
		    std::make_unique<jngl::ShaderProgram>(jngl::Sprite::vertexShader(), *texturedFragment);
		fadeFragment = std::make_unique<jngl::Shader>(jngl::readAsset("fade.frag"),
		                                              jngl::Shader::Type::FRAGMENT);
		fadeProgram =
		    std::make_unique<jngl::ShaderProgram>(jngl::Sprite::vertexShader(), *fadeFragment);
#endif
	}

	void step() override {
	}

	void draw() const override {
		jngl::pushMatrix();
		jngl::translate(100, 0);
		sprite.drawClipped({ 0.43, 0.1 }, { 0.61, 0.9 });
		jngl::translate(300, 30);
		sprite.draw(blurProgram.get());
		jngl::translate(-700, -200);
		const std::vector<jngl::Vertex> mesh{
			{ .x = 0, .y = 0, .u = 0, .v = 0 },        { .x = 300, .y = 0, .u = 3, .v = 0 },
			{ .x = 300, .y = 300, .u = 3, .v = 2.4f }, { .x = 0, .y = 0, .u = 0, .v = 0 },
			{ .x = 300, .y = 300, .u = 3, .v = 2.4f }, { .x = 0, .y = 300, .u = 0, .v = 2.4f }
		};
		sprite.drawMesh(mesh, texturedProgram.get());
		// The same tiled mesh as above, but with the edges faded out.
		jngl::translate(0, 330);
		sprite.drawMesh(mesh, fadeProgram.get());
		jngl::popMatrix();
	}

private:
	jngl::Sprite sprite{ "jngl" };
	std::unique_ptr<jngl::Shader> texturedFragment;
	std::unique_ptr<jngl::ShaderProgram> texturedProgram;
	std::unique_ptr<jngl::Shader> blurFragment;
	std::unique_ptr<jngl::ShaderProgram> blurProgram;
	std::unique_ptr<jngl::Shader> fadeFragment;
	std::unique_ptr<jngl::ShaderProgram> fadeProgram;
};

jngl::AppParameters jnglInit() {
	jngl::AppParameters params;
	params.start = []() { return std::make_shared<ShaderExample>(); };
	params.displayName = "Shader Example";
	params.screenSize = { 1280, 720 };
	return params;
}
