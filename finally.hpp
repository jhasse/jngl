/*
Copyright (C) 2007  Jan Niklas Hasse <jhasse@gmail.com>

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

#ifndef __FINALLY_HPP__
#define __FINALLY_HPP__

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

class Finally : boost::noncopyable
{
public:
	Finally(boost::function<void()> functionToCall);
	~Finally();
private:
	boost::function<void()> functionToCall_;
};

#endif
