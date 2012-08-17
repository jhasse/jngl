/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#include "work.hpp"
#include "window.hpp"

#include <functional>

namespace jngl {
	extern std::function<void()> mainInit;

	template<class T>
	class Main {
	public:
		Main(int width = getDesktopWidth(), int height = getDesktopHeight()) {
			mainInit = [=]() {
				jngl::showWindow("", width, height);
				T* t = new T;
				jngl::setWork(t);
			};
		}
	};
}