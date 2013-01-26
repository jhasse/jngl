/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "../jngl.hpp"

#define _USE_MATH_DEFINES
#include <cmath>
#include <sstream>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <map>
#include <vector>
#include <algorithm>

void drawBackground();
void drawMouse(int, int);
void drawTess();
void testKeys();
int performance = 1;
double factor = 0;

double absolute(double v);

class Test : public jngl::Work {
public:
	Test() : drawOnFrameBuffer(false), rotate(0), frameNumber(0), fb(100, 110), fb2(800, 600) {
		jngl::setTitle("JNGL Test Application");
		jngl::setIcon("jngl.png");
		jngl::setMouseVisible(false);
		fb.beginDraw();
		jngl::setColor(255, 0, 0);
		jngl::drawRect(0, 0, 64, 64);
		jngl::setColor(0, 255, 0);
		jngl::drawRect(0, 64, 64, 64);
		jngl::setColor(0, 0, 255);
		jngl::drawRect(64, 0, 64, 64);
		jngl::print("Frame\nBuffer\nObject", 50, 10);
		fb.endDraw();
		frameTime = jngl::getTime();
		lastTime = jngl::getTime();
	}
	void step() {
		rotate += 90.0 / 60.0; // 90 degree per second
		if (rotate > 360) {
			rotate = 0;
		}
	}
	void draw() const {
		using namespace jngl;
		if (drawOnFrameBuffer) {
			fb2.beginDraw();
			fb2.clear();
		}
		translate(-getWindowWidth() / 2, -getWindowHeight() / 2);
		pushMatrix();
		lastTime = getTime();
		for (int i = 0; i < 10; ++i) {
			if (keyDown(boost::lexical_cast<char>(i))) {
				performance = i == 0 ? 10 : i;
			}
		}
		drawBackground();
		drawTess();
		setColor(0,0,0,255);
		pushMatrix();
		translate(650, 450);
		jngl::rotate(rotate);
		drawLine(-50, -50, 50, 50);
		popMatrix();
		setSpriteAlpha(200);
		fb.draw(600, 300);
		translate(getWindowWidth() / 2, getWindowHeight() / 2);
		jngl::rotate(rotate);
		factor = sin(rotate / 360 * M_PI);
		setSpriteAlpha(static_cast<unsigned char>(absolute(factor * 255)));
		drawScaled("jngl.webp",
		           -getWidth("jngl.webp")  * factor,
		           -getHeight("jngl.webp") * factor,
		           static_cast<float>(factor * 2));
		setColor(0, 0, 0);
		drawRect(-125, 100, 250, 28);
		setFontColor(255, 255, 255);
		print("White text on black background", -115, 105);
		setFontColor(255, 255, 255);
		setFontSize(20);
		print("White text without background", -115, 135);
		setFontSize(12);
		popMatrix();
		std::stringstream sstream;
		sstream << "FPS" << (getVerticalSync() ? " (V-SYNC)" : "") << ": " << int(getFPS())
		        << "\nFactor: " << factor << "\nSize of double: " << sizeof(double);
		setColor(0, 0, 0);
		drawRect(0, 0, 200, 62);
		setFontColor(static_cast<unsigned char>(255 * (1 - factor)),
		             static_cast<unsigned char>(255 * factor), 255);
		setFontByName("Courier New");
		print(sstream.str(), 5, 5);
		setFontByName("sans-serif");
		setFontColor(0,0,0);
		setFontByName("Times New Roman");
		print("Black text on white background", 5, 75);
		setFontByName("Arial");
		setFontSize(20);
		print("UTF-8:   ä ö ü ß Ĉ Ψ ≈", 5, 105);
		print(" $", static_cast<int>(getTextWidth("UTF-8:   ä ö ü ß Ĉ Ψ ≈") + 5), 105);
		setFontSize(12);
		print("Press 1-9 to test the performance\nPress E to show a error box.", 5, 135);
		if (keyPressed('e')) {
			errorMessage("Hello World!");
		}
		print("Press F to turn drawing on a FBO " + std::string(drawOnFrameBuffer ? "off" : "on") + ".", 5, 410);
		print("Press V to toggle V-SYNC.", 5, 430);
		if (keyPressed('v')) {
			setVerticalSync(!getVerticalSync());
		}
		print("Press A to toggle Anti-Aliasing.", 5, 450);
		if (keyPressed('a')) {
			setAntiAliasing(!getAntiAliasing());
		}
		print("Press F1 to switch fullscreen mode.", 5, 470);
		if (keyPressed(key::F1)) {
			showWindow("JNGL Test Application", 800, 600, !getFullscreen());
		}
		print("Press K to test key codes.", 5, 490);
		print("Press P to play a sound.", 6, 510);
		if (keyPressed('p')) {
			stop("test.ogg");
			play("test.ogg");
		}
		static int playbackSpeed = 100;
		setPlaybackSpeed(playbackSpeed / 100.0f);
		print("Press + and - to change the audio playback speed: " +
		      boost::lexical_cast<std::string>(playbackSpeed) + " %", 6, 530);
		if (keyPressed('-')) {
			--playbackSpeed;
		}
		if (keyPressed('+')) {
			++playbackSpeed;
		}
		static float volume = 1;
		setVolume(volume);
		print("Use your mouse wheel to change the volume: " +
		      boost::lexical_cast<std::string>(int(volume * 100)) +
		      " %", 6, 550);
		volume += static_cast<float>(getMouseWheel()) / 100.0f;
		setColor(0,0,255,128);
		if (drawOnFrameBuffer) {
			fb2.endDraw();
			reset();
			setSpriteAlpha(255);
			fb2.draw(-getWindowWidth() / 2, -getWindowHeight() / 2);
		}
		if (keyPressed('f')) {
			drawOnFrameBuffer = !drawOnFrameBuffer;
		}
		drawMouse(getMouseX(), getMouseY());
		if (++frameNumber == 500) {
			std::cout << "It took " << getTime() - frameTime << " seconds to render 500 frames." << std::endl;
			frameNumber = 0;
			frameTime = getTime();
		}
		if (keyDown('k')) {
			testKeys();
		}
		if (keyPressed(key::Escape)) {
			quit();
		}
	}
private:
	mutable bool drawOnFrameBuffer;
	mutable double rotate;
	mutable int frameNumber;
	mutable double frameTime;
	mutable double lastTime;
	mutable jngl::FrameBuffer fb, fb2;
};

