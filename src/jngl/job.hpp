/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#include <boost/shared_ptr.hpp>

#ifndef _MSC_VER
#pragma GCC visibility push(default)
#endif
namespace jngl {
	class Job {
	public:
		virtual ~Job();
		virtual void step() = 0;
		virtual void draw() const = 0;
	};

	void addJob(boost::shared_ptr<Job> job);
}
#ifndef _MSC_VER
#pragma GCC visibility pop
#endif