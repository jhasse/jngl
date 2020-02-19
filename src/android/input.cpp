// Copyright 2019 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/input.hpp"

#include "../windowptr.hpp"
#include "windowimpl.hpp"

namespace jngl {

std::vector<std::shared_ptr<Controller>> getConnectedControllers() {
	return pWindow->getImpl()->getConnectedControllers();
}

} // namespace jngl
