// Copyright 2012-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#endif
#include <SDL.h>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

namespace jngl {

class SDL {
public:
	SDL(const SDL&) = delete;
	SDL& operator=(const SDL&) = delete;
	SDL(SDL&&) = delete;
	SDL& operator=(SDL&&) = delete;

	static void init();

private:
	SDL();
	~SDL();
	static void setHint(const char* name, bool);
};

} // namespace jngl
