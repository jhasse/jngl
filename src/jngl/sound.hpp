// Copyright 2012-2022 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Sound related functions
/// @file
#pragma once

#include <memory>
#include <string>

namespace jngl {

class SoundFile;

float getVolume();

/// Play an OGG audio file once
///
/// Might block if the file hasn't been played before. To avoid that use jngl::load before.
void play(const std::string& filename);

/// Stop an OGG audio file if it's currently playing
void stop(const std::string& filename);

/// \return whether \a filename is currently playing
bool isPlaying(const std::string& filename);

/// Play an OGG audio file in a loop
///
/// If it's already playing, this function won't play it twice, but simply set it to loop and return
/// a pointer to the same SoundFile.
std::shared_ptr<SoundFile> loop(const std::string& filename);

void setPlaybackSpeed(float speed);

void setVolume(float volume);

} // namespace jngl
