#include <cmath>
#include <jngl.hpp>
#include <jngl/init.hpp>

const int screenWidth = 800;
const int screenHeight = 600;

class Ball : public jngl::Work {
public:
	explicit Ball(const std::string& filename);
	void draw() const override;
	void step() override;
	void CheckMouse(int x, int y);

private:
	double x_{}, y_{}, xSpeed_{}, ySpeed_{};
	const std::string filename_;
	const int width_, height_;
};

const double timePerFrame = 0.01; // 100 FPS

jngl::AppParameters jnglInit() {
	jngl::AppParameters params;
	params.start = []() {
		jngl::setStepsPerSecond(std::lround(1. / timePerFrame));
		jngl::setPrefix("../examples/throw");
		return std::make_shared<Ball>("ball");
	};
	params.displayName = "Throw Example";
	params.screenSize = { screenWidth, screenHeight };
	return params;
}

Ball::Ball(const std::string& filename)
: filename_(filename), width_(jngl::getWidth(filename)), height_(jngl::getHeight(filename)) {
}

void Ball::draw() const {
	jngl::translate(-screenWidth / 2., -screenHeight / 2.);
	jngl::draw("ball", static_cast<int>(x_), static_cast<int>(y_));
}

void Ball::step() {
	CheckMouse(static_cast<int>(jngl::getMousePos().x + screenWidth / 2.),
	           static_cast<int>(jngl::getMousePos().y + screenHeight / 2.));
	x_ += xSpeed_ * timePerFrame; // 100 FPS also jedes mal ein hunderstel der Bewegung
	y_ += ySpeed_ * timePerFrame;
	xSpeed_ *= 0.99; // Die Geschwindigkeit wird 100 mal pro Sekunde um 1 % langsamer
	ySpeed_ *= 0.99;
	if (x_ + width_ > screenWidth) {
		xSpeed_ = -xSpeed_;
		x_ = screenWidth - width_;
	}
	if (y_ + height_ > screenHeight) {
		ySpeed_ = -ySpeed_;
		y_ = screenHeight - height_;
	}
	if (x_ < 0) {
		xSpeed_ = -xSpeed_;
		x_ = 0;
	}
	if (y_ < 0) {
		ySpeed_ = -ySpeed_;
		y_ = 0;
	}
}

void Ball::CheckMouse(const int x, const int y) {
	if (jngl::mouseDown()) {
		xSpeed_ = (x - width_ / 2. - x_) * 10;  // Geschwindigkeit vom Ball auf Entfernung von
		ySpeed_ = (y - height_ / 2. - y_) * 10; // Mausposition und Ballmittelpunkt mal 10 setzen
	}
}
