/*
Copyright 2009 Jan Niklas Hasse <jhasse@gmail.com>

This file is part of JNGL.

JNGL is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

JNGL is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with JNGL.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "window.hpp"

#include <boost/noncopyable.hpp>

namespace jngl
{
	class WindowPointer : boost::noncopyable {
	public:
		WindowPointer();
		~WindowPointer();
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
