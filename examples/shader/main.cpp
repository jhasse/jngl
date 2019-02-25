#include <fstream>
#include <sstream>
#include <jngl.hpp>

class ShaderExample : public jngl::Work {
public:
	ShaderExample() {
		{
			std::ifstream fin("data/blur.frag");
			std::stringstream buffer;
			buffer << fin.rdbuf();
			blurFragment =
			    std::make_unique<jngl::Shader>(buffer.str().c_str(), jngl::Shader::Type::FRAGMENT);
		}
		blurProgram =
		    std::make_unique<jngl::ShaderProgram>(jngl::Sprite::vertexShader(), *blurFragment);
		{
			std::ifstream fin("data/textured.frag");
			std::stringstream buffer;
			buffer << fin.rdbuf();
			texturedFragment =
			    std::make_unique<jngl::Shader>(buffer.str().c_str(), jngl::Shader::Type::FRAGMENT);
		}
		texturedProgram =
		    std::make_unique<jngl::ShaderProgram>(jngl::Sprite::vertexShader(), *texturedFragment);
	}

	void step() override {
	}

	void draw() const override {
		jngl::pushMatrix();
		jngl::translate(100, 0);
		sprite.drawClipped({0.43, 0.1}, {0.61, 0.9});
		jngl::translate(300, 30);
		sprite.draw(blurProgram.get());
		jngl::translate(-700, 0);
		sprite.drawMesh({ { 0, 0, 0, 0 },
		                  { 300, 0, 3, 0 },
		                  { 300, 300, 3, 2.4 },
		                  { 0, 0, 0, 0 },
		                  { 300, 300, 3, 2.4 },
		                  { 0, 300, 0, 2.4 } },
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

JNGL_MAIN_BEGIN {
	jngl::showWindow("Shader Example", 1280, 720);
	jngl::setWork(std::make_shared<ShaderExample>());
	jngl::mainLoop();
}
JNGL_MAIN_END
