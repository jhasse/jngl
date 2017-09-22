// Copyright 2007-2017 Jan Niklas Hasse <jhasse@gmail.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include <boost/function.hpp>

class Finally {
public:
	Finally(boost::function<void()> functionToCall);
	Finally(const Finally&) = delete;
	Finally& operator=(const Finally&) = delete;
	~Finally();
private:
	boost::function<void()> functionToCall;
};
