// Copyright 2012-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include <memory>

#include "dll.hpp"

namespace jngl {
	class JNGLDLL_API Job {
	public:
		virtual ~Job();
		virtual void step() = 0;
		virtual void draw() const = 0;
	};

	void JNGLDLL_API addJob(std::shared_ptr<Job> job);
}
