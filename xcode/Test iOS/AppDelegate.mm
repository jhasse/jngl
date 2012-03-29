#import "AppDelegate.h"

#import <JNGL/jngl.hpp>

@implementation AppDelegate

@synthesize window = _window;

class Test : public jngl::Work {
public:
	void Step() {
	}
	void Draw() {
		jngl::Draw("/Users/jhasse/git/jngl/jngl.png", 20, 20);
		jngl::Print("0x0", 0, 0);
		jngl::Print("100x100", 100, 100);
		jngl::Print("200x200", 200, 200);
		jngl::Print("960x480", jngl::GetWindowWidth()-jngl::GetTextWidth("960x480"), jngl::GetWindowHeight()-15);
	}
private:
};

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
	view = [[JNGLView alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
	
	[self.window addSubview: view];
    [self.window makeKeyAndVisible];
	
	jngl::SetWork(new Test);
	[view drawView];
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
