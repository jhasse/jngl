// Copyright 2012-2019 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "dll.hpp"

#include <string>

namespace jngl {

float JNGLDLL_API getVolume();

/// Play an OGG audio file once
void JNGLDLL_API play(const std::string& filename);

/// Stop an OGG audio file if it's currently playing
void JNGLDLL_API stop(const std::string& filename);

/// \return whether \a filename is currently playing
bool JNGLDLL_API isPlaying(const std::string& filename);

/// Play an OGG audio file in a loop
void JNGLDLL_API loop(const std::string& filename);

bool JNGLDLL_API isOpenALInstalled();

void JNGLDLL_API setPlaybackSpeed(float speed);

void JNGLDLL_API setVolume(float volume);

} // namespace jngl
