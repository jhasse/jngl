// Copyright 2012-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl.hpp"

#include <algorithm>
#include <boost/math/constants/constants.hpp>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <sstream>
#include <vector>

#ifdef _MSC_VER
#include <filesystem>
#endif

void drawBackground();
void drawMouse(jngl::Vec2);
void testKeys();
int performance = 1;
double factor = 0;

class Test : public jngl::Work {
public:
	Test() : fb2(jngl::getWindowWidth(), jngl::getWindowHeight()), logoWebp("jngl.webp") {
		jngl::setTitle(jngl::App::instance().getDisplayName() + " | UTF-8: äöüß");
		jngl::setIcon("jngl.png");
		jngl::setMouseVisible(false);
		frameTime = jngl::getTime();
		lastTime = jngl::getTime();
	}
	void step() override {
		rotate += 90.0 / 60.0; // 90 degree per second
		if (rotate > 360) {
			rotate = 0;
		}
		factor = std::sin(rotate / 360 * boost::math::constants::pi<double>());
		logoWebp.setPos(-logoWebp.getWidth() * factor, -logoWebp.getHeight() * factor);
		volume += static_cast<float>(jngl::getMouseWheel()) / 100.0f;
		if (jngl::keyPressed('p')) {
			jngl::stop("test.ogg");
			jngl::play("test.ogg");
		}
		if (jngl::keyPressed('s')) {
			useShader = !useShader;
			if (!shaderProgram) {
				fragmentShader = std::make_unique<jngl::Shader>(std::ifstream("data/blur.frag"),
				                                                jngl::Shader::Type::FRAGMENT);
				shaderProgram = std::make_unique<jngl::ShaderProgram>(jngl::Sprite::vertexShader(),
				                                                      *fragmentShader);
			}
		}
	}
	void draw() const override {
		std::optional<jngl::FrameBuffer::Context> fb2Context;
		if (drawOnFrameBuffer) {
			fb2Context = fb2.use();
			fb2Context->clear(0xffffff_rgb);
		}
		jngl::pushMatrix();
		jngl::translate(-jngl::getScreenSize() / 2);
		jngl::pushMatrix();
		lastTime = jngl::getTime();
		for (int i = 0; i < 10; ++i) {
			if (jngl::keyDown(char('0' + i))) {
				performance = i == 0 ? 10 : i;
			}
		}
		drawBackground();
		jngl::setColor(0,0,0,255);
		jngl::pushMatrix();
		jngl::translate(650, 450);
		jngl::rotate(rotate);
		jngl::drawLine(-50, -50, 50, 50);
		jngl::popMatrix();
		jngl::setSpriteAlpha(200);
		jngl::translate(jngl::getScreenSize() / 2);
		jngl::rotate(rotate);
		jngl::setSpriteAlpha(static_cast<unsigned char>(std::abs(factor * 255)));
		if (useShader) {
			logoWebp.drawScaled(static_cast<float>(factor * 2), static_cast<float>(factor * 2),
			                    &*shaderProgram);
		} else {
			logoWebp.drawScaled(static_cast<float>(factor * 2));
		}
		jngl::setColor(0, 0, 0);
		jngl::drawRect(-125, 100, 250, 28);
		jngl::setFontColor(255, 255, 255);
		jngl::print("White text on black background", -115, 105);
		jngl::setFontColor(255, 255, 255);
		jngl::setFontSize(20);
		jngl::print("White text without background", -115, 135);
		jngl::setFontSize(12);
		jngl::popMatrix();
		std::stringstream sstream;
		sstream << "FPS" << (jngl::getVerticalSync() ? " (V-SYNC)" : "") << ": "
		        << int(jngl::getFPS()) << "\nFactor: " << factor
		        << "\nSize of double: " << sizeof(double);
		jngl::setColor(0, 0, 0);
		jngl::drawRect(0, 0, 200, 62);
		jngl::setFontColor(static_cast<unsigned char>(255 * (1 - factor)),
		                   static_cast<unsigned char>(255 * factor), 255);
		jngl::setFontByName("Courier New");
		jngl::print(sstream.str(), 5, 5);
		jngl::setFontByName("sans-serif");
		jngl::setFontColor(0,0,0);
		jngl::setFontByName("Times New Roman");
		jngl::print("Black text on white background", 5, 75);
		jngl::setFontByName("Arial");
		jngl::setFontSize(20);
		jngl::print("UTF-8:   ä ö ü ß Ĉ Ψ ≈", 5, 105);
		jngl::print(" $", static_cast<int>(jngl::getTextWidth("UTF-8:   ä ö ü ß Ĉ Ψ ≈") + 5), 105);
		jngl::setFontSize(12);
		jngl::print("Press 1-9 to test the performance\nPress E to show a error box.", 5, 135);
		if (jngl::keyPressed('e')) {
			jngl::errorMessage("Hello World!");
		}
		jngl::print("Press S to use the blur shader.", 5, 390);
		jngl::print("Press F to turn drawing on a FBO " + std::string(drawOnFrameBuffer ? "off" : "on") + ".", 5, 410);
		jngl::print("Press V to toggle V-SYNC.", 5, 430);
		if (jngl::keyPressed('v')) {
			jngl::setVerticalSync(!jngl::getVerticalSync());
		}
		sstream.str({});
		sstream << "Press A to turn Anti-Aliasing " << (jngl::getAntiAliasing() ? "off" : "on")
		        << ".";
		jngl::print(sstream.str(), 5, 450);
		if (jngl::keyPressed('a')) {
			jngl::setAntiAliasing(!jngl::getAntiAliasing());
		}
		jngl::print("Press F1 to switch fullscreen mode.", 5, 470);
		if (jngl::keyPressed(jngl::key::F1)) {
			jngl::setFullscreen(!jngl::getFullscreen());
		}
		jngl::print("Press K to test key codes.", 5, 490);
		jngl::print("Press P to play a sound.", 6, 510);
		static int playbackSpeed = 100;
		jngl::setPlaybackSpeed(float(playbackSpeed) / 100.0f);
		jngl::print("Press + and - to change the audio playback speed: " +
		      std::to_string(playbackSpeed) + " %", 6, 530);
		if (jngl::keyPressed('-')) {
			--playbackSpeed;
		}
		if (jngl::keyPressed('+')) {
			++playbackSpeed;
		}
		jngl::setVolume(volume);
		jngl::print("Use your mouse wheel to change the volume: " +
		                std::to_string(int(volume * 100)) + " %",
		            6, 550);
		jngl::setColor(0,0,255,128);
		if (drawOnFrameBuffer) {
			fb2Context = {};
			jngl::reset();
			jngl::setSpriteAlpha(255);
			fb2.draw(-jngl::getScreenSize() / 2);
		}
		if (jngl::keyPressed('f')) {
			drawOnFrameBuffer = !drawOnFrameBuffer;
		}
		jngl::popMatrix();
		drawMouse(jngl::getMousePos());
		if (++frameNumber == 500) {
			const auto seconds = jngl::getTime() - frameTime;
			std::cout << "It took " << seconds << " seconds to render 500 frames (~"
			          << int(500.0 / seconds) << " FPS)" << std::endl;
			frameNumber = 0;
			frameTime = jngl::getTime();
		}
		if (jngl::keyDown('k')) {
			testKeys();
		}
		if (jngl::keyPressed(jngl::key::Escape)) {
			jngl::quit();
		}
	}
private:
	mutable bool drawOnFrameBuffer = false;
	bool useShader = false;
	mutable double rotate = 0;
	mutable int frameNumber = 0;
	mutable double frameTime = 0;
	mutable double lastTime;
	mutable jngl::FrameBuffer fb2;
	jngl::Sprite logoWebp;
	float volume = 1;
	std::unique_ptr<jngl::Shader> vertexShader, fragmentShader;
	std::unique_ptr<jngl::ShaderProgram> shaderProgram;
};

