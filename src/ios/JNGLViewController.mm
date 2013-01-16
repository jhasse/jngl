#import "JNGLViewController.h"

#include "../jngl/input.hpp"

#import <AVFoundation/AVAudioSession.h>

JNGLView* jnglView = nullptr;

@interface JNGLViewController ()

@end

@implementation JNGLViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
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
	self.view.multipleTouchEnabled = YES;
}

- (void)viewDidUnload
{
    [super viewDidUnload];
}

-(void)willRotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation duration:(NSTimeInterval)duration {
	CGRect screenRect = [[UIScreen mainScreen] bounds];
	CGRect rect;

	if(toInterfaceOrientation == UIInterfaceOrientationPortrait ||
	   toInterfaceOrientation == UIInterfaceOrientationPortraitUpsideDown) {
		rect = screenRect;
	} else if(toInterfaceOrientation == UIInterfaceOrientationLandscapeLeft ||
	          toInterfaceOrientation == UIInterfaceOrientationLandscapeRight) {
		rect.size = CGSizeMake( screenRect.size.height, screenRect.size.width );
	}
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    return YES;
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

@end

namespace jngl {
	void setKeyboardVisible(bool v) {
		if (v) {
			[jnglView becomeFirstResponder];
		} else {
			[jnglView resignFirstResponder];
		}
	}

	void sleep(int ms) {
		[NSThread sleepForTimeInterval:double(ms)/1000.0];
	}
}