#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

namespace jngl {
	class WindowImpl;
}

@interface JNGLView : UIView {
	EAGLContext* context;
	float angle;
	float desiredAngle;
	int width;
	int height;
	CFTimeInterval startTime;
	jngl::WindowImpl* impl;
}

- (void) drawView: (CADisplayLink*) displayLink;
- (void) didRotate: (NSNotification*) notification;
- (void) touchesEnded: (NSSet*) touches withEvent: (UIEvent*) event;
- (void) touchesMoved: (NSSet*) touches withEvent: (UIEvent*) event;
- (void) didRotate:(NSNotification*) notification;

@end
