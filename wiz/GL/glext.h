#ifndef __glext_h_
#define __glext_h_

#ifdef __cplusplus
extern "C" {
#endif

/*
** Copyright 1992-2003, Silicon Graphics, Inc.
** All Rights Reserved.
**
** This is UNPUBLISHED PROPRIETARY SOURCE CODE of Silicon Graphics, Inc.;
** the contents of this file may not be disclosed to third parties, copied or
** duplicated in any form, in whole or in part, without the prior written
** permission of Silicon Graphics, Inc.
**
** RESTRICTED RIGHTS LEGEND:
** Use, duplication or disclosure by the Government is subject to restrictions
** as set forth in subdivision (c)(1)(ii) of the Rights in Technical Data
** and Computer Software clause at DFARS 252.227-7013, and/or in similar or
** successor clauses in the FAR, DOD or NASA FAR Supplement. Unpublished -
** rights reserved under the Copyright Laws of the United States.
*/

#define GL_OES_query_matrix		    1


#define	GL_HARDWAREBUFFER_ARRAY_OES	0x9000

#define GL_HWBUFFER_SIZE_OES		0
#define GL_HWBUFFER_FORMAT_OES 		1	

// Fixed or Float(fixed:1, float:0) / Num of element( 1, 2, 3, 4, 5(?) ) / Format ( vctn:0, vnct:1, vtnc:2 )
#define GL_HWBUFFER_FV_OES			0x010
#define GL_HWBUFFER_XV_OES			0x110
#define GL_HWBUFFER_FVN_OES			0x021
#define GL_HWBUFFER_XVN_OES			0x121
#define GL_HWBUFFER_FVC_OES			0x020
#define GL_HWBUFFER_XVC_OES			0x120
//#define GL_HWBUFFER_FVCN_OES		6
//#define GL_HWBUFFER_XVCN_OES		7
#define GL_HWBUFFER_FVT_OES			0x022
#define GL_HWBUFFER_XVT_OES			0x122
//#define GL_HWBUFFER_FVTN_OES		10
//#define GL_HWBUFFER_XVTN_OES		11
//#define GL_HWBUFFER_FVCT_OES		12
//#define GL_HWBUFFER_XVCT_OES		13
#define GL_HWBUFFER_FVCTN_OES		0x040
#define GL_HWBUFFER_XVCTN_OES		0x140


/* OES_query_matrix */
GL_API GLbitfield GL_APIENTRY glQueryMatrixxOES(GLfixed *mantissa, GLint *exponent);

/* OES hardware buffer object */
GL_API void glGenHWBuffersOES(GLsizei n, GLuint *hwbuffers);
GL_API void glDeleteHWBuffersOES(GLsizei n, const GLuint *hwbuffers);
GL_API void glBindHWBufferOES(GLenum target, GLuint hwbuffer);
GL_API GLboolean glIsHWBufferOES(GLuint hwbuffer);
GL_API void glGetHWBufferParameterivOES(GLenum target, GLenum pname, GLint * params);
GL_API void glHWBufferDataOES(GLenum target, GLsizeiptr n, const GLvoid * data, GLenum format);
GL_API void glHWBufferSubDataOES(GLenum target, GLintptr offset_n, GLsizeiptr n, const GLvoid *data);

#ifndef LF1000
/* OES video texture */
GL_API void glTexImage2DFromVideoOES(const GLvoid *pY,const GLvoid *pCb,const GLvoid *pCr,
									 GLint clipx, GLint clipy, GLsizei clipwidth, GLsizei clipheight );

/* OES set color key */
GL_API void glTexColorKeyOES(GLboolean enable, GLshort colorkey_r5g6b5 );
#endif

#ifdef __cplusplus
}
#endif

#endif /* __glext_h_ */
