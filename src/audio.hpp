// Copyright 2010-2019 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include <string>

namespace jngl {

void LoadSound(const std::string&);
void checkAlError();
void pauseAudioDevice();
void resumeAudioDevice();

class Audio;

Audio& GetAudio();

} // namespace jngl
