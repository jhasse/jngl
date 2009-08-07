//------------------------------------------------------------------------------
//
//	libogl interface
//------------------------------------------------------------------------------
#ifndef __libogl_h__
#define __libogl_h__

//------------------------------------------------------------------------------
//
//
//  interface between the OpenGL ES 1.1 lite and operating systems
//
//
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
    void OS_InitFakeOS( void );
    NativeWindowType OS_CreateWindow();
    unsigned int OS_GetTickCount( void );
    void OS_GetWindowSize( NativeWindowType hNativeWnd, int* width, int* height );
    void OS_Sleep( unsigned int milliseconds );


    typedef int GLESOALbool;

    typedef struct
    {
        unsigned int	VirtualAddressOf3DCore  ; // virtual address of the 3D core register

    	unsigned int	Memory1D_VirtualAddress ; // must be 8byte aligned, non-cacheable
    	unsigned int    Memory1D_PhysicalAddress; // must be 8byte aligned, non-cacheable
    	unsigned int	Memory1D_SizeInMbyte    ; // size (Mbyte)

    	unsigned int	Memory2D_VirtualAddress ; // must be 4Mbyte aligned, non-cacheable
    	unsigned int    Memory2D_PhysicalAddress; // must be 4Mbyte aligned, non-cacheable
    	unsigned int	Memory2D_SizeInMbyte    ; // size (Mbyte), must be multiple of 4
    } ___OAL_MEMORY_INFORMATION__;


    GLESOALbool GLESOAL_Initalize( ___OAL_MEMORY_INFORMATION__* pMemoryInfomation, int FSAAEnb);
    void GLESOAL_Finalize( void );
    void GLESOAL_SwapBufferCallback( void );
    void GLESOAL_SetWindow    ( void* pNativeWindow  );
    void GLESOAL_GetWindowSize( int* pWidth, int* pHeight );
    void GLESOAL_WaitForDisplayAddressPatched( void );
    void GLESOAL_SetDisplayAddress( const unsigned int DisplayBufferPhysicalAddress );
    void GLESOAL_Sleep( unsigned long Milliseconds );

#ifdef __cplusplus
}
#endif

#endif // ___libogl_h__
