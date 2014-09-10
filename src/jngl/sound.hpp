/*
Copyright 2012-2014 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#include "dll.hpp"

#ifndef _MSC_VER
#pragma GCC visibility push(default)
#endif

namespace jngl {
	float JNGLDLL_API getVolume();

	void JNGLDLL_API play(const std::string& filename);

	void JNGLDLL_API stop(const std::string& filename);

	bool JNGLDLL_API isPlaying(const std::string& filename);

	bool JNGLDLL_API isOpenALInstalled();

	void JNGLDLL_API setPlaybackSpeed(float speed);

	void JNGLDLL_API setVolume(float volume);
}
#ifndef _MSC_VER
#pragma GCC visibility pop
#endif
