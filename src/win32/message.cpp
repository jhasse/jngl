/*
Copyright 2009 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/
#include <windows.h>
#include <string>

namespace jngl
{
	void errorMessage(const std::string& text)
	{
		MessageBox(NULL, text.c_str(), "Error", MB_OK | MB_ICONERROR);
	}
}
