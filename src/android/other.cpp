// Copyright 2023-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/other.hpp"

#ifdef JNGL_GPU_BACKEND
#include "../GpuBackend.hpp"
#endif

namespace jngl {

#ifdef JNGL_GPU_BACKEND
void setVerticalSync(const bool enabled) {
	gpuRenderer().setVerticalSync(enabled);
}

bool getVerticalSync() {
	return gpuRenderer().getVerticalSync();
}
#else
void setVerticalSync(bool) {
}

bool getVerticalSync() {
	return true;
}
#endif

} // namespace jngl
