#import "JNGLView.h"

#include "jngl.hpp"
#include "windowptr.hpp"
#include "time.hpp"
#include "sprite.hpp"
#include "windowimpl.hpp"
#include "main.hpp"

#include <iostream>

@implementation JNGLView

+ (Class) layerClass {
	return [CAEAGLLayer class];
}

- (id) initWithFrame: (CGRect)frame {
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

		jngl::showWindow("", width, height);
		
		CADisplayLink* displayLink;
		displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(drawView:)];
		
		[displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
		
		[[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
		
		startTime = -1;
		pause = false;
		needToResetFrameLimiter = false;
		jngl::setPrefix(std::string([[[NSBundle mainBundle] resourcePath] UTF8String]) + "/");
		jngl::setConfigPath(std::string([[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES)
		                                  objectAtIndex:0]
		                                 UTF8String]) + "/");
		impl = jngl::pWindow->getImpl();
		[UIView setAnimationsEnabled:NO];
	}
	return self;
}

- (void) drawView: (CADisplayLink*) displayLink {
	if (!pause) {
		if (displayLink) {
			if (startTime < 0) {
				startTime = displayLink.timestamp;
			} else {
				jngl::elapsedSeconds = displayLink.timestamp - startTime;
				if (needToResetFrameLimiter) {
					jngl::resetFrameLimiter();
					needToResetFrameLimiter = false;
				}
			}

			jngl::pWindow->stepIfNeeded();
		}
		glLoadIdentity();
		// jngl::clearBackgroundColor();
		glClear(GL_COLOR_BUFFER_BIT);
		jngl::rotate(90);
		
		jngl::pWindow->draw();
		[context presentRenderbuffer:GL_RENDERBUFFER_OES];
	}
}

- (void) touchesBegan: (NSSet*) touches withEvent: (UIEvent*) event {
	UITouch* touch = [touches anyObject];
	CGPoint location = [touch locationInView: self];
	impl->setMouse(location.x * self.contentScaleFactor, location.y * self.contentScaleFactor);
	impl->setMouseDown(true);
}

- (void) touchesEnded: (NSSet*) touches withEvent: (UIEvent*) event {
	UITouch* touch = [touches anyObject];
	CGPoint location = [touch locationInView: self];
	impl->setMouse(location.x * self.contentScaleFactor, location.y * self.contentScaleFactor);
	impl->setMouseDown(false);
}

- (void) touchesMoved: (NSSet*) touches withEvent: (UIEvent*) event {
	UITouch* touch = [touches anyObject];
	CGPoint location = [touch locationInView: self];
	impl->setMouse(location.x * self.contentScaleFactor, location.y * self.contentScaleFactor);
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

-(void) setPause: (bool) p {
	pause = p;
	if (!p) {
		needToResetFrameLimiter = true;
	}
}

- (UIKeyboardType) keyboardType {
	if (jngl::getKeyboardType() == jngl::Numpad) {
		return UIKeyboardTypeNumberPad;
	}
	return UIKeyboardTypeDefault;
}

@end
