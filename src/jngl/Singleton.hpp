// Copyright 2022 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::Singleton base class
/// \file
#pragma once

#include "window.hpp"

namespace jngl {

template <class T> class Singleton {
public:
	~Singleton() = default;
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;
	Singleton(Singleton&&) = delete;
	Singleton& operator=(Singleton&&) = delete;

	static T& handle() {
		if (!instance) {
			instance = new T;
			atExit([]() {
				delete instance;
				instance = nullptr;
			});
		}
		return *instance;
	}

protected:
	Singleton() = default;

private:
	static T* instance;
};

template <class T> T* Singleton<T>::instance = nullptr;

} // namespace jngl
