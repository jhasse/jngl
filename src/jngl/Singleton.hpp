// Copyright 2022-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::Singleton base class
/// \file
#pragma once

#include "window.hpp"

namespace jngl {

/// Inherit from this class to create a singleton that will be destroyed when your games exits. You
/// should use this instead of global / static variables, because some platforms (Android) don't
/// destroy the process on exit and instead re-enter the main function.
///
/// \code
/// class Foo : public jngl::Singleton<Foo>
/// 	// ...
/// }
///
/// // somewhere else:
/// Foo::handle().bar();
/// \endcode
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
