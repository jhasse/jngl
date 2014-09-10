/*
Copyright 2014 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#include "dll.hpp"

namespace jngl {
    double JNGLDLL_API getTime();

    void JNGLDLL_API sleep(int milliseconds);
}

#ifndef _MSC_VER
#pragma GCC visibility pop
#endif
