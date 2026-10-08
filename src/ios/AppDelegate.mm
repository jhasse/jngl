// Copyright 2012-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#import "AppDelegate.h"

#import "SceneDelegate.h"

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
	[application setIdleTimerDisabled:YES];
	return YES;
}

// Apps built against the iOS 26 SDK or newer don't launch unless they have adopted the UIScene life
// cycle. Implementing this marks JNGL as having adopted it, so that games don't need to add a
// UIApplicationSceneManifest to their Info.plist. Everything which used to live in this class has
// moved to SceneDelegate, as UIKit no longer calls the applicationDid…/applicationWill… methods.
- (UISceneConfiguration*)application:(UIApplication*)application
    configurationForConnectingSceneSession:(UISceneSession*)connectingSceneSession
                                   options:(UISceneConnectionOptions*)options {
	UISceneConfiguration* configuration =
	    [[UISceneConfiguration alloc] initWithName:@"Default Configuration"
	                                   sessionRole:connectingSceneSession.role];
	configuration.delegateClass = [SceneDelegate class];
	return configuration;
}

@end
