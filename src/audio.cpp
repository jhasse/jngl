// Copyright 2009-2019 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "audio.hpp"

#include "Sound.hpp"
#include "jngl.hpp"

#ifdef WEAK_LINKING_OPENAL
#include "win32/openal.hpp"
#endif

namespace jngl {

	bool isOpenALInstalled() {
#ifdef WEAK_LINKING_OPENAL
		try {
			GetAudio();
		} catch(WeakLinkingError& e) {
			debug(e.what()); debug("\n");
			return false;
		}
#endif
		return true;
	}

	float getVolume() {
		return Sound::masterVolume;
	}
} // namespace jngl
