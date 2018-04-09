// Copyright 2012-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "dll.hpp"

#include <memory>

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
