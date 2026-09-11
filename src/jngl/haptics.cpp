// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "haptics.hpp"

#if !defined(ANDROID) && !defined(IOS)

namespace jngl {

void hapticFeedback(HapticFeedback) {
	// Not implemented on this platform.
}

} // namespace jngl

#endif
