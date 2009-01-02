#include "windowptr.hpp"

#include <stdexcept>
#include <cassert>

namespace jngl
{
	WindowPointer::WindowPointer() : ptr_(0)
	{
	}

	Window* WindowPointer::operator->() const
	{
		ThrowIfNull();
		return ptr_;
	}

	WindowPointer::operator bool() const
	{
		return ptr_ != 0;
	}

	void WindowPointer::Set(Window* ptr)
	{
		assert(ptr_ == 0);
		ptr_ = ptr;
	}

	void WindowPointer::Delete()
	{
		delete ptr_;
		ptr_ = 0;
	}

	void WindowPointer::ThrowIfNull() const
	{
		if(ptr_ == 0)
		{
			throw std::runtime_error("Window hasn't been created yet. Use jngl::ShowWindow.");
		}
	}
}
