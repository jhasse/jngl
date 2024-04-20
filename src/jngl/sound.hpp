// Copyright 2012-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Sound related functions
/// @file
#pragma once

#include "Finally.hpp"

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
///
/// When playing a SoundFile multiple times (i.e. calling jngl::play multiple times), this will
/// return true if at least one SoundFile is still playing (i.e. hasn't finished and wasn't
/// stopped).
///
/// \note Unaffected by jngl::pauseAudio
bool isPlaying(const std::string& filename);

/// Play an OGG audio file in a loop
///
/// If it's already playing, this function won't play it twice, but simply set it to loop and return
/// a pointer to the same SoundFile.
std::shared_ptr<SoundFile> loop(const std::string& filename);

/// Set global pitch in (0.0f, ∞]. Default is 1.0f
void setPlaybackSpeed(float speed);

/// Set global volume in [0, ∞]. Default is 1.0f
void setVolume(float volume);

/// Pauses the playback of all audio; destroying the returned Finally object will resume again
///
/// Note that this doesn't change the status of jngl::isPlaying as that only depends on the status
/// of the SoundFile.
///
/// Example where you want to pause all audio in a menu:
/// \code
/// class PauseMenu : public jngl::Work {
///     jngl::Finally paused;
///
/// public:
///     PauseMenu() : paused(jngl::pauseAudio()) {}
///     void step() override { /* ... */ }
///     void draw() const override { /* ... */ }
/// };
/// \endcode
///
/// Or if you want to toggle pausing audio using <kbd>Space</kbd>:
/// \code
/// class PauseExample : public jngl::Work {
///     std::optional<jngl::Finally> paused;
///
/// public:
///     void step() override {
///         if (jngl::keyPressed(jngl::key::Space)) {
///             if (paused) {
///                 paused = {}; // calls ~Finally() and resumes playback
///             } else {
///                 paused = jngl::pauseAudio();
///             }
///         }
///     }
///     void draw() const override { /* ... */ }
/// };
/// \endcode
[[nodiscard]] Finally pauseAudio();

} // namespace jngl
