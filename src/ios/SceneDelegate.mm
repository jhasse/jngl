// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#import "SceneDelegate.h"

#import "JNGLViewController.h"

#include "../jngl.hpp"
#include "../jngl/init.hpp"

@implementation SceneDelegate

@synthesize window = _window;

- (void)scene:(UIScene*)scene
    willConnectToSession:(UISceneSession*)session
                 options:(UISceneConnectionOptions*)connectionOptions {
	if (![scene isKindOfClass:[UIWindowScene class]]) {
		return;
	}
	UIWindowScene* windowScene = (UIWindowScene*)scene;
	self.window = [[UIWindow alloc] initWithWindowScene:windowScene];
	jngl::setPrefix(std::string([NSBundle mainBundle].resourcePath.UTF8String) + "/");
	jngl::AppParameters params = jnglInit();
	view = [[JNGLView alloc] initWithFrame:windowScene.screen.bounds withAppParameters:params];
	jnglView = view; // must be set before the view controller loads its view (see -loadView)

	JNGLViewController* jvc = [[JNGLViewController alloc] initWithNibName:nil bundle:nil];
	self.window.rootViewController = jvc;

	jngl::setScene(params.start());

	[view drawView:nil];
	[self.window makeKeyAndVisible];

	// This runs after UIApplicationDidFinishLaunchingNotification has been posted, so the app has
	// fully launched and the controllers can be initialized right away.
	[view initControllers];
}

- (void)sceneWillResignActive:(UIScene*)scene {
	// Sent when the scene is about to move from active to inactive state. This can occur for
	// certain types of temporary interruptions (such as an incoming phone call or SMS message) or
	// when the user quits the app and it begins the transition to the background state. Use this
	// method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games
	// should use this method to pause the game.
	[view setPause:true];
}

- (void)sceneDidBecomeActive:(UIScene*)scene {
	// Restart any tasks that were paused (or not yet started) while the scene was inactive. If the
	// scene was previously in the background, optionally refresh the user interface.
	[view setPause:false];
}

@end
