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
	private:
		Window* ptr_;
	};

	extern WindowPointer pWindow;
}
