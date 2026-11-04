#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#ifndef JNGL_METAL
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>
#endif

namespace jngl {
struct AppParameters;
class WindowImpl;
class Finally;
} // namespace jngl

/// Backed by a CAMetalLayer that the MetalRenderer draws into with JNGL_METAL, otherwise by a
/// CAEAGLLayer for OpenGL ES
@interface JNGLView : UIView <UIKeyInput> {
#ifndef JNGL_METAL
	EAGLContext* context;
	GLuint renderbuffer;
#endif
	float angle;
	int width;
	int height;
	CFTimeInterval startTime;
	jngl::WindowImpl* impl;
	bool pause;
	bool needToResetFrameLimiter;
	jngl::Finally* cleanAppImpl;
}

- (instancetype)initWithFrame:(CGRect)frame withAppParameters:(const jngl::AppParameters&)params;
- (void) drawView: (CADisplayLink*) displayLink;
- (void) touchesEnded: (NSSet*) touches withEvent: (UIEvent*) event;
- (void) touchesMoved: (NSSet*) touches withEvent: (UIEvent*) event;
- (void) setPause: (bool) p;
- (void) initControllers;

@end
