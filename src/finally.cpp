/*
Copyright 2007-2015 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "finally.hpp"

Finally::Finally(boost::function<void()> functionToCall) : functionToCall(functionToCall) {
}

Finally::~Finally() {
	functionToCall();
}
