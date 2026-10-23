// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#pragma once

#include "log.hpp"

#include <chrono>
#include <cstdlib>
#include <string>

namespace jngl::internal {

/// Returns true when JNGL_STARTUP_PROFILE is set to a non-zero value.
inline bool startupProfilingEnabled() {
	static const bool enabled = [] {
		const char* env = std::getenv("JNGL_STARTUP_PROFILE");
		return env && std::string(env) != "0";
	}();
	return enabled;
}

/// Logs how long a scope took when startup profiling is enabled (see startupProfilingEnabled()).
class StartupProfiler {
public:
	explicit StartupProfiler(std::string label)
	: label_(std::move(label)), start_(std::chrono::steady_clock::now()),
	  enabled_(startupProfilingEnabled()) {}

	~StartupProfiler() {
		if (!enabled_ || label_.empty()) {
			return;
		}
		const double ms = std::chrono::duration<double, std::milli>(
		                      std::chrono::steady_clock::now() - start_)
		                      .count();
		info("startup: {} took {:.1f} ms", label_, ms);
	}

private:
	std::string label_;
	std::chrono::steady_clock::time_point start_;
	bool enabled_;
};

} // namespace jngl::internal
