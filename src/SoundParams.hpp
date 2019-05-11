// Copyright 2019 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#pragma once

#ifdef WEAK_LINKING_OPENAL
 #include "win32/openal.hpp"
#else
 #ifdef __APPLE__
  #include <OpenAL/al.h>
  #include <OpenAL/alc.h>
 #else
  #include <AL/al.h>
  #include <AL/alc.h>
 #endif
 #define OV_EXCLUDE_STATIC_CALLBACKS
 #include <vorbis/vorbisfile.h>
#endif

namespace jngl {

struct SoundParams {
	ALenum format;
	ALsizei freq;
};

} // namespace jngl