int main() {
	std::cout << "Size of Desktop: " << jngl::getDesktopWidth()
	          << "x" << jngl::getDesktopHeight() << std::endl
	          << "Size of jngl.png: " << jngl::getWidth("jngl.png")
	          << "x" << jngl::getHeight("jngl.png") << std::endl;
	jngl::showWindow("setTitle not working!", 800, 600);
	jngl::setWork(new Test);
	jngl::mainLoop();
}

void drawBackground() {
	jngl::setSpriteAlpha(100);
	if (performance > 1) {
		for (int x = 0; x < performance; ++x) {
			for (int y = 0; y < performance; ++y) {
				jngl::drawScaled("jngl.png",
								 x * jngl::getWindowWidth() / performance,
								 y * jngl::getWindowHeight() / performance,
								 (float)jngl::getWindowWidth() / performance / jngl::getWidth("jngl.png"),
								 (float)jngl::getWindowHeight() / performance / jngl::getHeight("jngl.png"));
			}
		}
	}
	else {
		jngl::drawClipped("jngl",
		                  jngl::getWindowWidth() / 2- jngl::getWidth("jngl") / 2,
		                  jngl::getWindowHeight() / 2- jngl::getHeight("jngl") / 2,
						  0.5 - factor / 2, 0.5 + factor / 2,
						  0.5 - factor / 2, 0.5 + factor / 2);
	}
	jngl::setColor(255, 0, 0, 100);
	jngl::drawRect(600, 30, 100, 100);
	jngl::setColor(0, 255, 0, 100);
	jngl::drawRect(600, 400, 100, 100);
	jngl::setColor(0, 0, 255, 100);
	jngl::drawEllipse(80, 400, 50, 80);
}

void drawMouse(int x, int y) {
	unsigned char red, green, blue;
	jngl::readPixel(jngl::getMouseX(), jngl::getMouseY(), red, green, blue);
	std::stringstream sstream;
	sstream << "R: " << static_cast<int>(red)
	      << "\nG: " << static_cast<int>(green)
	      << "\nB: " << static_cast<int>(blue);
	jngl::setFontSize(8);
	jngl::setFontColor(0, 255, 0, 200);
	jngl::print(sstream.str(), x + 30, y + 10);
	jngl::translate(x, y);
	jngl::rotate(-45);
	jngl::setFontSize(30);
	jngl::setFontColor(10, 10, 200);
	jngl::print("↑", -8, -2);
	jngl::setFontSize(12);
	jngl::reset();
}

