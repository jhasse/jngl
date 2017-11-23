// Copyright 2012-2017 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include <SDL.h>

namespace jngl {
	class SDL {
	public:
		static void init();
	private:
		SDL();
		~SDL();
	};
}
