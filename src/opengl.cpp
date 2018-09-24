// Copyright 2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "opengl.hpp"

#include <boost/qvm/mat_operations3.hpp>
#include <boost/qvm/map_vec_mat.hpp>
#include <boost/qvm/vec.hpp>

namespace opengl {

boost::qvm::mat<float, 3, 3> modelview;
boost::qvm::mat<float, 4, 4> projection;

void translate(float x, float y) {
	modelview *= boost::qvm::translation_mat(boost::qvm::vec<float, 2>{x, y});
}

void scale(const float x, const float y) {
	modelview *= boost::qvm::diag_mat(boost::qvm::vec<float, 3>{x, y, 1});
}

} // namespace opengl
