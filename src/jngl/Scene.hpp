// Copyright 2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::Scene class and related functions
/// @file
#pragma once

#include "work.hpp"

namespace jngl {

/// Because it's more intuitive, jngl::Scene is the new name for jngl::Work. The latter will be
/// deprecated in the future.
using Scene = Work;

/// Returns the currently active Scene
inline std::shared_ptr<Scene> getActiveScene() {
	return getWork();
}

/// Sets the passed Scene to be active in the main loop for the next frame
void setScene(std::shared_ptr<Scene>);

/// The same as setScene(std::shared_ptr<Scene>) but creates the Scene for you
template <class T, class... Args> T& setScene(Args&&... args) {
	auto shared = std::make_shared<T>(std::forward<Args>(args)...);
	auto& rtn = *shared;
	setScene(std::move(shared));
	return rtn;
}

/// Returns the Scene that will be active in the next frame. If you haven't called setScene in this
/// frame yet, it will return nullptr.
///
/// If you want to undo a scene change, use `setScene(getActiveScene())`.
std::shared_ptr<Scene> getNextScene();

} // namespace jngl