JNGL_MAIN_BEGIN {
	try {
		jngl::App app("JNGL Test Application");
#ifdef _MSC_VER
		std::filesystem::current_path(jngl::getBinaryPath());
#endif
		std::cout << "Size of Desktop: " << jngl::getDesktopWidth() << "x"
		          << jngl::getDesktopHeight() << std::endl
		          << "Path of binary: " << jngl::getBinaryPath() << std::endl
		          << "Config path: " << jngl::getConfigPath() << std::endl;
		try {
			std::cout << "Size of jngl.png: " << jngl::getWidth("jngl.png") << "x"
			                                  << jngl::getHeight("jngl.png") << std::endl;
		} catch (std::runtime_error& e) {
			std::cout << e.what() << std::endl;
		}
		const int scaleFactor =
		    std::min((jngl::getDesktopWidth() - 50) / 800, (jngl::getDesktopHeight() - 50) / 600);
		jngl::setScaleFactor(scaleFactor);
		jngl::showWindow("setTitle not working!", 800 * scaleFactor, 600 * scaleFactor);
		jngl::onControllerChanged([]() {
			const auto controllers = jngl::getConnectedControllers();
			std::cout << "Number of connected controllers: " << controllers.size() << std::endl;
		});
		jngl::setWork(std::make_shared<Test>());
		app.mainLoop();
	} catch(std::exception& e) {
		jngl::hideWindow();
		jngl::errorMessage(e.what());
	}
} JNGL_MAIN_END

