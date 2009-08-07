/*
Copyright (C) 2007-2009 Olli Hinkka

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#ifndef __WIZGLES__H__
#define __WIZGLES__H__

#include "GL/egltypes.h"

#ifdef __cplusplus
extern "C" {
#endif

NativeWindowType OS_CreateWindow();
unsigned int OS_GetTickCount(void);
void GLESOAL_Finalize( void );
void GLESOAL_SwapBufferCallback( void );
void GLESOAL_SetWindow    ( void* pNativeWindow  );
void GLESOAL_GetWindowSize( int* pWidth, int* pHeight );
void GLESOAL_WaitForDisplayAddressPatched( void );
void GLESOAL_SetDisplayAddress( const unsigned int DisplayBufferPhysicalAddress );
void GLESOAL_Sleep( unsigned long Milliseconds );
int GLESOAL_GetDisplayDirection(void);

#ifdef __cplusplus
}
#endif


#endif
