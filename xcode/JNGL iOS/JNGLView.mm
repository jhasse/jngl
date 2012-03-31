#import "JNGLView.h"

#include "jngl.hpp"
#include "windowptr.hpp"
#include "time.hpp"

#include <iostream>

@implementation JNGLView

+ (Class) layerClass
{
	return [CAEAGLLayer class];
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
		if ([[UIScreen mainScreen] respondsToSelector: NSSelectorFromString(@"scale")]) {
			if ([self respondsToSelector: NSSelectorFromString(@"contentScaleFactor")]) {
				self.contentScaleFactor = [[UIScreen mainScreen] scale];
			}
		}
		CAEAGLLayer* eaglLayer = (CAEAGLLayer*) super.layer;
		eaglLayer.opaque = YES;
		
		context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
		
		if (!context || ![EAGLContext setCurrentContext:context]) {
			return nil;
		}
		
		GLuint framebuffer, renderbuffer;
		glGenFramebuffersOES(1, &framebuffer);
		glGenRenderbuffersOES(1, &renderbuffer);
		
		glBindFramebufferOES(GL_FRAMEBUFFER_OES, framebuffer);
		glBindRenderbufferOES(GL_RENDERBUFFER_OES, renderbuffer);

		[context
		 renderbufferStorage:GL_RENDERBUFFER_OES
		 fromDrawable: eaglLayer];
		
		glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES,
									 GL_RENDERBUFFER_OES, renderbuffer);
		
		width = CGRectGetWidth(frame);
		height = CGRectGetHeight(frame);
		glViewport(0, 0, width, height);
		
		jngl::ShowWindow("", width, height);
		std::cout << "Resolution: " << width << "x" << height << std::endl;
		
		CADisplayLink* displayLink;
		displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(drawView:)];
		
		[displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
		
		[[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
		
		[[NSNotificationCenter defaultCenter]
		 addObserver:self
		 selector:@selector(didRotate:)
		 name:UIDeviceOrientationDidChangeNotification object:nil];
		
		startTime = -1;
    }
    return self;
}

- (void) drawView: (CADisplayLink*) displayLink
{
	if (startTime < 0) {
		startTime = displayLink.timestamp;
	} else {
		jngl::elapsedSeconds = displayLink.timestamp - startTime;
	}
	
	jngl::pWindow->stepIfNeeded();

	glLoadIdentity();
	glClearColor(1.0f, 1.0f, 1.0f, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	jngl::Translate(0, height);
	jngl::Rotate(-90);
	jngl::Translate(height / 2, width / 2);
	jngl::Rotate(angle);
	angle += (desiredAngle - angle) * 0.1;
	jngl::Translate(-height / 2, -width / 2);
	
	jngl::pWindow->draw();
	
	[context presentRenderbuffer:GL_RENDERBUFFER_OES];
}

- (void) didRotate:(NSNotification*) notification
{
	UIDeviceOrientation orientation = [[UIDevice currentDevice] orientation];
	if (orientation == UIDeviceOrientationLandscapeLeft) {
		desiredAngle = 180;
	} else {
		desiredAngle = 0;
	}
}

@end
