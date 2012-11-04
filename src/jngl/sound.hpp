/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#ifndef _MSC_VER
#pragma GCC visibility push(default)
#endif
namespace jngl {
	float getVolume();

	void play(const std::string& filename);

	void stop(const std::string& filename);

	bool isPlaying(const std::string& filename);

	bool isOpenALInstalled();

	void setPlaybackSpeed(float speed);

	void setVolume(float volume);
}
#ifndef _MSC_VER
#pragma GCC visibility pop
#endif