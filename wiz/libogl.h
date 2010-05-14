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

    typedef int GLESOALbool;

    typedef struct
    {	
    	// virtual address of the 3D core register
    	unsigned int VirtualAddressOf3DCore; 
    
        unsigned int    Memory1D_NumberOfHeaps; 
        struct
        {
            // must be 8byte aligned, non-cacheable
        	unsigned int	VirtualAddress ;
        	// must be 8byte aligned, non-cacheable
        	unsigned int    PhysicalAddress;
        	// size (Mbyte)
        	unsigned int	SizeInMbyte    ;
        } Memory1D_Heaps[32];
        
        unsigned int	Memory2D_NumberOfHeaps; 
        struct
        {
            // must be 4Mbyte aligned, non-cacheable
        	unsigned int	VirtualAddress ;
        	// must be 4Mbyte aligned, non-cacheable
        	unsigned int    PhysicalAddress;
        	// size (Mbyte), must be multiple of 4
        	unsigned int	SizeInMbyte    ;
        } Memory2D_Heaps[32];
    } ___OAL_MEMORY_INFORMATION__;

    void OS_InitFakeOS( void); 
    void* OS_CreateWindow(); 
    unsigned int OS_GetTickCount( void);
    void OS_GetWindowSize( void* hNativeWnd, int* width, int* height);
    void OS_Sleep( unsigned int milliseconds );


    int GLESOAL_GetDisplayDirection( void );
    GLESOALbool GLESOAL_Initalize( ___OAL_MEMORY_INFORMATION__* pMemoryInfomation, int FSAAEnb);
    void GLESOAL_Finalize( void );
    void GLESOAL_SwapBufferCallback( void );
    void GLESOAL_SetWindow( void* pNativeWindow  );
    void GLESOAL_GetWindowSize( int* pWidth, int* pHeight );
    void GLESOAL_WaitForDisplayAddressPatched( void );
    void GLESOAL_SetDisplayAddress(const unsigned int DisplayBufferPhysicalAddress);
    void GLESOAL_Sleep( unsigned long Milliseconds );

#ifdef __cplusplus
}
#endif

#endif // ___libogl_h__
