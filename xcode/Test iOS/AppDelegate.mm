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
			fbo->beginDraw();
			jngl::print("fbo", -jngl::getWindowWidth() / 2, -jngl::getWindowHeight() / 2);
			jngl::draw("jngl.webp", -300, -140);
			fbo->endDraw();
			fbo->draw(-jngl::getWindowWidth() / 2, -jngl::getWindowHeight() / 2);
		}
		if (jngl::mouseDown()) {
			jngl::setSpriteColor(0, 0, 0);
			jngl::setKeyboardVisible(true);
		} else {
			jngl::setSpriteColor(255, 255, 255);
		}
		jngl::draw("jngl.webp", -300 + jngl::getMouseX(), -140 + jngl::getMouseY());
		std::stringstream sstream;
		sstream << "FPS: " << int(jngl::getFPS()) << " Time: " << jngl::getTime();
		jngl::print(sstream.str(), -230, -150);
		jngl::print("Retina Display!", 260, 340);
		jngl::setColor(255, 0, 0);
		jngl::drawLine(-240, 160, 239, 160);
		jngl::drawLine(239, -160, 239, 160);
		jngl::setColor(0, 0, 0, 100);
		jngl::drawEllipse(0, 0, 100, 100, 2);
		jngl::translate(-40, 140);
		jngl::rotate(angle);
		jngl::print("Hallo Welt!", -50, -10);
	}
private:
	int angle;
	const char* ogg;
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

	jngl::showWindow("", jngl::getWindowWidth(), jngl::getWindowHeight());
	jngl::setWork(new Test);
	std::cout << "START" << std::endl;
	[view drawView:nil];
    [self.window makeKeyAndVisible];
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
	// Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
	// Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
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
}

- (void)applicationWillTerminate:(UIApplication *)application
{
	// Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

@end
