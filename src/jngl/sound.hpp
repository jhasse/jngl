// Copyright 2012-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include <string>

namespace jngl {

float getVolume();

/// Play an OGG audio file once
void play(const std::string& filename);

/// Stop an OGG audio file if it's currently playing
void stop(const std::string& filename);

/// \return whether \a filename is currently playing
bool isPlaying(const std::string& filename);

/// Play an OGG audio file in a loop
void loop(const std::string& filename);

bool isOpenALInstalled();

void setPlaybackSpeed(float speed);

void setVolume(float volume);

} // namespace jngl
