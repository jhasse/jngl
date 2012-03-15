//
//  AppDelegate.h
//  Test iOS
//
//  Created by Jan Niklas Hasse on 01.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "JNGLView.h"

@interface AppDelegate : UIResponder <UIApplicationDelegate> {
	JNGLView* view;
}

@property (strong, nonatomic) UIWindow *window;

@end
