// Copyright 2018-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "opengl.hpp"

#include "jngl/App.hpp"

#include <boost/qvm/map_vec_mat.hpp>
#include <boost/qvm/mat_operations3.hpp>
#include <boost/qvm/vec.hpp>

namespace opengl {

boost::qvm::mat<float, 3, 3> modelview;
boost::qvm::mat<float, 4, 4> projection;
GLuint vaoStream;
GLuint vboStream;

void translate(float x, float y) {
	modelview *= boost::qvm::translation_mat(boost::qvm::vec<float, 2>{{ x, y }});
}

void scale(const float x, const float y) {
	modelview *= boost::qvm::diag_mat(boost::qvm::vec<float, 3>{{ x, y, 1 }});
}

GLuint genAndBindTexture() {
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	try {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		                jngl::App::instance().isPixelArt() ? GL_NEAREST : GL_LINEAR);
	} catch (std::runtime_error&) {
		// TODO: Don't ignore an exception
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	return texture;
}

} // namespace opengl
