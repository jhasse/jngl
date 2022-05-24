// Copyright 2012-2022 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#import "JNGLViewController.h"

#include "../jngl/input.hpp"

#import <AVFoundation/AVAudioSession.h>

JNGLView* jnglView = nullptr;

@interface JNGLViewController ()

@end

@implementation JNGLViewController

- (instancetype)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
		[[AVAudioSession sharedInstance] setCategory: AVAudioSessionCategoryAmbient error: nil];
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
#if !TARGET_OS_TV
	self.view.multipleTouchEnabled = YES;
#endif

	// JNGL can't override applicationDidFinishLaunching since the AppDelegate belongs to the app.
	// Therefore we use this method to init our controllers when the app has started:
	[[NSNotificationCenter defaultCenter] addObserver:self
	                                         selector:@selector(initControllers:)
	                                             name:UIApplicationDidFinishLaunchingNotification
	                                           object:nil];
}

- (void)initControllers:(NSNotification*)notification {
	[jnglView initControllers];
}

- (void)viewDidUnload
{
    [super viewDidUnload];
}

- (BOOL)shouldAutorotate {
	return YES;
}

- (NSUInteger)supportedInterfaceOrientations {
	return UIInterfaceOrientationMaskLandscapeLeft | UIInterfaceOrientationMaskLandscapeRight;
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	[jnglView touchesBegan:touches withEvent:event];
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	[jnglView touchesMoved:touches withEvent:event];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	[jnglView touchesEnded:touches withEvent:event];
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	[jnglView touchesCancelled:touches withEvent:event];
}

- (BOOL)prefersHomeIndicatorAutoHidden {
	return YES;
}

@end

namespace jngl {
	void setKeyboardVisible(bool v) {
		if (v) {
			[jnglView becomeFirstResponder];
		} else {
			[jnglView resignFirstResponder];
		}
	}
}
