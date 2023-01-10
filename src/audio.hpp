// Copyright 2010-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "audio/mixer.hpp"

#include <memory>

namespace jngl {

void checkAlError();
void pauseAudioDevice();
void resumeAudioDevice();

class Audio;

Audio& GetAudio();
std::shared_ptr<psemek::audio::mixer> getMixer();

} // namespace jngl
