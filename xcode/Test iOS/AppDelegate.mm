// Copyright 2012-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#import "AppDelegate.h"

#include <jngl.hpp>
#include <sstream>
#include <iostream>

@implementation AppDelegate

@synthesize window = _window;

class Test : public jngl::Work {
public:
	Test() : angle(0), fbo(0) {
		jngl::setFont("Arial.ttf");
		std::cout << "Resolution: " << jngl::getWindowWidth() << "x" << jngl::getWindowHeight() << std::endl
		          << "Config path: " << jngl::getConfigPath() << std::endl;
		auto sstream = jngl::readAsset("Arial.ttf");
		sstream.seekp(0, std::ios::end);
		std::cout << "Size of Arial.ttf in bytes: " << sstream.tellp() << std::endl;
	}
	void step() {
		angle += 1;
		if (!fbo) {
			fbo = new jngl::FrameBuffer(jngl::getWindowWidth(), jngl::getWindowHeight() * 0.75);
		}
		for (char c = ' '; c < '~'; ++c) {
			if (jngl::keyPressed(c)) {
				std::cout << c << std::flush;
			}
		}
		if (jngl::keyPressed(jngl::key::Return)) {
			std::cout << std::endl;
			jngl::play("test.ogg");
		}
		if (jngl::keyPressed(jngl::key::BackSpace)) {
			jngl::setKeyboardVisible(false);
		}
	}
	void draw() const {
		if (fbo) {
			{
				const auto context = fbo->use();
				jngl::print("fbo", -jngl::getWindowWidth() / 2, -jngl::getWindowHeight() / 2);
				jngl::draw("jngl.webp", -300, -140);
			}
			fbo->draw(-jngl::getWindowWidth() / 2, -jngl::getWindowHeight() / 2);
		}
		if (jngl::isMultitouch()) {
			jngl::setSpriteColor(0, 0, 0);
			jngl::setKeyboardVisible(true);
		} else {
			jngl::setSpriteColor(255, 255, 255);
		}
		jngl::draw("jngl.webp", jngl::getMousePos() - jngl::Vec2(300, 140));
		std::stringstream sstream;
		static double fps = 0.0;
		fps *= 0.9;
		fps += jngl::getFPS() * 0.1;
		if (fps > 1000) fps = 0;
		static int count = 0;
		static double fpsCache = fps;
		if (++count % 10 == 0) {
			fpsCache = fps;
		}
		sstream << "FPS: " << int(fpsCache) << " Time: " << jngl::getTime();
		jngl::print(sstream.str(), -230, -150);
		jngl::print("Retina Display!", 260, 340);
		jngl::setColor(255, 0, 0);
		jngl::drawLine(-240, 160, 239, 160);
		jngl::drawLine(239, -160, 239, 160);
		jngl::setColor(0, 0, 0, 100);
		jngl::drawEllipse(0, 0, 100, 100, 2);

		jngl::pushMatrix();
		jngl::translate(-40, 140);
		jngl::rotate(angle);
		jngl::print("Hallo Welt!", -50, -10);
		jngl::popMatrix();

		int controllerNr = 1;
		for (const auto& controller : jngl::getConnectedControllers()) {
			std::stringstream sstream;
			sstream << "Controller " << controllerNr << " connected." << std::endl
			        << "Trigger: " << controller->state(jngl::controller::LeftTrigger) << " "
			        << controller->state(jngl::controller::RightTrigger)
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
			        << "\n↑: " << controller->state(jngl::controller::DpadUp)
			        << " ↓: " << controller->state(jngl::controller::DpadDown)
			        << " ←: " << controller->state(jngl::controller::DpadLeft)
			        << " →: " << controller->state(jngl::controller::DpadRight);
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
			for (const jngl::Vec2 stick :
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
			jngl::drawRect({ 500, 40. + double(controllerNr - 1) * 110. }, { 300, 120 });
			jngl::print(sstream.str(), 558, 50 + (controllerNr - 1) * 110);
			++controllerNr;
		}
	}
private:
	int angle;
	mutable jngl::FrameBuffer* fbo;
};

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
	view = [[JNGLView alloc] initWithFrame:[[UIScreen mainScreen] bounds]];

	JNGLViewController* jvc = [[JNGLViewController alloc] initWithNibName:nil bundle:nil];
	self.window.rootViewController = jvc;
	jnglView = view;

	[self.window addSubview:view];

	jngl::setWork(std::make_shared<Test>());
	std::cout << "START" << std::endl;
	[view drawView:nil];
    [self.window makeKeyAndVisible];
	[[UIApplication sharedApplication] setIdleTimerDisabled:YES];
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
	// Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
	// Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
	[view setPause:true];
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
	// Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
	// If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
	// Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
	// Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
	[view setPause:false];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
	// Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

@end