void drawBackground() {
	jngl::setSpriteAlpha(100);
	if (performance > 1) {
		const int size = performance * performance;
		for (int x = 0; x < size; ++x) {
			for (int y = 0; y < size; ++y) {
				jngl::drawScaled("jngl", x * jngl::getScreenWidth() / size,
				                 y * jngl::getScreenHeight() / size,
				                 float(jngl::getScreenWidth() / size / jngl::getWidth("jngl")),
				                 float(jngl::getScreenHeight() / size / jngl::getHeight("jngl")));
			}
		}
	} else {
		jngl::drawClipped("jngl", jngl::getScreenWidth() / 2 - jngl::getWidth("jngl") / 2.,
		                  jngl::getScreenHeight() / 2 - jngl::getHeight("jngl") / 2.,
		                  float(0.5 - factor / 2), float(0.5 + factor / 2),
		                  float(0.5 - factor / 2), float(0.5 + factor / 2));
	}
	jngl::setColor(255, 0, 0, 100);
	jngl::drawTriangle(600, 30, 700, 30, 650, 130);
	jngl::setColor(0, 255, 0, 100);
	jngl::drawRect(600, 400, 100, 100);
	jngl::setColor(0, 0, 255, 100);
	jngl::drawEllipse(80, 400, 50, 80);
}

void drawMouse(const jngl::Vec2 mouse) {
	jngl::translate(mouse.x, mouse.y);
	jngl::rotate(-45);
	jngl::setFontSize(30);
	jngl::setFontColor(10, 10, 200, 200);
	jngl::print("↑", -8, -2);
	jngl::setFontSize(12);
	jngl::reset();
}

class RecentlyPressedKey {
public:
	RecentlyPressedKey(std::string name, int x, int y)
	: name_(std::move(name)), alpha_(255), x_(x), y_(y), lastTime_(jngl::getTime()) {
	}
	void Draw() {
		double timeSinceLastFrame = jngl::getTime() - lastTime_;
		lastTime_ = jngl::getTime();
		alpha_ -= timeSinceLastFrame * 60;
		x_ += timeSinceLastFrame * 40;
		jngl::setFontColor(0, 0, 0, static_cast<unsigned char>(alpha_));
		jngl::print(name_, int(x_), int(y_));
		jngl::setFontColor(0, 0, 0, 255);
	}
	[[nodiscard]] int GetAlpha() const {
		return int(alpha_);
	}
private:
	std::string name_;
	double alpha_, x_, y_, lastTime_;
};


