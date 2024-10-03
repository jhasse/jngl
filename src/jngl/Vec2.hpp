// Copyright 2018-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::Vec2 class
/// @file
#pragma once

#include <boost/qvm_lite.hpp>
#include <iosfwd>

#if __has_include(<format>) && (!defined(_LIBCPP_VERSION) || _LIBCPP_VERSION >= 170000)
#include <format>
#endif

namespace jngl {

/// Two-dimensional vector
///
/// JNGL doesn't provide any linear algebra functions itself but relies on the excellent
/// [Boost.QVM](https://www.boost.org/doc/libs/release/libs/qvm/index.html):
///
/// \code
/// #include <jngl/Vec2.hpp>
///
/// jngl::Vec2 v(3, 4);
/// double length = boost::qvm::mag(v);
/// assert(length == 5);
/// \endcode
///
/// Common math operators are overloaded:
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

	/// rotates the vector clock-wise by \a angle (radian)
	void rotate(float angle);

	/// Helper function to use with cereal or Boost.Serialization
	template <class Archive> void serialize(Archive& ar, const unsigned int) {
		ar(x, y);
	}
};

/// Prints the vector like this: [x=…, y=…]
std::ostream& operator<<(std::ostream&, const Vec2&);

} // namespace jngl

#if __has_include(<format>) && (!defined(_LIBCPP_VERSION) || _LIBCPP_VERSION >= 170000)
template <> struct std::formatter<jngl::Vec2> {
	constexpr auto parse(std::format_parse_context& ctx) {
		return ctx.begin();
	}
	auto format(const jngl::Vec2& v, auto& ctx) const {
		return std::format_to(ctx.out(), "[x={}, y={}]", v.x, v.y);
	}
};
#endif

namespace boost::qvm {
template <> struct vec_traits<jngl::Vec2> {
	static int const dim = 2;
	using scalar_type = double;

	template <int I> static scalar_type& write_element(jngl::Vec2& v) {
		return (&v.x)[I];
	}
	template <int I> static scalar_type read_element(const jngl::Vec2& v) {
		return (&v.x)[I];
	}

	static scalar_type& write_element_idx(int i, jngl::Vec2& v) {
		return (&v.x)[i];
	}
	static scalar_type read_element_idx(int i, jngl::Vec2 const& v) {
		return (&v.x)[i];
	}
};
} // namespace boost::qvm

namespace jngl {
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
} // namespace jngl
