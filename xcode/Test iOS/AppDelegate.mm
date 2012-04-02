#import "AppDelegate.h"

#import <JNGL/jngl.hpp>
#include <sstream>

@implementation AppDelegate

@synthesize window = _window;

class Test : public jngl::Work {
public:
	Test() : angle(0) {
		jngl::SetFont("Arial.ttf");
		jngl::Play("test.ogg");
		std::cout << "Resolution: " << jngl::GetWindowWidth() << "x" << jngl::GetWindowHeight() << std::endl;
	}
	void Step() {
		angle += 1;
	}
	void Draw() {
		jngl::Draw("jngl.png", -300, -140);
		std::stringstream sstream;
		sstream << "FPS: " << int(jngl::FPS()) << " Time: " << jngl::Time();
		jngl::Print(sstream.str(), -230, -150);
		jngl::Print("Retina Display!", 260, 340);
		jngl::Translate(-40, 140);
		jngl::Rotate(angle);
		jngl::Print("Hallo Welt!", -50, -10);
	}
private:
	int angle;
	const char* ogg;
};

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
	view = [[JNGLView alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
	
	[self.window addSubview: view];
    [self.window makeKeyAndVisible];
	
	jngl::ShowWindow("", jngl::GetWindowWidth(), jngl::GetWindowHeight());
	jngl::SetWork(new Test);
	std::cout << "START" << std::endl;
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
