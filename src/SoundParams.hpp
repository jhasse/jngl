// Copyright 2019-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#pragma once

#include <AL/al.h>
#include <AL/alc.h>
#define OV_EXCLUDE_STATIC_CALLBACKS
#include <vorbis/vorbisfile.h>

namespace jngl {

struct SoundParams {
	ALenum format;
	ALsizei freq;
};

} // namespace jngl
