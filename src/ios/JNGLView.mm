// Copyright 2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#import "JNGLView.h"

#include "../App.hpp"
#include "../helper.hpp"
#include "../jngl.hpp"
#include "../windowptr.hpp"
#include "../jngl/AppParameters.hpp"
#include "../jngl/sprite.hpp"
#include "windowimpl.hpp"
#include "AppleController.h"

#include <iostream>

#import <GameController/GameController.h>

std::unique_ptr<jngl::App> jnglApp;

@implementation JNGLView

+ (Class) layerClass {
	return [CAEAGLLayer class];
}

- (instancetype) initWithFrame: (CGRect)frame withAppParameters: (const jngl::AppParameters&)params {
	self = [super initWithFrame:frame];
	if (self) {
		if ([[UIScreen mainScreen] respondsToSelector: NSSelectorFromString(@"scale")]) {
			if ([self respondsToSelector: NSSelectorFromString(@"contentScaleFactor")]) {
				self.contentScaleFactor = [UIScreen mainScreen].scale;
			}
		}

		CAEAGLLayer* eaglLayer = (CAEAGLLayer*) super.layer;
		eaglLayer.opaque = YES;

		context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];

		if (!context || ![EAGLContext setCurrentContext:context]) {
			return nil;
		}

		GLuint framebuffer, renderbuffer;
		glGenFramebuffers(1, &framebuffer);
		glGenRenderbuffers(1, &renderbuffer);

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);


		[context
		 renderbufferStorage:GL_RENDERBUFFER
		 fromDrawable: eaglLayer];

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
									 GL_RENDERBUFFER, renderbuffer);

		glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
		glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);

		jnglApp.reset(new jngl::App(params));
		jngl::showWindow("", width, height, true,
		                 params.minAspectRatio ? *params.minAspectRatio : std::make_pair(1, 3),
		                 params.maxAspectRatio ? *params.maxAspectRatio : std::make_pair(3, 1));

		CADisplayLink* displayLink;
		displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(drawView:)];

		[displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];

#if !TARGET_OS_TV
		[[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
#endif

		startTime = -1;
		pause = false;
		needToResetFrameLimiter = false;
		jngl::setPrefix(std::string([NSBundle mainBundle].resourcePath.UTF8String) + "/");
		jngl::setConfigPath(std::string(NSSearchPathForDirectoriesInDomains(
		                                    NSDocumentDirectory, NSUserDomainMask, YES)[0]
		                                    .UTF8String) +
		                    "/");
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
				if (needToResetFrameLimiter) {
					jngl::resetFrameLimiter();
					needToResetFrameLimiter = false;
				}
			}

			jngl::pWindow->stepIfNeeded();
		}
		jngl::reset();
		// jngl::clearBackgroundColor();
		glClear(GL_COLOR_BUFFER_BIT);

		jngl::pWindow->draw();
		[context presentRenderbuffer:GL_RENDERBUFFER];
		jngl::pWindow->sleepIfNeeded();
	}
}

- (void) handleTouch: (NSSet*) touches withType: (jngl::WindowImpl::Touch) type {
	const auto positions = std::make_unique<std::pair<intptr_t, jngl::Vec2>[]>(touches.count);
	size_t i = 0;
	for (UITouch* touch in touches) {
		CGPoint location = [touch locationInView: self];
		positions[i].first = intptr_t(touch);
		positions[i].second.x = location.x * self.contentScaleFactor;
		positions[i].second.y = location.y * self.contentScaleFactor;
		++i;
	}
	impl->handleTouch(positions.get(), touches.count, type);
}

- (void) touchesBegan: (NSSet*) touches withEvent: (UIEvent*) event {
	[self handleTouch: touches withType: jngl::WindowImpl::Touch::BEGAN];
}

- (void) touchesEnded: (NSSet*) touches withEvent: (UIEvent*) event {
	[self handleTouch: touches withType: jngl::WindowImpl::Touch::ENDED];
}

- (void) touchesCancelled: (NSSet*) touches withEvent: (UIEvent*) event {
	[self handleTouch: touches withType: jngl::WindowImpl::Touch::ENDED];
}

- (void) touchesMoved: (NSSet*) touches withEvent: (UIEvent*) event {
	[self handleTouch: touches withType: jngl::WindowImpl::Touch::MOVED];
}

-(void) insertText: (NSString*) text {
	std::string c = text.UTF8String;
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
	if (p && !pause && jngl::pWindow) {
		if (const auto work = jngl::pWindow->getWork()) {
			work->onPauseEvent();
		}
	}
	if (!p) {
		needToResetFrameLimiter = true;
	}
	pause = p;
}

- (UIKeyboardType) keyboardType {
	if (jngl::getKeyboardType() == jngl::Numpad) {
		return UIKeyboardTypeNumberPad;
	}
	return UIKeyboardTypeDefault;
}

-(void) initControllers {
	assert(impl->appleControllers.empty());
	if ([GCController controllers].count > 0) {
		impl->shouldCallControllerChangedCallback = true;
	}

	NSNotificationCenter* center = [NSNotificationCenter defaultCenter];
	[center addObserverForName:GCControllerDidConnectNotification
	                    object:nil
	                     queue:nil
	                usingBlock:^(NSNotification* note) {
		              impl->shouldCallControllerChangedCallback = true;
	                }];
	[center addObserverForName:GCControllerDidDisconnectNotification
	                    object:nil
	                     queue:nil
	                usingBlock:^(NSNotification* note) {
		              impl->shouldCallControllerChangedCallback = true;
	                }];
}

@end

namespace jngl {

std::vector<std::shared_ptr<Controller>> getConnectedControllers() {
	const auto controllers = [GCController controllers];
	std::vector<std::shared_ptr<Controller>> ret;
	const auto impl = jngl::pWindow->getImpl();
	for (GCController* controller in controllers) {
		if (controller.extendedGamepad == nil) {
			continue; // Micro Gamepad (Siri Remote)
		}
		auto it = impl->appleControllers.find(controller);
		if (it == impl->appleControllers.end()) {
			it = impl->appleControllers.emplace(controller, std::make_shared<AppleController>(controller)).first;
		}
		ret.push_back(it->second);
	}
	return ret;
}

std::string canonical(const std::string& key) {
	// Since the other platforms use key as a filename, we need to make sure
	//   writeConfig("foo/./bar.txt", "foobar");
	// and
	//   readConfig("foo//bar.txt");
	// will refer to the same value. Of course there's even more to keep in mind, but ultimately
	// this should be fixed in the game then.
	std::string tmp = sanitizePath(key);
	if (tmp != key) {
		debug("WARNING: Canonicalized `");
		debug(key);
		debug("` to `");
		debug(tmp);
		debugLn("`.");
	}
	return tmp;
}

std::string readConfig(const std::string& key) {
	const auto defaults = [NSUserDefaults standardUserDefaults];
	NSString* const value = [defaults objectForKey:@(canonical(key).c_str())];
	if (!value) {
		return {};
	}
	return value.UTF8String;
}

void writeConfig(const std::string& key, const std::string& value) {
	const auto defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:@(value.c_str()) forKey:@(canonical(key).c_str())];
	const bool result = [defaults synchronize];
	if (!result) {
		jngl::debug("Couldn't write to config key: ");
		jngl::debugLn(key);
	}
}

}
