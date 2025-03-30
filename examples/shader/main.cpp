#include <jngl.hpp>
#include <jngl/init.hpp>

class ShaderExample : public jngl::Work {
public:
	ShaderExample() {
		blurFragment = std::make_unique<jngl::Shader>(jngl::readAsset("blur.frag"),
		                                              jngl::Shader::Type::FRAGMENT);
		blurProgram =
		    std::make_unique<jngl::ShaderProgram>(jngl::Sprite::vertexShader(), *blurFragment);
		texturedFragment = std::make_unique<jngl::Shader>(jngl::readAsset("textured.frag"),
		                                                  jngl::Shader::Type::FRAGMENT);
		texturedProgram =
		    std::make_unique<jngl::ShaderProgram>(jngl::Sprite::vertexShader(), *texturedFragment);
	}

	void step() override {
	}

	void draw() const override {
		jngl::pushMatrix();
		jngl::translate(100, 0);
		sprite.drawClipped({ 0.43, 0.1 }, { 0.61, 0.9 });
		jngl::translate(300, 30);
		sprite.draw(blurProgram.get());
		jngl::translate(-700, 0);
		sprite.drawMesh({ { 0, 0, 0, 0 },
		                  { 300, 0, 3, 0 },
		                  { 300, 300, 3, 2.4f },
		                  { 0, 0, 0, 0 },
		                  { 300, 300, 3, 2.4f },
		                  { 0, 300, 0, 2.4f } },
		                texturedProgram.get());
		jngl::popMatrix();
	}

private:
	jngl::Sprite sprite{ "jngl" };
	std::unique_ptr<jngl::Shader> texturedFragment;
	std::unique_ptr<jngl::ShaderProgram> texturedProgram;
	std::unique_ptr<jngl::Shader> blurFragment;
	std::unique_ptr<jngl::ShaderProgram> blurProgram;
};

jngl::AppParameters jnglInit() {
	jngl::AppParameters params;
	params.start = []() { return std::make_shared<ShaderExample>(); };
	params.displayName = "Shader Example";
	params.screenSize = { 1280, 720 };
	return params;
}
