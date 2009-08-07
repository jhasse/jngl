//------------------------------------------------------------------------------
//
//	Copyright (C) 2003 MagicEyes Digital Co., Ltd All Rights Reserved
//	MagicEyes Digital Co. Proprietary & Confidential
//
//	MAGICEYES INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//	AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//	BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR
//	FITNESS FOR A PARTICULAR PURPOSE.
//
//	Module     :
//	File       : egltypes.h
//	Description: 
//	Author     : Gamza(nik@mesdigital.com)
//	Export     :
//	History    :
//	   2006/04/24 Gamza first implementation
//------------------------------------------------------------------------------
#ifndef __egltypes_h__
#define __egltypes_h__

#if defined(WIN32) || defined(UNDER_CE)
#pragma warning(push,3)
#include <windows.h>
#pragma warning(pop)
#endif

typedef void *NativeDisplayType;
typedef void *NativeWindowType;
typedef void *NativePixmapType;

/*
** Types and resources
*/
typedef int EGLBoolean;
typedef int EGLint;
typedef void *EGLDisplay;
typedef void *EGLConfig;
typedef void *EGLSurface;
typedef void *EGLContext;

/*
** EGL and native handle values
*/
#define EGL_DEFAULT_DISPLAY ((NativeDisplayType)0)
#define EGL_NO_CONTEXT ((EGLContext)0)
#define EGL_NO_DISPLAY ((EGLDisplay)0)
#define EGL_NO_SURFACE ((EGLSurface)0)

#endif // ___egltypes_h__

