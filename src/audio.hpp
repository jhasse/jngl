// Copyright 2010-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

namespace jngl {

void checkAlError();
void pauseAudioDevice();
void resumeAudioDevice();

class Audio;

Audio& GetAudio();

} // namespace jngl
