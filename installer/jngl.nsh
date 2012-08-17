!macro Header
	SetOutPath $INSTDIR\include
	File ..\src\jngl.hpp
	SetOutPath $INSTDIR\include\jngl
	File ..\src\jngl\sprite.hpp
	File ..\src\jngl\types.hpp
!macroend