void drawTess() {
	jngl::setColor(3, 10, 89, 50);
	jngl::beginPolygon();
	jngl::vertex(80, 580);
	jngl::vertex(130, 480);
	jngl::vertex(180, 380);
	jngl::vertex(230, 380);
	jngl::vertex(280, 480);
	jngl::vertex(330, 480);
	jngl::vertex(380, 380);
	jngl::vertex(430, 480);
	jngl::vertex(480, 480);
	jngl::vertex(530, 580);
	jngl::endPolygon();
	jngl::setFontColor(255, 0, 0);
	jngl::print("A", 80, 580);
	jngl::print("B", 130, 480);
	jngl::print("C", 180, 380);
	jngl::print("D", 230, 380);
	jngl::print("E", 280, 480);
	jngl::print("F", 330, 480);
	jngl::print("G", 380, 380);
	jngl::print("H", 430, 480);
	jngl::print("I", 480, 480);
	jngl::print("J", 530, 580);
}

class RecentlyPressedKey {
public:
	RecentlyPressedKey(std::string name, int x, int y)
		: name_(name), alpha_(255), x_(x), y_(y), lastTime_(jngl::getTime()) {
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
	int GetAlpha() const {
		return int(alpha_);
	}
private:
	std::string name_;
	double alpha_, x_, y_, lastTime_;
};


void testKeys() {
	jngl::setRelativeMouseMode(true);
	int xpos = 400;
	int ypos = 300;
	typedef std::map<std::string, jngl::key::KeyType> MapType;
	MapType keys;
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
	while (jngl::running()) {
		jngl::updateInput();
		jngl::setFontSize(10);
		jngl::translate(-400, -300);
		int y = 10;
		for (MapType::iterator it = keys.begin(); it != keys.end(); ++it) {
			if (jngl::keyDown(it->second)) {
				jngl::setFontColor(0, 0, 0);
			}
			else {
				jngl::setFontColor(150, 150, 150);
			}
			jngl::print(it->first, 100, y);
			if (jngl::keyPressed(it->second)){
				recentlyPressedKeys.push_back(RecentlyPressedKey(it->first, 100, y));
			}
			y += 15;
		}
		y = 10;
		for (char c = '0'; c <= 'z'; ++c) {
			char cString[2];
			cString[0] = c;
			cString[1] = 0;
			if(jngl::keyDown(c)) {
				jngl::setFontColor(0, 0, 0);
			}
			else {
				jngl::setFontColor(150, 150, 150);
			}
			jngl::print(cString, 500, y);
			if (jngl::keyPressed(c)) {
				recentlyPressedKeys.push_back(RecentlyPressedKey(cString, 500, y));
			}
			y += 15;
			if (c == '9') {
				c = 'a' - 1;
			}
		}
		std::map<std::string, jngl::mouse::Button> buttons;
		buttons["Left Mouse Button"] = jngl::mouse::Left;
		buttons["Middle Mouse Button"] = jngl::mouse::Middle;
		buttons["Right Mouse Button"] = jngl::mouse::Right;
		for (auto it = buttons.begin(); it != buttons.end(); ++it) {
			if (jngl::mouseDown(it->second)) {
				jngl::setFontColor(0, 0, 0);
			}
			else {
				jngl::setFontColor(150, 150, 150);
			}
			jngl::print(it->first, 500, y);
			if (jngl::mousePressed(it->second)) {
				recentlyPressedKeys.push_back(RecentlyPressedKey(it->first, 500, y));
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
		sstream << "X: " << jngl::getMouseX() << "\nY: " << jngl::getMouseY() << std::endl;
		jngl::print(sstream.str(), 5, 5);
		for (int i = 0; i < 4; ++i) {
			std::stringstream sstream;
			sstream << "Controller " << i << " connected: " << jngl::isControllerConnected(i) << std::endl
			        << "Trigger: " << jngl::getControllerState(i, jngl::controller::LeftTrigger) << " " << jngl::getControllerState(i, jngl::controller::RightTrigger)
			        << " A: " << jngl::getControllerState(i, jngl::controller::A);
			jngl::print(sstream.str(), 500, 100 + i * 50);
		}
		if (jngl::getRelativeMouseMode()) {
			xpos += jngl::getMouseX();
			ypos += jngl::getMouseY();
			drawMouse(xpos, ypos);
		}
		if (jngl::keyPressed(jngl::key::Escape)) {
			jngl::setRelativeMouseMode(false);
			jngl::setMouseVisible(true);
			jngl::setMouse(xpos, ypos);
		}
		jngl::swapBuffers();
	}
}

double absolute(double v) {
	return v < 0 ? -v : v;
}