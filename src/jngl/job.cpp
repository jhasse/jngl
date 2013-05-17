/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "job.hpp"

#include "../windowptr.hpp"

namespace jngl {
	Job::~Job() {
	}

	void addJob(std::shared_ptr<Job> job) {
		return pWindow->addJob(job);
	}
}
