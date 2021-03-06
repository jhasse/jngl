// Copyright 2018-2021 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::Vec2 class
/// @file
#pragma once

#if !defined(__has_include) || __has_include("boost/version.hpp")
#include <boost/version.hpp>
#endif
#include <iosfwd>

namespace jngl {

/// Two-dimensional vector
class Vec2 {
public:
	/// Null vector
	Vec2();

	/// Creates a vector {x, y}
	Vec2(double x, double y);

	/// x component
	double x = 0;

	/// y component
	double y = 0;

	/// Helper function to use with cereal or Boost.Serialization
	template <class Archive> void serialize(Archive& ar, const unsigned int) {
		ar(x, y);
	}
};

} // namespace jngl

/// Prints the vector like this: [x=…, y=…]
std::ostream& operator<<(std::ostream&, const jngl::Vec2&);

#if BOOST_VERSION >= 106200 || __has_include("boost/qvm/vec_traits.hpp")
#include <boost/qvm/vec_traits.hpp>

namespace boost::qvm {
template <> struct vec_traits<jngl::Vec2> {
	static int const dim = 2;
	using scalar_type = double;

	template <int I> static inline scalar_type& write_element(jngl::Vec2& v) {
		return (&v.x)[I];
	}
	template <int I> static inline scalar_type read_element(const jngl::Vec2& v) {
		return (&v.x)[I];
	}

	static inline scalar_type& write_element_idx(int i, jngl::Vec2& v) {
		return (&v.x)[i];
	}
	static inline scalar_type read_element_idx(int i, jngl::Vec2 const& v) {
		return (&v.x)[i];
	}
};
} // namespace boost::qvm

#include <boost/qvm/vec_operations.hpp>

using boost::qvm::operator+=;
using boost::qvm::operator*=;
using boost::qvm::operator/=;
using boost::qvm::operator-=;
using boost::qvm::operator==;
using boost::qvm::operator!=;
using boost::qvm::operator+;
using boost::qvm::operator-;
using boost::qvm::operator/;
using boost::qvm::operator*;
#endif
