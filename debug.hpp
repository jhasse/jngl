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
