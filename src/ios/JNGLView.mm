#import "JNGLView.h"

#include "jngl.hpp"
#include "windowptr.hpp"
#include "time.hpp"
#include "sprite.hpp"
#include "windowimpl.hpp"
#include "main.hpp"

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

		jngl::showWindow("", height, width);
		
		CADisplayLink* displayLink;
		displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(drawView:)];
		
		[displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
		
		[[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(didRotate:) name:UIDeviceOrientationDidChangeNotification object:nil];
		
		startTime = -1;
		desiredAngle = angle = 0;
		jngl::setPrefix(std::string([[[NSBundle mainBundle] resourcePath] UTF8String]) + "/");
		jngl::setConfigPath(std::string([[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES)
		                                  objectAtIndex:0]
		                                 UTF8String]) + "/");
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
		
		while (jngl::pWindow->stepIfNeeded()) {
			if (angle != desiredAngle) {
				const float speed = 5.8f;
				if (angle < desiredAngle) {
					if (desiredAngle - angle < speed*2) {
						angle = desiredAngle;
					} else {
						angle += speed;
					}
				}
				if (angle > desiredAngle) {
					if (angle - desiredAngle < speed*2) {
						angle = desiredAngle;
					} else {
						angle -= speed;
					}
				}
			}
		}
	}

	glLoadIdentity();
	jngl::clearBackgroundColor();
	glClear(GL_COLOR_BUFFER_BIT);
	jngl::rotate(angle);
	
	jngl::pWindow->draw();
	
	[context presentRenderbuffer:GL_RENDERBUFFER_OES];
}

- (void) touchesBegan: (NSSet*) touches withEvent: (UIEvent*) event
{
	UITouch* touch = [touches anyObject];
	CGPoint location = [touch locationInView: self];
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
			impl->setFlip(true);
			break;
		case UIDeviceOrientationLandscapeRight:
			desiredAngle = 0;
			impl->setFlip(false);
			break;
		default: break;
	}
}

-(void) insertText: (NSString*) text {
	std::string c = [text UTF8String];
	if (c == "\n") {
		jngl::setKeyPressed(jngl::key::Return, true);
	} else {
		jngl::setKeyPressed(c, true);
	}
}

-(void) deleteBackward {
	jngl::setKeyPressed(jngl::key::BackSpace, true);
}

-(BOOL) canBecomeFirstResponder {
	return YES;
}

-(BOOL) hasText {
	return YES;
}

@end