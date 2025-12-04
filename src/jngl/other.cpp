// Copyright 2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "other.hpp"

namespace jngl {

namespace {

std::vector<std::string> gArgs;

} // namespace

std::vector<std::string> getArgs() {
	return gArgs;
}

namespace internal {

void setArgs(std::vector<std::string> args) {
	gArgs = std::move(args);
}

} // namespace internal

} // namespace jngl
