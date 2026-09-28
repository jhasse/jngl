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

- (void)loadView {
	self.view = jnglView; // Use the JNGLView created by the AppDelegate as our view instead of an
	                      // empty UIView
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

// Must return NO: at least since iOS 18.2 (verified on iOS 26.6.1) the system ignores
// -preferredScreenEdgesDeferringSystemGestures while the home indicator is auto-hidden, so a
// single swipe from the bottom edge would immediately leave the app. With the indicator visible
// the first swipe only reveals it and a second swipe is needed to leave. Since iOS 26 the
// indicator fades out on its own shortly after launch anyway.
- (BOOL)prefersHomeIndicatorAutoHidden {
	return NO;
}

// Defers all system edge gestures (home indicator, Control Center, Notification Center, app
// switcher) so that touches near any screen edge are delivered to the game immediately instead of
// being delayed while iOS waits to see if a system gesture is starting. This requires a second
// swipe from an edge to actually trigger the corresponding system gesture.
- (UIRectEdge)preferredScreenEdgesDeferringSystemGestures {
	return UIRectEdgeAll;
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
