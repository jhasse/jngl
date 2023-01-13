// Copyright 2009-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "jngl/Mat3.hpp"
#include "jngl/Mat4.hpp"

#ifdef IOS
	#include <OpenGLES/ES3/gl.h>
	#include <OpenGLES/ES3/glext.h>
	#include "ios/glew.h"
#else
	#ifdef ANDROID
		#include <EGL/egl.h>
		#include <GLES3/gl3.h>
		#include <GLES3/gl3ext.h>
	#else
		#if defined (JNGL_UWP) || defined (__EMSCRIPTEN__)
			#define GL_GLEXT_PROTOTYPES 1
			#include <GLES2/gl2.h>
			#include <GLES2/gl2ext.h>

			#define glGenVertexArrays glGenVertexArraysOES
			#define glBindVertexArray glBindVertexArrayOES
			#define glDeleteVertexArrays glDeleteVertexArraysOES
		#else
			#ifdef _MSC_VER
				#include <windows.h> // To avoid warnings about APIENTRY
			#endif
			#include <epoxy/gl.h>
		#endif
	#endif
#endif

#ifndef GL_BGR
#define GL_BGR 0x80e0
#endif

namespace opengl
{
	extern jngl::Mat3 modelview;
	extern jngl::Mat4 projection;

	/// A global VAO and VBO which are used with GL_STREAM_DRAW
	extern GLuint vaoStream;
	extern GLuint vboStream;

	void translate(float x, float y);
	void scale(float x, float y);

	/// Generates a textures, binds it to GL_TEXTURE_2D and sets some common parameters
	GLuint genAndBindTexture();

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
		using type = GLdouble;
#else
		const static GLenum constant = GL_FLOAT;
		using type = GLfloat;
#endif
	};

#ifdef GL_DOUBLE
	using CoordType = GLdouble;
#else
	using CoordType = GLfloat;
#endif
} // namespace opengl
