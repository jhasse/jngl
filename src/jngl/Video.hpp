// Copyright 2018-2019 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "sprite.hpp"

#include <memory>
#include <string>

namespace jngl {

/// Ogg Theory video file
///
/// Example:
/// \code
/// #include <jngl.hpp>
///
/// JNGL_MAIN_BEGIN {
/// 	const auto filename = jngl::getArgs().at(0);
/// 	jngl::Video video(filename);
/// 	jngl::showWindow(filename, video.getWidth(), video.getHeight());
/// 	while (jngl::running()) {
/// 		jngl::updateInput();
/// 		video.draw();
/// 		jngl::swapBuffers();
/// 	}
/// } JNGL_MAIN_END
/// \endcode
class Video {
public:
	Video(const std::string& filename);
	~Video();

	void draw() const;

	/// Returns the actual pixel width of the video
	int getWidth() const;

	/// Returns the actual pixel height of the video
	int getHeight() const;

private:
	class Impl;

	std::unique_ptr<Impl> impl;
};

} // namespace jngl
