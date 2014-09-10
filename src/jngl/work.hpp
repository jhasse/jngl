/*
Copyright 2012-2014 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#include <memory>

#include "dll.hpp"

#ifndef _MSC_VER
#pragma GCC visibility push(default)
#endif

namespace jngl {
	class JNGLDLL_API Work {
	public:
		virtual void step() = 0;
		virtual void draw() const = 0;
		virtual void onQuitEvent();
		virtual void onLoad();
		virtual ~Work();
	};

	std::shared_ptr<Work> JNGLDLL_API getWork();

	void JNGLDLL_API setWork(std::shared_ptr<Work> work);

	void JNGLDLL_API setWork(Work*);

	void JNGLDLL_API resetFrameLimiter();
}
#ifndef _MSC_VER
#pragma GCC visibility pop
#endif
