/*
Copyright 2007 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

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