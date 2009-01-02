#pragma once

#include "window.hpp"

#include <boost/noncopyable.hpp>

namespace jngl
{
	class WindowPointer : boost::noncopyable {
	public:
		WindowPointer();
		Window* operator->() const;
		operator bool() const;
		void Delete();
		void Set(Window*);
		void ThrowIfNull() const;
	private:
		Window* ptr_;
	};

	extern WindowPointer pWindow;
}
