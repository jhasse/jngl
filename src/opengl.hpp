/*
Copyright 2009-2014 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/


#pragma once

#ifdef WIZ
	#include <GLES/gl.h>
	#include <GLES/egl.h>
	#include <libogl.h>
#else
	#ifdef IOS
		#include <OpenGLES/ES1/gl.h>
		#include <OpenGLES/ES1/glext.h>
		#include "ios/glew.h"
	#else
	#ifdef ANDROID
		#include <EGL/egl.h>
		#include <GLES/gl.h>
		#define GL_GLEXT_PROTOTYPES
		#include <GLES/glext.h>
		#include "android/glew.h"
	#else
		#ifdef __APPLE__
			#include <OpenGL/gl.h>
			#include <OpenGL/gl3ext.h>
			#define glBindVertexArray glBindVertexArrayAPPLE
			#define glDeleteVertexArrays glDeleteVertexArraysAPPLE
			#define glGenVertexArrays glGenVertexArraysAPPLE
		#else
			#include <GL/glew.h>
		#endif
	#endif
	#endif
#endif

#ifndef GL_BGR
#define GL_BGR 0x80e0
#endif

namespace opengl
{
	inline void translate(float x, float y) { glTranslatef(x, y, 0); }
	inline void scale(float x, float y) { glScalef(x, y, 0); }
#ifdef GL_DOUBLE
	template<class T, class U>
	inline void translate(T x, U y) { glTranslated(static_cast<double>(x), static_cast<double>(y), 0); }
	template<class T, class U>
	inline void scale(T x, U y) { glScaled(static_cast<double>(x), static_cast<double>(y), 0); }
#else
	template<class T, class U>
	inline void translate(T x, U y) { translate(static_cast<float>(x), static_cast<float>(y)); }
	template<class T, class U>
	inline void scale(T x, U y) { scale(static_cast<float>(x), static_cast<float>(y)); }
#endif

	template<class T> struct Type {};

	template<>
	struct Type<float>
	{
		const static GLenum constant = GL_FLOAT;
	};

	template<>
	struct Type<double>
	{
#ifdef GL_DOUBLE
		const static GLenum constant = GL_DOUBLE;
		typedef GLdouble type;
#else
		const static GLenum constant = GL_FLOAT;
		typedef GLfloat type;
#endif
	};

#ifdef GL_DOUBLE
	typedef GLdouble CoordType;
#else
	typedef GLfloat CoordType;
#endif

	void BindArrayBuffer(GLuint);
}