void testKeys() {
	jngl::setRelativeMouseMode(true);
	double xpos = 0;
	double ypos = 0;
	std::map<std::string, jngl::key::KeyType> keys;
	keys["Left"] = jngl::key::Left;
	keys["Up"] = jngl::key::Up;
	keys["Right"] = jngl::key::Right;
	keys["Down"] = jngl::key::Down;
	keys["PageUp"] = jngl::key::PageUp;
	keys["PageDown"] = jngl::key::PageDown;
	keys["Home"] = jngl::key::Home;
	keys["End"] = jngl::key::End;
	keys["BackSpace"] = jngl::key::BackSpace;
	keys["Tab"] = jngl::key::Tab;
	keys["Clear"] = jngl::key::Clear;
	keys["Return"] = jngl::key::Return;
	keys["Pause"] = jngl::key::Pause;
	keys["Escape"] = jngl::key::Escape;
	keys["Delete"] = jngl::key::Delete;
	keys["ControlL"] = jngl::key::ControlL;
	keys["ControlR"] = jngl::key::ControlR;
	keys["CapsLock"] = jngl::key::CapsLock;
	keys["AltL"] = jngl::key::AltL;
	keys["AltR"] = jngl::key::AltR;
	keys["SuperL"] = jngl::key::SuperL;
	keys["SuperR"] = jngl::key::SuperR;
	keys["Space"] = jngl::key::Space;
	keys["ShiftL"] = jngl::key::ShiftL;
	keys["ShiftR"] = jngl::key::ShiftR;
	keys["F1"] = jngl::key::F1;
	keys["F2"] = jngl::key::F2;
	keys["F3"] = jngl::key::F3;
	keys["F4"] = jngl::key::F4;
	keys["F5"] = jngl::key::F5;
	keys["F6"] = jngl::key::F6;
	keys["F7"] = jngl::key::F7;
	keys["F8"] = jngl::key::F8;
	keys["F9"] = jngl::key::F9;
	keys["F10"] = jngl::key::F10;
	keys["F11"] = jngl::key::F11;
	keys["F12"] = jngl::key::F12;
	keys["Any"] = jngl::key::Any;
	std::vector<RecentlyPressedKey> recentlyPressedKeys;
	std::string textInput;
	while (jngl::running()) {
		jngl::updateInput();
		jngl::setFontSize(10);
		jngl::pushMatrix();
		jngl::translate(-400, -300);
		int y = 10;
		for (const auto& it : keys) {
			if (jngl::keyDown(it.second)) {
				jngl::setFontColor(0, 0, 0);
			}
			else {
				jngl::setFontColor(150, 150, 150);
			}
			jngl::print(it.first, 100, y);
			if (jngl::keyPressed(it.second)){
				recentlyPressedKeys.emplace_back(it.first, 100, y);
			}
			y += 15;
		}
		y = 10;
		const auto printChar = [&recentlyPressedKeys](const char c, const int x, const int y) {
			char cString[2];
			cString[0] = c;
			cString[1] = 0;
			if(jngl::keyDown(c)) {
				jngl::setFontColor(0, 0, 0);
			}
			else {
				jngl::setFontColor(150, 150, 150);
			}
			jngl::print(cString, x, y);
			if (jngl::keyPressed(c)) {
				recentlyPressedKeys.emplace_back(cString, x, y);
			}
		};
		for (char c = '0'; c <= 'z'; ++c) {
			printChar(c, 380, y);
			if (c >= 'a' && c <= 'z') {
				printChar(char(toupper(c)), 610, y);
			}
			y += 15;
			if (c == '9') {
				c = 'a' - 1;
			}
		}

		if (textInput.size() > 40) { textInput.clear(); }
		textInput += jngl::getTextInput();
		jngl::setFontColor(0, 0, 0);
		jngl::print(textInput, 100, 583);

		std::map<std::string, jngl::mouse::Button> buttons;
		buttons["Left Mouse Button"] = jngl::mouse::Left;
		buttons["Middle Mouse Button"] = jngl::mouse::Middle;
		buttons["Right Mouse Button"] = jngl::mouse::Right;
		for (const auto& it : buttons) {
			if (jngl::mouseDown(it.second)) {
				jngl::setFontColor(0, 0, 0);
			}
			else {
				jngl::setFontColor(150, 150, 150);
			}
			jngl::print(it.first, 380, y);
			if (jngl::mousePressed(it.second)) {
				recentlyPressedKeys.emplace_back(it.first, 380, y);
			}
			y += 15;
		}
		auto end = recentlyPressedKeys.end();
		std::for_each(recentlyPressedKeys.begin(), end, [](RecentlyPressedKey& k) {
			k.Draw();
		});
		recentlyPressedKeys.erase(std::remove_if(recentlyPressedKeys.begin(), end,
		                          [](const RecentlyPressedKey& k) -> bool {
			return k.GetAlpha() <= 0;
		}), end);
		std::stringstream sstream;
		sstream << "X: " << jngl::getMousePos().x << "\nY: " << jngl::getMousePos().y << std::endl;
		jngl::print(sstream.str(), 5, 5);
		int controllerNr = 1;
		for (const auto& controller : jngl::getConnectedControllers()) {
			std::stringstream sstream;
			sstream << "Controller " << controllerNr << " connected." << std::endl
			        << "Trigger: " << controller->state(jngl::controller::LeftTrigger) << " " << controller->state(jngl::controller::RightTrigger)
			        << "\nA: " << controller->down(jngl::controller::A)
			        << " B: " << controller->down(jngl::controller::B)
			        << " X: " << controller->down(jngl::controller::X)
			        << " Y: " << controller->down(jngl::controller::Y)
			        << "\nLB: " << controller->down(jngl::controller::LeftButton)
			        << " RB: " << controller->down(jngl::controller::RightButton)
			        << " LT: " << controller->down(jngl::controller::LeftTrigger)
			        << " RT: " << controller->down(jngl::controller::RightTrigger)
			        << "\nLS: " << controller->down(jngl::controller::LeftStick)
			        << " RS: " << controller->down(jngl::controller::RightStick)
			        << " Start: " << controller->down(jngl::controller::Start)
			        << " Back: " << controller->down(jngl::controller::Back)
			        << "\n↑: " << controller->down(jngl::controller::DpadUp)
			        << " ↓: " << controller->down(jngl::controller::DpadDown)
			        << " ←: " << controller->down(jngl::controller::DpadLeft)
			        << " →: " << controller->down(jngl::controller::DpadRight);
			static uint8_t pressedFade = 150;
			if (controller->pressed(jngl::controller::A)) {
				assert(controller->pressed(jngl::controller::A));
				using namespace std::chrono_literals;
				controller->rumble(0.5f, 200ms);
				pressedFade = 0;
			} else {
				if (pressedFade < 150) {
					pressedFade += 10;
				}
			}
			jngl::setFontColor(pressedFade, pressedFade, pressedFade);

			jngl::pushMatrix();
			for (const jngl::Vec2& stick :
			     { jngl::Vec2(controller->state(jngl::controller::LeftStickX),
			                  -controller->state(jngl::controller::LeftStickY)),
			       jngl::Vec2(controller->state(jngl::controller::RightStickX),
			                  -controller->state(jngl::controller::RightStickY)) }) {
				const float circleRadius = 20;
				const auto circlePos = jngl::Vec2(530, double(-40 + controllerNr * 110));
				jngl::setColor(100, 100, 100, 255);
				jngl::drawEllipse(circlePos, circleRadius, circleRadius);
				jngl::setColor(255, 255, 255, 255);
				jngl::drawCircle(circlePos + circleRadius * stick, 4);
				jngl::translate(0, 2 * circleRadius + 10);
			}
			jngl::popMatrix();

			jngl::setColor(255, 255, 255, 150);
			jngl::drawRect({500, 40. + double(controllerNr - 1) * 110.}, {300, 120});
			jngl::print(sstream.str(), 558, 50 + (controllerNr - 1) * 110);
			++controllerNr;
		}
		jngl::popMatrix();
		if (jngl::getRelativeMouseMode()) {
			xpos += jngl::getMousePos().x;
			ypos += jngl::getMousePos().y;
			drawMouse({xpos, ypos});
		}
		if (jngl::keyPressed(jngl::key::Escape)) {
			jngl::setRelativeMouseMode(false);
			jngl::setMouseVisible(true);
			jngl::setMouse({xpos, ypos});
		}
		jngl::swapBuffers();
	}
}
