/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#ifndef NDEBUG
#include <iostream>
#endif

template<class T>
void Debug(const T& text)
{
#ifndef NDEBUG
	std::cout << text << std::flush;
#endif
}
