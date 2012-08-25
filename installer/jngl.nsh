!macro Header
	SetOutPath $INSTDIR\include
	File ..\src\jngl.hpp
	SetOutPath $INSTDIR\include\jngl
	File ..\src\jngl\sprite.hpp
	File ..\src\jngl\types.hpp
	File ..\src\jngl\window.hpp
	File ..\src\jngl\work.hpp
!macroend