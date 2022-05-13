#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>

namespace jngl {
	struct AppParameters;
	class WindowImpl;
}

@interface JNGLView : UIView <UIKeyInput> {
	EAGLContext* context;
	float angle;
	int width;
	int height;
	CFTimeInterval startTime;
	jngl::WindowImpl* impl;
	bool pause;
	bool needToResetFrameLimiter;
}

- (instancetype)initWithFrame:(CGRect)frame withAppParameters:(const jngl::AppParameters&)params;
- (void) drawView: (CADisplayLink*) displayLink;
- (void) touchesEnded: (NSSet*) touches withEvent: (UIEvent*) event;
- (void) touchesMoved: (NSSet*) touches withEvent: (UIEvent*) event;
- (void) setPause: (bool) p;
- (void) initControllers;

@end
