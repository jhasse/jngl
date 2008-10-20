/*
Copyright 2007-2008 Jan Niklas Hasse <jhasse@gmail.com>

This file is part of jngl.

jngl is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

jngl is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with jngl.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "window.hpp"
#include <boost/assign/ptr_map_inserter.hpp>

namespace jngl
{
	void Window::Print(const std::string& text, const double xposition, const double yposition)
	{
		fonts_[fontSize_][fontName_].Print(xposition, yposition, text);
	}

	void Window::SetFont(const std::string& filename)
	{
		try
		{
			if(fonts_[fontSize_].find(fontName_) == fonts_[fontSize_].end()) // Only load font if it doesn't exist yet
			{
				boost::assign::ptr_map_insert(fonts_[fontSize_])(filename, filename.c_str(), fontSize_);
			}
		}
		catch(std::exception& e)
		{
			throw e;
		}
		fontName_ = filename;
	}

	void Window::SetFontByName(const std::string& name)
	{
		Window::SetFont(GetFontFileByName(name));
	}


	void Window::FontSize(const int size)
	{
		int oldSize = fontSize_;
		fontSize_ = size;
		try
		{
			SetFont(fontName_); // We changed the size we also need to reload the current font
		}
		catch(std::exception& e) // Someting went wrong ...
		{
			fontSize_ = oldSize; // ... so let's set fontSize_ back to the previous size
			throw e;
		}
	}
}
