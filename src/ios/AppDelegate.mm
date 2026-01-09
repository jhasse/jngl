// Copyright 2012-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#import "AppDelegate.h"

#include "../jngl.hpp"
#include "../jngl/init.hpp"

#include <sstream>

@implementation AppDelegate

@synthesize window = _window;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
	self.window = [[UIWindow alloc] initWithFrame:[UIScreen mainScreen].bounds];
	jngl::setPrefix(std::string([NSBundle mainBundle].resourcePath.UTF8String) + "/");
	jngl::AppParameters params = jnglInit();
	view = [[JNGLView alloc] initWithFrame:[UIScreen mainScreen].bounds withAppParameters:params];

	JNGLViewController* jvc = [[JNGLViewController alloc] initWithNibName:nil bundle:nil];
	self.window.rootViewController = jvc;
	jnglView = view;

	[self.window addSubview:view];

	jngl::setScene(params.start());

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
