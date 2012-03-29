#import "JNGLView.h"

#include "jngl.hpp"
#include "windowptr.hpp"

@implementation JNGLView

+ (Class) layerClass
{
	return [CAEAGLLayer class];
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
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
		
		int width = CGRectGetWidth(frame);
		int height = CGRectGetHeight(frame);
		glViewport(0, 0, width, height);
		
		jngl::ShowWindow("", width, height);
    }
    return self;
}

- (void) drawView
{
	glClearColor(1.0f, 1.0f, 1.0f, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	jngl::Translate(0, 480);
	jngl::Rotate(-90);
	
	jngl::pWindow->draw();
	
	[context presentRenderbuffer:GL_RENDERBUFFER_OES];
}

@end
