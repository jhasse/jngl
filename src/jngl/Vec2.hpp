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
///
/// JNGL doesn't provide any linear algebra functions but relies on the excelent
/// [Boost.QVM](https://www.boost.org/doc/libs/release/libs/qvm/index.html):
///
/// \code
/// #include <boost/qvm/vec_operations.hpp>
///
/// jngl::Vec2 v(3, 4);
/// double length = boost::qvm::mag(v);
/// assert(length == 5);
/// \endcode
///
/// If the header `boost/qvm/vec_traits.hpp` is available, common math operators will also be
/// brought into the global namespace:
///
/// \code
/// jngl::Vec2 v(3, 4);
/// v *= 2.0;
/// assert(std::lround(v.x) == 6);
/// \endcode
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

	/// returns true if both x and y are exactly 0
	bool isNull() const;

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
