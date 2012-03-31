#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

@interface JNGLView : UIView {
	EAGLContext* context;
	float angle;
	float desiredAngle;
	int width;
	int height;
	CFTimeInterval startTime;
	
}

- (void) drawView: (CADisplayLink*) displayLink;
- (void) didRotate: (NSNotification*) notification;

@end
