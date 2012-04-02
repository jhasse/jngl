#import "JNGLView.h"

#include "jngl.hpp"
#include "windowptr.hpp"
#include "time.hpp"
#include "sprite.hpp"
#include "windowimpl.hpp"

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
		
		glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &width);
		glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &height);
		
		jngl::ShowWindow("", height, width);
		
		CADisplayLink* displayLink;
		displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(drawView:)];
		
		[displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
		
		[[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
		
		[[NSNotificationCenter defaultCenter]
		 addObserver:self
		 selector:@selector(didRotate:)
		 name:UIDeviceOrientationDidChangeNotification object:nil];
		
		startTime = -1;
		desiredAngle = angle = 0;
		jngl::setPrefix(std::string([[[NSBundle mainBundle] resourcePath] UTF8String]) + "/");
		impl = jngl::pWindow->getImpl();
    }
    return self;
}

- (void) drawView: (CADisplayLink*) displayLink
{
	if (displayLink) {
		if (startTime < 0) {
			startTime = displayLink.timestamp;
			std::cout << "DRAW" << std::endl;
		} else {
			jngl::elapsedSeconds = displayLink.timestamp - startTime;
		}
		
		if (jngl::pWindow->stepIfNeeded()) {
			angle += (desiredAngle - angle) * 0.1;
		}
	}

	glLoadIdentity();
	glClearColor(1.0f, 1.0f, 1.0f, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	jngl::Translate(-width/2, height/2);
	jngl::Rotate(-90);
	jngl::Translate(height/2, width/2);
	jngl::Rotate(angle);
	
	jngl::pWindow->draw();
	
	[context presentRenderbuffer:GL_RENDERBUFFER_OES];
}

- (void) touchesBegan: (NSSet*) touches withEvent: (UIEvent*) event
{
	UITouch* touch = [touches anyObject];
	CGPoint location = [touch locationInView: self];
	std::cout << "pWindow: " << jngl::pWindow.get() << "\n";
	impl->setMouse(location.x, location.y);
	impl->setMouseDown(true);
}

- (void) touchesEnded: (NSSet*) touches withEvent: (UIEvent*) event
{
	UITouch* touch = [touches anyObject];
	CGPoint location = [touch locationInView: self];
	impl->setMouse(location.x, location.y);
	impl->setMouseDown(false);
}

- (void) touchesMoved: (NSSet*) touches withEvent: (UIEvent*) event
{
	UITouch* touch = [touches anyObject];
	CGPoint location = [touch locationInView: self];
	impl->setMouse(location.x, location.y);
}

- (void) didRotate:(NSNotification*) notification
{
	UIDeviceOrientation orientation = [[UIDevice currentDevice] orientation];
	switch (orientation) {
		case UIDeviceOrientationLandscapeLeft:
			desiredAngle = 180;
			break;
		case UIDeviceOrientationLandscapeRight:
			desiredAngle = 0;
			break;
		default: break;
	}
}

@end
