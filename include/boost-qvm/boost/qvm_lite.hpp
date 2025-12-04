// QVM single header distribution. Do not edit.
// Generated on Jan 19, 2025 from https://github.com/boostorg/qvm/tree/daabde8.

// Latest published versions:
//	https://raw.githubusercontent.com/boostorg/qvm/gh-pages/qvm.hpp
//	https://raw.githubusercontent.com/boostorg/qvm/gh-pages/qvm_lite.hpp

// Copyright 2008-2024 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_QVM_LITE_HPP_INCLUDED
#define BOOST_QVM_LITE_HPP_INCLUDED

// >>> #include <boost/qvm/lite.hpp>
// #line 5 "boost/qvm/lite.hpp"
// >>> #include <boost/qvm/error.hpp>
#ifndef BOOST_QVM_ERROR_HPP_INCLUDED
#define BOOST_QVM_ERROR_HPP_INCLUDED

// #line 8 "boost/qvm/error.hpp"
#include <exception>

namespace boost { namespace qvm {

struct
error:
    std::exception
    {
    char const *
    what() const throw()
        {
        return "Boost QVM error";
        }

    ~error() throw()
        {
        }
    };

struct zero_determinant_error: error { };
struct zero_magnitude_error: error { };

} }

#endif
// <<< #include <boost/qvm/error.hpp>
// #line 6 "boost/qvm/lite.hpp"
// >>> #include <boost/qvm/to_string.hpp>
#ifndef BOOST_QVM_TO_STRING_HPP_INCLUDED
#define BOOST_QVM_TO_STRING_HPP_INCLUDED

// #line 8 "boost/qvm/to_string.hpp"
#include <string>

#if __cplusplus >= 201103L

namespace boost { namespace qvm {

namespace
qvm_to_string_detail
    {
    using std::to_string;
    }

} }

#else

#include <sstream>

namespace boost { namespace qvm {

namespace
qvm_to_string_detail
    {
    template <class T>
    std::string
    to_string( T const & x )
        {
        std::stringstream s;
        s << x;
        return s.str();
        }
    }

} }

#endif

#endif
// <<< #include <boost/qvm/to_string.hpp>
// #line 7 "boost/qvm/lite.hpp"
// >>> #include <boost/qvm/mat_operations.hpp>
#ifndef BOOST_QVM_MAT_OPERATIONS_HPP_INCLUDED
#define BOOST_QVM_MAT_OPERATIONS_HPP_INCLUDED

// #line 9 "boost/qvm/mat_operations.hpp"
// >>> #include <boost/qvm/detail/mat_assign.hpp>
#ifndef BOOST_QVM_DETAIL_MAT_ASSIGN_HPP_INCLUDED
#define BOOST_QVM_DETAIL_MAT_ASSIGN_HPP_INCLUDED

// #line 8 "boost/qvm/detail/mat_assign.hpp"
// >>> #include <boost/qvm/gen/mat_assign2.hpp>
#ifndef BOOST_QVM_GEN_MAT_ASSIGN2_HPP_INCLUDED
#define BOOST_QVM_GEN_MAT_ASSIGN2_HPP_INCLUDED

// #line 9 "boost/qvm/gen/mat_assign2.hpp"
// >>> #include <boost/qvm/config.hpp>
#ifndef BOOST_QVM_CONFIG_HPP_INCLUDED
#define BOOST_QVM_CONFIG_HPP_INCLUDED

// #line 8 "boost/qvm/config.hpp"
#if defined( BOOST_STRICT_CONFIG ) || defined( BOOST_QVM_NO_WORKAROUNDS )
#   define BOOST_QVM_WORKAROUND( symbol, test ) 0
#else
#   define BOOST_QVM_WORKAROUND( symbol, test ) ((symbol) != 0 && ((symbol) test))
#endif

#define BOOST_QVM_CLANG 0
#if defined(__clang__)
#   undef BOOST_QVM_CLANG
#   define BOOST_QVM_CLANG (__clang_major__ * 100 + __clang_minor__)
#endif

#if BOOST_QVM_WORKAROUND( BOOST_QVM_CLANG, < 304 )
#   define BOOST_QVM_DEPRECATED(msg)
#elif defined(__GNUC__) || defined(__clang__)
#   define BOOST_QVM_DEPRECATED(msg) __attribute__((deprecated(msg)))
#elif defined(_MSC_VER) && _MSC_VER >= 1900
#   define BOOST_QVM_DEPRECATED(msg) [[deprecated(msg)]]
#else
#   define BOOST_QVM_DEPRECATED(msg)
#endif

#ifndef BOOST_QVM_FORCEINLINE
#   if defined(_MSC_VER)
#       define BOOST_QVM_FORCEINLINE __forceinline
#   elif defined(__GNUC__) && __GNUC__>3
#       define BOOST_QVM_FORCEINLINE inline __attribute__ ((always_inline))
#   else
#       define BOOST_QVM_FORCEINLINE inline
#   endif
#endif

#ifndef BOOST_QVM_INLINE
#   define BOOST_QVM_INLINE inline
#endif

#ifndef BOOST_QVM_INLINE_TRIVIAL
#   define BOOST_QVM_INLINE_TRIVIAL BOOST_QVM_FORCEINLINE
#endif

#ifndef BOOST_QVM_INLINE_CRITICAL
#   define BOOST_QVM_INLINE_CRITICAL BOOST_QVM_FORCEINLINE
#endif

#ifndef BOOST_QVM_INLINE_OPERATIONS
#   define BOOST_QVM_INLINE_OPERATIONS BOOST_QVM_INLINE
#endif

#ifndef BOOST_QVM_INLINE_RECURSION
#   define BOOST_QVM_INLINE_RECURSION BOOST_QVM_INLINE_OPERATIONS
#endif

#ifndef BOOST_QVM_CONSTEXPR
#   if __cplusplus >= 201703L
#       define BOOST_QVM_CONSTEXPR constexpr
#   else
#       define BOOST_QVM_CONSTEXPR
#   endif
#endif

#endif
// <<< #include <boost/qvm/config.hpp>
// #line 10 "boost/qvm/gen/mat_assign2.hpp"
// >>> #include <boost/qvm/enable_if.hpp>
#ifndef BOOST_QVM_ENABLE_IF_HPP_INCLUDED
#define BOOST_QVM_ENABLE_IF_HPP_INCLUDED

// #line 8 "boost/qvm/enable_if.hpp"
// Boost enable_if library

// Copyright 2003 (c) The Trustees of Indiana University.

//    Authors: Jaakko Jarvi (jajarvi at osl.iu.edu)
//             Jeremiah Willcock (jewillco at osl.iu.edu)
//             Andrew Lumsdaine (lums at osl.iu.edu)

namespace boost { namespace qvm {

  template<typename T, typename R=void>
  struct enable_if_has_type
  {
    typedef R type;
  };

  template <bool B, class T = void>
  struct enable_if_c {
    typedef T type;
  };

  template <class T>
  struct enable_if_c<false, T> {};

  template <class Cond, class T = void>
  struct enable_if : public enable_if_c<Cond::value, T> {};

  template <bool B, class T>
  struct lazy_enable_if_c {
    typedef typename T::type type;
  };

  template <class T>
  struct lazy_enable_if_c<false, T> {};

  template <class Cond, class T>
  struct lazy_enable_if : public lazy_enable_if_c<Cond::value, T> {};


  template <bool B, class T = void>
  struct disable_if_c {
    typedef T type;
  };

  template <class T>
  struct disable_if_c<true, T> {};

  template <class Cond, class T = void>
  struct disable_if : public disable_if_c<Cond::value, T> {};

  template <bool B, class T>
  struct lazy_disable_if_c {
    typedef typename T::type type;
  };

  template <class T>
  struct lazy_disable_if_c<true, T> {};

  template <class Cond, class T>
  struct lazy_disable_if : public lazy_disable_if_c<Cond::value, T> {};

////////////////////////////////////////////////

  // The types below are a copy of the original types above, to workaround MSVC-12 bugs.

  template<typename T, typename R=void>
  struct enable_if_has_type2
  {
    typedef R type;
  };

  template <bool B, class T = void>
  struct enable_if_c2 {
    typedef T type;
  };

  template <class T>
  struct enable_if_c2<false, T> {};

  template <class Cond, class T = void>
  struct enable_if2 : public enable_if_c2<Cond::value, T> {};

  template <bool B, class T>
  struct lazy_enable_if_c2 {
    typedef typename T::type type;
  };

  template <class T>
  struct lazy_enable_if_c2<false, T> {};

  template <class Cond, class T>
  struct lazy_enable_if2 : public lazy_enable_if_c2<Cond::value, T> {};


  template <bool B, class T = void>
  struct disable_if_c2 {
    typedef T type;
  };

  template <class T>
  struct disable_if_c2<true, T> {};

  template <class Cond, class T = void>
  struct disable_if2 : public disable_if_c2<Cond::value, T> {};

  template <bool B, class T>
  struct lazy_disable_if_c2 {
    typedef typename T::type type;
  };

  template <class T>
  struct lazy_disable_if_c2<true, T> {};

  template <class Cond, class T>
  struct lazy_disable_if2 : public lazy_disable_if_c2<Cond::value, T> {};

} }

#endif
// <<< #include <boost/qvm/enable_if.hpp>
// #line 11 "boost/qvm/gen/mat_assign2.hpp"
// >>> #include <boost/qvm/mat_traits.hpp>
#ifndef BOOST_QVM_TRAITS_HPP_INCLUDED
#define BOOST_QVM_TRAITS_HPP_INCLUDED

// #line 8 "boost/qvm/mat_traits.hpp"
// >>> #include <boost/qvm/is_scalar.hpp>
#ifndef BOOST_QVM_IS_SCALAR_HPP_INCLUDED
#define BOOST_QVM_IS_SCALAR_HPP_INCLUDED

// #line 8 "boost/qvm/is_scalar.hpp"
namespace boost { namespace qvm {

template <class T>
struct
is_scalar
    {
    static bool const value=false;
    };
template <class T>
struct
is_scalar<T const>:
    is_scalar<T>
    {
    };
template <> struct is_scalar<signed char> { static bool const value=true; };
template <> struct is_scalar<unsigned char> { static bool const value=true; };
template <> struct is_scalar<signed short> { static bool const value=true; };
template <> struct is_scalar<unsigned short> { static bool const value=true; };
template <> struct is_scalar<signed int> { static bool const value=true; };
template <> struct is_scalar<unsigned int> { static bool const value=true; };
template <> struct is_scalar<signed long> { static bool const value=true; };
template <> struct is_scalar<unsigned long> { static bool const value=true; };
template <> struct is_scalar<signed long long> { static bool const value=true; };
template <> struct is_scalar<unsigned long long> { static bool const value=true; };
template <> struct is_scalar<float> { static bool const value=true; };
template <> struct is_scalar<double> { static bool const value=true; };
template <> struct is_scalar<long double> { static bool const value=true; };
} }

#endif
// <<< #include <boost/qvm/is_scalar.hpp>
// #line 9 "boost/qvm/mat_traits.hpp"
// #include <boost/qvm/enable_if.hpp> // Expanded at line 177
// #include <boost/qvm/config.hpp> // Expanded at line 109

namespace boost { namespace qvm {

template <class M>
struct
mat_traits
    {
    static int const rows=0;
    static int const cols=0;
    typedef void scalar_type;
    };

template <class T>
struct
is_mat
    {
    static bool const value = is_scalar<typename mat_traits<T>::scalar_type>::value && mat_traits<T>::rows>0 && mat_traits<T>::cols>0;
    };

namespace
qvm_detail
    {
    template <class T, T>
    struct
    mtr_dispatch_yes
        {
        char x, y;
        };
    }

template <class T>
class
mat_write_element_ref
    {
    template <class U>
    static qvm_detail::mtr_dispatch_yes<typename mat_traits<U>::scalar_type & (*)( U & ), &mat_traits<U>::template write_element<0,0> > check(int);

    template <class>
    static char check(long);

    public:

    static bool const value = sizeof(check<T>(0)) > 1;
    };

template <int R, int C, class M>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
typename enable_if_c<
    mat_write_element_ref<M>::value,
    void>::type
write_mat_element( M & m, typename mat_traits<M>::scalar_type s )
    {
    mat_traits<M>::template write_element<R,C>(m) = s;
    }

template <int R, int C, class M>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
typename enable_if_c<
    !mat_write_element_ref<M>::value,
    void>::type
write_mat_element( M & m, typename mat_traits<M>::scalar_type s )
    {
    mat_traits<M>::template write_element<R,C>(m, s);
    }

template <class M>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
typename enable_if_c<
    mat_write_element_ref<M>::value,
    void>::type
write_mat_element_idx( int r, int c, M & m, typename mat_traits<M>::scalar_type s )
    {
    mat_traits<M>::write_element_idx(r, c, m) = s;
    }

template <class M>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
typename enable_if_c<
    !mat_write_element_ref<M>::value,
    void>::type
write_mat_element_idx( int r, int c, M & m, typename mat_traits<M>::scalar_type s )
    {
    mat_traits<M>::write_element_idx(r, c, m, s);
    }

} }

#endif
// <<< #include <boost/qvm/mat_traits.hpp>
// #line 12 "boost/qvm/gen/mat_assign2.hpp"

namespace boost { namespace qvm {

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==2 && mat_traits<B>::rows==2 &&
    mat_traits<A>::cols==2 && mat_traits<B>::cols==2,
    A &>::type
assign( A & a, B const & b )
    {
    write_mat_element<0,0>(a,mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<0,1>(a,mat_traits<B>::template read_element<0,1>(b));
    write_mat_element<1,0>(a,mat_traits<B>::template read_element<1,0>(b));
    write_mat_element<1,1>(a,mat_traits<B>::template read_element<1,1>(b));
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::assign;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct assign_mm_defined;

    template <>
    struct
    assign_mm_defined<2,2>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==2 && mat_traits<B>::rows==2 &&
    mat_traits<A>::cols==1 && mat_traits<B>::cols==1,
    A &>::type
assign( A & a, B const & b )
    {
    write_mat_element<0,0>(a,mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<1,0>(a,mat_traits<B>::template read_element<1,0>(b));
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::assign;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct assign_mm_defined;

    template <>
    struct
    assign_mm_defined<2,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==1 && mat_traits<B>::rows==1 &&
    mat_traits<A>::cols==2 && mat_traits<B>::cols==2,
    A &>::type
assign( A & a, B const & b )
    {
    write_mat_element<0,0>(a,mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<0,1>(a,mat_traits<B>::template read_element<0,1>(b));
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::assign;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct assign_mm_defined;

    template <>
    struct
    assign_mm_defined<1,2>
        {
        static bool const value=true;
        };
    }

} }

#endif
// <<< #include <boost/qvm/gen/mat_assign2.hpp>
// #line 9 "boost/qvm/detail/mat_assign.hpp"
// >>> #include <boost/qvm/gen/mat_assign3.hpp>
#ifndef BOOST_QVM_GEN_MAT_ASSIGN3_HPP_INCLUDED
#define BOOST_QVM_GEN_MAT_ASSIGN3_HPP_INCLUDED

// #line 9 "boost/qvm/gen/mat_assign3.hpp"
// #include <boost/qvm/config.hpp> // Expanded at line 109
// #include <boost/qvm/enable_if.hpp> // Expanded at line 177
// #include <boost/qvm/mat_traits.hpp> // Expanded at line 303

namespace boost { namespace qvm {

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==3 && mat_traits<B>::rows==3 &&
    mat_traits<A>::cols==3 && mat_traits<B>::cols==3,
    A &>::type
assign( A & a, B const & b )
    {
    write_mat_element<0,0>(a,mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<0,1>(a,mat_traits<B>::template read_element<0,1>(b));
    write_mat_element<0,2>(a,mat_traits<B>::template read_element<0,2>(b));
    write_mat_element<1,0>(a,mat_traits<B>::template read_element<1,0>(b));
    write_mat_element<1,1>(a,mat_traits<B>::template read_element<1,1>(b));
    write_mat_element<1,2>(a,mat_traits<B>::template read_element<1,2>(b));
    write_mat_element<2,0>(a,mat_traits<B>::template read_element<2,0>(b));
    write_mat_element<2,1>(a,mat_traits<B>::template read_element<2,1>(b));
    write_mat_element<2,2>(a,mat_traits<B>::template read_element<2,2>(b));
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::assign;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct assign_mm_defined;

    template <>
    struct
    assign_mm_defined<3,3>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==3 && mat_traits<B>::rows==3 &&
    mat_traits<A>::cols==1 && mat_traits<B>::cols==1,
    A &>::type
assign( A & a, B const & b )
    {
    write_mat_element<0,0>(a,mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<1,0>(a,mat_traits<B>::template read_element<1,0>(b));
    write_mat_element<2,0>(a,mat_traits<B>::template read_element<2,0>(b));
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::assign;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct assign_mm_defined;

    template <>
    struct
    assign_mm_defined<3,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==1 && mat_traits<B>::rows==1 &&
    mat_traits<A>::cols==3 && mat_traits<B>::cols==3,
    A &>::type
assign( A & a, B const & b )
    {
    write_mat_element<0,0>(a,mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<0,1>(a,mat_traits<B>::template read_element<0,1>(b));
    write_mat_element<0,2>(a,mat_traits<B>::template read_element<0,2>(b));
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::assign;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct assign_mm_defined;

    template <>
    struct
    assign_mm_defined<1,3>
        {
        static bool const value=true;
        };
    }

} }

#endif
// <<< #include <boost/qvm/gen/mat_assign3.hpp>
// #line 10 "boost/qvm/detail/mat_assign.hpp"
// >>> #include <boost/qvm/gen/mat_assign4.hpp>
#ifndef BOOST_QVM_GEN_MAT_ASSIGN4_HPP_INCLUDED
#define BOOST_QVM_GEN_MAT_ASSIGN4_HPP_INCLUDED

// #line 9 "boost/qvm/gen/mat_assign4.hpp"
// #include <boost/qvm/config.hpp> // Expanded at line 109
// #include <boost/qvm/enable_if.hpp> // Expanded at line 177
// #include <boost/qvm/mat_traits.hpp> // Expanded at line 303

namespace boost { namespace qvm {

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==4 && mat_traits<B>::rows==4 &&
    mat_traits<A>::cols==4 && mat_traits<B>::cols==4,
    A &>::type
assign( A & a, B const & b )
    {
    write_mat_element<0,0>(a,mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<0,1>(a,mat_traits<B>::template read_element<0,1>(b));
    write_mat_element<0,2>(a,mat_traits<B>::template read_element<0,2>(b));
    write_mat_element<0,3>(a,mat_traits<B>::template read_element<0,3>(b));
    write_mat_element<1,0>(a,mat_traits<B>::template read_element<1,0>(b));
    write_mat_element<1,1>(a,mat_traits<B>::template read_element<1,1>(b));
    write_mat_element<1,2>(a,mat_traits<B>::template read_element<1,2>(b));
    write_mat_element<1,3>(a,mat_traits<B>::template read_element<1,3>(b));
    write_mat_element<2,0>(a,mat_traits<B>::template read_element<2,0>(b));
    write_mat_element<2,1>(a,mat_traits<B>::template read_element<2,1>(b));
    write_mat_element<2,2>(a,mat_traits<B>::template read_element<2,2>(b));
    write_mat_element<2,3>(a,mat_traits<B>::template read_element<2,3>(b));
    write_mat_element<3,0>(a,mat_traits<B>::template read_element<3,0>(b));
    write_mat_element<3,1>(a,mat_traits<B>::template read_element<3,1>(b));
    write_mat_element<3,2>(a,mat_traits<B>::template read_element<3,2>(b));
    write_mat_element<3,3>(a,mat_traits<B>::template read_element<3,3>(b));
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::assign;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct assign_mm_defined;

    template <>
    struct
    assign_mm_defined<4,4>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==4 && mat_traits<B>::rows==4 &&
    mat_traits<A>::cols==1 && mat_traits<B>::cols==1,
    A &>::type
assign( A & a, B const & b )
    {
    write_mat_element<0,0>(a,mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<1,0>(a,mat_traits<B>::template read_element<1,0>(b));
    write_mat_element<2,0>(a,mat_traits<B>::template read_element<2,0>(b));
    write_mat_element<3,0>(a,mat_traits<B>::template read_element<3,0>(b));
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::assign;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct assign_mm_defined;

    template <>
    struct
    assign_mm_defined<4,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==1 && mat_traits<B>::rows==1 &&
    mat_traits<A>::cols==4 && mat_traits<B>::cols==4,
    A &>::type
assign( A & a, B const & b )
    {
    write_mat_element<0,0>(a,mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<0,1>(a,mat_traits<B>::template read_element<0,1>(b));
    write_mat_element<0,2>(a,mat_traits<B>::template read_element<0,2>(b));
    write_mat_element<0,3>(a,mat_traits<B>::template read_element<0,3>(b));
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::assign;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct assign_mm_defined;

    template <>
    struct
    assign_mm_defined<1,4>
        {
        static bool const value=true;
        };
    }

} }

#endif
// <<< #include <boost/qvm/gen/mat_assign4.hpp>
// #line 11 "boost/qvm/detail/mat_assign.hpp"

namespace boost { namespace qvm {

namespace
qvm_detail
    {
    template <int M,int N>
    struct
    assign_mm_defined
        {
        static bool const value=false;
        };

    template <int I,int N>
    struct
    copy_matrix_elements
        {
        template <class A,class B>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        typename enable_if_c<
            mat_write_element_ref<A>::value,
            void>::type
        f( A & a, B const & b )
            {
            mat_traits<A>::template write_element<I/mat_traits<A>::cols,I%mat_traits<A>::cols>(a) =
                mat_traits<B>::template read_element<I/mat_traits<B>::cols,I%mat_traits<B>::cols>(b);
            copy_matrix_elements<I+1,N>::f(a,b);
            }

        template <class A,class B>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        typename enable_if_c<
            !mat_write_element_ref<A>::value,
            void>::type
        f( A & a, B const & b )
            {
            mat_traits<A>::template write_element<I/mat_traits<A>::cols,I%mat_traits<A>::cols>(a,
                mat_traits<B>::template read_element<I/mat_traits<B>::cols,I%mat_traits<B>::cols>(b));
            copy_matrix_elements<I+1,N>::f(a,b);
            }
        };

    template <int N>
    struct
    copy_matrix_elements<N,N>
        {
        template <class A,class B>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        void
        f( A &, B const & )
            {
            }
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
typename enable_if_c<
    is_mat<A>::value && is_mat<B>::value &&
    mat_traits<A>::rows==mat_traits<B>::rows &&
    mat_traits<A>::cols==mat_traits<B>::cols &&
    !qvm_detail::assign_mm_defined<mat_traits<A>::rows,mat_traits<A>::cols>::value,
    A &>::type
assign( A & a, B const & b )
    {
    qvm_detail::copy_matrix_elements<0,mat_traits<A>::rows*mat_traits<A>::cols>::f(a,b);
    return a;
    }

} }

#endif
// <<< #include <boost/qvm/detail/mat_assign.hpp>
// #line 10 "boost/qvm/mat_operations.hpp"
// >>> #include <boost/qvm/mat_operations2.hpp>
// #line 5 "boost/qvm/mat_operations2.hpp"
// >>> #include <boost/qvm/gen/mat_operations2.hpp>
#ifndef BOOST_QVM_GEN_MAT_OPERATIONS2_HPP_INCLUDED
#define BOOST_QVM_GEN_MAT_OPERATIONS2_HPP_INCLUDED

// #line 9 "boost/qvm/gen/mat_operations2.hpp"
// >>> #include <boost/qvm/assert.hpp>
// #line 5 "boost/qvm/assert.hpp"
#ifndef BOOST_QVM_ASSERT
#	ifdef BOOST_ASSERT
#		define BOOST_QVM_ASSERT BOOST_ASSERT
#	else
#		include <cassert>
#		define BOOST_QVM_ASSERT assert
#	endif
#endif
// <<< #include <boost/qvm/assert.hpp>
// #line 10 "boost/qvm/gen/mat_operations2.hpp"
// >>> #include <boost/qvm/deduce_mat.hpp>
#ifndef BOOST_QVM_DEDUCE_MAT_HPP_INCLUDED
#define BOOST_QVM_DEDUCE_MAT_HPP_INCLUDED

// #line 8 "boost/qvm/deduce_mat.hpp"
// >>> #include <boost/qvm/deduce_scalar.hpp>
#ifndef BOOST_QVM_DEDUCE_SCALAR_HPP_INCLUDED
#define BOOST_QVM_DEDUCE_SCALAR_HPP_INCLUDED

// #line 8 "boost/qvm/deduce_scalar.hpp"
// >>> #include <boost/qvm/scalar_traits.hpp>
#ifndef BOOST_QVM_SCALAR_TRAITS_HPP_INCLUDED
#define BOOST_QVM_SCALAR_TRAITS_HPP_INCLUDED

// #line 8 "boost/qvm/scalar_traits.hpp"
// >>> #include <boost/qvm/quat_traits.hpp>
#ifndef BOOST_QVM_QUAT_TRAITS
#define BOOST_QVM_QUAT_TRAITS

// #line 8 "boost/qvm/quat_traits.hpp"
// #include <boost/qvm/is_scalar.hpp> // Expanded at line 308
// #include <boost/qvm/enable_if.hpp> // Expanded at line 177
// #include <boost/qvm/config.hpp> // Expanded at line 109

namespace boost { namespace qvm {

template <class Q>
struct
quat_traits
    {
    typedef void scalar_type;
    };

template <class T>
struct
is_quat
    {
    static bool const value = is_scalar<typename quat_traits<T>::scalar_type>::value;
    };

namespace
qvm_detail
    {
    template <class T, T>
    struct
    qtr_dispatch_yes
        {
        char x, y;
        };
    }

template <class T>
class
quat_write_element_ref
    {
    template <class U>
    static qvm_detail::qtr_dispatch_yes<typename quat_traits<U>::scalar_type & (*)( U & ), &quat_traits<U>::template write_element<0> > check(int);

    template <class>
    static char check(long);

    public:

    static bool const value = sizeof(check<T>(0)) > 1;
    };

template <int I, class Q>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
typename enable_if_c<
    quat_write_element_ref<Q>::value,
    void>::type
write_quat_element( Q & q, typename quat_traits<Q>::scalar_type s )
    {
    quat_traits<Q>::template write_element<I>(q) = s;
    }

template <int I, class Q>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
typename enable_if_c<
    !quat_write_element_ref<Q>::value,
    void>::type
write_quat_element( Q & q, typename quat_traits<Q>::scalar_type s )
    {
    quat_traits<Q>::template write_element<I>(q, s);
    }

template <class Q>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
typename enable_if_c<
    quat_write_element_ref<Q>::value,
    void>::type
write_quat_element_idx( int i, Q & q, typename quat_traits<Q>::scalar_type s )
    {
    quat_traits<Q>::write_element_idx(i, q) = s;
    }

template <class Q>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
typename enable_if_c<
    !quat_write_element_ref<Q>::value,
    void>::type
write_vec_element_idx( int i, Q & q, typename quat_traits<Q>::scalar_type s )
    {
    quat_traits<Q>::write_element_idx(i, q, s);
    }

} }

#endif
// <<< #include <boost/qvm/quat_traits.hpp>
// #line 9 "boost/qvm/scalar_traits.hpp"
// >>> #include <boost/qvm/vec_traits.hpp>
#ifndef BOOST_QVM_VEC_TRAITS_HPP_INCLUDED
#define BOOST_QVM_VEC_TRAITS_HPP_INCLUDED

// #line 8 "boost/qvm/vec_traits.hpp"
// #include <boost/qvm/is_scalar.hpp> // Expanded at line 308
// #include <boost/qvm/enable_if.hpp> // Expanded at line 177
// #include <boost/qvm/config.hpp> // Expanded at line 109

namespace boost { namespace qvm {

template <class V>
struct
vec_traits
    {
    static int const dim=0;
    typedef void scalar_type;
    };

template <class T>
struct
is_vec
    {
    static bool const value = is_scalar<typename vec_traits<T>::scalar_type>::value && vec_traits<T>::dim>0;
    };

namespace
qvm_detail
    {
    template <class T, T>
    struct
    vtr_dispatch_yes
        {
        char x, y;
        };
    }

template <class T>
class
vec_write_element_ref
    {
    template <class U>
    static qvm_detail::vtr_dispatch_yes<typename vec_traits<U>::scalar_type & (*)( U & ), &vec_traits<U>::template write_element<0> > check(int);

    template <class>
    static char check(long);

    public:

    static bool const value = sizeof(check<T>(0)) > 1;
    };

template <int I, class V>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
typename enable_if_c<
    vec_write_element_ref<V>::value,
    void>::type
write_vec_element( V & v, typename vec_traits<V>::scalar_type s )
    {
    vec_traits<V>::template write_element<I>(v) = s;
    }

template <int I, class V>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
typename enable_if_c<
    !vec_write_element_ref<V>::value,
    void>::type
write_vec_element( V & v, typename vec_traits<V>::scalar_type s )
    {
    vec_traits<V>::template write_element<I>(v, s);
    }

template <class V>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
typename enable_if_c<
    vec_write_element_ref<V>::value,
    void>::type
write_vec_element_idx( int i, V & v, typename vec_traits<V>::scalar_type s )
    {
    vec_traits<V>::write_element_idx(i, v) = s;
    }

template <class V>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
typename enable_if_c<
    !vec_write_element_ref<V>::value,
    void>::type
write_vec_element_idx( int i, V & v, typename vec_traits<V>::scalar_type s )
    {
    vec_traits<V>::write_element_idx(i, v, s);
    }

} }

#endif
// <<< #include <boost/qvm/vec_traits.hpp>
// #line 10 "boost/qvm/scalar_traits.hpp"
// #include <boost/qvm/mat_traits.hpp> // Expanded at line 303
// #include <boost/qvm/config.hpp> // Expanded at line 109

namespace boost { namespace qvm {

template <class Scalar>
struct
scalar_traits
    {
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    Scalar
    value( int v )
        {
        return Scalar(v);
        }
    };

namespace
qvm_detail
    {
    template <class A,
        bool IsQ=is_quat<A>::value,
        bool IsV=is_vec<A>::value,
        bool IsM=is_mat<A>::value,
        bool IsS=is_scalar<A>::value>
    struct
    scalar_impl
        {
        typedef void type;
        };

    template <class A>
    struct
    scalar_impl<A,false,false,false,true>
        {
        typedef A type;
        };

    template <class A>
    struct
    scalar_impl<A,false,false,true,false>
        {
        typedef typename mat_traits<A>::scalar_type type;
        };

    template <class A>
    struct
    scalar_impl<A,false,true,false,false>
        {
        typedef typename vec_traits<A>::scalar_type type;
        };

    template <class A>
    struct
    scalar_impl<A,true,false,false,false>
        {
        typedef typename quat_traits<A>::scalar_type type;
        };
    }

template <class A>
struct
scalar
    {
    typedef typename qvm_detail::scalar_impl<A>::type type;
    };

} }

#endif
// <<< #include <boost/qvm/scalar_traits.hpp>
// #line 9 "boost/qvm/deduce_scalar.hpp"
// >>> #include <boost/qvm/static_assert.hpp>
// #line 5 "boost/qvm/static_assert.hpp"
#ifndef BOOST_QVM_STATIC_ASSERT

#	if __cplusplus >= 201103L

#		include <utility>
#		define BOOST_QVM_STATIC_ASSERT(condition) static_assert(condition, "Boost QVM static assertion failure")

#	else

#		ifdef __GNUC__
#			define BOOST_QVM_ATTRIBUTE_UNUSED __attribute__((unused))
#		else
#			define BOOST_QVM_ATTRIBUTE_UNUSED
#		endif

#		define BOOST_QVM_TOKEN_PASTE(x, y) x ## y
#		define BOOST_QVM_TOKEN_PASTE2(x, y) BOOST_QVM_TOKEN_PASTE(x, y)
#		define BOOST_QVM_STATIC_ASSERT(condition) typedef char BOOST_QVM_TOKEN_PASTE2(boost_qvm_static_assert_failure_,__LINE__)[(condition)?1:-1] BOOST_QVM_ATTRIBUTE_UNUSED

#	endif

#endif
// <<< #include <boost/qvm/static_assert.hpp>
// #line 10 "boost/qvm/deduce_scalar.hpp"
// >>> #include <boost/qvm/detail/remove_const.hpp>
#ifndef BOOST_QVM_DETAIL_REMOVE_CONST_HPP_INCLUDED
#define BOOST_QVM_DETAIL_REMOVE_CONST_HPP_INCLUDED

// #line 8 "boost/qvm/detail/remove_const.hpp"
namespace boost { namespace qvm {

namespace
qvm_detail
    {
    template <class T>
    struct
    remove_const
        {
        typedef T type;
        };

    template <class T>
    struct
    remove_const<T const>
        {
        typedef T type;
        };
    }

} }

#endif
// <<< #include <boost/qvm/detail/remove_const.hpp>
// #line 11 "boost/qvm/deduce_scalar.hpp"

namespace boost { namespace qvm {

namespace
deduce_scalar_detail
    {
    template <class A,class B>
    struct
    deduce_scalar_impl
        {
        typedef void type;
        };

    template <class T>
    struct
    deduce_scalar_impl<T,T>
        {
        typedef T type;
        };

    template <> struct deduce_scalar_impl<signed char,unsigned char> { typedef unsigned char type; };
    template <> struct deduce_scalar_impl<signed char,unsigned short> { typedef unsigned short type; };
    template <> struct deduce_scalar_impl<signed char,unsigned int> { typedef unsigned int type; };
    template <> struct deduce_scalar_impl<signed char,unsigned long> { typedef unsigned long type; };
    template <> struct deduce_scalar_impl<signed char,signed short> { typedef signed short type; };
    template <> struct deduce_scalar_impl<signed char,signed int> { typedef signed int type; };
    template <> struct deduce_scalar_impl<signed char,signed long> { typedef signed long type; };
    template <> struct deduce_scalar_impl<signed char,float> { typedef float type; };
    template <> struct deduce_scalar_impl<signed char,double> { typedef double type; };
    template <> struct deduce_scalar_impl<unsigned char,unsigned short> { typedef unsigned short type; };
    template <> struct deduce_scalar_impl<unsigned char,unsigned int> { typedef unsigned int type; };
    template <> struct deduce_scalar_impl<unsigned char,unsigned long> { typedef unsigned long type; };
    template <> struct deduce_scalar_impl<unsigned char,signed short> { typedef signed short type; };
    template <> struct deduce_scalar_impl<unsigned char,signed int> { typedef signed int type; };
    template <> struct deduce_scalar_impl<unsigned char,signed long> { typedef signed long type; };
    template <> struct deduce_scalar_impl<unsigned char,float> { typedef float type; };
    template <> struct deduce_scalar_impl<unsigned char,double> { typedef double type; };
    template <> struct deduce_scalar_impl<signed short,unsigned short> { typedef unsigned short type; };
    template <> struct deduce_scalar_impl<signed short,unsigned int> { typedef unsigned int type; };
    template <> struct deduce_scalar_impl<signed short,unsigned long> { typedef unsigned long type; };
    template <> struct deduce_scalar_impl<signed short,signed int> { typedef signed int type; };
    template <> struct deduce_scalar_impl<signed short,signed long> { typedef signed long type; };
    template <> struct deduce_scalar_impl<signed short,float> { typedef float type; };
    template <> struct deduce_scalar_impl<signed short,double> { typedef double type; };
    template <> struct deduce_scalar_impl<unsigned short,unsigned int> { typedef unsigned int type; };
    template <> struct deduce_scalar_impl<unsigned short,unsigned long> { typedef unsigned long type; };
    template <> struct deduce_scalar_impl<unsigned short,signed int> { typedef signed int type; };
    template <> struct deduce_scalar_impl<unsigned short,signed long> { typedef signed long type; };
    template <> struct deduce_scalar_impl<unsigned short,float> { typedef float type; };
    template <> struct deduce_scalar_impl<unsigned short,double> { typedef double type; };
    template <> struct deduce_scalar_impl<signed int,unsigned int> { typedef unsigned int type; };
    template <> struct deduce_scalar_impl<signed int,unsigned long> { typedef unsigned long type; };
    template <> struct deduce_scalar_impl<signed int,signed long> { typedef signed long type; };
    template <> struct deduce_scalar_impl<signed int,float> { typedef float type; };
    template <> struct deduce_scalar_impl<signed int,double> { typedef double type; };
    template <> struct deduce_scalar_impl<unsigned int,unsigned long> { typedef unsigned long type; };
    template <> struct deduce_scalar_impl<unsigned int,signed long> { typedef signed long type; };
    template <> struct deduce_scalar_impl<unsigned int,float> { typedef float type; };
    template <> struct deduce_scalar_impl<unsigned int,double> { typedef double type; };
    template <> struct deduce_scalar_impl<signed long,unsigned long> { typedef unsigned long type; };
    template <> struct deduce_scalar_impl<signed long,float> { typedef float type; };
    template <> struct deduce_scalar_impl<signed long,double> { typedef double type; };
    template <> struct deduce_scalar_impl<unsigned long,float> { typedef float type; };
    template <> struct deduce_scalar_impl<unsigned long,double> { typedef double type; };
    template <> struct deduce_scalar_impl<float,double> { typedef double type; };

    template <> struct deduce_scalar_impl<unsigned char,signed char> { typedef unsigned char type; };
    template <> struct deduce_scalar_impl<unsigned short,signed char> { typedef unsigned short type; };
    template <> struct deduce_scalar_impl<unsigned int,signed char> { typedef unsigned int type; };
    template <> struct deduce_scalar_impl<unsigned long,signed char> { typedef unsigned long type; };
    template <> struct deduce_scalar_impl<signed short,signed char> { typedef signed short type; };
    template <> struct deduce_scalar_impl<signed int,signed char> { typedef signed int type; };
    template <> struct deduce_scalar_impl<signed long,signed char> { typedef signed long type; };
    template <> struct deduce_scalar_impl<float,signed char> { typedef float type; };
    template <> struct deduce_scalar_impl<double,signed char> { typedef double type; };
    template <> struct deduce_scalar_impl<unsigned short,unsigned char> { typedef unsigned short type; };
    template <> struct deduce_scalar_impl<unsigned int,unsigned char> { typedef unsigned int type; };
    template <> struct deduce_scalar_impl<unsigned long,unsigned char> { typedef unsigned long type; };
    template <> struct deduce_scalar_impl<signed short,unsigned char> { typedef signed short type; };
    template <> struct deduce_scalar_impl<signed int,unsigned char> { typedef signed int type; };
    template <> struct deduce_scalar_impl<signed long,unsigned char> { typedef signed long type; };
    template <> struct deduce_scalar_impl<float,unsigned char> { typedef float type; };
    template <> struct deduce_scalar_impl<double,unsigned char> { typedef double type; };
    template <> struct deduce_scalar_impl<unsigned short,signed short> { typedef unsigned short type; };
    template <> struct deduce_scalar_impl<unsigned int,signed short> { typedef unsigned int type; };
    template <> struct deduce_scalar_impl<unsigned long,signed short> { typedef unsigned long type; };
    template <> struct deduce_scalar_impl<signed int,signed short> { typedef signed int type; };
    template <> struct deduce_scalar_impl<signed long,signed short> { typedef signed long type; };
    template <> struct deduce_scalar_impl<float,signed short> { typedef float type; };
    template <> struct deduce_scalar_impl<double,signed short> { typedef double type; };
    template <> struct deduce_scalar_impl<unsigned int,unsigned short> { typedef unsigned int type; };
    template <> struct deduce_scalar_impl<unsigned long,unsigned short> { typedef unsigned long type; };
    template <> struct deduce_scalar_impl<signed int,unsigned short> { typedef signed int type; };
    template <> struct deduce_scalar_impl<signed long,unsigned short> { typedef signed long type; };
    template <> struct deduce_scalar_impl<float,unsigned short> { typedef float type; };
    template <> struct deduce_scalar_impl<double,unsigned short> { typedef double type; };
    template <> struct deduce_scalar_impl<unsigned int,signed int> { typedef unsigned int type; };
    template <> struct deduce_scalar_impl<unsigned long,signed int> { typedef unsigned long type; };
    template <> struct deduce_scalar_impl<signed long,signed int> { typedef signed long type; };
    template <> struct deduce_scalar_impl<float,signed int> { typedef float type; };
    template <> struct deduce_scalar_impl<double,signed int> { typedef double type; };
    template <> struct deduce_scalar_impl<unsigned long,unsigned int> { typedef unsigned long type; };
    template <> struct deduce_scalar_impl<signed long,unsigned int> { typedef signed long type; };
    template <> struct deduce_scalar_impl<float,unsigned int> { typedef float type; };
    template <> struct deduce_scalar_impl<double,unsigned int> { typedef double type; };
    template <> struct deduce_scalar_impl<unsigned long,signed long> { typedef unsigned long type; };
    template <> struct deduce_scalar_impl<float,signed long> { typedef float type; };
    template <> struct deduce_scalar_impl<double,signed long> { typedef double type; };
    template <> struct deduce_scalar_impl<float,unsigned long> { typedef float type; };
    template <> struct deduce_scalar_impl<double,unsigned long> { typedef double type; };
    template <> struct deduce_scalar_impl<double,float> { typedef double type; };
    }

template <class A,class B>
struct
deduce_scalar
    {
    typedef typename deduce_scalar_detail::deduce_scalar_impl<typename qvm_detail::remove_const<A>::type,typename qvm_detail::remove_const<B>::type>::type type;
    };

} }

#endif
// <<< #include <boost/qvm/deduce_scalar.hpp>
// #line 9 "boost/qvm/deduce_mat.hpp"
// #include <boost/qvm/mat_traits.hpp> // Expanded at line 303
// #include <boost/qvm/static_assert.hpp> // Expanded at line 1180

namespace boost { namespace qvm {

template <class T,int Rows,int Cols>
struct mat;

namespace
qvm_detail
    {
    template <class M,int R,int C,class S,
        int MR=mat_traits<M>::rows,
        int MC=mat_traits<M>::cols,
        class MS=typename mat_traits<M>::scalar_type>
    struct
    deduce_m_default
        {
        BOOST_QVM_STATIC_ASSERT(is_mat<M>::value);
        typedef mat<typename mat_traits<M>::scalar_type,R,C> type;
        };

    template <class M,int R,int C,class S>
    struct
    deduce_m_default<M,R,C,S,R,C,S>
        {
        BOOST_QVM_STATIC_ASSERT(is_mat<M>::value);
        typedef M type;
        };
    }

template <class M,int R=mat_traits<M>::rows,int C=mat_traits<M>::cols,class S=typename mat_traits<M>::scalar_type>
struct
deduce_mat
    {
    BOOST_QVM_STATIC_ASSERT(is_mat<M>::value);
    typedef typename qvm_detail::deduce_m_default<M,R,C,S>::type type;
    };

namespace
qvm_detail
    {
    template <class A,class B,int R,int C,class S,
        bool IsScalarA=is_scalar<A>::value,
        bool IsScalarB=is_scalar<B>::value>
    struct
    deduce_m2_default
        {
        typedef mat<S,R,C> type;
        };

    template <class M,int R,int C,class S>
    struct
    deduce_m2_default<M,M,R,C,S,false,false>
        {
        BOOST_QVM_STATIC_ASSERT(is_mat<M>::value);
        typedef M type;
        };

    template <class A,class B,int R,int C,class S>
    struct
    deduce_m2_default<A,B,R,C,S,false,true>
        {
        BOOST_QVM_STATIC_ASSERT(is_mat<A>::value);
        typedef typename deduce_mat<A,R,C,S>::type type;
        };

    template <class A,class B,int R,int C,class S>
    struct
    deduce_m2_default<A,B,R,C,S,true,false>
        {
        BOOST_QVM_STATIC_ASSERT(is_mat<B>::value);
        typedef typename deduce_mat<B,R,C,S>::type type;
        };
    }

template <class A,class B,int R,int C,class S=typename deduce_scalar<typename scalar<A>::type,typename scalar<B>::type>::type>
struct
deduce_mat2
    {
    BOOST_QVM_STATIC_ASSERT(is_mat<A>::value || is_mat<B>::value);
    typedef typename qvm_detail::deduce_m2_default<A,B,R,C,S>::type type;
    };

} }

#endif
// <<< #include <boost/qvm/deduce_mat.hpp>
// #line 11 "boost/qvm/gen/mat_operations2.hpp"
// >>> #include <boost/qvm/deduce_vec.hpp>
#ifndef BOOST_QVM_DEDUCE_VEC_HPP_INCLUDED
#define BOOST_QVM_DEDUCE_VEC_HPP_INCLUDED

// #line 8 "boost/qvm/deduce_vec.hpp"
// #include <boost/qvm/deduce_scalar.hpp> // Expanded at line 904
// #include <boost/qvm/vec_traits.hpp> // Expanded at line 1010
// #include <boost/qvm/static_assert.hpp> // Expanded at line 1180

namespace boost { namespace qvm {

template <class T,int D>
struct vec;

namespace
qvm_detail
    {
    template <class V,int D,class S,
        int VD=vec_traits<V>::dim,
        class VS=typename vec_traits<V>::scalar_type>
    struct
    deduce_v_default
        {
        BOOST_QVM_STATIC_ASSERT(is_vec<V>::value);
        typedef vec<typename vec_traits<V>::scalar_type,D> type;
        };

    template <class V,int D,class S>
    struct
    deduce_v_default<V,D,S,D,S>
        {
        BOOST_QVM_STATIC_ASSERT(is_vec<V>::value);
        typedef V type;
        };
    }

template <class V,int D=vec_traits<V>::dim,class S=typename vec_traits<V>::scalar_type>
struct
deduce_vec
    {
    BOOST_QVM_STATIC_ASSERT(is_vec<V>::value);
    typedef typename qvm_detail::deduce_v_default<V,D,S>::type type;
    };

namespace
qvm_detail
    {
    template <class A,class B,int D,class S,
        bool IsScalarA=is_scalar<A>::value,
        bool IsScalarB=is_scalar<B>::value>
    struct
    deduce_v2_default
        {
        typedef vec<S,D> type;
        };

    template <class V,int D,class S>
    struct
    deduce_v2_default<V,V,D,S,false,false>
        {
        BOOST_QVM_STATIC_ASSERT(is_vec<V>::value);
        typedef V type;
        };

    template <class A,class B,int D,class S>
    struct
    deduce_v2_default<A,B,D,S,false,true>
        {
        BOOST_QVM_STATIC_ASSERT(is_vec<A>::value);
        typedef typename deduce_vec<A,D,S>::type type;
        };

    template <class A,class B,int D,class S>
    struct
    deduce_v2_default<A,B,D,S,true,false>
        {
        BOOST_QVM_STATIC_ASSERT(is_vec<B>::value);
        typedef typename deduce_vec<B,D,S>::type type;
        };
    }

template <class A,class B,int D,class S=typename deduce_scalar<typename scalar<A>::type,typename scalar<B>::type>::type>
struct
deduce_vec2
    {
    BOOST_QVM_STATIC_ASSERT(is_vec<A>::value || is_vec<B>::value);
    typedef typename qvm_detail::deduce_v2_default<A,B,D,S>::type type;
    };

} }

#endif
// <<< #include <boost/qvm/deduce_vec.hpp>
// #line 12 "boost/qvm/gen/mat_operations2.hpp"
// #include <boost/qvm/error.hpp> // Expanded at line 17
// #include <boost/qvm/gen/mat_assign2.hpp> // Expanded at line 104
// >>> #include <boost/qvm/throw_exception.hpp>
// #line 5 "boost/qvm/throw_exception.hpp"
#ifndef BOOST_QVM_THROW_EXCEPTION

#	define BOOST_QVM_THROW_EXCEPTION ::boost::qvm::throw_exception

#	include <exception>

#	ifndef BOOST_QVM_NO_EXCEPTIONS
#   	if defined(__clang__) && !defined(__ibmxl__) // Clang C++ emulates GCC, so it has to appear early.
#       	if !__has_feature(cxx_exceptions)
#           	define BOOST_QVM_NO_EXCEPTIONS
#       	endif
#   	elif defined(__DMC__) // Digital Mars C++
#       	if !defined(_CPPUNWIND)
#           	define BOOST_QVM_NO_EXCEPTIONS
#       	endif
#   	elif defined(__GNUC__) && !defined(__ibmxl__) // GNU C++:
#       	if !defined(__EXCEPTIONS)
#           	define BOOST_QVM_NO_EXCEPTIONS
#       	endif
#   	elif defined(__KCC) // Kai C++
#       	if !defined(_EXCEPTIONS)
#           	define BOOST_QVM_NO_EXCEPTIONS
#       	endif
#   	elif defined(__CODEGEARC__) // CodeGear - must be checked for before Borland
#       	if !defined(_CPPUNWIND) && !defined(__EXCEPTIONS)
#           	define BOOST_QVM_NO_EXCEPTIONS
#       	endif
#   	elif defined(__BORLANDC__) // Borland
#       	if !defined(_CPPUNWIND) && !defined(__EXCEPTIONS)
#           	define BOOST_QVM_NO_EXCEPTIONS
#       	endif
#   	elif defined(__MWERKS__) // Metrowerks CodeWarrior
#       	if !__option(exceptions)
#           	define BOOST_QVM_NO_EXCEPTIONS
#       	endif
#   	elif defined(__IBMCPP__) && defined(__COMPILER_VER__) && defined(__MVS__) // IBM z/OS XL C/C++
#       	if !defined(_CPPUNWIND) && !defined(__EXCEPTIONS)
#           define BOOST_QVM_NO_EXCEPTIONS
#       	endif
#   	elif defined(__ibmxl__) // IBM XL C/C++ for Linux (Little Endian)
#       	if !__has_feature(cxx_exceptions)
#           	define BOOST_QVM_NO_EXCEPTIONS
#       	endif
#   	elif defined(_MSC_VER) // Microsoft Visual C++
			// Must remain the last #elif since some other vendors (Metrowerks, for
			// example) also #define _MSC_VER
#       	if !defined(_CPPUNWIND)
#           	define BOOST_QVM_NO_EXCEPTIONS
#       	endif
#   	endif
#	endif

////////////////////////////////////////

#	ifdef BOOST_NORETURN
#   	define BOOST_QVM_NORETURN BOOST_NORETURN
#	else
#   	if defined(_MSC_VER)
#       	define BOOST_QVM_NORETURN __declspec(noreturn)
#   	elif defined(__GNUC__)
#       	define BOOST_QVM_NORETURN __attribute__ ((__noreturn__))
#   	elif defined(__has_attribute) && defined(__SUNPRO_CC) && (__SUNPRO_CC > 0x5130)
#       	if __has_attribute(noreturn)
#           	define BOOST_QVM_NORETURN [[noreturn]]
#       	endif
#   	elif defined(__has_cpp_attribute)
#       	if __has_cpp_attribute(noreturn)
#           	define BOOST_QVM_NORETURN [[noreturn]]
#       	endif
#   	endif
#	endif

#	if !defined(BOOST_QVM_NORETURN)
#  		define BOOST_QVM_NORETURN
#	endif

////////////////////////////////////////

#	ifdef BOOST_QVM_NO_EXCEPTIONS

namespace boost
{
    BOOST_QVM_NORETURN void throw_exception( std::exception const & ); // user defined
}

namespace boost { namespace qvm {

    template <class T>
    BOOST_QVM_NORETURN void throw_exception( T const & e )
    {
        ::boost::throw_exception(e);
    }

} }

#	else

namespace boost { namespace qvm {

    template <class T>
    BOOST_QVM_NORETURN void throw_exception( T const & e )
    {
        throw e;
    }

} }

#	endif

#endif
// <<< #include <boost/qvm/throw_exception.hpp>
// #line 15 "boost/qvm/gen/mat_operations2.hpp"

namespace boost { namespace qvm {

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==2 && mat_traits<B>::rows==2 &&
    mat_traits<A>::cols==2 && mat_traits<B>::cols==2,
    deduce_mat2<A,B,2,2> >::type
operator+( A const & a, B const & b )
    {
    typedef typename deduce_mat2<A,B,2,2>::type R;
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::rows==2);
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::cols==2);
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a)+mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<0,1>(r,mat_traits<A>::template read_element<0,1>(a)+mat_traits<B>::template read_element<0,1>(b));
    write_mat_element<1,0>(r,mat_traits<A>::template read_element<1,0>(a)+mat_traits<B>::template read_element<1,0>(b));
    write_mat_element<1,1>(r,mat_traits<A>::template read_element<1,1>(a)+mat_traits<B>::template read_element<1,1>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator+;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct plus_mm_defined;

    template <>
    struct
    plus_mm_defined<2,2>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==2 && mat_traits<B>::rows==2 &&
    mat_traits<A>::cols==1 && mat_traits<B>::cols==1,
    deduce_mat2<A,B,2,1> >::type
operator+( A const & a, B const & b )
    {
    typedef typename deduce_mat2<A,B,2,1>::type R;
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::rows==2);
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::cols==1);
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a)+mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<1,0>(r,mat_traits<A>::template read_element<1,0>(a)+mat_traits<B>::template read_element<1,0>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator+;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct plus_mm_defined;

    template <>
    struct
    plus_mm_defined<2,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==1 && mat_traits<B>::rows==1 &&
    mat_traits<A>::cols==2 && mat_traits<B>::cols==2,
    deduce_mat2<A,B,1,2> >::type
operator+( A const & a, B const & b )
    {
    typedef typename deduce_mat2<A,B,1,2>::type R;
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::rows==1);
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::cols==2);
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a)+mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<0,1>(r,mat_traits<A>::template read_element<0,1>(a)+mat_traits<B>::template read_element<0,1>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator+;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct plus_mm_defined;

    template <>
    struct
    plus_mm_defined<1,2>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==2 && mat_traits<B>::rows==2 &&
    mat_traits<A>::cols==2 && mat_traits<B>::cols==2,
    deduce_mat2<A,B,2,2> >::type
operator-( A const & a, B const & b )
    {
    typedef typename deduce_mat2<A,B,2,2>::type R;
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::rows==2);
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::cols==2);
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a)-mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<0,1>(r,mat_traits<A>::template read_element<0,1>(a)-mat_traits<B>::template read_element<0,1>(b));
    write_mat_element<1,0>(r,mat_traits<A>::template read_element<1,0>(a)-mat_traits<B>::template read_element<1,0>(b));
    write_mat_element<1,1>(r,mat_traits<A>::template read_element<1,1>(a)-mat_traits<B>::template read_element<1,1>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator-;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct minus_mm_defined;

    template <>
    struct
    minus_mm_defined<2,2>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==2 && mat_traits<B>::rows==2 &&
    mat_traits<A>::cols==1 && mat_traits<B>::cols==1,
    deduce_mat2<A,B,2,1> >::type
operator-( A const & a, B const & b )
    {
    typedef typename deduce_mat2<A,B,2,1>::type R;
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::rows==2);
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::cols==1);
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a)-mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<1,0>(r,mat_traits<A>::template read_element<1,0>(a)-mat_traits<B>::template read_element<1,0>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator-;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct minus_mm_defined;

    template <>
    struct
    minus_mm_defined<2,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==1 && mat_traits<B>::rows==1 &&
    mat_traits<A>::cols==2 && mat_traits<B>::cols==2,
    deduce_mat2<A,B,1,2> >::type
operator-( A const & a, B const & b )
    {
    typedef typename deduce_mat2<A,B,1,2>::type R;
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::rows==1);
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::cols==2);
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a)-mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<0,1>(r,mat_traits<A>::template read_element<0,1>(a)-mat_traits<B>::template read_element<0,1>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator-;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct minus_mm_defined;

    template <>
    struct
    minus_mm_defined<1,2>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==2 && mat_traits<B>::rows==2 &&
    mat_traits<A>::cols==2 && mat_traits<B>::cols==2,
    A &>::type
operator+=( A & a, B const & b )
    {
    write_mat_element<0,0>(a,mat_traits<A>::template read_element<0,0>(a)+mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<0,1>(a,mat_traits<A>::template read_element<0,1>(a)+mat_traits<B>::template read_element<0,1>(b));
    write_mat_element<1,0>(a,mat_traits<A>::template read_element<1,0>(a)+mat_traits<B>::template read_element<1,0>(b));
    write_mat_element<1,1>(a,mat_traits<A>::template read_element<1,1>(a)+mat_traits<B>::template read_element<1,1>(b));
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator+=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct plus_eq_mm_defined;

    template <>
    struct
    plus_eq_mm_defined<2,2>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==2 && mat_traits<B>::rows==2 &&
    mat_traits<A>::cols==1 && mat_traits<B>::cols==1,
    A &>::type
operator+=( A & a, B const & b )
    {
    write_mat_element<0,0>(a,mat_traits<A>::template read_element<0,0>(a)+mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<1,0>(a,mat_traits<A>::template read_element<1,0>(a)+mat_traits<B>::template read_element<1,0>(b));
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator+=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct plus_eq_mm_defined;

    template <>
    struct
    plus_eq_mm_defined<2,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==1 && mat_traits<B>::rows==1 &&
    mat_traits<A>::cols==2 && mat_traits<B>::cols==2,
    A &>::type
operator+=( A & a, B const & b )
    {
    write_mat_element<0,0>(a,mat_traits<A>::template read_element<0,0>(a)+mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<0,1>(a,mat_traits<A>::template read_element<0,1>(a)+mat_traits<B>::template read_element<0,1>(b));
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator+=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct plus_eq_mm_defined;

    template <>
    struct
    plus_eq_mm_defined<1,2>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==2 && mat_traits<B>::rows==2 &&
    mat_traits<A>::cols==2 && mat_traits<B>::cols==2,
    A &>::type
operator-=( A & a, B const & b )
    {
    write_mat_element<0,0>(a,mat_traits<A>::template read_element<0,0>(a)-mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<0,1>(a,mat_traits<A>::template read_element<0,1>(a)-mat_traits<B>::template read_element<0,1>(b));
    write_mat_element<1,0>(a,mat_traits<A>::template read_element<1,0>(a)-mat_traits<B>::template read_element<1,0>(b));
    write_mat_element<1,1>(a,mat_traits<A>::template read_element<1,1>(a)-mat_traits<B>::template read_element<1,1>(b));
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator-=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct minus_eq_mm_defined;

    template <>
    struct
    minus_eq_mm_defined<2,2>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==2 && mat_traits<B>::rows==2 &&
    mat_traits<A>::cols==1 && mat_traits<B>::cols==1,
    A &>::type
operator-=( A & a, B const & b )
    {
    write_mat_element<0,0>(a,mat_traits<A>::template read_element<0,0>(a)-mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<1,0>(a,mat_traits<A>::template read_element<1,0>(a)-mat_traits<B>::template read_element<1,0>(b));
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator-=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct minus_eq_mm_defined;

    template <>
    struct
    minus_eq_mm_defined<2,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==1 && mat_traits<B>::rows==1 &&
    mat_traits<A>::cols==2 && mat_traits<B>::cols==2,
    A &>::type
operator-=( A & a, B const & b )
    {
    write_mat_element<0,0>(a,mat_traits<A>::template read_element<0,0>(a)-mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<0,1>(a,mat_traits<A>::template read_element<0,1>(a)-mat_traits<B>::template read_element<0,1>(b));
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator-=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct minus_eq_mm_defined;

    template <>
    struct
    minus_eq_mm_defined<1,2>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==2 && mat_traits<A>::cols==2 && is_scalar<B>::value,
    deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols> >::type
operator*( A const & a, B b )
    {
    typedef typename deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols>::type R;
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a)*b);
    write_mat_element<0,1>(r,mat_traits<A>::template read_element<0,1>(a)*b);
    write_mat_element<1,0>(r,mat_traits<A>::template read_element<1,0>(a)*b);
    write_mat_element<1,1>(r,mat_traits<A>::template read_element<1,1>(a)*b);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct mul_ms_defined;

    template <>
    struct
    mul_ms_defined<2,2>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_scalar<A>::value && mat_traits<B>::rows==2 && mat_traits<B>::cols==2,
    deduce_mat2<A,B,mat_traits<B>::rows,mat_traits<B>::cols> >::type
operator*( A a, B const & b )
    {
    typedef typename deduce_mat2<A,B,mat_traits<B>::rows,mat_traits<B>::cols>::type R;
    R r;
    write_mat_element<0,0>(r,a*mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<0,1>(r,a*mat_traits<B>::template read_element<0,1>(b));
    write_mat_element<1,0>(r,a*mat_traits<B>::template read_element<1,0>(b));
    write_mat_element<1,1>(r,a*mat_traits<B>::template read_element<1,1>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct mul_sm_defined;

    template <>
    struct
    mul_sm_defined<2,2>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==2 && mat_traits<A>::cols==1 && is_scalar<B>::value,
    deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols> >::type
operator*( A const & a, B b )
    {
    typedef typename deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols>::type R;
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a)*b);
    write_mat_element<1,0>(r,mat_traits<A>::template read_element<1,0>(a)*b);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct mul_ms_defined;

    template <>
    struct
    mul_ms_defined<2,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_scalar<A>::value && mat_traits<B>::rows==2 && mat_traits<B>::cols==1,
    deduce_mat2<A,B,mat_traits<B>::rows,mat_traits<B>::cols> >::type
operator*( A a, B const & b )
    {
    typedef typename deduce_mat2<A,B,mat_traits<B>::rows,mat_traits<B>::cols>::type R;
    R r;
    write_mat_element<0,0>(r,a*mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<1,0>(r,a*mat_traits<B>::template read_element<1,0>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct mul_sm_defined;

    template <>
    struct
    mul_sm_defined<2,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==1 && mat_traits<A>::cols==2 && is_scalar<B>::value,
    deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols> >::type
operator*( A const & a, B b )
    {
    typedef typename deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols>::type R;
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a)*b);
    write_mat_element<0,1>(r,mat_traits<A>::template read_element<0,1>(a)*b);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct mul_ms_defined;

    template <>
    struct
    mul_ms_defined<1,2>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_scalar<A>::value && mat_traits<B>::rows==1 && mat_traits<B>::cols==2,
    deduce_mat2<A,B,mat_traits<B>::rows,mat_traits<B>::cols> >::type
operator*( A a, B const & b )
    {
    typedef typename deduce_mat2<A,B,mat_traits<B>::rows,mat_traits<B>::cols>::type R;
    R r;
    write_mat_element<0,0>(r,a*mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<0,1>(r,a*mat_traits<B>::template read_element<0,1>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct mul_sm_defined;

    template <>
    struct
    mul_sm_defined<1,2>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==2 && mat_traits<A>::cols==2 && is_scalar<B>::value,
    A &>::type
operator*=( A & a, B b )
    {
    write_mat_element<0,0>(a,mat_traits<A>::template read_element<0,0>(a)*b);
    write_mat_element<0,1>(a,mat_traits<A>::template read_element<0,1>(a)*b);
    write_mat_element<1,0>(a,mat_traits<A>::template read_element<1,0>(a)*b);
    write_mat_element<1,1>(a,mat_traits<A>::template read_element<1,1>(a)*b);
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct mul_eq_ms_defined;

    template <>
    struct
    mul_eq_ms_defined<2,2>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==2 && mat_traits<A>::cols==1 && is_scalar<B>::value,
    A &>::type
operator*=( A & a, B b )
    {
    write_mat_element<0,0>(a,mat_traits<A>::template read_element<0,0>(a)*b);
    write_mat_element<1,0>(a,mat_traits<A>::template read_element<1,0>(a)*b);
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct mul_eq_ms_defined;

    template <>
    struct
    mul_eq_ms_defined<2,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==1 && mat_traits<A>::cols==2 && is_scalar<B>::value,
    A &>::type
operator*=( A & a, B b )
    {
    write_mat_element<0,0>(a,mat_traits<A>::template read_element<0,0>(a)*b);
    write_mat_element<0,1>(a,mat_traits<A>::template read_element<0,1>(a)*b);
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct mul_eq_ms_defined;

    template <>
    struct
    mul_eq_ms_defined<1,2>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==2 && mat_traits<A>::cols==2 && is_scalar<B>::value,
    deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols> >::type
operator/( A const & a, B b )
    {
    typedef typename deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols>::type R;
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a)/b);
    write_mat_element<0,1>(r,mat_traits<A>::template read_element<0,1>(a)/b);
    write_mat_element<1,0>(r,mat_traits<A>::template read_element<1,0>(a)/b);
    write_mat_element<1,1>(r,mat_traits<A>::template read_element<1,1>(a)/b);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator/;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct div_ms_defined;

    template <>
    struct
    div_ms_defined<2,2>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_scalar<A>::value && mat_traits<B>::rows==2 && mat_traits<B>::cols==2,
    deduce_mat2<A,B,mat_traits<B>::rows,mat_traits<B>::cols> >::type
operator/( A a, B const & b )
    {
    typedef typename deduce_mat2<A,B,mat_traits<B>::rows,mat_traits<B>::cols>::type R;
    R r;
    write_mat_element<0,0>(r,a/mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<0,1>(r,a/mat_traits<B>::template read_element<0,1>(b));
    write_mat_element<1,0>(r,a/mat_traits<B>::template read_element<1,0>(b));
    write_mat_element<1,1>(r,a/mat_traits<B>::template read_element<1,1>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator/;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct div_sm_defined;

    template <>
    struct
    div_sm_defined<2,2>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==2 && mat_traits<A>::cols==1 && is_scalar<B>::value,
    deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols> >::type
operator/( A const & a, B b )
    {
    typedef typename deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols>::type R;
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a)/b);
    write_mat_element<1,0>(r,mat_traits<A>::template read_element<1,0>(a)/b);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator/;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct div_ms_defined;

    template <>
    struct
    div_ms_defined<2,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_scalar<A>::value && mat_traits<B>::rows==2 && mat_traits<B>::cols==1,
    deduce_mat2<A,B,mat_traits<B>::rows,mat_traits<B>::cols> >::type
operator/( A a, B const & b )
    {
    typedef typename deduce_mat2<A,B,mat_traits<B>::rows,mat_traits<B>::cols>::type R;
    R r;
    write_mat_element<0,0>(r,a/mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<1,0>(r,a/mat_traits<B>::template read_element<1,0>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator/;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct div_sm_defined;

    template <>
    struct
    div_sm_defined<2,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==1 && mat_traits<A>::cols==2 && is_scalar<B>::value,
    deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols> >::type
operator/( A const & a, B b )
    {
    typedef typename deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols>::type R;
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a)/b);
    write_mat_element<0,1>(r,mat_traits<A>::template read_element<0,1>(a)/b);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator/;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct div_ms_defined;

    template <>
    struct
    div_ms_defined<1,2>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==2 && mat_traits<A>::cols==2 && is_scalar<B>::value,
    A &>::type
operator/=( A & a, B b )
    {
    write_mat_element<0,0>(a,mat_traits<A>::template read_element<0,0>(a)/b);
    write_mat_element<0,1>(a,mat_traits<A>::template read_element<0,1>(a)/b);
    write_mat_element<1,0>(a,mat_traits<A>::template read_element<1,0>(a)/b);
    write_mat_element<1,1>(a,mat_traits<A>::template read_element<1,1>(a)/b);
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator/=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct div_eq_ms_defined;

    template <>
    struct
    div_eq_ms_defined<2,2>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==2 && mat_traits<A>::cols==1 && is_scalar<B>::value,
    A &>::type
operator/=( A & a, B b )
    {
    write_mat_element<0,0>(a,mat_traits<A>::template read_element<0,0>(a)/b);
    write_mat_element<1,0>(a,mat_traits<A>::template read_element<1,0>(a)/b);
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator/=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct div_eq_ms_defined;

    template <>
    struct
    div_eq_ms_defined<2,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==1 && mat_traits<A>::cols==2 && is_scalar<B>::value,
    A &>::type
operator/=( A & a, B b )
    {
    write_mat_element<0,0>(a,mat_traits<A>::template read_element<0,0>(a)/b);
    write_mat_element<0,1>(a,mat_traits<A>::template read_element<0,1>(a)/b);
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator/=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct div_eq_ms_defined;

    template <>
    struct
    div_eq_ms_defined<1,2>
        {
        static bool const value=true;
        };
    }

template <class R,class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<R>::rows==2 && mat_traits<A>::rows==2 &&
    mat_traits<R>::cols==2 && mat_traits<A>::cols==2,
    R>::type
convert_to( A const & a )
    {
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a));
    write_mat_element<0,1>(r,mat_traits<A>::template read_element<0,1>(a));
    write_mat_element<1,0>(r,mat_traits<A>::template read_element<1,0>(a));
    write_mat_element<1,1>(r,mat_traits<A>::template read_element<1,1>(a));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::convert_to;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct convert_to_m_defined;

    template <>
    struct
    convert_to_m_defined<2,2>
        {
        static bool const value=true;
        };
    }

template <class R,class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<R>::rows==2 && mat_traits<A>::rows==2 &&
    mat_traits<R>::cols==1 && mat_traits<A>::cols==1,
    R>::type
convert_to( A const & a )
    {
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a));
    write_mat_element<1,0>(r,mat_traits<A>::template read_element<1,0>(a));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::convert_to;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct convert_to_m_defined;

    template <>
    struct
    convert_to_m_defined<2,1>
        {
        static bool const value=true;
        };
    }

template <class R,class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<R>::rows==1 && mat_traits<A>::rows==1 &&
    mat_traits<R>::cols==2 && mat_traits<A>::cols==2,
    R>::type
convert_to( A const & a )
    {
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a));
    write_mat_element<0,1>(r,mat_traits<A>::template read_element<0,1>(a));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::convert_to;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct convert_to_m_defined;

    template <>
    struct
    convert_to_m_defined<1,2>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==2 && mat_traits<B>::rows==2 &&
    mat_traits<A>::cols==2 && mat_traits<B>::cols==2,
    bool>::type
operator==( A const & a, B const & b )
    {
    return
        mat_traits<A>::template read_element<0,0>(a)==mat_traits<B>::template read_element<0,0>(b) &&
        mat_traits<A>::template read_element<0,1>(a)==mat_traits<B>::template read_element<0,1>(b) &&
        mat_traits<A>::template read_element<1,0>(a)==mat_traits<B>::template read_element<1,0>(b) &&
        mat_traits<A>::template read_element<1,1>(a)==mat_traits<B>::template read_element<1,1>(b);
    }

namespace
sfinae
    {
    using ::boost::qvm::operator==;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct eq_mm_defined;

    template <>
    struct
    eq_mm_defined<2,2>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==2 && mat_traits<B>::rows==2 &&
    mat_traits<A>::cols==1 && mat_traits<B>::cols==1,
    bool>::type
operator==( A const & a, B const & b )
    {
    return
        mat_traits<A>::template read_element<0,0>(a)==mat_traits<B>::template read_element<0,0>(b) &&
        mat_traits<A>::template read_element<1,0>(a)==mat_traits<B>::template read_element<1,0>(b);
    }

namespace
sfinae
    {
    using ::boost::qvm::operator==;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct eq_mm_defined;

    template <>
    struct
    eq_mm_defined<2,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==1 && mat_traits<B>::rows==1 &&
    mat_traits<A>::cols==2 && mat_traits<B>::cols==2,
    bool>::type
operator==( A const & a, B const & b )
    {
    return
        mat_traits<A>::template read_element<0,0>(a)==mat_traits<B>::template read_element<0,0>(b) &&
        mat_traits<A>::template read_element<0,1>(a)==mat_traits<B>::template read_element<0,1>(b);
    }

namespace
sfinae
    {
    using ::boost::qvm::operator==;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct eq_mm_defined;

    template <>
    struct
    eq_mm_defined<1,2>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==2 && mat_traits<B>::rows==2 &&
    mat_traits<A>::cols==2 && mat_traits<B>::cols==2,
    bool>::type
operator!=( A const & a, B const & b )
    {
    return
        !(mat_traits<A>::template read_element<0,0>(a)==mat_traits<B>::template read_element<0,0>(b)) ||
        !(mat_traits<A>::template read_element<0,1>(a)==mat_traits<B>::template read_element<0,1>(b)) ||
        !(mat_traits<A>::template read_element<1,0>(a)==mat_traits<B>::template read_element<1,0>(b)) ||
        !(mat_traits<A>::template read_element<1,1>(a)==mat_traits<B>::template read_element<1,1>(b));
    }

namespace
sfinae
    {
    using ::boost::qvm::operator!=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct neq_mm_defined;

    template <>
    struct
    neq_mm_defined<2,2>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==2 && mat_traits<B>::rows==2 &&
    mat_traits<A>::cols==1 && mat_traits<B>::cols==1,
    bool>::type
operator!=( A const & a, B const & b )
    {
    return
        !(mat_traits<A>::template read_element<0,0>(a)==mat_traits<B>::template read_element<0,0>(b)) ||
        !(mat_traits<A>::template read_element<1,0>(a)==mat_traits<B>::template read_element<1,0>(b));
    }

namespace
sfinae
    {
    using ::boost::qvm::operator!=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct neq_mm_defined;

    template <>
    struct
    neq_mm_defined<2,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==1 && mat_traits<B>::rows==1 &&
    mat_traits<A>::cols==2 && mat_traits<B>::cols==2,
    bool>::type
operator!=( A const & a, B const & b )
    {
    return
        !(mat_traits<A>::template read_element<0,0>(a)==mat_traits<B>::template read_element<0,0>(b)) ||
        !(mat_traits<A>::template read_element<0,1>(a)==mat_traits<B>::template read_element<0,1>(b));
    }

namespace
sfinae
    {
    using ::boost::qvm::operator!=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct neq_mm_defined;

    template <>
    struct
    neq_mm_defined<1,2>
        {
        static bool const value=true;
        };
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==2 && mat_traits<A>::cols==2,
    deduce_mat<A> >::type
operator-( A const & a )
    {
    typedef typename deduce_mat<A>::type R;
    R r;
    write_mat_element<0,0>(r,-mat_traits<A>::template read_element<0,0>(a));
    write_mat_element<0,1>(r,-mat_traits<A>::template read_element<0,1>(a));
    write_mat_element<1,0>(r,-mat_traits<A>::template read_element<1,0>(a));
    write_mat_element<1,1>(r,-mat_traits<A>::template read_element<1,1>(a));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator-;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct minus_m_defined;

    template <>
    struct
    minus_m_defined<2,2>
        {
        static bool const value=true;
        };
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==2 && mat_traits<A>::cols==1,
    deduce_mat<A> >::type
operator-( A const & a )
    {
    typedef typename deduce_mat<A>::type R;
    R r;
    write_mat_element<0,0>(r,-mat_traits<A>::template read_element<0,0>(a));
    write_mat_element<1,0>(r,-mat_traits<A>::template read_element<1,0>(a));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator-;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct minus_m_defined;

    template <>
    struct
    minus_m_defined<2,1>
        {
        static bool const value=true;
        };
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==1 && mat_traits<A>::cols==2,
    deduce_mat<A> >::type
operator-( A const & a )
    {
    typedef typename deduce_mat<A>::type R;
    R r;
    write_mat_element<0,0>(r,-mat_traits<A>::template read_element<0,0>(a));
    write_mat_element<0,1>(r,-mat_traits<A>::template read_element<0,1>(a));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator-;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct minus_m_defined;

    template <>
    struct
    minus_m_defined<1,2>
        {
        static bool const value=true;
        };
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==2 && mat_traits<A>::cols==2,
    typename mat_traits<A>::scalar_type>::type
determinant( A const & a )
    {
    typedef typename mat_traits<A>::scalar_type T;
    T const a00=mat_traits<A>::template read_element<0,0>(a);
    T const a01=mat_traits<A>::template read_element<0,1>(a);
    T const a10=mat_traits<A>::template read_element<1,0>(a);
    T const a11=mat_traits<A>::template read_element<1,1>(a);
    T det=(a00*a11-a01*a10);
    return det;
    }

namespace
sfinae
    {
    using ::boost::qvm::determinant;
    }

namespace
qvm_detail
    {
    template <int D>
    struct determinant_defined;

    template <>
    struct
    determinant_defined<2>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==2 && mat_traits<A>::cols==2 && is_scalar<B>::value,
    deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols> >::type
inverse( A const & a, B det )
    {
    typedef typename mat_traits<A>::scalar_type T;
    BOOST_QVM_ASSERT(det!=scalar_traits<B>::value(0));
    T const a00=mat_traits<A>::template read_element<0,0>(a);
    T const a01=mat_traits<A>::template read_element<0,1>(a);
    T const a10=mat_traits<A>::template read_element<1,0>(a);
    T const a11=mat_traits<A>::template read_element<1,1>(a);
    T const f=scalar_traits<T>::value(1)/det;
    typedef typename deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols>::type R;
    R r;
    write_mat_element<0,0>(r, f*a11);
    write_mat_element<0,1>(r,-f*a01);
    write_mat_element<1,0>(r,-f*a10);
    write_mat_element<1,1>(r, f*a00);
    return r;
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==2 && mat_traits<A>::cols==2,
    deduce_mat<A> >::type
inverse( A const & a )
    {
    typedef typename mat_traits<A>::scalar_type T;
    T det=determinant(a);
    if( det==scalar_traits<T>::value(0) )
        BOOST_QVM_THROW_EXCEPTION(zero_determinant_error());
    return inverse(a,det);
    }

namespace
sfinae
    {
    using ::boost::qvm::inverse;
    }

namespace
qvm_detail
    {
    template <int D>
    struct inverse_m_defined;

    template <>
    struct
    inverse_m_defined<2>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==2 && mat_traits<B>::rows==2 &&
    mat_traits<A>::cols==2 && mat_traits<B>::cols==2,
    deduce_mat2<A,B,2,2> >::type
operator*( A const & a, B const & b )
    {
    typedef typename mat_traits<A>::scalar_type Ta;
    typedef typename mat_traits<B>::scalar_type Tb;
    Ta const a00 = mat_traits<A>::template read_element<0,0>(a);
    Ta const a01 = mat_traits<A>::template read_element<0,1>(a);
    Ta const a10 = mat_traits<A>::template read_element<1,0>(a);
    Ta const a11 = mat_traits<A>::template read_element<1,1>(a);
    Tb const b00 = mat_traits<B>::template read_element<0,0>(b);
    Tb const b01 = mat_traits<B>::template read_element<0,1>(b);
    Tb const b10 = mat_traits<B>::template read_element<1,0>(b);
    Tb const b11 = mat_traits<B>::template read_element<1,1>(b);
    typedef typename deduce_mat2<A,B,2,2>::type R;
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::rows==2);
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::cols==2);
    R r;
    write_mat_element<0,0>(r,a00*b00+a01*b10);
    write_mat_element<0,1>(r,a00*b01+a01*b11);
    write_mat_element<1,0>(r,a10*b00+a11*b10);
    write_mat_element<1,1>(r,a10*b01+a11*b11);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

namespace
qvm_detail
    {
    template <int R,int /*CR*/,int C>
    struct mul_mm_defined;

    template <>
    struct
    mul_mm_defined<2,2,2>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==2 && mat_traits<B>::rows==2 &&
    mat_traits<A>::cols==2 && mat_traits<B>::cols==2,
    A &>::type
operator*=( A & a, B const & b )
    {
    typedef typename mat_traits<A>::scalar_type Ta;
    typedef typename mat_traits<B>::scalar_type Tb;
    Ta const a00 = mat_traits<A>::template read_element<0,0>(a);
    Ta const a01 = mat_traits<A>::template read_element<0,1>(a);
    Ta const a10 = mat_traits<A>::template read_element<1,0>(a);
    Ta const a11 = mat_traits<A>::template read_element<1,1>(a);
    Tb const b00 = mat_traits<B>::template read_element<0,0>(b);
    Tb const b01 = mat_traits<B>::template read_element<0,1>(b);
    Tb const b10 = mat_traits<B>::template read_element<1,0>(b);
    Tb const b11 = mat_traits<B>::template read_element<1,1>(b);
    write_mat_element<0,0>(a,a00*b00+a01*b10);
    write_mat_element<0,1>(a,a00*b01+a01*b11);
    write_mat_element<1,0>(a,a10*b00+a11*b10);
    write_mat_element<1,1>(a,a10*b01+a11*b11);
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*=;
    }

namespace
qvm_detail
    {
    template <int D>
    struct mul_eq_mm_defined;

    template <>
    struct
    mul_eq_mm_defined<2>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==2 && mat_traits<B>::rows==2 &&
    mat_traits<A>::cols==2 && mat_traits<B>::cols==1,
    deduce_mat2<A,B,2,1> >::type
operator*( A const & a, B const & b )
    {
    typedef typename mat_traits<A>::scalar_type Ta;
    typedef typename mat_traits<B>::scalar_type Tb;
    Ta const a00 = mat_traits<A>::template read_element<0,0>(a);
    Ta const a01 = mat_traits<A>::template read_element<0,1>(a);
    Ta const a10 = mat_traits<A>::template read_element<1,0>(a);
    Ta const a11 = mat_traits<A>::template read_element<1,1>(a);
    Tb const b00 = mat_traits<B>::template read_element<0,0>(b);
    Tb const b10 = mat_traits<B>::template read_element<1,0>(b);
    typedef typename deduce_mat2<A,B,2,1>::type R;
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::rows==2);
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::cols==1);
    R r;
    write_mat_element<0,0>(r,a00*b00+a01*b10);
    write_mat_element<1,0>(r,a10*b00+a11*b10);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

namespace
qvm_detail
    {
    template <int R,int /*CR*/,int C>
    struct mul_mm_defined;

    template <>
    struct
    mul_mm_defined<2,2,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==1 && mat_traits<B>::rows==2 &&
    mat_traits<A>::cols==2 && mat_traits<B>::cols==2,
    deduce_mat2<A,B,1,2> >::type
operator*( A const & a, B const & b )
    {
    typedef typename mat_traits<A>::scalar_type Ta;
    typedef typename mat_traits<B>::scalar_type Tb;
    Ta const a00 = mat_traits<A>::template read_element<0,0>(a);
    Ta const a01 = mat_traits<A>::template read_element<0,1>(a);
    Tb const b00 = mat_traits<B>::template read_element<0,0>(b);
    Tb const b01 = mat_traits<B>::template read_element<0,1>(b);
    Tb const b10 = mat_traits<B>::template read_element<1,0>(b);
    Tb const b11 = mat_traits<B>::template read_element<1,1>(b);
    typedef typename deduce_mat2<A,B,1,2>::type R;
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::rows==1);
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::cols==2);
    R r;
    write_mat_element<0,0>(r,a00*b00+a01*b10);
    write_mat_element<0,1>(r,a00*b01+a01*b11);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

namespace
qvm_detail
    {
    template <int R,int /*CR*/,int C>
    struct mul_mm_defined;

    template <>
    struct
    mul_mm_defined<1,2,2>
        {
        static bool const value=true;
        };
    }

} }

#endif
// <<< #include <boost/qvm/gen/mat_operations2.hpp>
// #line 6 "boost/qvm/mat_operations2.hpp"
// <<< #include <boost/qvm/mat_operations2.hpp>
// #line 11 "boost/qvm/mat_operations.hpp"
// >>> #include <boost/qvm/mat_operations3.hpp>
// #line 5 "boost/qvm/mat_operations3.hpp"
// >>> #include <boost/qvm/gen/mat_operations3.hpp>
#ifndef BOOST_QVM_GEN_MAT_OPERATIONS3_HPP_INCLUDED
#define BOOST_QVM_GEN_MAT_OPERATIONS3_HPP_INCLUDED

// #line 9 "boost/qvm/gen/mat_operations3.hpp"
// #include <boost/qvm/assert.hpp> // Expanded at line 887
// #include <boost/qvm/deduce_mat.hpp> // Expanded at line 899
// #include <boost/qvm/deduce_vec.hpp> // Expanded at line 1450
// #include <boost/qvm/error.hpp> // Expanded at line 17
// #include <boost/qvm/gen/mat_assign3.hpp> // Expanded at line 546
// #include <boost/qvm/quat_traits.hpp> // Expanded at line 914
// #include <boost/qvm/scalar_traits.hpp> // Expanded at line 909
// #include <boost/qvm/throw_exception.hpp> // Expanded at line 1546

namespace boost { namespace qvm {

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==3 && mat_traits<B>::rows==3 &&
    mat_traits<A>::cols==3 && mat_traits<B>::cols==3,
    deduce_mat2<A,B,3,3> >::type
operator+( A const & a, B const & b )
    {
    typedef typename deduce_mat2<A,B,3,3>::type R;
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::rows==3);
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::cols==3);
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a)+mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<0,1>(r,mat_traits<A>::template read_element<0,1>(a)+mat_traits<B>::template read_element<0,1>(b));
    write_mat_element<0,2>(r,mat_traits<A>::template read_element<0,2>(a)+mat_traits<B>::template read_element<0,2>(b));
    write_mat_element<1,0>(r,mat_traits<A>::template read_element<1,0>(a)+mat_traits<B>::template read_element<1,0>(b));
    write_mat_element<1,1>(r,mat_traits<A>::template read_element<1,1>(a)+mat_traits<B>::template read_element<1,1>(b));
    write_mat_element<1,2>(r,mat_traits<A>::template read_element<1,2>(a)+mat_traits<B>::template read_element<1,2>(b));
    write_mat_element<2,0>(r,mat_traits<A>::template read_element<2,0>(a)+mat_traits<B>::template read_element<2,0>(b));
    write_mat_element<2,1>(r,mat_traits<A>::template read_element<2,1>(a)+mat_traits<B>::template read_element<2,1>(b));
    write_mat_element<2,2>(r,mat_traits<A>::template read_element<2,2>(a)+mat_traits<B>::template read_element<2,2>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator+;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct plus_mm_defined;

    template <>
    struct
    plus_mm_defined<3,3>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==3 && mat_traits<B>::rows==3 &&
    mat_traits<A>::cols==1 && mat_traits<B>::cols==1,
    deduce_mat2<A,B,3,1> >::type
operator+( A const & a, B const & b )
    {
    typedef typename deduce_mat2<A,B,3,1>::type R;
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::rows==3);
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::cols==1);
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a)+mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<1,0>(r,mat_traits<A>::template read_element<1,0>(a)+mat_traits<B>::template read_element<1,0>(b));
    write_mat_element<2,0>(r,mat_traits<A>::template read_element<2,0>(a)+mat_traits<B>::template read_element<2,0>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator+;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct plus_mm_defined;

    template <>
    struct
    plus_mm_defined<3,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==1 && mat_traits<B>::rows==1 &&
    mat_traits<A>::cols==3 && mat_traits<B>::cols==3,
    deduce_mat2<A,B,1,3> >::type
operator+( A const & a, B const & b )
    {
    typedef typename deduce_mat2<A,B,1,3>::type R;
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::rows==1);
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::cols==3);
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a)+mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<0,1>(r,mat_traits<A>::template read_element<0,1>(a)+mat_traits<B>::template read_element<0,1>(b));
    write_mat_element<0,2>(r,mat_traits<A>::template read_element<0,2>(a)+mat_traits<B>::template read_element<0,2>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator+;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct plus_mm_defined;

    template <>
    struct
    plus_mm_defined<1,3>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==3 && mat_traits<B>::rows==3 &&
    mat_traits<A>::cols==3 && mat_traits<B>::cols==3,
    deduce_mat2<A,B,3,3> >::type
operator-( A const & a, B const & b )
    {
    typedef typename deduce_mat2<A,B,3,3>::type R;
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::rows==3);
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::cols==3);
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a)-mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<0,1>(r,mat_traits<A>::template read_element<0,1>(a)-mat_traits<B>::template read_element<0,1>(b));
    write_mat_element<0,2>(r,mat_traits<A>::template read_element<0,2>(a)-mat_traits<B>::template read_element<0,2>(b));
    write_mat_element<1,0>(r,mat_traits<A>::template read_element<1,0>(a)-mat_traits<B>::template read_element<1,0>(b));
    write_mat_element<1,1>(r,mat_traits<A>::template read_element<1,1>(a)-mat_traits<B>::template read_element<1,1>(b));
    write_mat_element<1,2>(r,mat_traits<A>::template read_element<1,2>(a)-mat_traits<B>::template read_element<1,2>(b));
    write_mat_element<2,0>(r,mat_traits<A>::template read_element<2,0>(a)-mat_traits<B>::template read_element<2,0>(b));
    write_mat_element<2,1>(r,mat_traits<A>::template read_element<2,1>(a)-mat_traits<B>::template read_element<2,1>(b));
    write_mat_element<2,2>(r,mat_traits<A>::template read_element<2,2>(a)-mat_traits<B>::template read_element<2,2>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator-;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct minus_mm_defined;

    template <>
    struct
    minus_mm_defined<3,3>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==3 && mat_traits<B>::rows==3 &&
    mat_traits<A>::cols==1 && mat_traits<B>::cols==1,
    deduce_mat2<A,B,3,1> >::type
operator-( A const & a, B const & b )
    {
    typedef typename deduce_mat2<A,B,3,1>::type R;
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::rows==3);
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::cols==1);
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a)-mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<1,0>(r,mat_traits<A>::template read_element<1,0>(a)-mat_traits<B>::template read_element<1,0>(b));
    write_mat_element<2,0>(r,mat_traits<A>::template read_element<2,0>(a)-mat_traits<B>::template read_element<2,0>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator-;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct minus_mm_defined;

    template <>
    struct
    minus_mm_defined<3,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==1 && mat_traits<B>::rows==1 &&
    mat_traits<A>::cols==3 && mat_traits<B>::cols==3,
    deduce_mat2<A,B,1,3> >::type
operator-( A const & a, B const & b )
    {
    typedef typename deduce_mat2<A,B,1,3>::type R;
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::rows==1);
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::cols==3);
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a)-mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<0,1>(r,mat_traits<A>::template read_element<0,1>(a)-mat_traits<B>::template read_element<0,1>(b));
    write_mat_element<0,2>(r,mat_traits<A>::template read_element<0,2>(a)-mat_traits<B>::template read_element<0,2>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator-;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct minus_mm_defined;

    template <>
    struct
    minus_mm_defined<1,3>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==3 && mat_traits<B>::rows==3 &&
    mat_traits<A>::cols==3 && mat_traits<B>::cols==3,
    A &>::type
operator+=( A & a, B const & b )
    {
    write_mat_element<0,0>(a,mat_traits<A>::template read_element<0,0>(a)+mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<0,1>(a,mat_traits<A>::template read_element<0,1>(a)+mat_traits<B>::template read_element<0,1>(b));
    write_mat_element<0,2>(a,mat_traits<A>::template read_element<0,2>(a)+mat_traits<B>::template read_element<0,2>(b));
    write_mat_element<1,0>(a,mat_traits<A>::template read_element<1,0>(a)+mat_traits<B>::template read_element<1,0>(b));
    write_mat_element<1,1>(a,mat_traits<A>::template read_element<1,1>(a)+mat_traits<B>::template read_element<1,1>(b));
    write_mat_element<1,2>(a,mat_traits<A>::template read_element<1,2>(a)+mat_traits<B>::template read_element<1,2>(b));
    write_mat_element<2,0>(a,mat_traits<A>::template read_element<2,0>(a)+mat_traits<B>::template read_element<2,0>(b));
    write_mat_element<2,1>(a,mat_traits<A>::template read_element<2,1>(a)+mat_traits<B>::template read_element<2,1>(b));
    write_mat_element<2,2>(a,mat_traits<A>::template read_element<2,2>(a)+mat_traits<B>::template read_element<2,2>(b));
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator+=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct plus_eq_mm_defined;

    template <>
    struct
    plus_eq_mm_defined<3,3>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==3 && mat_traits<B>::rows==3 &&
    mat_traits<A>::cols==1 && mat_traits<B>::cols==1,
    A &>::type
operator+=( A & a, B const & b )
    {
    write_mat_element<0,0>(a,mat_traits<A>::template read_element<0,0>(a)+mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<1,0>(a,mat_traits<A>::template read_element<1,0>(a)+mat_traits<B>::template read_element<1,0>(b));
    write_mat_element<2,0>(a,mat_traits<A>::template read_element<2,0>(a)+mat_traits<B>::template read_element<2,0>(b));
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator+=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct plus_eq_mm_defined;

    template <>
    struct
    plus_eq_mm_defined<3,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==1 && mat_traits<B>::rows==1 &&
    mat_traits<A>::cols==3 && mat_traits<B>::cols==3,
    A &>::type
operator+=( A & a, B const & b )
    {
    write_mat_element<0,0>(a,mat_traits<A>::template read_element<0,0>(a)+mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<0,1>(a,mat_traits<A>::template read_element<0,1>(a)+mat_traits<B>::template read_element<0,1>(b));
    write_mat_element<0,2>(a,mat_traits<A>::template read_element<0,2>(a)+mat_traits<B>::template read_element<0,2>(b));
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator+=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct plus_eq_mm_defined;

    template <>
    struct
    plus_eq_mm_defined<1,3>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==3 && mat_traits<B>::rows==3 &&
    mat_traits<A>::cols==3 && mat_traits<B>::cols==3,
    A &>::type
operator-=( A & a, B const & b )
    {
    write_mat_element<0,0>(a,mat_traits<A>::template read_element<0,0>(a)-mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<0,1>(a,mat_traits<A>::template read_element<0,1>(a)-mat_traits<B>::template read_element<0,1>(b));
    write_mat_element<0,2>(a,mat_traits<A>::template read_element<0,2>(a)-mat_traits<B>::template read_element<0,2>(b));
    write_mat_element<1,0>(a,mat_traits<A>::template read_element<1,0>(a)-mat_traits<B>::template read_element<1,0>(b));
    write_mat_element<1,1>(a,mat_traits<A>::template read_element<1,1>(a)-mat_traits<B>::template read_element<1,1>(b));
    write_mat_element<1,2>(a,mat_traits<A>::template read_element<1,2>(a)-mat_traits<B>::template read_element<1,2>(b));
    write_mat_element<2,0>(a,mat_traits<A>::template read_element<2,0>(a)-mat_traits<B>::template read_element<2,0>(b));
    write_mat_element<2,1>(a,mat_traits<A>::template read_element<2,1>(a)-mat_traits<B>::template read_element<2,1>(b));
    write_mat_element<2,2>(a,mat_traits<A>::template read_element<2,2>(a)-mat_traits<B>::template read_element<2,2>(b));
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator-=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct minus_eq_mm_defined;

    template <>
    struct
    minus_eq_mm_defined<3,3>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==3 && mat_traits<B>::rows==3 &&
    mat_traits<A>::cols==1 && mat_traits<B>::cols==1,
    A &>::type
operator-=( A & a, B const & b )
    {
    write_mat_element<0,0>(a,mat_traits<A>::template read_element<0,0>(a)-mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<1,0>(a,mat_traits<A>::template read_element<1,0>(a)-mat_traits<B>::template read_element<1,0>(b));
    write_mat_element<2,0>(a,mat_traits<A>::template read_element<2,0>(a)-mat_traits<B>::template read_element<2,0>(b));
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator-=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct minus_eq_mm_defined;

    template <>
    struct
    minus_eq_mm_defined<3,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==1 && mat_traits<B>::rows==1 &&
    mat_traits<A>::cols==3 && mat_traits<B>::cols==3,
    A &>::type
operator-=( A & a, B const & b )
    {
    write_mat_element<0,0>(a,mat_traits<A>::template read_element<0,0>(a)-mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<0,1>(a,mat_traits<A>::template read_element<0,1>(a)-mat_traits<B>::template read_element<0,1>(b));
    write_mat_element<0,2>(a,mat_traits<A>::template read_element<0,2>(a)-mat_traits<B>::template read_element<0,2>(b));
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator-=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct minus_eq_mm_defined;

    template <>
    struct
    minus_eq_mm_defined<1,3>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==3 && mat_traits<A>::cols==3 && is_scalar<B>::value,
    deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols> >::type
operator*( A const & a, B b )
    {
    typedef typename deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols>::type R;
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a)*b);
    write_mat_element<0,1>(r,mat_traits<A>::template read_element<0,1>(a)*b);
    write_mat_element<0,2>(r,mat_traits<A>::template read_element<0,2>(a)*b);
    write_mat_element<1,0>(r,mat_traits<A>::template read_element<1,0>(a)*b);
    write_mat_element<1,1>(r,mat_traits<A>::template read_element<1,1>(a)*b);
    write_mat_element<1,2>(r,mat_traits<A>::template read_element<1,2>(a)*b);
    write_mat_element<2,0>(r,mat_traits<A>::template read_element<2,0>(a)*b);
    write_mat_element<2,1>(r,mat_traits<A>::template read_element<2,1>(a)*b);
    write_mat_element<2,2>(r,mat_traits<A>::template read_element<2,2>(a)*b);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct mul_ms_defined;

    template <>
    struct
    mul_ms_defined<3,3>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_scalar<A>::value && mat_traits<B>::rows==3 && mat_traits<B>::cols==3,
    deduce_mat2<A,B,mat_traits<B>::rows,mat_traits<B>::cols> >::type
operator*( A a, B const & b )
    {
    typedef typename deduce_mat2<A,B,mat_traits<B>::rows,mat_traits<B>::cols>::type R;
    R r;
    write_mat_element<0,0>(r,a*mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<0,1>(r,a*mat_traits<B>::template read_element<0,1>(b));
    write_mat_element<0,2>(r,a*mat_traits<B>::template read_element<0,2>(b));
    write_mat_element<1,0>(r,a*mat_traits<B>::template read_element<1,0>(b));
    write_mat_element<1,1>(r,a*mat_traits<B>::template read_element<1,1>(b));
    write_mat_element<1,2>(r,a*mat_traits<B>::template read_element<1,2>(b));
    write_mat_element<2,0>(r,a*mat_traits<B>::template read_element<2,0>(b));
    write_mat_element<2,1>(r,a*mat_traits<B>::template read_element<2,1>(b));
    write_mat_element<2,2>(r,a*mat_traits<B>::template read_element<2,2>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct mul_sm_defined;

    template <>
    struct
    mul_sm_defined<3,3>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==3 && mat_traits<A>::cols==1 && is_scalar<B>::value,
    deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols> >::type
operator*( A const & a, B b )
    {
    typedef typename deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols>::type R;
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a)*b);
    write_mat_element<1,0>(r,mat_traits<A>::template read_element<1,0>(a)*b);
    write_mat_element<2,0>(r,mat_traits<A>::template read_element<2,0>(a)*b);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct mul_ms_defined;

    template <>
    struct
    mul_ms_defined<3,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_scalar<A>::value && mat_traits<B>::rows==3 && mat_traits<B>::cols==1,
    deduce_mat2<A,B,mat_traits<B>::rows,mat_traits<B>::cols> >::type
operator*( A a, B const & b )
    {
    typedef typename deduce_mat2<A,B,mat_traits<B>::rows,mat_traits<B>::cols>::type R;
    R r;
    write_mat_element<0,0>(r,a*mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<1,0>(r,a*mat_traits<B>::template read_element<1,0>(b));
    write_mat_element<2,0>(r,a*mat_traits<B>::template read_element<2,0>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct mul_sm_defined;

    template <>
    struct
    mul_sm_defined<3,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==1 && mat_traits<A>::cols==3 && is_scalar<B>::value,
    deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols> >::type
operator*( A const & a, B b )
    {
    typedef typename deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols>::type R;
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a)*b);
    write_mat_element<0,1>(r,mat_traits<A>::template read_element<0,1>(a)*b);
    write_mat_element<0,2>(r,mat_traits<A>::template read_element<0,2>(a)*b);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct mul_ms_defined;

    template <>
    struct
    mul_ms_defined<1,3>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_scalar<A>::value && mat_traits<B>::rows==1 && mat_traits<B>::cols==3,
    deduce_mat2<A,B,mat_traits<B>::rows,mat_traits<B>::cols> >::type
operator*( A a, B const & b )
    {
    typedef typename deduce_mat2<A,B,mat_traits<B>::rows,mat_traits<B>::cols>::type R;
    R r;
    write_mat_element<0,0>(r,a*mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<0,1>(r,a*mat_traits<B>::template read_element<0,1>(b));
    write_mat_element<0,2>(r,a*mat_traits<B>::template read_element<0,2>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct mul_sm_defined;

    template <>
    struct
    mul_sm_defined<1,3>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==3 && mat_traits<A>::cols==3 && is_scalar<B>::value,
    A &>::type
operator*=( A & a, B b )
    {
    write_mat_element<0,0>(a,mat_traits<A>::template read_element<0,0>(a)*b);
    write_mat_element<0,1>(a,mat_traits<A>::template read_element<0,1>(a)*b);
    write_mat_element<0,2>(a,mat_traits<A>::template read_element<0,2>(a)*b);
    write_mat_element<1,0>(a,mat_traits<A>::template read_element<1,0>(a)*b);
    write_mat_element<1,1>(a,mat_traits<A>::template read_element<1,1>(a)*b);
    write_mat_element<1,2>(a,mat_traits<A>::template read_element<1,2>(a)*b);
    write_mat_element<2,0>(a,mat_traits<A>::template read_element<2,0>(a)*b);
    write_mat_element<2,1>(a,mat_traits<A>::template read_element<2,1>(a)*b);
    write_mat_element<2,2>(a,mat_traits<A>::template read_element<2,2>(a)*b);
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct mul_eq_ms_defined;

    template <>
    struct
    mul_eq_ms_defined<3,3>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==3 && mat_traits<A>::cols==1 && is_scalar<B>::value,
    A &>::type
operator*=( A & a, B b )
    {
    write_mat_element<0,0>(a,mat_traits<A>::template read_element<0,0>(a)*b);
    write_mat_element<1,0>(a,mat_traits<A>::template read_element<1,0>(a)*b);
    write_mat_element<2,0>(a,mat_traits<A>::template read_element<2,0>(a)*b);
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct mul_eq_ms_defined;

    template <>
    struct
    mul_eq_ms_defined<3,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==1 && mat_traits<A>::cols==3 && is_scalar<B>::value,
    A &>::type
operator*=( A & a, B b )
    {
    write_mat_element<0,0>(a,mat_traits<A>::template read_element<0,0>(a)*b);
    write_mat_element<0,1>(a,mat_traits<A>::template read_element<0,1>(a)*b);
    write_mat_element<0,2>(a,mat_traits<A>::template read_element<0,2>(a)*b);
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct mul_eq_ms_defined;

    template <>
    struct
    mul_eq_ms_defined<1,3>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==3 && mat_traits<A>::cols==3 && is_scalar<B>::value,
    deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols> >::type
operator/( A const & a, B b )
    {
    typedef typename deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols>::type R;
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a)/b);
    write_mat_element<0,1>(r,mat_traits<A>::template read_element<0,1>(a)/b);
    write_mat_element<0,2>(r,mat_traits<A>::template read_element<0,2>(a)/b);
    write_mat_element<1,0>(r,mat_traits<A>::template read_element<1,0>(a)/b);
    write_mat_element<1,1>(r,mat_traits<A>::template read_element<1,1>(a)/b);
    write_mat_element<1,2>(r,mat_traits<A>::template read_element<1,2>(a)/b);
    write_mat_element<2,0>(r,mat_traits<A>::template read_element<2,0>(a)/b);
    write_mat_element<2,1>(r,mat_traits<A>::template read_element<2,1>(a)/b);
    write_mat_element<2,2>(r,mat_traits<A>::template read_element<2,2>(a)/b);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator/;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct div_ms_defined;

    template <>
    struct
    div_ms_defined<3,3>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_scalar<A>::value && mat_traits<B>::rows==3 && mat_traits<B>::cols==3,
    deduce_mat2<A,B,mat_traits<B>::rows,mat_traits<B>::cols> >::type
operator/( A a, B const & b )
    {
    typedef typename deduce_mat2<A,B,mat_traits<B>::rows,mat_traits<B>::cols>::type R;
    R r;
    write_mat_element<0,0>(r,a/mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<0,1>(r,a/mat_traits<B>::template read_element<0,1>(b));
    write_mat_element<0,2>(r,a/mat_traits<B>::template read_element<0,2>(b));
    write_mat_element<1,0>(r,a/mat_traits<B>::template read_element<1,0>(b));
    write_mat_element<1,1>(r,a/mat_traits<B>::template read_element<1,1>(b));
    write_mat_element<1,2>(r,a/mat_traits<B>::template read_element<1,2>(b));
    write_mat_element<2,0>(r,a/mat_traits<B>::template read_element<2,0>(b));
    write_mat_element<2,1>(r,a/mat_traits<B>::template read_element<2,1>(b));
    write_mat_element<2,2>(r,a/mat_traits<B>::template read_element<2,2>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator/;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct div_sm_defined;

    template <>
    struct
    div_sm_defined<3,3>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==3 && mat_traits<A>::cols==1 && is_scalar<B>::value,
    deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols> >::type
operator/( A const & a, B b )
    {
    typedef typename deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols>::type R;
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a)/b);
    write_mat_element<1,0>(r,mat_traits<A>::template read_element<1,0>(a)/b);
    write_mat_element<2,0>(r,mat_traits<A>::template read_element<2,0>(a)/b);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator/;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct div_ms_defined;

    template <>
    struct
    div_ms_defined<3,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_scalar<A>::value && mat_traits<B>::rows==3 && mat_traits<B>::cols==1,
    deduce_mat2<A,B,mat_traits<B>::rows,mat_traits<B>::cols> >::type
operator/( A a, B const & b )
    {
    typedef typename deduce_mat2<A,B,mat_traits<B>::rows,mat_traits<B>::cols>::type R;
    R r;
    write_mat_element<0,0>(r,a/mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<1,0>(r,a/mat_traits<B>::template read_element<1,0>(b));
    write_mat_element<2,0>(r,a/mat_traits<B>::template read_element<2,0>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator/;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct div_sm_defined;

    template <>
    struct
    div_sm_defined<3,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==1 && mat_traits<A>::cols==3 && is_scalar<B>::value,
    deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols> >::type
operator/( A const & a, B b )
    {
    typedef typename deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols>::type R;
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a)/b);
    write_mat_element<0,1>(r,mat_traits<A>::template read_element<0,1>(a)/b);
    write_mat_element<0,2>(r,mat_traits<A>::template read_element<0,2>(a)/b);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator/;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct div_ms_defined;

    template <>
    struct
    div_ms_defined<1,3>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==3 && mat_traits<A>::cols==3 && is_scalar<B>::value,
    A &>::type
operator/=( A & a, B b )
    {
    write_mat_element<0,0>(a,mat_traits<A>::template read_element<0,0>(a)/b);
    write_mat_element<0,1>(a,mat_traits<A>::template read_element<0,1>(a)/b);
    write_mat_element<0,2>(a,mat_traits<A>::template read_element<0,2>(a)/b);
    write_mat_element<1,0>(a,mat_traits<A>::template read_element<1,0>(a)/b);
    write_mat_element<1,1>(a,mat_traits<A>::template read_element<1,1>(a)/b);
    write_mat_element<1,2>(a,mat_traits<A>::template read_element<1,2>(a)/b);
    write_mat_element<2,0>(a,mat_traits<A>::template read_element<2,0>(a)/b);
    write_mat_element<2,1>(a,mat_traits<A>::template read_element<2,1>(a)/b);
    write_mat_element<2,2>(a,mat_traits<A>::template read_element<2,2>(a)/b);
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator/=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct div_eq_ms_defined;

    template <>
    struct
    div_eq_ms_defined<3,3>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==3 && mat_traits<A>::cols==1 && is_scalar<B>::value,
    A &>::type
operator/=( A & a, B b )
    {
    write_mat_element<0,0>(a,mat_traits<A>::template read_element<0,0>(a)/b);
    write_mat_element<1,0>(a,mat_traits<A>::template read_element<1,0>(a)/b);
    write_mat_element<2,0>(a,mat_traits<A>::template read_element<2,0>(a)/b);
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator/=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct div_eq_ms_defined;

    template <>
    struct
    div_eq_ms_defined<3,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==1 && mat_traits<A>::cols==3 && is_scalar<B>::value,
    A &>::type
operator/=( A & a, B b )
    {
    write_mat_element<0,0>(a,mat_traits<A>::template read_element<0,0>(a)/b);
    write_mat_element<0,1>(a,mat_traits<A>::template read_element<0,1>(a)/b);
    write_mat_element<0,2>(a,mat_traits<A>::template read_element<0,2>(a)/b);
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator/=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct div_eq_ms_defined;

    template <>
    struct
    div_eq_ms_defined<1,3>
        {
        static bool const value=true;
        };
    }

template <class R,class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<R>::rows==3 && mat_traits<A>::rows==3 &&
    mat_traits<R>::cols==3 && mat_traits<A>::cols==3,
    R>::type
convert_to( A const & a )
    {
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a));
    write_mat_element<0,1>(r,mat_traits<A>::template read_element<0,1>(a));
    write_mat_element<0,2>(r,mat_traits<A>::template read_element<0,2>(a));
    write_mat_element<1,0>(r,mat_traits<A>::template read_element<1,0>(a));
    write_mat_element<1,1>(r,mat_traits<A>::template read_element<1,1>(a));
    write_mat_element<1,2>(r,mat_traits<A>::template read_element<1,2>(a));
    write_mat_element<2,0>(r,mat_traits<A>::template read_element<2,0>(a));
    write_mat_element<2,1>(r,mat_traits<A>::template read_element<2,1>(a));
    write_mat_element<2,2>(r,mat_traits<A>::template read_element<2,2>(a));
    return r;
    }

template <class R,class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE
typename enable_if_c<
    is_mat<R>::value && is_quat<A>::value &&
    mat_traits<R>::rows==3 && mat_traits<R>::cols==3,
    R>::type
convert_to( A const & q )
    {
    typedef typename mat_traits<R>::scalar_type T;
    T const a=quat_traits<A>::template read_element<0>(q);
    T const b=quat_traits<A>::template read_element<1>(q);
    T const c=quat_traits<A>::template read_element<2>(q);
    T const d=quat_traits<A>::template read_element<3>(q);
    T const bb = b*b;
    T const cc = c*c;
    T const dd = d*d;
    T const bc = b*c;
    T const bd = b*d;
    T const cd = c*d;
    T const ab = a*b;
    T const ac = a*c;
    T const ad = a*d;
    T const one = scalar_traits<T>::value(1);
    T const two = one+one;
    R r;
    write_mat_element<0,0>(r,one - two*(cc+dd));
    write_mat_element<0,1>(r,two*(bc-ad));
    write_mat_element<0,2>(r,two*(bd+ac));
    write_mat_element<1,0>(r,two*(bc+ad));
    write_mat_element<1,1>(r,one - two*(bb+dd));
    write_mat_element<1,2>(r,two*(cd-ab));
    write_mat_element<2,0>(r,two*(bd-ac));
    write_mat_element<2,1>(r,two*(cd+ab));
    write_mat_element<2,2>(r,one - two*(bb+cc));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::convert_to;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct convert_to_m_defined;

    template <>
    struct
    convert_to_m_defined<3,3>
        {
        static bool const value=true;
        };
    }

template <class R,class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<R>::rows==3 && mat_traits<A>::rows==3 &&
    mat_traits<R>::cols==1 && mat_traits<A>::cols==1,
    R>::type
convert_to( A const & a )
    {
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a));
    write_mat_element<1,0>(r,mat_traits<A>::template read_element<1,0>(a));
    write_mat_element<2,0>(r,mat_traits<A>::template read_element<2,0>(a));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::convert_to;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct convert_to_m_defined;

    template <>
    struct
    convert_to_m_defined<3,1>
        {
        static bool const value=true;
        };
    }

template <class R,class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<R>::rows==1 && mat_traits<A>::rows==1 &&
    mat_traits<R>::cols==3 && mat_traits<A>::cols==3,
    R>::type
convert_to( A const & a )
    {
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a));
    write_mat_element<0,1>(r,mat_traits<A>::template read_element<0,1>(a));
    write_mat_element<0,2>(r,mat_traits<A>::template read_element<0,2>(a));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::convert_to;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct convert_to_m_defined;

    template <>
    struct
    convert_to_m_defined<1,3>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==3 && mat_traits<B>::rows==3 &&
    mat_traits<A>::cols==3 && mat_traits<B>::cols==3,
    bool>::type
operator==( A const & a, B const & b )
    {
    return
        mat_traits<A>::template read_element<0,0>(a)==mat_traits<B>::template read_element<0,0>(b) &&
        mat_traits<A>::template read_element<0,1>(a)==mat_traits<B>::template read_element<0,1>(b) &&
        mat_traits<A>::template read_element<0,2>(a)==mat_traits<B>::template read_element<0,2>(b) &&
        mat_traits<A>::template read_element<1,0>(a)==mat_traits<B>::template read_element<1,0>(b) &&
        mat_traits<A>::template read_element<1,1>(a)==mat_traits<B>::template read_element<1,1>(b) &&
        mat_traits<A>::template read_element<1,2>(a)==mat_traits<B>::template read_element<1,2>(b) &&
        mat_traits<A>::template read_element<2,0>(a)==mat_traits<B>::template read_element<2,0>(b) &&
        mat_traits<A>::template read_element<2,1>(a)==mat_traits<B>::template read_element<2,1>(b) &&
        mat_traits<A>::template read_element<2,2>(a)==mat_traits<B>::template read_element<2,2>(b);
    }

namespace
sfinae
    {
    using ::boost::qvm::operator==;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct eq_mm_defined;

    template <>
    struct
    eq_mm_defined<3,3>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==3 && mat_traits<B>::rows==3 &&
    mat_traits<A>::cols==1 && mat_traits<B>::cols==1,
    bool>::type
operator==( A const & a, B const & b )
    {
    return
        mat_traits<A>::template read_element<0,0>(a)==mat_traits<B>::template read_element<0,0>(b) &&
        mat_traits<A>::template read_element<1,0>(a)==mat_traits<B>::template read_element<1,0>(b) &&
        mat_traits<A>::template read_element<2,0>(a)==mat_traits<B>::template read_element<2,0>(b);
    }

namespace
sfinae
    {
    using ::boost::qvm::operator==;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct eq_mm_defined;

    template <>
    struct
    eq_mm_defined<3,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==1 && mat_traits<B>::rows==1 &&
    mat_traits<A>::cols==3 && mat_traits<B>::cols==3,
    bool>::type
operator==( A const & a, B const & b )
    {
    return
        mat_traits<A>::template read_element<0,0>(a)==mat_traits<B>::template read_element<0,0>(b) &&
        mat_traits<A>::template read_element<0,1>(a)==mat_traits<B>::template read_element<0,1>(b) &&
        mat_traits<A>::template read_element<0,2>(a)==mat_traits<B>::template read_element<0,2>(b);
    }

namespace
sfinae
    {
    using ::boost::qvm::operator==;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct eq_mm_defined;

    template <>
    struct
    eq_mm_defined<1,3>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==3 && mat_traits<B>::rows==3 &&
    mat_traits<A>::cols==3 && mat_traits<B>::cols==3,
    bool>::type
operator!=( A const & a, B const & b )
    {
    return
        !(mat_traits<A>::template read_element<0,0>(a)==mat_traits<B>::template read_element<0,0>(b)) ||
        !(mat_traits<A>::template read_element<0,1>(a)==mat_traits<B>::template read_element<0,1>(b)) ||
        !(mat_traits<A>::template read_element<0,2>(a)==mat_traits<B>::template read_element<0,2>(b)) ||
        !(mat_traits<A>::template read_element<1,0>(a)==mat_traits<B>::template read_element<1,0>(b)) ||
        !(mat_traits<A>::template read_element<1,1>(a)==mat_traits<B>::template read_element<1,1>(b)) ||
        !(mat_traits<A>::template read_element<1,2>(a)==mat_traits<B>::template read_element<1,2>(b)) ||
        !(mat_traits<A>::template read_element<2,0>(a)==mat_traits<B>::template read_element<2,0>(b)) ||
        !(mat_traits<A>::template read_element<2,1>(a)==mat_traits<B>::template read_element<2,1>(b)) ||
        !(mat_traits<A>::template read_element<2,2>(a)==mat_traits<B>::template read_element<2,2>(b));
    }

namespace
sfinae
    {
    using ::boost::qvm::operator!=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct neq_mm_defined;

    template <>
    struct
    neq_mm_defined<3,3>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==3 && mat_traits<B>::rows==3 &&
    mat_traits<A>::cols==1 && mat_traits<B>::cols==1,
    bool>::type
operator!=( A const & a, B const & b )
    {
    return
        !(mat_traits<A>::template read_element<0,0>(a)==mat_traits<B>::template read_element<0,0>(b)) ||
        !(mat_traits<A>::template read_element<1,0>(a)==mat_traits<B>::template read_element<1,0>(b)) ||
        !(mat_traits<A>::template read_element<2,0>(a)==mat_traits<B>::template read_element<2,0>(b));
    }

namespace
sfinae
    {
    using ::boost::qvm::operator!=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct neq_mm_defined;

    template <>
    struct
    neq_mm_defined<3,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==1 && mat_traits<B>::rows==1 &&
    mat_traits<A>::cols==3 && mat_traits<B>::cols==3,
    bool>::type
operator!=( A const & a, B const & b )
    {
    return
        !(mat_traits<A>::template read_element<0,0>(a)==mat_traits<B>::template read_element<0,0>(b)) ||
        !(mat_traits<A>::template read_element<0,1>(a)==mat_traits<B>::template read_element<0,1>(b)) ||
        !(mat_traits<A>::template read_element<0,2>(a)==mat_traits<B>::template read_element<0,2>(b));
    }

namespace
sfinae
    {
    using ::boost::qvm::operator!=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct neq_mm_defined;

    template <>
    struct
    neq_mm_defined<1,3>
        {
        static bool const value=true;
        };
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==3 && mat_traits<A>::cols==3,
    deduce_mat<A> >::type
operator-( A const & a )
    {
    typedef typename deduce_mat<A>::type R;
    R r;
    write_mat_element<0,0>(r,-mat_traits<A>::template read_element<0,0>(a));
    write_mat_element<0,1>(r,-mat_traits<A>::template read_element<0,1>(a));
    write_mat_element<0,2>(r,-mat_traits<A>::template read_element<0,2>(a));
    write_mat_element<1,0>(r,-mat_traits<A>::template read_element<1,0>(a));
    write_mat_element<1,1>(r,-mat_traits<A>::template read_element<1,1>(a));
    write_mat_element<1,2>(r,-mat_traits<A>::template read_element<1,2>(a));
    write_mat_element<2,0>(r,-mat_traits<A>::template read_element<2,0>(a));
    write_mat_element<2,1>(r,-mat_traits<A>::template read_element<2,1>(a));
    write_mat_element<2,2>(r,-mat_traits<A>::template read_element<2,2>(a));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator-;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct minus_m_defined;

    template <>
    struct
    minus_m_defined<3,3>
        {
        static bool const value=true;
        };
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==3 && mat_traits<A>::cols==1,
    deduce_mat<A> >::type
operator-( A const & a )
    {
    typedef typename deduce_mat<A>::type R;
    R r;
    write_mat_element<0,0>(r,-mat_traits<A>::template read_element<0,0>(a));
    write_mat_element<1,0>(r,-mat_traits<A>::template read_element<1,0>(a));
    write_mat_element<2,0>(r,-mat_traits<A>::template read_element<2,0>(a));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator-;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct minus_m_defined;

    template <>
    struct
    minus_m_defined<3,1>
        {
        static bool const value=true;
        };
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==1 && mat_traits<A>::cols==3,
    deduce_mat<A> >::type
operator-( A const & a )
    {
    typedef typename deduce_mat<A>::type R;
    R r;
    write_mat_element<0,0>(r,-mat_traits<A>::template read_element<0,0>(a));
    write_mat_element<0,1>(r,-mat_traits<A>::template read_element<0,1>(a));
    write_mat_element<0,2>(r,-mat_traits<A>::template read_element<0,2>(a));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator-;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct minus_m_defined;

    template <>
    struct
    minus_m_defined<1,3>
        {
        static bool const value=true;
        };
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==3 && mat_traits<A>::cols==3,
    typename mat_traits<A>::scalar_type>::type
determinant( A const & a )
    {
    typedef typename mat_traits<A>::scalar_type T;
    T const a00=mat_traits<A>::template read_element<0,0>(a);
    T const a01=mat_traits<A>::template read_element<0,1>(a);
    T const a02=mat_traits<A>::template read_element<0,2>(a);
    T const a10=mat_traits<A>::template read_element<1,0>(a);
    T const a11=mat_traits<A>::template read_element<1,1>(a);
    T const a12=mat_traits<A>::template read_element<1,2>(a);
    T const a20=mat_traits<A>::template read_element<2,0>(a);
    T const a21=mat_traits<A>::template read_element<2,1>(a);
    T const a22=mat_traits<A>::template read_element<2,2>(a);
    T det=(a00*(a11*a22-a12*a21)-a01*(a10*a22-a12*a20)+a02*(a10*a21-a11*a20));
    return det;
    }

namespace
sfinae
    {
    using ::boost::qvm::determinant;
    }

namespace
qvm_detail
    {
    template <int D>
    struct determinant_defined;

    template <>
    struct
    determinant_defined<3>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==3 && mat_traits<A>::cols==3 && is_scalar<B>::value,
    deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols> >::type
inverse( A const & a, B det )
    {
    typedef typename mat_traits<A>::scalar_type T;
    BOOST_QVM_ASSERT(det!=scalar_traits<B>::value(0));
    T const a00=mat_traits<A>::template read_element<0,0>(a);
    T const a01=mat_traits<A>::template read_element<0,1>(a);
    T const a02=mat_traits<A>::template read_element<0,2>(a);
    T const a10=mat_traits<A>::template read_element<1,0>(a);
    T const a11=mat_traits<A>::template read_element<1,1>(a);
    T const a12=mat_traits<A>::template read_element<1,2>(a);
    T const a20=mat_traits<A>::template read_element<2,0>(a);
    T const a21=mat_traits<A>::template read_element<2,1>(a);
    T const a22=mat_traits<A>::template read_element<2,2>(a);
    T const f=scalar_traits<T>::value(1)/det;
    typedef typename deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols>::type R;
    R r;
    write_mat_element<0,0>(r, f*(a11*a22-a12*a21));
    write_mat_element<0,1>(r,-f*(a01*a22-a02*a21));
    write_mat_element<0,2>(r, f*(a01*a12-a02*a11));
    write_mat_element<1,0>(r,-f*(a10*a22-a12*a20));
    write_mat_element<1,1>(r, f*(a00*a22-a02*a20));
    write_mat_element<1,2>(r,-f*(a00*a12-a02*a10));
    write_mat_element<2,0>(r, f*(a10*a21-a11*a20));
    write_mat_element<2,1>(r,-f*(a00*a21-a01*a20));
    write_mat_element<2,2>(r, f*(a00*a11-a01*a10));
    return r;
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==3 && mat_traits<A>::cols==3,
    deduce_mat<A> >::type
inverse( A const & a )
    {
    typedef typename mat_traits<A>::scalar_type T;
    T det=determinant(a);
    if( det==scalar_traits<T>::value(0) )
        BOOST_QVM_THROW_EXCEPTION(zero_determinant_error());
    return inverse(a,det);
    }

namespace
sfinae
    {
    using ::boost::qvm::inverse;
    }

namespace
qvm_detail
    {
    template <int D>
    struct inverse_m_defined;

    template <>
    struct
    inverse_m_defined<3>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==3 && mat_traits<B>::rows==3 &&
    mat_traits<A>::cols==3 && mat_traits<B>::cols==3,
    deduce_mat2<A,B,3,3> >::type
operator*( A const & a, B const & b )
    {
    typedef typename mat_traits<A>::scalar_type Ta;
    typedef typename mat_traits<B>::scalar_type Tb;
    Ta const a00 = mat_traits<A>::template read_element<0,0>(a);
    Ta const a01 = mat_traits<A>::template read_element<0,1>(a);
    Ta const a02 = mat_traits<A>::template read_element<0,2>(a);
    Ta const a10 = mat_traits<A>::template read_element<1,0>(a);
    Ta const a11 = mat_traits<A>::template read_element<1,1>(a);
    Ta const a12 = mat_traits<A>::template read_element<1,2>(a);
    Ta const a20 = mat_traits<A>::template read_element<2,0>(a);
    Ta const a21 = mat_traits<A>::template read_element<2,1>(a);
    Ta const a22 = mat_traits<A>::template read_element<2,2>(a);
    Tb const b00 = mat_traits<B>::template read_element<0,0>(b);
    Tb const b01 = mat_traits<B>::template read_element<0,1>(b);
    Tb const b02 = mat_traits<B>::template read_element<0,2>(b);
    Tb const b10 = mat_traits<B>::template read_element<1,0>(b);
    Tb const b11 = mat_traits<B>::template read_element<1,1>(b);
    Tb const b12 = mat_traits<B>::template read_element<1,2>(b);
    Tb const b20 = mat_traits<B>::template read_element<2,0>(b);
    Tb const b21 = mat_traits<B>::template read_element<2,1>(b);
    Tb const b22 = mat_traits<B>::template read_element<2,2>(b);
    typedef typename deduce_mat2<A,B,3,3>::type R;
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::rows==3);
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::cols==3);
    R r;
    write_mat_element<0,0>(r,a00*b00+a01*b10+a02*b20);
    write_mat_element<0,1>(r,a00*b01+a01*b11+a02*b21);
    write_mat_element<0,2>(r,a00*b02+a01*b12+a02*b22);
    write_mat_element<1,0>(r,a10*b00+a11*b10+a12*b20);
    write_mat_element<1,1>(r,a10*b01+a11*b11+a12*b21);
    write_mat_element<1,2>(r,a10*b02+a11*b12+a12*b22);
    write_mat_element<2,0>(r,a20*b00+a21*b10+a22*b20);
    write_mat_element<2,1>(r,a20*b01+a21*b11+a22*b21);
    write_mat_element<2,2>(r,a20*b02+a21*b12+a22*b22);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

namespace
qvm_detail
    {
    template <int R,int /*CR*/,int C>
    struct mul_mm_defined;

    template <>
    struct
    mul_mm_defined<3,3,3>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==3 && mat_traits<B>::rows==3 &&
    mat_traits<A>::cols==3 && mat_traits<B>::cols==3,
    A &>::type
operator*=( A & a, B const & b )
    {
    typedef typename mat_traits<A>::scalar_type Ta;
    typedef typename mat_traits<B>::scalar_type Tb;
    Ta const a00 = mat_traits<A>::template read_element<0,0>(a);
    Ta const a01 = mat_traits<A>::template read_element<0,1>(a);
    Ta const a02 = mat_traits<A>::template read_element<0,2>(a);
    Ta const a10 = mat_traits<A>::template read_element<1,0>(a);
    Ta const a11 = mat_traits<A>::template read_element<1,1>(a);
    Ta const a12 = mat_traits<A>::template read_element<1,2>(a);
    Ta const a20 = mat_traits<A>::template read_element<2,0>(a);
    Ta const a21 = mat_traits<A>::template read_element<2,1>(a);
    Ta const a22 = mat_traits<A>::template read_element<2,2>(a);
    Tb const b00 = mat_traits<B>::template read_element<0,0>(b);
    Tb const b01 = mat_traits<B>::template read_element<0,1>(b);
    Tb const b02 = mat_traits<B>::template read_element<0,2>(b);
    Tb const b10 = mat_traits<B>::template read_element<1,0>(b);
    Tb const b11 = mat_traits<B>::template read_element<1,1>(b);
    Tb const b12 = mat_traits<B>::template read_element<1,2>(b);
    Tb const b20 = mat_traits<B>::template read_element<2,0>(b);
    Tb const b21 = mat_traits<B>::template read_element<2,1>(b);
    Tb const b22 = mat_traits<B>::template read_element<2,2>(b);
    write_mat_element<0,0>(a,a00*b00+a01*b10+a02*b20);
    write_mat_element<0,1>(a,a00*b01+a01*b11+a02*b21);
    write_mat_element<0,2>(a,a00*b02+a01*b12+a02*b22);
    write_mat_element<1,0>(a,a10*b00+a11*b10+a12*b20);
    write_mat_element<1,1>(a,a10*b01+a11*b11+a12*b21);
    write_mat_element<1,2>(a,a10*b02+a11*b12+a12*b22);
    write_mat_element<2,0>(a,a20*b00+a21*b10+a22*b20);
    write_mat_element<2,1>(a,a20*b01+a21*b11+a22*b21);
    write_mat_element<2,2>(a,a20*b02+a21*b12+a22*b22);
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*=;
    }

namespace
qvm_detail
    {
    template <int D>
    struct mul_eq_mm_defined;

    template <>
    struct
    mul_eq_mm_defined<3>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==3 && mat_traits<B>::rows==3 &&
    mat_traits<A>::cols==3 && mat_traits<B>::cols==1,
    deduce_mat2<A,B,3,1> >::type
operator*( A const & a, B const & b )
    {
    typedef typename mat_traits<A>::scalar_type Ta;
    typedef typename mat_traits<B>::scalar_type Tb;
    Ta const a00 = mat_traits<A>::template read_element<0,0>(a);
    Ta const a01 = mat_traits<A>::template read_element<0,1>(a);
    Ta const a02 = mat_traits<A>::template read_element<0,2>(a);
    Ta const a10 = mat_traits<A>::template read_element<1,0>(a);
    Ta const a11 = mat_traits<A>::template read_element<1,1>(a);
    Ta const a12 = mat_traits<A>::template read_element<1,2>(a);
    Ta const a20 = mat_traits<A>::template read_element<2,0>(a);
    Ta const a21 = mat_traits<A>::template read_element<2,1>(a);
    Ta const a22 = mat_traits<A>::template read_element<2,2>(a);
    Tb const b00 = mat_traits<B>::template read_element<0,0>(b);
    Tb const b10 = mat_traits<B>::template read_element<1,0>(b);
    Tb const b20 = mat_traits<B>::template read_element<2,0>(b);
    typedef typename deduce_mat2<A,B,3,1>::type R;
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::rows==3);
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::cols==1);
    R r;
    write_mat_element<0,0>(r,a00*b00+a01*b10+a02*b20);
    write_mat_element<1,0>(r,a10*b00+a11*b10+a12*b20);
    write_mat_element<2,0>(r,a20*b00+a21*b10+a22*b20);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

namespace
qvm_detail
    {
    template <int R,int /*CR*/,int C>
    struct mul_mm_defined;

    template <>
    struct
    mul_mm_defined<3,3,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==1 && mat_traits<B>::rows==3 &&
    mat_traits<A>::cols==3 && mat_traits<B>::cols==3,
    deduce_mat2<A,B,1,3> >::type
operator*( A const & a, B const & b )
    {
    typedef typename mat_traits<A>::scalar_type Ta;
    typedef typename mat_traits<B>::scalar_type Tb;
    Ta const a00 = mat_traits<A>::template read_element<0,0>(a);
    Ta const a01 = mat_traits<A>::template read_element<0,1>(a);
    Ta const a02 = mat_traits<A>::template read_element<0,2>(a);
    Tb const b00 = mat_traits<B>::template read_element<0,0>(b);
    Tb const b01 = mat_traits<B>::template read_element<0,1>(b);
    Tb const b02 = mat_traits<B>::template read_element<0,2>(b);
    Tb const b10 = mat_traits<B>::template read_element<1,0>(b);
    Tb const b11 = mat_traits<B>::template read_element<1,1>(b);
    Tb const b12 = mat_traits<B>::template read_element<1,2>(b);
    Tb const b20 = mat_traits<B>::template read_element<2,0>(b);
    Tb const b21 = mat_traits<B>::template read_element<2,1>(b);
    Tb const b22 = mat_traits<B>::template read_element<2,2>(b);
    typedef typename deduce_mat2<A,B,1,3>::type R;
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::rows==1);
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::cols==3);
    R r;
    write_mat_element<0,0>(r,a00*b00+a01*b10+a02*b20);
    write_mat_element<0,1>(r,a00*b01+a01*b11+a02*b21);
    write_mat_element<0,2>(r,a00*b02+a01*b12+a02*b22);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

namespace
qvm_detail
    {
    template <int R,int /*CR*/,int C>
    struct mul_mm_defined;

    template <>
    struct
    mul_mm_defined<1,3,3>
        {
        static bool const value=true;
        };
    }

} }

#endif
// <<< #include <boost/qvm/gen/mat_operations3.hpp>
// #line 6 "boost/qvm/mat_operations3.hpp"
// <<< #include <boost/qvm/mat_operations3.hpp>
// #line 12 "boost/qvm/mat_operations.hpp"
// >>> #include <boost/qvm/mat_operations4.hpp>
// #line 5 "boost/qvm/mat_operations4.hpp"
// >>> #include <boost/qvm/gen/mat_operations4.hpp>
#ifndef BOOST_QVM_GEN_MAT_OPERATIONS4_HPP_INCLUDED
#define BOOST_QVM_GEN_MAT_OPERATIONS4_HPP_INCLUDED

// #line 9 "boost/qvm/gen/mat_operations4.hpp"
// #include <boost/qvm/assert.hpp> // Expanded at line 887
// #include <boost/qvm/deduce_mat.hpp> // Expanded at line 899
// #include <boost/qvm/deduce_vec.hpp> // Expanded at line 1450
// #include <boost/qvm/error.hpp> // Expanded at line 17
// #include <boost/qvm/gen/mat_assign4.hpp> // Expanded at line 670
// #include <boost/qvm/quat_traits.hpp> // Expanded at line 914
// #include <boost/qvm/scalar_traits.hpp> // Expanded at line 909
// #include <boost/qvm/throw_exception.hpp> // Expanded at line 1546

namespace boost { namespace qvm {

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==4 && mat_traits<B>::rows==4 &&
    mat_traits<A>::cols==4 && mat_traits<B>::cols==4,
    deduce_mat2<A,B,4,4> >::type
operator+( A const & a, B const & b )
    {
    typedef typename deduce_mat2<A,B,4,4>::type R;
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::rows==4);
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::cols==4);
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a)+mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<0,1>(r,mat_traits<A>::template read_element<0,1>(a)+mat_traits<B>::template read_element<0,1>(b));
    write_mat_element<0,2>(r,mat_traits<A>::template read_element<0,2>(a)+mat_traits<B>::template read_element<0,2>(b));
    write_mat_element<0,3>(r,mat_traits<A>::template read_element<0,3>(a)+mat_traits<B>::template read_element<0,3>(b));
    write_mat_element<1,0>(r,mat_traits<A>::template read_element<1,0>(a)+mat_traits<B>::template read_element<1,0>(b));
    write_mat_element<1,1>(r,mat_traits<A>::template read_element<1,1>(a)+mat_traits<B>::template read_element<1,1>(b));
    write_mat_element<1,2>(r,mat_traits<A>::template read_element<1,2>(a)+mat_traits<B>::template read_element<1,2>(b));
    write_mat_element<1,3>(r,mat_traits<A>::template read_element<1,3>(a)+mat_traits<B>::template read_element<1,3>(b));
    write_mat_element<2,0>(r,mat_traits<A>::template read_element<2,0>(a)+mat_traits<B>::template read_element<2,0>(b));
    write_mat_element<2,1>(r,mat_traits<A>::template read_element<2,1>(a)+mat_traits<B>::template read_element<2,1>(b));
    write_mat_element<2,2>(r,mat_traits<A>::template read_element<2,2>(a)+mat_traits<B>::template read_element<2,2>(b));
    write_mat_element<2,3>(r,mat_traits<A>::template read_element<2,3>(a)+mat_traits<B>::template read_element<2,3>(b));
    write_mat_element<3,0>(r,mat_traits<A>::template read_element<3,0>(a)+mat_traits<B>::template read_element<3,0>(b));
    write_mat_element<3,1>(r,mat_traits<A>::template read_element<3,1>(a)+mat_traits<B>::template read_element<3,1>(b));
    write_mat_element<3,2>(r,mat_traits<A>::template read_element<3,2>(a)+mat_traits<B>::template read_element<3,2>(b));
    write_mat_element<3,3>(r,mat_traits<A>::template read_element<3,3>(a)+mat_traits<B>::template read_element<3,3>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator+;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct plus_mm_defined;

    template <>
    struct
    plus_mm_defined<4,4>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==4 && mat_traits<B>::rows==4 &&
    mat_traits<A>::cols==1 && mat_traits<B>::cols==1,
    deduce_mat2<A,B,4,1> >::type
operator+( A const & a, B const & b )
    {
    typedef typename deduce_mat2<A,B,4,1>::type R;
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::rows==4);
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::cols==1);
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a)+mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<1,0>(r,mat_traits<A>::template read_element<1,0>(a)+mat_traits<B>::template read_element<1,0>(b));
    write_mat_element<2,0>(r,mat_traits<A>::template read_element<2,0>(a)+mat_traits<B>::template read_element<2,0>(b));
    write_mat_element<3,0>(r,mat_traits<A>::template read_element<3,0>(a)+mat_traits<B>::template read_element<3,0>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator+;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct plus_mm_defined;

    template <>
    struct
    plus_mm_defined<4,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==1 && mat_traits<B>::rows==1 &&
    mat_traits<A>::cols==4 && mat_traits<B>::cols==4,
    deduce_mat2<A,B,1,4> >::type
operator+( A const & a, B const & b )
    {
    typedef typename deduce_mat2<A,B,1,4>::type R;
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::rows==1);
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::cols==4);
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a)+mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<0,1>(r,mat_traits<A>::template read_element<0,1>(a)+mat_traits<B>::template read_element<0,1>(b));
    write_mat_element<0,2>(r,mat_traits<A>::template read_element<0,2>(a)+mat_traits<B>::template read_element<0,2>(b));
    write_mat_element<0,3>(r,mat_traits<A>::template read_element<0,3>(a)+mat_traits<B>::template read_element<0,3>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator+;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct plus_mm_defined;

    template <>
    struct
    plus_mm_defined<1,4>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==4 && mat_traits<B>::rows==4 &&
    mat_traits<A>::cols==4 && mat_traits<B>::cols==4,
    deduce_mat2<A,B,4,4> >::type
operator-( A const & a, B const & b )
    {
    typedef typename deduce_mat2<A,B,4,4>::type R;
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::rows==4);
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::cols==4);
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a)-mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<0,1>(r,mat_traits<A>::template read_element<0,1>(a)-mat_traits<B>::template read_element<0,1>(b));
    write_mat_element<0,2>(r,mat_traits<A>::template read_element<0,2>(a)-mat_traits<B>::template read_element<0,2>(b));
    write_mat_element<0,3>(r,mat_traits<A>::template read_element<0,3>(a)-mat_traits<B>::template read_element<0,3>(b));
    write_mat_element<1,0>(r,mat_traits<A>::template read_element<1,0>(a)-mat_traits<B>::template read_element<1,0>(b));
    write_mat_element<1,1>(r,mat_traits<A>::template read_element<1,1>(a)-mat_traits<B>::template read_element<1,1>(b));
    write_mat_element<1,2>(r,mat_traits<A>::template read_element<1,2>(a)-mat_traits<B>::template read_element<1,2>(b));
    write_mat_element<1,3>(r,mat_traits<A>::template read_element<1,3>(a)-mat_traits<B>::template read_element<1,3>(b));
    write_mat_element<2,0>(r,mat_traits<A>::template read_element<2,0>(a)-mat_traits<B>::template read_element<2,0>(b));
    write_mat_element<2,1>(r,mat_traits<A>::template read_element<2,1>(a)-mat_traits<B>::template read_element<2,1>(b));
    write_mat_element<2,2>(r,mat_traits<A>::template read_element<2,2>(a)-mat_traits<B>::template read_element<2,2>(b));
    write_mat_element<2,3>(r,mat_traits<A>::template read_element<2,3>(a)-mat_traits<B>::template read_element<2,3>(b));
    write_mat_element<3,0>(r,mat_traits<A>::template read_element<3,0>(a)-mat_traits<B>::template read_element<3,0>(b));
    write_mat_element<3,1>(r,mat_traits<A>::template read_element<3,1>(a)-mat_traits<B>::template read_element<3,1>(b));
    write_mat_element<3,2>(r,mat_traits<A>::template read_element<3,2>(a)-mat_traits<B>::template read_element<3,2>(b));
    write_mat_element<3,3>(r,mat_traits<A>::template read_element<3,3>(a)-mat_traits<B>::template read_element<3,3>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator-;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct minus_mm_defined;

    template <>
    struct
    minus_mm_defined<4,4>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==4 && mat_traits<B>::rows==4 &&
    mat_traits<A>::cols==1 && mat_traits<B>::cols==1,
    deduce_mat2<A,B,4,1> >::type
operator-( A const & a, B const & b )
    {
    typedef typename deduce_mat2<A,B,4,1>::type R;
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::rows==4);
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::cols==1);
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a)-mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<1,0>(r,mat_traits<A>::template read_element<1,0>(a)-mat_traits<B>::template read_element<1,0>(b));
    write_mat_element<2,0>(r,mat_traits<A>::template read_element<2,0>(a)-mat_traits<B>::template read_element<2,0>(b));
    write_mat_element<3,0>(r,mat_traits<A>::template read_element<3,0>(a)-mat_traits<B>::template read_element<3,0>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator-;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct minus_mm_defined;

    template <>
    struct
    minus_mm_defined<4,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==1 && mat_traits<B>::rows==1 &&
    mat_traits<A>::cols==4 && mat_traits<B>::cols==4,
    deduce_mat2<A,B,1,4> >::type
operator-( A const & a, B const & b )
    {
    typedef typename deduce_mat2<A,B,1,4>::type R;
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::rows==1);
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::cols==4);
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a)-mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<0,1>(r,mat_traits<A>::template read_element<0,1>(a)-mat_traits<B>::template read_element<0,1>(b));
    write_mat_element<0,2>(r,mat_traits<A>::template read_element<0,2>(a)-mat_traits<B>::template read_element<0,2>(b));
    write_mat_element<0,3>(r,mat_traits<A>::template read_element<0,3>(a)-mat_traits<B>::template read_element<0,3>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator-;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct minus_mm_defined;

    template <>
    struct
    minus_mm_defined<1,4>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==4 && mat_traits<B>::rows==4 &&
    mat_traits<A>::cols==4 && mat_traits<B>::cols==4,
    A &>::type
operator+=( A & a, B const & b )
    {
    write_mat_element<0,0>(a,mat_traits<A>::template read_element<0,0>(a)+mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<0,1>(a,mat_traits<A>::template read_element<0,1>(a)+mat_traits<B>::template read_element<0,1>(b));
    write_mat_element<0,2>(a,mat_traits<A>::template read_element<0,2>(a)+mat_traits<B>::template read_element<0,2>(b));
    write_mat_element<0,3>(a,mat_traits<A>::template read_element<0,3>(a)+mat_traits<B>::template read_element<0,3>(b));
    write_mat_element<1,0>(a,mat_traits<A>::template read_element<1,0>(a)+mat_traits<B>::template read_element<1,0>(b));
    write_mat_element<1,1>(a,mat_traits<A>::template read_element<1,1>(a)+mat_traits<B>::template read_element<1,1>(b));
    write_mat_element<1,2>(a,mat_traits<A>::template read_element<1,2>(a)+mat_traits<B>::template read_element<1,2>(b));
    write_mat_element<1,3>(a,mat_traits<A>::template read_element<1,3>(a)+mat_traits<B>::template read_element<1,3>(b));
    write_mat_element<2,0>(a,mat_traits<A>::template read_element<2,0>(a)+mat_traits<B>::template read_element<2,0>(b));
    write_mat_element<2,1>(a,mat_traits<A>::template read_element<2,1>(a)+mat_traits<B>::template read_element<2,1>(b));
    write_mat_element<2,2>(a,mat_traits<A>::template read_element<2,2>(a)+mat_traits<B>::template read_element<2,2>(b));
    write_mat_element<2,3>(a,mat_traits<A>::template read_element<2,3>(a)+mat_traits<B>::template read_element<2,3>(b));
    write_mat_element<3,0>(a,mat_traits<A>::template read_element<3,0>(a)+mat_traits<B>::template read_element<3,0>(b));
    write_mat_element<3,1>(a,mat_traits<A>::template read_element<3,1>(a)+mat_traits<B>::template read_element<3,1>(b));
    write_mat_element<3,2>(a,mat_traits<A>::template read_element<3,2>(a)+mat_traits<B>::template read_element<3,2>(b));
    write_mat_element<3,3>(a,mat_traits<A>::template read_element<3,3>(a)+mat_traits<B>::template read_element<3,3>(b));
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator+=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct plus_eq_mm_defined;

    template <>
    struct
    plus_eq_mm_defined<4,4>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==4 && mat_traits<B>::rows==4 &&
    mat_traits<A>::cols==1 && mat_traits<B>::cols==1,
    A &>::type
operator+=( A & a, B const & b )
    {
    write_mat_element<0,0>(a,mat_traits<A>::template read_element<0,0>(a)+mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<1,0>(a,mat_traits<A>::template read_element<1,0>(a)+mat_traits<B>::template read_element<1,0>(b));
    write_mat_element<2,0>(a,mat_traits<A>::template read_element<2,0>(a)+mat_traits<B>::template read_element<2,0>(b));
    write_mat_element<3,0>(a,mat_traits<A>::template read_element<3,0>(a)+mat_traits<B>::template read_element<3,0>(b));
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator+=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct plus_eq_mm_defined;

    template <>
    struct
    plus_eq_mm_defined<4,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==1 && mat_traits<B>::rows==1 &&
    mat_traits<A>::cols==4 && mat_traits<B>::cols==4,
    A &>::type
operator+=( A & a, B const & b )
    {
    write_mat_element<0,0>(a,mat_traits<A>::template read_element<0,0>(a)+mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<0,1>(a,mat_traits<A>::template read_element<0,1>(a)+mat_traits<B>::template read_element<0,1>(b));
    write_mat_element<0,2>(a,mat_traits<A>::template read_element<0,2>(a)+mat_traits<B>::template read_element<0,2>(b));
    write_mat_element<0,3>(a,mat_traits<A>::template read_element<0,3>(a)+mat_traits<B>::template read_element<0,3>(b));
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator+=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct plus_eq_mm_defined;

    template <>
    struct
    plus_eq_mm_defined<1,4>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==4 && mat_traits<B>::rows==4 &&
    mat_traits<A>::cols==4 && mat_traits<B>::cols==4,
    A &>::type
operator-=( A & a, B const & b )
    {
    write_mat_element<0,0>(a,mat_traits<A>::template read_element<0,0>(a)-mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<0,1>(a,mat_traits<A>::template read_element<0,1>(a)-mat_traits<B>::template read_element<0,1>(b));
    write_mat_element<0,2>(a,mat_traits<A>::template read_element<0,2>(a)-mat_traits<B>::template read_element<0,2>(b));
    write_mat_element<0,3>(a,mat_traits<A>::template read_element<0,3>(a)-mat_traits<B>::template read_element<0,3>(b));
    write_mat_element<1,0>(a,mat_traits<A>::template read_element<1,0>(a)-mat_traits<B>::template read_element<1,0>(b));
    write_mat_element<1,1>(a,mat_traits<A>::template read_element<1,1>(a)-mat_traits<B>::template read_element<1,1>(b));
    write_mat_element<1,2>(a,mat_traits<A>::template read_element<1,2>(a)-mat_traits<B>::template read_element<1,2>(b));
    write_mat_element<1,3>(a,mat_traits<A>::template read_element<1,3>(a)-mat_traits<B>::template read_element<1,3>(b));
    write_mat_element<2,0>(a,mat_traits<A>::template read_element<2,0>(a)-mat_traits<B>::template read_element<2,0>(b));
    write_mat_element<2,1>(a,mat_traits<A>::template read_element<2,1>(a)-mat_traits<B>::template read_element<2,1>(b));
    write_mat_element<2,2>(a,mat_traits<A>::template read_element<2,2>(a)-mat_traits<B>::template read_element<2,2>(b));
    write_mat_element<2,3>(a,mat_traits<A>::template read_element<2,3>(a)-mat_traits<B>::template read_element<2,3>(b));
    write_mat_element<3,0>(a,mat_traits<A>::template read_element<3,0>(a)-mat_traits<B>::template read_element<3,0>(b));
    write_mat_element<3,1>(a,mat_traits<A>::template read_element<3,1>(a)-mat_traits<B>::template read_element<3,1>(b));
    write_mat_element<3,2>(a,mat_traits<A>::template read_element<3,2>(a)-mat_traits<B>::template read_element<3,2>(b));
    write_mat_element<3,3>(a,mat_traits<A>::template read_element<3,3>(a)-mat_traits<B>::template read_element<3,3>(b));
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator-=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct minus_eq_mm_defined;

    template <>
    struct
    minus_eq_mm_defined<4,4>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==4 && mat_traits<B>::rows==4 &&
    mat_traits<A>::cols==1 && mat_traits<B>::cols==1,
    A &>::type
operator-=( A & a, B const & b )
    {
    write_mat_element<0,0>(a,mat_traits<A>::template read_element<0,0>(a)-mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<1,0>(a,mat_traits<A>::template read_element<1,0>(a)-mat_traits<B>::template read_element<1,0>(b));
    write_mat_element<2,0>(a,mat_traits<A>::template read_element<2,0>(a)-mat_traits<B>::template read_element<2,0>(b));
    write_mat_element<3,0>(a,mat_traits<A>::template read_element<3,0>(a)-mat_traits<B>::template read_element<3,0>(b));
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator-=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct minus_eq_mm_defined;

    template <>
    struct
    minus_eq_mm_defined<4,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==1 && mat_traits<B>::rows==1 &&
    mat_traits<A>::cols==4 && mat_traits<B>::cols==4,
    A &>::type
operator-=( A & a, B const & b )
    {
    write_mat_element<0,0>(a,mat_traits<A>::template read_element<0,0>(a)-mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<0,1>(a,mat_traits<A>::template read_element<0,1>(a)-mat_traits<B>::template read_element<0,1>(b));
    write_mat_element<0,2>(a,mat_traits<A>::template read_element<0,2>(a)-mat_traits<B>::template read_element<0,2>(b));
    write_mat_element<0,3>(a,mat_traits<A>::template read_element<0,3>(a)-mat_traits<B>::template read_element<0,3>(b));
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator-=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct minus_eq_mm_defined;

    template <>
    struct
    minus_eq_mm_defined<1,4>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==4 && mat_traits<A>::cols==4 && is_scalar<B>::value,
    deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols> >::type
operator*( A const & a, B b )
    {
    typedef typename deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols>::type R;
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a)*b);
    write_mat_element<0,1>(r,mat_traits<A>::template read_element<0,1>(a)*b);
    write_mat_element<0,2>(r,mat_traits<A>::template read_element<0,2>(a)*b);
    write_mat_element<0,3>(r,mat_traits<A>::template read_element<0,3>(a)*b);
    write_mat_element<1,0>(r,mat_traits<A>::template read_element<1,0>(a)*b);
    write_mat_element<1,1>(r,mat_traits<A>::template read_element<1,1>(a)*b);
    write_mat_element<1,2>(r,mat_traits<A>::template read_element<1,2>(a)*b);
    write_mat_element<1,3>(r,mat_traits<A>::template read_element<1,3>(a)*b);
    write_mat_element<2,0>(r,mat_traits<A>::template read_element<2,0>(a)*b);
    write_mat_element<2,1>(r,mat_traits<A>::template read_element<2,1>(a)*b);
    write_mat_element<2,2>(r,mat_traits<A>::template read_element<2,2>(a)*b);
    write_mat_element<2,3>(r,mat_traits<A>::template read_element<2,3>(a)*b);
    write_mat_element<3,0>(r,mat_traits<A>::template read_element<3,0>(a)*b);
    write_mat_element<3,1>(r,mat_traits<A>::template read_element<3,1>(a)*b);
    write_mat_element<3,2>(r,mat_traits<A>::template read_element<3,2>(a)*b);
    write_mat_element<3,3>(r,mat_traits<A>::template read_element<3,3>(a)*b);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct mul_ms_defined;

    template <>
    struct
    mul_ms_defined<4,4>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_scalar<A>::value && mat_traits<B>::rows==4 && mat_traits<B>::cols==4,
    deduce_mat2<A,B,mat_traits<B>::rows,mat_traits<B>::cols> >::type
operator*( A a, B const & b )
    {
    typedef typename deduce_mat2<A,B,mat_traits<B>::rows,mat_traits<B>::cols>::type R;
    R r;
    write_mat_element<0,0>(r,a*mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<0,1>(r,a*mat_traits<B>::template read_element<0,1>(b));
    write_mat_element<0,2>(r,a*mat_traits<B>::template read_element<0,2>(b));
    write_mat_element<0,3>(r,a*mat_traits<B>::template read_element<0,3>(b));
    write_mat_element<1,0>(r,a*mat_traits<B>::template read_element<1,0>(b));
    write_mat_element<1,1>(r,a*mat_traits<B>::template read_element<1,1>(b));
    write_mat_element<1,2>(r,a*mat_traits<B>::template read_element<1,2>(b));
    write_mat_element<1,3>(r,a*mat_traits<B>::template read_element<1,3>(b));
    write_mat_element<2,0>(r,a*mat_traits<B>::template read_element<2,0>(b));
    write_mat_element<2,1>(r,a*mat_traits<B>::template read_element<2,1>(b));
    write_mat_element<2,2>(r,a*mat_traits<B>::template read_element<2,2>(b));
    write_mat_element<2,3>(r,a*mat_traits<B>::template read_element<2,3>(b));
    write_mat_element<3,0>(r,a*mat_traits<B>::template read_element<3,0>(b));
    write_mat_element<3,1>(r,a*mat_traits<B>::template read_element<3,1>(b));
    write_mat_element<3,2>(r,a*mat_traits<B>::template read_element<3,2>(b));
    write_mat_element<3,3>(r,a*mat_traits<B>::template read_element<3,3>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct mul_sm_defined;

    template <>
    struct
    mul_sm_defined<4,4>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==4 && mat_traits<A>::cols==1 && is_scalar<B>::value,
    deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols> >::type
operator*( A const & a, B b )
    {
    typedef typename deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols>::type R;
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a)*b);
    write_mat_element<1,0>(r,mat_traits<A>::template read_element<1,0>(a)*b);
    write_mat_element<2,0>(r,mat_traits<A>::template read_element<2,0>(a)*b);
    write_mat_element<3,0>(r,mat_traits<A>::template read_element<3,0>(a)*b);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct mul_ms_defined;

    template <>
    struct
    mul_ms_defined<4,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_scalar<A>::value && mat_traits<B>::rows==4 && mat_traits<B>::cols==1,
    deduce_mat2<A,B,mat_traits<B>::rows,mat_traits<B>::cols> >::type
operator*( A a, B const & b )
    {
    typedef typename deduce_mat2<A,B,mat_traits<B>::rows,mat_traits<B>::cols>::type R;
    R r;
    write_mat_element<0,0>(r,a*mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<1,0>(r,a*mat_traits<B>::template read_element<1,0>(b));
    write_mat_element<2,0>(r,a*mat_traits<B>::template read_element<2,0>(b));
    write_mat_element<3,0>(r,a*mat_traits<B>::template read_element<3,0>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct mul_sm_defined;

    template <>
    struct
    mul_sm_defined<4,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==1 && mat_traits<A>::cols==4 && is_scalar<B>::value,
    deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols> >::type
operator*( A const & a, B b )
    {
    typedef typename deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols>::type R;
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a)*b);
    write_mat_element<0,1>(r,mat_traits<A>::template read_element<0,1>(a)*b);
    write_mat_element<0,2>(r,mat_traits<A>::template read_element<0,2>(a)*b);
    write_mat_element<0,3>(r,mat_traits<A>::template read_element<0,3>(a)*b);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct mul_ms_defined;

    template <>
    struct
    mul_ms_defined<1,4>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_scalar<A>::value && mat_traits<B>::rows==1 && mat_traits<B>::cols==4,
    deduce_mat2<A,B,mat_traits<B>::rows,mat_traits<B>::cols> >::type
operator*( A a, B const & b )
    {
    typedef typename deduce_mat2<A,B,mat_traits<B>::rows,mat_traits<B>::cols>::type R;
    R r;
    write_mat_element<0,0>(r,a*mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<0,1>(r,a*mat_traits<B>::template read_element<0,1>(b));
    write_mat_element<0,2>(r,a*mat_traits<B>::template read_element<0,2>(b));
    write_mat_element<0,3>(r,a*mat_traits<B>::template read_element<0,3>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct mul_sm_defined;

    template <>
    struct
    mul_sm_defined<1,4>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==4 && mat_traits<A>::cols==4 && is_scalar<B>::value,
    A &>::type
operator*=( A & a, B b )
    {
    write_mat_element<0,0>(a,mat_traits<A>::template read_element<0,0>(a)*b);
    write_mat_element<0,1>(a,mat_traits<A>::template read_element<0,1>(a)*b);
    write_mat_element<0,2>(a,mat_traits<A>::template read_element<0,2>(a)*b);
    write_mat_element<0,3>(a,mat_traits<A>::template read_element<0,3>(a)*b);
    write_mat_element<1,0>(a,mat_traits<A>::template read_element<1,0>(a)*b);
    write_mat_element<1,1>(a,mat_traits<A>::template read_element<1,1>(a)*b);
    write_mat_element<1,2>(a,mat_traits<A>::template read_element<1,2>(a)*b);
    write_mat_element<1,3>(a,mat_traits<A>::template read_element<1,3>(a)*b);
    write_mat_element<2,0>(a,mat_traits<A>::template read_element<2,0>(a)*b);
    write_mat_element<2,1>(a,mat_traits<A>::template read_element<2,1>(a)*b);
    write_mat_element<2,2>(a,mat_traits<A>::template read_element<2,2>(a)*b);
    write_mat_element<2,3>(a,mat_traits<A>::template read_element<2,3>(a)*b);
    write_mat_element<3,0>(a,mat_traits<A>::template read_element<3,0>(a)*b);
    write_mat_element<3,1>(a,mat_traits<A>::template read_element<3,1>(a)*b);
    write_mat_element<3,2>(a,mat_traits<A>::template read_element<3,2>(a)*b);
    write_mat_element<3,3>(a,mat_traits<A>::template read_element<3,3>(a)*b);
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct mul_eq_ms_defined;

    template <>
    struct
    mul_eq_ms_defined<4,4>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==4 && mat_traits<A>::cols==1 && is_scalar<B>::value,
    A &>::type
operator*=( A & a, B b )
    {
    write_mat_element<0,0>(a,mat_traits<A>::template read_element<0,0>(a)*b);
    write_mat_element<1,0>(a,mat_traits<A>::template read_element<1,0>(a)*b);
    write_mat_element<2,0>(a,mat_traits<A>::template read_element<2,0>(a)*b);
    write_mat_element<3,0>(a,mat_traits<A>::template read_element<3,0>(a)*b);
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct mul_eq_ms_defined;

    template <>
    struct
    mul_eq_ms_defined<4,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==1 && mat_traits<A>::cols==4 && is_scalar<B>::value,
    A &>::type
operator*=( A & a, B b )
    {
    write_mat_element<0,0>(a,mat_traits<A>::template read_element<0,0>(a)*b);
    write_mat_element<0,1>(a,mat_traits<A>::template read_element<0,1>(a)*b);
    write_mat_element<0,2>(a,mat_traits<A>::template read_element<0,2>(a)*b);
    write_mat_element<0,3>(a,mat_traits<A>::template read_element<0,3>(a)*b);
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct mul_eq_ms_defined;

    template <>
    struct
    mul_eq_ms_defined<1,4>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==4 && mat_traits<A>::cols==4 && is_scalar<B>::value,
    deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols> >::type
operator/( A const & a, B b )
    {
    typedef typename deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols>::type R;
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a)/b);
    write_mat_element<0,1>(r,mat_traits<A>::template read_element<0,1>(a)/b);
    write_mat_element<0,2>(r,mat_traits<A>::template read_element<0,2>(a)/b);
    write_mat_element<0,3>(r,mat_traits<A>::template read_element<0,3>(a)/b);
    write_mat_element<1,0>(r,mat_traits<A>::template read_element<1,0>(a)/b);
    write_mat_element<1,1>(r,mat_traits<A>::template read_element<1,1>(a)/b);
    write_mat_element<1,2>(r,mat_traits<A>::template read_element<1,2>(a)/b);
    write_mat_element<1,3>(r,mat_traits<A>::template read_element<1,3>(a)/b);
    write_mat_element<2,0>(r,mat_traits<A>::template read_element<2,0>(a)/b);
    write_mat_element<2,1>(r,mat_traits<A>::template read_element<2,1>(a)/b);
    write_mat_element<2,2>(r,mat_traits<A>::template read_element<2,2>(a)/b);
    write_mat_element<2,3>(r,mat_traits<A>::template read_element<2,3>(a)/b);
    write_mat_element<3,0>(r,mat_traits<A>::template read_element<3,0>(a)/b);
    write_mat_element<3,1>(r,mat_traits<A>::template read_element<3,1>(a)/b);
    write_mat_element<3,2>(r,mat_traits<A>::template read_element<3,2>(a)/b);
    write_mat_element<3,3>(r,mat_traits<A>::template read_element<3,3>(a)/b);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator/;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct div_ms_defined;

    template <>
    struct
    div_ms_defined<4,4>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_scalar<A>::value && mat_traits<B>::rows==4 && mat_traits<B>::cols==4,
    deduce_mat2<A,B,mat_traits<B>::rows,mat_traits<B>::cols> >::type
operator/( A a, B const & b )
    {
    typedef typename deduce_mat2<A,B,mat_traits<B>::rows,mat_traits<B>::cols>::type R;
    R r;
    write_mat_element<0,0>(r,a/mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<0,1>(r,a/mat_traits<B>::template read_element<0,1>(b));
    write_mat_element<0,2>(r,a/mat_traits<B>::template read_element<0,2>(b));
    write_mat_element<0,3>(r,a/mat_traits<B>::template read_element<0,3>(b));
    write_mat_element<1,0>(r,a/mat_traits<B>::template read_element<1,0>(b));
    write_mat_element<1,1>(r,a/mat_traits<B>::template read_element<1,1>(b));
    write_mat_element<1,2>(r,a/mat_traits<B>::template read_element<1,2>(b));
    write_mat_element<1,3>(r,a/mat_traits<B>::template read_element<1,3>(b));
    write_mat_element<2,0>(r,a/mat_traits<B>::template read_element<2,0>(b));
    write_mat_element<2,1>(r,a/mat_traits<B>::template read_element<2,1>(b));
    write_mat_element<2,2>(r,a/mat_traits<B>::template read_element<2,2>(b));
    write_mat_element<2,3>(r,a/mat_traits<B>::template read_element<2,3>(b));
    write_mat_element<3,0>(r,a/mat_traits<B>::template read_element<3,0>(b));
    write_mat_element<3,1>(r,a/mat_traits<B>::template read_element<3,1>(b));
    write_mat_element<3,2>(r,a/mat_traits<B>::template read_element<3,2>(b));
    write_mat_element<3,3>(r,a/mat_traits<B>::template read_element<3,3>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator/;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct div_sm_defined;

    template <>
    struct
    div_sm_defined<4,4>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==4 && mat_traits<A>::cols==1 && is_scalar<B>::value,
    deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols> >::type
operator/( A const & a, B b )
    {
    typedef typename deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols>::type R;
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a)/b);
    write_mat_element<1,0>(r,mat_traits<A>::template read_element<1,0>(a)/b);
    write_mat_element<2,0>(r,mat_traits<A>::template read_element<2,0>(a)/b);
    write_mat_element<3,0>(r,mat_traits<A>::template read_element<3,0>(a)/b);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator/;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct div_ms_defined;

    template <>
    struct
    div_ms_defined<4,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_scalar<A>::value && mat_traits<B>::rows==4 && mat_traits<B>::cols==1,
    deduce_mat2<A,B,mat_traits<B>::rows,mat_traits<B>::cols> >::type
operator/( A a, B const & b )
    {
    typedef typename deduce_mat2<A,B,mat_traits<B>::rows,mat_traits<B>::cols>::type R;
    R r;
    write_mat_element<0,0>(r,a/mat_traits<B>::template read_element<0,0>(b));
    write_mat_element<1,0>(r,a/mat_traits<B>::template read_element<1,0>(b));
    write_mat_element<2,0>(r,a/mat_traits<B>::template read_element<2,0>(b));
    write_mat_element<3,0>(r,a/mat_traits<B>::template read_element<3,0>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator/;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct div_sm_defined;

    template <>
    struct
    div_sm_defined<4,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==1 && mat_traits<A>::cols==4 && is_scalar<B>::value,
    deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols> >::type
operator/( A const & a, B b )
    {
    typedef typename deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols>::type R;
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a)/b);
    write_mat_element<0,1>(r,mat_traits<A>::template read_element<0,1>(a)/b);
    write_mat_element<0,2>(r,mat_traits<A>::template read_element<0,2>(a)/b);
    write_mat_element<0,3>(r,mat_traits<A>::template read_element<0,3>(a)/b);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator/;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct div_ms_defined;

    template <>
    struct
    div_ms_defined<1,4>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==4 && mat_traits<A>::cols==4 && is_scalar<B>::value,
    A &>::type
operator/=( A & a, B b )
    {
    write_mat_element<0,0>(a,mat_traits<A>::template read_element<0,0>(a)/b);
    write_mat_element<0,1>(a,mat_traits<A>::template read_element<0,1>(a)/b);
    write_mat_element<0,2>(a,mat_traits<A>::template read_element<0,2>(a)/b);
    write_mat_element<0,3>(a,mat_traits<A>::template read_element<0,3>(a)/b);
    write_mat_element<1,0>(a,mat_traits<A>::template read_element<1,0>(a)/b);
    write_mat_element<1,1>(a,mat_traits<A>::template read_element<1,1>(a)/b);
    write_mat_element<1,2>(a,mat_traits<A>::template read_element<1,2>(a)/b);
    write_mat_element<1,3>(a,mat_traits<A>::template read_element<1,3>(a)/b);
    write_mat_element<2,0>(a,mat_traits<A>::template read_element<2,0>(a)/b);
    write_mat_element<2,1>(a,mat_traits<A>::template read_element<2,1>(a)/b);
    write_mat_element<2,2>(a,mat_traits<A>::template read_element<2,2>(a)/b);
    write_mat_element<2,3>(a,mat_traits<A>::template read_element<2,3>(a)/b);
    write_mat_element<3,0>(a,mat_traits<A>::template read_element<3,0>(a)/b);
    write_mat_element<3,1>(a,mat_traits<A>::template read_element<3,1>(a)/b);
    write_mat_element<3,2>(a,mat_traits<A>::template read_element<3,2>(a)/b);
    write_mat_element<3,3>(a,mat_traits<A>::template read_element<3,3>(a)/b);
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator/=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct div_eq_ms_defined;

    template <>
    struct
    div_eq_ms_defined<4,4>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==4 && mat_traits<A>::cols==1 && is_scalar<B>::value,
    A &>::type
operator/=( A & a, B b )
    {
    write_mat_element<0,0>(a,mat_traits<A>::template read_element<0,0>(a)/b);
    write_mat_element<1,0>(a,mat_traits<A>::template read_element<1,0>(a)/b);
    write_mat_element<2,0>(a,mat_traits<A>::template read_element<2,0>(a)/b);
    write_mat_element<3,0>(a,mat_traits<A>::template read_element<3,0>(a)/b);
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator/=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct div_eq_ms_defined;

    template <>
    struct
    div_eq_ms_defined<4,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==1 && mat_traits<A>::cols==4 && is_scalar<B>::value,
    A &>::type
operator/=( A & a, B b )
    {
    write_mat_element<0,0>(a,mat_traits<A>::template read_element<0,0>(a)/b);
    write_mat_element<0,1>(a,mat_traits<A>::template read_element<0,1>(a)/b);
    write_mat_element<0,2>(a,mat_traits<A>::template read_element<0,2>(a)/b);
    write_mat_element<0,3>(a,mat_traits<A>::template read_element<0,3>(a)/b);
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator/=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct div_eq_ms_defined;

    template <>
    struct
    div_eq_ms_defined<1,4>
        {
        static bool const value=true;
        };
    }

template <class R,class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<R>::rows==4 && mat_traits<A>::rows==4 &&
    mat_traits<R>::cols==4 && mat_traits<A>::cols==4,
    R>::type
convert_to( A const & a )
    {
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a));
    write_mat_element<0,1>(r,mat_traits<A>::template read_element<0,1>(a));
    write_mat_element<0,2>(r,mat_traits<A>::template read_element<0,2>(a));
    write_mat_element<0,3>(r,mat_traits<A>::template read_element<0,3>(a));
    write_mat_element<1,0>(r,mat_traits<A>::template read_element<1,0>(a));
    write_mat_element<1,1>(r,mat_traits<A>::template read_element<1,1>(a));
    write_mat_element<1,2>(r,mat_traits<A>::template read_element<1,2>(a));
    write_mat_element<1,3>(r,mat_traits<A>::template read_element<1,3>(a));
    write_mat_element<2,0>(r,mat_traits<A>::template read_element<2,0>(a));
    write_mat_element<2,1>(r,mat_traits<A>::template read_element<2,1>(a));
    write_mat_element<2,2>(r,mat_traits<A>::template read_element<2,2>(a));
    write_mat_element<2,3>(r,mat_traits<A>::template read_element<2,3>(a));
    write_mat_element<3,0>(r,mat_traits<A>::template read_element<3,0>(a));
    write_mat_element<3,1>(r,mat_traits<A>::template read_element<3,1>(a));
    write_mat_element<3,2>(r,mat_traits<A>::template read_element<3,2>(a));
    write_mat_element<3,3>(r,mat_traits<A>::template read_element<3,3>(a));
    return r;
    }

template <class R,class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE
typename enable_if_c<
    is_mat<R>::value && is_quat<A>::value &&
    mat_traits<R>::rows==4 && mat_traits<R>::cols==4,
    R>::type
convert_to( A const & q )
    {
    typedef typename mat_traits<R>::scalar_type T;
    T const a=quat_traits<A>::template read_element<0>(q);
    T const b=quat_traits<A>::template read_element<1>(q);
    T const c=quat_traits<A>::template read_element<2>(q);
    T const d=quat_traits<A>::template read_element<3>(q);
    T const bb = b*b;
    T const cc = c*c;
    T const dd = d*d;
    T const bc = b*c;
    T const bd = b*d;
    T const cd = c*d;
    T const ab = a*b;
    T const ac = a*c;
    T const ad = a*d;
    T const zero = scalar_traits<T>::value(0);
    T const one = scalar_traits<T>::value(1);
    T const two = one+one;
    R r;
    write_mat_element<0,0>(r,one - two*(cc+dd));
    write_mat_element<0,1>(r,two*(bc-ad));
    write_mat_element<0,2>(r,two*(bd+ac));
    write_mat_element<0,3>(r,zero);
    write_mat_element<1,0>(r,two*(bc+ad));
    write_mat_element<1,1>(r,one - two*(bb+dd));
    write_mat_element<1,2>(r,two*(cd-ab));
    write_mat_element<1,3>(r,zero);
    write_mat_element<2,0>(r,two*(bd-ac));
    write_mat_element<2,1>(r,two*(cd+ab));
    write_mat_element<2,2>(r,one - two*(bb+cc));
    write_mat_element<2,3>(r,zero);
    write_mat_element<3,0>(r,zero);
    write_mat_element<3,1>(r,zero);
    write_mat_element<3,2>(r,zero);
    write_mat_element<3,3>(r,one);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::convert_to;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct convert_to_m_defined;

    template <>
    struct
    convert_to_m_defined<4,4>
        {
        static bool const value=true;
        };
    }

template <class R,class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<R>::rows==4 && mat_traits<A>::rows==4 &&
    mat_traits<R>::cols==1 && mat_traits<A>::cols==1,
    R>::type
convert_to( A const & a )
    {
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a));
    write_mat_element<1,0>(r,mat_traits<A>::template read_element<1,0>(a));
    write_mat_element<2,0>(r,mat_traits<A>::template read_element<2,0>(a));
    write_mat_element<3,0>(r,mat_traits<A>::template read_element<3,0>(a));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::convert_to;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct convert_to_m_defined;

    template <>
    struct
    convert_to_m_defined<4,1>
        {
        static bool const value=true;
        };
    }

template <class R,class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<R>::rows==1 && mat_traits<A>::rows==1 &&
    mat_traits<R>::cols==4 && mat_traits<A>::cols==4,
    R>::type
convert_to( A const & a )
    {
    R r;
    write_mat_element<0,0>(r,mat_traits<A>::template read_element<0,0>(a));
    write_mat_element<0,1>(r,mat_traits<A>::template read_element<0,1>(a));
    write_mat_element<0,2>(r,mat_traits<A>::template read_element<0,2>(a));
    write_mat_element<0,3>(r,mat_traits<A>::template read_element<0,3>(a));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::convert_to;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct convert_to_m_defined;

    template <>
    struct
    convert_to_m_defined<1,4>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==4 && mat_traits<B>::rows==4 &&
    mat_traits<A>::cols==4 && mat_traits<B>::cols==4,
    bool>::type
operator==( A const & a, B const & b )
    {
    return
        mat_traits<A>::template read_element<0,0>(a)==mat_traits<B>::template read_element<0,0>(b) &&
        mat_traits<A>::template read_element<0,1>(a)==mat_traits<B>::template read_element<0,1>(b) &&
        mat_traits<A>::template read_element<0,2>(a)==mat_traits<B>::template read_element<0,2>(b) &&
        mat_traits<A>::template read_element<0,3>(a)==mat_traits<B>::template read_element<0,3>(b) &&
        mat_traits<A>::template read_element<1,0>(a)==mat_traits<B>::template read_element<1,0>(b) &&
        mat_traits<A>::template read_element<1,1>(a)==mat_traits<B>::template read_element<1,1>(b) &&
        mat_traits<A>::template read_element<1,2>(a)==mat_traits<B>::template read_element<1,2>(b) &&
        mat_traits<A>::template read_element<1,3>(a)==mat_traits<B>::template read_element<1,3>(b) &&
        mat_traits<A>::template read_element<2,0>(a)==mat_traits<B>::template read_element<2,0>(b) &&
        mat_traits<A>::template read_element<2,1>(a)==mat_traits<B>::template read_element<2,1>(b) &&
        mat_traits<A>::template read_element<2,2>(a)==mat_traits<B>::template read_element<2,2>(b) &&
        mat_traits<A>::template read_element<2,3>(a)==mat_traits<B>::template read_element<2,3>(b) &&
        mat_traits<A>::template read_element<3,0>(a)==mat_traits<B>::template read_element<3,0>(b) &&
        mat_traits<A>::template read_element<3,1>(a)==mat_traits<B>::template read_element<3,1>(b) &&
        mat_traits<A>::template read_element<3,2>(a)==mat_traits<B>::template read_element<3,2>(b) &&
        mat_traits<A>::template read_element<3,3>(a)==mat_traits<B>::template read_element<3,3>(b);
    }

namespace
sfinae
    {
    using ::boost::qvm::operator==;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct eq_mm_defined;

    template <>
    struct
    eq_mm_defined<4,4>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==4 && mat_traits<B>::rows==4 &&
    mat_traits<A>::cols==1 && mat_traits<B>::cols==1,
    bool>::type
operator==( A const & a, B const & b )
    {
    return
        mat_traits<A>::template read_element<0,0>(a)==mat_traits<B>::template read_element<0,0>(b) &&
        mat_traits<A>::template read_element<1,0>(a)==mat_traits<B>::template read_element<1,0>(b) &&
        mat_traits<A>::template read_element<2,0>(a)==mat_traits<B>::template read_element<2,0>(b) &&
        mat_traits<A>::template read_element<3,0>(a)==mat_traits<B>::template read_element<3,0>(b);
    }

namespace
sfinae
    {
    using ::boost::qvm::operator==;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct eq_mm_defined;

    template <>
    struct
    eq_mm_defined<4,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==1 && mat_traits<B>::rows==1 &&
    mat_traits<A>::cols==4 && mat_traits<B>::cols==4,
    bool>::type
operator==( A const & a, B const & b )
    {
    return
        mat_traits<A>::template read_element<0,0>(a)==mat_traits<B>::template read_element<0,0>(b) &&
        mat_traits<A>::template read_element<0,1>(a)==mat_traits<B>::template read_element<0,1>(b) &&
        mat_traits<A>::template read_element<0,2>(a)==mat_traits<B>::template read_element<0,2>(b) &&
        mat_traits<A>::template read_element<0,3>(a)==mat_traits<B>::template read_element<0,3>(b);
    }

namespace
sfinae
    {
    using ::boost::qvm::operator==;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct eq_mm_defined;

    template <>
    struct
    eq_mm_defined<1,4>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==4 && mat_traits<B>::rows==4 &&
    mat_traits<A>::cols==4 && mat_traits<B>::cols==4,
    bool>::type
operator!=( A const & a, B const & b )
    {
    return
        !(mat_traits<A>::template read_element<0,0>(a)==mat_traits<B>::template read_element<0,0>(b)) ||
        !(mat_traits<A>::template read_element<0,1>(a)==mat_traits<B>::template read_element<0,1>(b)) ||
        !(mat_traits<A>::template read_element<0,2>(a)==mat_traits<B>::template read_element<0,2>(b)) ||
        !(mat_traits<A>::template read_element<0,3>(a)==mat_traits<B>::template read_element<0,3>(b)) ||
        !(mat_traits<A>::template read_element<1,0>(a)==mat_traits<B>::template read_element<1,0>(b)) ||
        !(mat_traits<A>::template read_element<1,1>(a)==mat_traits<B>::template read_element<1,1>(b)) ||
        !(mat_traits<A>::template read_element<1,2>(a)==mat_traits<B>::template read_element<1,2>(b)) ||
        !(mat_traits<A>::template read_element<1,3>(a)==mat_traits<B>::template read_element<1,3>(b)) ||
        !(mat_traits<A>::template read_element<2,0>(a)==mat_traits<B>::template read_element<2,0>(b)) ||
        !(mat_traits<A>::template read_element<2,1>(a)==mat_traits<B>::template read_element<2,1>(b)) ||
        !(mat_traits<A>::template read_element<2,2>(a)==mat_traits<B>::template read_element<2,2>(b)) ||
        !(mat_traits<A>::template read_element<2,3>(a)==mat_traits<B>::template read_element<2,3>(b)) ||
        !(mat_traits<A>::template read_element<3,0>(a)==mat_traits<B>::template read_element<3,0>(b)) ||
        !(mat_traits<A>::template read_element<3,1>(a)==mat_traits<B>::template read_element<3,1>(b)) ||
        !(mat_traits<A>::template read_element<3,2>(a)==mat_traits<B>::template read_element<3,2>(b)) ||
        !(mat_traits<A>::template read_element<3,3>(a)==mat_traits<B>::template read_element<3,3>(b));
    }

namespace
sfinae
    {
    using ::boost::qvm::operator!=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct neq_mm_defined;

    template <>
    struct
    neq_mm_defined<4,4>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==4 && mat_traits<B>::rows==4 &&
    mat_traits<A>::cols==1 && mat_traits<B>::cols==1,
    bool>::type
operator!=( A const & a, B const & b )
    {
    return
        !(mat_traits<A>::template read_element<0,0>(a)==mat_traits<B>::template read_element<0,0>(b)) ||
        !(mat_traits<A>::template read_element<1,0>(a)==mat_traits<B>::template read_element<1,0>(b)) ||
        !(mat_traits<A>::template read_element<2,0>(a)==mat_traits<B>::template read_element<2,0>(b)) ||
        !(mat_traits<A>::template read_element<3,0>(a)==mat_traits<B>::template read_element<3,0>(b));
    }

namespace
sfinae
    {
    using ::boost::qvm::operator!=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct neq_mm_defined;

    template <>
    struct
    neq_mm_defined<4,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==1 && mat_traits<B>::rows==1 &&
    mat_traits<A>::cols==4 && mat_traits<B>::cols==4,
    bool>::type
operator!=( A const & a, B const & b )
    {
    return
        !(mat_traits<A>::template read_element<0,0>(a)==mat_traits<B>::template read_element<0,0>(b)) ||
        !(mat_traits<A>::template read_element<0,1>(a)==mat_traits<B>::template read_element<0,1>(b)) ||
        !(mat_traits<A>::template read_element<0,2>(a)==mat_traits<B>::template read_element<0,2>(b)) ||
        !(mat_traits<A>::template read_element<0,3>(a)==mat_traits<B>::template read_element<0,3>(b));
    }

namespace
sfinae
    {
    using ::boost::qvm::operator!=;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct neq_mm_defined;

    template <>
    struct
    neq_mm_defined<1,4>
        {
        static bool const value=true;
        };
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==4 && mat_traits<A>::cols==4,
    deduce_mat<A> >::type
operator-( A const & a )
    {
    typedef typename deduce_mat<A>::type R;
    R r;
    write_mat_element<0,0>(r,-mat_traits<A>::template read_element<0,0>(a));
    write_mat_element<0,1>(r,-mat_traits<A>::template read_element<0,1>(a));
    write_mat_element<0,2>(r,-mat_traits<A>::template read_element<0,2>(a));
    write_mat_element<0,3>(r,-mat_traits<A>::template read_element<0,3>(a));
    write_mat_element<1,0>(r,-mat_traits<A>::template read_element<1,0>(a));
    write_mat_element<1,1>(r,-mat_traits<A>::template read_element<1,1>(a));
    write_mat_element<1,2>(r,-mat_traits<A>::template read_element<1,2>(a));
    write_mat_element<1,3>(r,-mat_traits<A>::template read_element<1,3>(a));
    write_mat_element<2,0>(r,-mat_traits<A>::template read_element<2,0>(a));
    write_mat_element<2,1>(r,-mat_traits<A>::template read_element<2,1>(a));
    write_mat_element<2,2>(r,-mat_traits<A>::template read_element<2,2>(a));
    write_mat_element<2,3>(r,-mat_traits<A>::template read_element<2,3>(a));
    write_mat_element<3,0>(r,-mat_traits<A>::template read_element<3,0>(a));
    write_mat_element<3,1>(r,-mat_traits<A>::template read_element<3,1>(a));
    write_mat_element<3,2>(r,-mat_traits<A>::template read_element<3,2>(a));
    write_mat_element<3,3>(r,-mat_traits<A>::template read_element<3,3>(a));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator-;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct minus_m_defined;

    template <>
    struct
    minus_m_defined<4,4>
        {
        static bool const value=true;
        };
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==4 && mat_traits<A>::cols==1,
    deduce_mat<A> >::type
operator-( A const & a )
    {
    typedef typename deduce_mat<A>::type R;
    R r;
    write_mat_element<0,0>(r,-mat_traits<A>::template read_element<0,0>(a));
    write_mat_element<1,0>(r,-mat_traits<A>::template read_element<1,0>(a));
    write_mat_element<2,0>(r,-mat_traits<A>::template read_element<2,0>(a));
    write_mat_element<3,0>(r,-mat_traits<A>::template read_element<3,0>(a));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator-;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct minus_m_defined;

    template <>
    struct
    minus_m_defined<4,1>
        {
        static bool const value=true;
        };
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==1 && mat_traits<A>::cols==4,
    deduce_mat<A> >::type
operator-( A const & a )
    {
    typedef typename deduce_mat<A>::type R;
    R r;
    write_mat_element<0,0>(r,-mat_traits<A>::template read_element<0,0>(a));
    write_mat_element<0,1>(r,-mat_traits<A>::template read_element<0,1>(a));
    write_mat_element<0,2>(r,-mat_traits<A>::template read_element<0,2>(a));
    write_mat_element<0,3>(r,-mat_traits<A>::template read_element<0,3>(a));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator-;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct minus_m_defined;

    template <>
    struct
    minus_m_defined<1,4>
        {
        static bool const value=true;
        };
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==4 && mat_traits<A>::cols==4,
    typename mat_traits<A>::scalar_type>::type
determinant( A const & a )
    {
    typedef typename mat_traits<A>::scalar_type T;
    T const a00=mat_traits<A>::template read_element<0,0>(a);
    T const a01=mat_traits<A>::template read_element<0,1>(a);
    T const a02=mat_traits<A>::template read_element<0,2>(a);
    T const a03=mat_traits<A>::template read_element<0,3>(a);
    T const a10=mat_traits<A>::template read_element<1,0>(a);
    T const a11=mat_traits<A>::template read_element<1,1>(a);
    T const a12=mat_traits<A>::template read_element<1,2>(a);
    T const a13=mat_traits<A>::template read_element<1,3>(a);
    T const a20=mat_traits<A>::template read_element<2,0>(a);
    T const a21=mat_traits<A>::template read_element<2,1>(a);
    T const a22=mat_traits<A>::template read_element<2,2>(a);
    T const a23=mat_traits<A>::template read_element<2,3>(a);
    T const a30=mat_traits<A>::template read_element<3,0>(a);
    T const a31=mat_traits<A>::template read_element<3,1>(a);
    T const a32=mat_traits<A>::template read_element<3,2>(a);
    T const a33=mat_traits<A>::template read_element<3,3>(a);
    T det=(a00*(a11*(a22*a33-a23*a32)-a12*(a21*a33-a23*a31)+a13*(a21*a32-a22*a31))-a01*(a10*(a22*a33-a23*a32)-a12*(a20*a33-a23*a30)+a13*(a20*a32-a22*a30))+a02*(a10*(a21*a33-a23*a31)-a11*(a20*a33-a23*a30)+a13*(a20*a31-a21*a30))-a03*(a10*(a21*a32-a22*a31)-a11*(a20*a32-a22*a30)+a12*(a20*a31-a21*a30)));
    return det;
    }

namespace
sfinae
    {
    using ::boost::qvm::determinant;
    }

namespace
qvm_detail
    {
    template <int D>
    struct determinant_defined;

    template <>
    struct
    determinant_defined<4>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==4 && mat_traits<A>::cols==4 && is_scalar<B>::value,
    deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols> >::type
inverse( A const & a, B det )
    {
    typedef typename mat_traits<A>::scalar_type T;
    BOOST_QVM_ASSERT(det!=scalar_traits<B>::value(0));
    T const a00=mat_traits<A>::template read_element<0,0>(a);
    T const a01=mat_traits<A>::template read_element<0,1>(a);
    T const a02=mat_traits<A>::template read_element<0,2>(a);
    T const a03=mat_traits<A>::template read_element<0,3>(a);
    T const a10=mat_traits<A>::template read_element<1,0>(a);
    T const a11=mat_traits<A>::template read_element<1,1>(a);
    T const a12=mat_traits<A>::template read_element<1,2>(a);
    T const a13=mat_traits<A>::template read_element<1,3>(a);
    T const a20=mat_traits<A>::template read_element<2,0>(a);
    T const a21=mat_traits<A>::template read_element<2,1>(a);
    T const a22=mat_traits<A>::template read_element<2,2>(a);
    T const a23=mat_traits<A>::template read_element<2,3>(a);
    T const a30=mat_traits<A>::template read_element<3,0>(a);
    T const a31=mat_traits<A>::template read_element<3,1>(a);
    T const a32=mat_traits<A>::template read_element<3,2>(a);
    T const a33=mat_traits<A>::template read_element<3,3>(a);
    T const f=scalar_traits<T>::value(1)/det;
    typedef typename deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols>::type R;
    R r;
    write_mat_element<0,0>(r, f*(a11*(a22*a33-a23*a32)-a12*(a21*a33-a23*a31)+a13*(a21*a32-a22*a31)));
    write_mat_element<0,1>(r,-f*(a01*(a22*a33-a23*a32)-a02*(a21*a33-a23*a31)+a03*(a21*a32-a22*a31)));
    write_mat_element<0,2>(r, f*(a01*(a12*a33-a13*a32)-a02*(a11*a33-a13*a31)+a03*(a11*a32-a12*a31)));
    write_mat_element<0,3>(r,-f*(a01*(a12*a23-a13*a22)-a02*(a11*a23-a13*a21)+a03*(a11*a22-a12*a21)));
    write_mat_element<1,0>(r,-f*(a10*(a22*a33-a23*a32)-a12*(a20*a33-a23*a30)+a13*(a20*a32-a22*a30)));
    write_mat_element<1,1>(r, f*(a00*(a22*a33-a23*a32)-a02*(a20*a33-a23*a30)+a03*(a20*a32-a22*a30)));
    write_mat_element<1,2>(r,-f*(a00*(a12*a33-a13*a32)-a02*(a10*a33-a13*a30)+a03*(a10*a32-a12*a30)));
    write_mat_element<1,3>(r, f*(a00*(a12*a23-a13*a22)-a02*(a10*a23-a13*a20)+a03*(a10*a22-a12*a20)));
    write_mat_element<2,0>(r, f*(a10*(a21*a33-a23*a31)-a11*(a20*a33-a23*a30)+a13*(a20*a31-a21*a30)));
    write_mat_element<2,1>(r,-f*(a00*(a21*a33-a23*a31)-a01*(a20*a33-a23*a30)+a03*(a20*a31-a21*a30)));
    write_mat_element<2,2>(r, f*(a00*(a11*a33-a13*a31)-a01*(a10*a33-a13*a30)+a03*(a10*a31-a11*a30)));
    write_mat_element<2,3>(r,-f*(a00*(a11*a23-a13*a21)-a01*(a10*a23-a13*a20)+a03*(a10*a21-a11*a20)));
    write_mat_element<3,0>(r,-f*(a10*(a21*a32-a22*a31)-a11*(a20*a32-a22*a30)+a12*(a20*a31-a21*a30)));
    write_mat_element<3,1>(r, f*(a00*(a21*a32-a22*a31)-a01*(a20*a32-a22*a30)+a02*(a20*a31-a21*a30)));
    write_mat_element<3,2>(r,-f*(a00*(a11*a32-a12*a31)-a01*(a10*a32-a12*a30)+a02*(a10*a31-a11*a30)));
    write_mat_element<3,3>(r, f*(a00*(a11*a22-a12*a21)-a01*(a10*a22-a12*a20)+a02*(a10*a21-a11*a20)));
    return r;
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==4 && mat_traits<A>::cols==4,
    deduce_mat<A> >::type
inverse( A const & a )
    {
    typedef typename mat_traits<A>::scalar_type T;
    T det=determinant(a);
    if( det==scalar_traits<T>::value(0) )
        BOOST_QVM_THROW_EXCEPTION(zero_determinant_error());
    return inverse(a,det);
    }

namespace
sfinae
    {
    using ::boost::qvm::inverse;
    }

namespace
qvm_detail
    {
    template <int D>
    struct inverse_m_defined;

    template <>
    struct
    inverse_m_defined<4>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==4 && mat_traits<B>::rows==4 &&
    mat_traits<A>::cols==4 && mat_traits<B>::cols==4,
    deduce_mat2<A,B,4,4> >::type
operator*( A const & a, B const & b )
    {
    typedef typename mat_traits<A>::scalar_type Ta;
    typedef typename mat_traits<B>::scalar_type Tb;
    Ta const a00 = mat_traits<A>::template read_element<0,0>(a);
    Ta const a01 = mat_traits<A>::template read_element<0,1>(a);
    Ta const a02 = mat_traits<A>::template read_element<0,2>(a);
    Ta const a03 = mat_traits<A>::template read_element<0,3>(a);
    Ta const a10 = mat_traits<A>::template read_element<1,0>(a);
    Ta const a11 = mat_traits<A>::template read_element<1,1>(a);
    Ta const a12 = mat_traits<A>::template read_element<1,2>(a);
    Ta const a13 = mat_traits<A>::template read_element<1,3>(a);
    Ta const a20 = mat_traits<A>::template read_element<2,0>(a);
    Ta const a21 = mat_traits<A>::template read_element<2,1>(a);
    Ta const a22 = mat_traits<A>::template read_element<2,2>(a);
    Ta const a23 = mat_traits<A>::template read_element<2,3>(a);
    Ta const a30 = mat_traits<A>::template read_element<3,0>(a);
    Ta const a31 = mat_traits<A>::template read_element<3,1>(a);
    Ta const a32 = mat_traits<A>::template read_element<3,2>(a);
    Ta const a33 = mat_traits<A>::template read_element<3,3>(a);
    Tb const b00 = mat_traits<B>::template read_element<0,0>(b);
    Tb const b01 = mat_traits<B>::template read_element<0,1>(b);
    Tb const b02 = mat_traits<B>::template read_element<0,2>(b);
    Tb const b03 = mat_traits<B>::template read_element<0,3>(b);
    Tb const b10 = mat_traits<B>::template read_element<1,0>(b);
    Tb const b11 = mat_traits<B>::template read_element<1,1>(b);
    Tb const b12 = mat_traits<B>::template read_element<1,2>(b);
    Tb const b13 = mat_traits<B>::template read_element<1,3>(b);
    Tb const b20 = mat_traits<B>::template read_element<2,0>(b);
    Tb const b21 = mat_traits<B>::template read_element<2,1>(b);
    Tb const b22 = mat_traits<B>::template read_element<2,2>(b);
    Tb const b23 = mat_traits<B>::template read_element<2,3>(b);
    Tb const b30 = mat_traits<B>::template read_element<3,0>(b);
    Tb const b31 = mat_traits<B>::template read_element<3,1>(b);
    Tb const b32 = mat_traits<B>::template read_element<3,2>(b);
    Tb const b33 = mat_traits<B>::template read_element<3,3>(b);
    typedef typename deduce_mat2<A,B,4,4>::type R;
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::rows==4);
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::cols==4);
    R r;
    write_mat_element<0,0>(r,a00*b00+a01*b10+a02*b20+a03*b30);
    write_mat_element<0,1>(r,a00*b01+a01*b11+a02*b21+a03*b31);
    write_mat_element<0,2>(r,a00*b02+a01*b12+a02*b22+a03*b32);
    write_mat_element<0,3>(r,a00*b03+a01*b13+a02*b23+a03*b33);
    write_mat_element<1,0>(r,a10*b00+a11*b10+a12*b20+a13*b30);
    write_mat_element<1,1>(r,a10*b01+a11*b11+a12*b21+a13*b31);
    write_mat_element<1,2>(r,a10*b02+a11*b12+a12*b22+a13*b32);
    write_mat_element<1,3>(r,a10*b03+a11*b13+a12*b23+a13*b33);
    write_mat_element<2,0>(r,a20*b00+a21*b10+a22*b20+a23*b30);
    write_mat_element<2,1>(r,a20*b01+a21*b11+a22*b21+a23*b31);
    write_mat_element<2,2>(r,a20*b02+a21*b12+a22*b22+a23*b32);
    write_mat_element<2,3>(r,a20*b03+a21*b13+a22*b23+a23*b33);
    write_mat_element<3,0>(r,a30*b00+a31*b10+a32*b20+a33*b30);
    write_mat_element<3,1>(r,a30*b01+a31*b11+a32*b21+a33*b31);
    write_mat_element<3,2>(r,a30*b02+a31*b12+a32*b22+a33*b32);
    write_mat_element<3,3>(r,a30*b03+a31*b13+a32*b23+a33*b33);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

namespace
qvm_detail
    {
    template <int R,int /*CR*/,int C>
    struct mul_mm_defined;

    template <>
    struct
    mul_mm_defined<4,4,4>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    mat_traits<A>::rows==4 && mat_traits<B>::rows==4 &&
    mat_traits<A>::cols==4 && mat_traits<B>::cols==4,
    A &>::type
operator*=( A & a, B const & b )
    {
    typedef typename mat_traits<A>::scalar_type Ta;
    typedef typename mat_traits<B>::scalar_type Tb;
    Ta const a00 = mat_traits<A>::template read_element<0,0>(a);
    Ta const a01 = mat_traits<A>::template read_element<0,1>(a);
    Ta const a02 = mat_traits<A>::template read_element<0,2>(a);
    Ta const a03 = mat_traits<A>::template read_element<0,3>(a);
    Ta const a10 = mat_traits<A>::template read_element<1,0>(a);
    Ta const a11 = mat_traits<A>::template read_element<1,1>(a);
    Ta const a12 = mat_traits<A>::template read_element<1,2>(a);
    Ta const a13 = mat_traits<A>::template read_element<1,3>(a);
    Ta const a20 = mat_traits<A>::template read_element<2,0>(a);
    Ta const a21 = mat_traits<A>::template read_element<2,1>(a);
    Ta const a22 = mat_traits<A>::template read_element<2,2>(a);
    Ta const a23 = mat_traits<A>::template read_element<2,3>(a);
    Ta const a30 = mat_traits<A>::template read_element<3,0>(a);
    Ta const a31 = mat_traits<A>::template read_element<3,1>(a);
    Ta const a32 = mat_traits<A>::template read_element<3,2>(a);
    Ta const a33 = mat_traits<A>::template read_element<3,3>(a);
    Tb const b00 = mat_traits<B>::template read_element<0,0>(b);
    Tb const b01 = mat_traits<B>::template read_element<0,1>(b);
    Tb const b02 = mat_traits<B>::template read_element<0,2>(b);
    Tb const b03 = mat_traits<B>::template read_element<0,3>(b);
    Tb const b10 = mat_traits<B>::template read_element<1,0>(b);
    Tb const b11 = mat_traits<B>::template read_element<1,1>(b);
    Tb const b12 = mat_traits<B>::template read_element<1,2>(b);
    Tb const b13 = mat_traits<B>::template read_element<1,3>(b);
    Tb const b20 = mat_traits<B>::template read_element<2,0>(b);
    Tb const b21 = mat_traits<B>::template read_element<2,1>(b);
    Tb const b22 = mat_traits<B>::template read_element<2,2>(b);
    Tb const b23 = mat_traits<B>::template read_element<2,3>(b);
    Tb const b30 = mat_traits<B>::template read_element<3,0>(b);
    Tb const b31 = mat_traits<B>::template read_element<3,1>(b);
    Tb const b32 = mat_traits<B>::template read_element<3,2>(b);
    Tb const b33 = mat_traits<B>::template read_element<3,3>(b);
    write_mat_element<0,0>(a,a00*b00+a01*b10+a02*b20+a03*b30);
    write_mat_element<0,1>(a,a00*b01+a01*b11+a02*b21+a03*b31);
    write_mat_element<0,2>(a,a00*b02+a01*b12+a02*b22+a03*b32);
    write_mat_element<0,3>(a,a00*b03+a01*b13+a02*b23+a03*b33);
    write_mat_element<1,0>(a,a10*b00+a11*b10+a12*b20+a13*b30);
    write_mat_element<1,1>(a,a10*b01+a11*b11+a12*b21+a13*b31);
    write_mat_element<1,2>(a,a10*b02+a11*b12+a12*b22+a13*b32);
    write_mat_element<1,3>(a,a10*b03+a11*b13+a12*b23+a13*b33);
    write_mat_element<2,0>(a,a20*b00+a21*b10+a22*b20+a23*b30);
    write_mat_element<2,1>(a,a20*b01+a21*b11+a22*b21+a23*b31);
    write_mat_element<2,2>(a,a20*b02+a21*b12+a22*b22+a23*b32);
    write_mat_element<2,3>(a,a20*b03+a21*b13+a22*b23+a23*b33);
    write_mat_element<3,0>(a,a30*b00+a31*b10+a32*b20+a33*b30);
    write_mat_element<3,1>(a,a30*b01+a31*b11+a32*b21+a33*b31);
    write_mat_element<3,2>(a,a30*b02+a31*b12+a32*b22+a33*b32);
    write_mat_element<3,3>(a,a30*b03+a31*b13+a32*b23+a33*b33);
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*=;
    }

namespace
qvm_detail
    {
    template <int D>
    struct mul_eq_mm_defined;

    template <>
    struct
    mul_eq_mm_defined<4>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==4 && mat_traits<B>::rows==4 &&
    mat_traits<A>::cols==4 && mat_traits<B>::cols==1,
    deduce_mat2<A,B,4,1> >::type
operator*( A const & a, B const & b )
    {
    typedef typename mat_traits<A>::scalar_type Ta;
    typedef typename mat_traits<B>::scalar_type Tb;
    Ta const a00 = mat_traits<A>::template read_element<0,0>(a);
    Ta const a01 = mat_traits<A>::template read_element<0,1>(a);
    Ta const a02 = mat_traits<A>::template read_element<0,2>(a);
    Ta const a03 = mat_traits<A>::template read_element<0,3>(a);
    Ta const a10 = mat_traits<A>::template read_element<1,0>(a);
    Ta const a11 = mat_traits<A>::template read_element<1,1>(a);
    Ta const a12 = mat_traits<A>::template read_element<1,2>(a);
    Ta const a13 = mat_traits<A>::template read_element<1,3>(a);
    Ta const a20 = mat_traits<A>::template read_element<2,0>(a);
    Ta const a21 = mat_traits<A>::template read_element<2,1>(a);
    Ta const a22 = mat_traits<A>::template read_element<2,2>(a);
    Ta const a23 = mat_traits<A>::template read_element<2,3>(a);
    Ta const a30 = mat_traits<A>::template read_element<3,0>(a);
    Ta const a31 = mat_traits<A>::template read_element<3,1>(a);
    Ta const a32 = mat_traits<A>::template read_element<3,2>(a);
    Ta const a33 = mat_traits<A>::template read_element<3,3>(a);
    Tb const b00 = mat_traits<B>::template read_element<0,0>(b);
    Tb const b10 = mat_traits<B>::template read_element<1,0>(b);
    Tb const b20 = mat_traits<B>::template read_element<2,0>(b);
    Tb const b30 = mat_traits<B>::template read_element<3,0>(b);
    typedef typename deduce_mat2<A,B,4,1>::type R;
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::rows==4);
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::cols==1);
    R r;
    write_mat_element<0,0>(r,a00*b00+a01*b10+a02*b20+a03*b30);
    write_mat_element<1,0>(r,a10*b00+a11*b10+a12*b20+a13*b30);
    write_mat_element<2,0>(r,a20*b00+a21*b10+a22*b20+a23*b30);
    write_mat_element<3,0>(r,a30*b00+a31*b10+a32*b20+a33*b30);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

namespace
qvm_detail
    {
    template <int R,int /*CR*/,int C>
    struct mul_mm_defined;

    template <>
    struct
    mul_mm_defined<4,4,1>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==1 && mat_traits<B>::rows==4 &&
    mat_traits<A>::cols==4 && mat_traits<B>::cols==4,
    deduce_mat2<A,B,1,4> >::type
operator*( A const & a, B const & b )
    {
    typedef typename mat_traits<A>::scalar_type Ta;
    typedef typename mat_traits<B>::scalar_type Tb;
    Ta const a00 = mat_traits<A>::template read_element<0,0>(a);
    Ta const a01 = mat_traits<A>::template read_element<0,1>(a);
    Ta const a02 = mat_traits<A>::template read_element<0,2>(a);
    Ta const a03 = mat_traits<A>::template read_element<0,3>(a);
    Tb const b00 = mat_traits<B>::template read_element<0,0>(b);
    Tb const b01 = mat_traits<B>::template read_element<0,1>(b);
    Tb const b02 = mat_traits<B>::template read_element<0,2>(b);
    Tb const b03 = mat_traits<B>::template read_element<0,3>(b);
    Tb const b10 = mat_traits<B>::template read_element<1,0>(b);
    Tb const b11 = mat_traits<B>::template read_element<1,1>(b);
    Tb const b12 = mat_traits<B>::template read_element<1,2>(b);
    Tb const b13 = mat_traits<B>::template read_element<1,3>(b);
    Tb const b20 = mat_traits<B>::template read_element<2,0>(b);
    Tb const b21 = mat_traits<B>::template read_element<2,1>(b);
    Tb const b22 = mat_traits<B>::template read_element<2,2>(b);
    Tb const b23 = mat_traits<B>::template read_element<2,3>(b);
    Tb const b30 = mat_traits<B>::template read_element<3,0>(b);
    Tb const b31 = mat_traits<B>::template read_element<3,1>(b);
    Tb const b32 = mat_traits<B>::template read_element<3,2>(b);
    Tb const b33 = mat_traits<B>::template read_element<3,3>(b);
    typedef typename deduce_mat2<A,B,1,4>::type R;
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::rows==1);
    BOOST_QVM_STATIC_ASSERT(mat_traits<R>::cols==4);
    R r;
    write_mat_element<0,0>(r,a00*b00+a01*b10+a02*b20+a03*b30);
    write_mat_element<0,1>(r,a00*b01+a01*b11+a02*b21+a03*b31);
    write_mat_element<0,2>(r,a00*b02+a01*b12+a02*b22+a03*b32);
    write_mat_element<0,3>(r,a00*b03+a01*b13+a02*b23+a03*b33);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

namespace
qvm_detail
    {
    template <int R,int /*CR*/,int C>
    struct mul_mm_defined;

    template <>
    struct
    mul_mm_defined<1,4,4>
        {
        static bool const value=true;
        };
    }

} }

#endif
// <<< #include <boost/qvm/gen/mat_operations4.hpp>
// #line 6 "boost/qvm/mat_operations4.hpp"
// <<< #include <boost/qvm/mat_operations4.hpp>
// #line 13 "boost/qvm/mat_operations.hpp"
// >>> #include <boost/qvm/math.hpp>
#ifndef BOOST_QVM_MATH_HPP_INCLUDED
#define BOOST_QVM_MATH_HPP_INCLUDED

// #line 8 "boost/qvm/math.hpp"
#include <math.h>
// #include <boost/qvm/config.hpp> // Expanded at line 109

namespace boost { namespace qvm {

template <class T> T acos( T );
template <class T> T asin( T );
template <class T> T atan( T );
template <class T> T atan2( T, T );
template <class T> T cos( T );
template <class T> T sin( T );
template <class T> T tan( T );
template <class T> T cosh( T );
template <class T> T sinh( T );
template <class T> T tanh( T );
template <class T> T exp( T );
template <class T> T log( T );
template <class T> T log10( T );
template <class T> T mod( T , T );
template <class T> T pow( T, T );
template <class T> T sqrt( T );
template <class T> T ceil( T );
template <class T> T abs( T );
template <class T> T floor( T );
template <class T> T mod( T, T );
template <class T> T ldexp( T, int );
template <class T> T sign( T );

template <> BOOST_QVM_INLINE_TRIVIAL float acos<float>( float x ) { return ::acosf(x); }
template <> BOOST_QVM_INLINE_TRIVIAL float asin<float>( float x ) { return ::asinf(x); }
template <> BOOST_QVM_INLINE_TRIVIAL float atan<float>( float x ) { return ::atanf(x); }
template <> BOOST_QVM_INLINE_TRIVIAL float atan2<float>( float x, float y ) { return ::atan2f(x,y); }
template <> BOOST_QVM_INLINE_TRIVIAL float cos<float>( float x ) { return ::cosf(x); }
template <> BOOST_QVM_INLINE_TRIVIAL float sin<float>( float x ) { return ::sinf(x); }
template <> BOOST_QVM_INLINE_TRIVIAL float tan<float>( float x ) { return ::tanf(x); }
template <> BOOST_QVM_INLINE_TRIVIAL float cosh<float>( float x ) { return ::coshf(x); }
template <> BOOST_QVM_INLINE_TRIVIAL float sinh<float>( float x ) { return ::sinhf(x); }
template <> BOOST_QVM_INLINE_TRIVIAL float tanh<float>( float x ) { return ::tanhf(x); }
template <> BOOST_QVM_INLINE_TRIVIAL float exp<float>( float x ) { return ::expf(x); }
template <> BOOST_QVM_INLINE_TRIVIAL float log<float>( float x ) { return ::logf(x); }
template <> BOOST_QVM_INLINE_TRIVIAL float log10<float>( float x ) { return ::log10f(x); }
template <> BOOST_QVM_INLINE_TRIVIAL float mod<float>( float x, float y ) { return ::fmodf(x,y); }
template <> BOOST_QVM_INLINE_TRIVIAL float pow<float>( float x, float y ) { return ::powf(x,y); }
template <> BOOST_QVM_INLINE_TRIVIAL float sqrt<float>( float x ) { return ::sqrtf(x); }
template <> BOOST_QVM_INLINE_TRIVIAL float ceil<float>( float x ) { return ::ceilf(x); }
template <> BOOST_QVM_INLINE_TRIVIAL float abs<float>( float x ) { return ::fabsf(x); }
template <> BOOST_QVM_INLINE_TRIVIAL float floor<float>( float x ) { return ::floorf(x); }
template <> BOOST_QVM_INLINE_TRIVIAL float ldexp<float>( float x, int y ) { return ::ldexpf(x,y); }
template <> BOOST_QVM_INLINE_TRIVIAL float sign<float>( float x ) { return x<0 ? -1.f : +1.f; }

template <> BOOST_QVM_INLINE_TRIVIAL double acos<double>( double x ) { return ::acos(x); }
template <> BOOST_QVM_INLINE_TRIVIAL double asin<double>( double x ) { return ::asin(x); }
template <> BOOST_QVM_INLINE_TRIVIAL double atan<double>( double x ) { return ::atan(x); }
template <> BOOST_QVM_INLINE_TRIVIAL double atan2<double>( double x, double y ) { return ::atan2(x,y); }
template <> BOOST_QVM_INLINE_TRIVIAL double cos<double>( double x ) { return ::cos(x); }
template <> BOOST_QVM_INLINE_TRIVIAL double sin<double>( double x ) { return ::sin(x); }
template <> BOOST_QVM_INLINE_TRIVIAL double tan<double>( double x ) { return ::tan(x); }
template <> BOOST_QVM_INLINE_TRIVIAL double cosh<double>( double x ) { return ::cosh(x); }
template <> BOOST_QVM_INLINE_TRIVIAL double sinh<double>( double x ) { return ::sinh(x); }
template <> BOOST_QVM_INLINE_TRIVIAL double tanh<double>( double x ) { return ::tanh(x); }
template <> BOOST_QVM_INLINE_TRIVIAL double exp<double>( double x ) { return ::exp(x); }
template <> BOOST_QVM_INLINE_TRIVIAL double log<double>( double x ) { return ::log(x); }
template <> BOOST_QVM_INLINE_TRIVIAL double log10<double>( double x ) { return ::log10(x); }
template <> BOOST_QVM_INLINE_TRIVIAL double mod<double>( double x, double y ) { return ::fmod(x,y); }
template <> BOOST_QVM_INLINE_TRIVIAL double pow<double>( double x, double y ) { return ::pow(x,y); }
template <> BOOST_QVM_INLINE_TRIVIAL double sqrt<double>( double x ) { return ::sqrt(x); }
template <> BOOST_QVM_INLINE_TRIVIAL double ceil<double>( double x ) { return ::ceil(x); }
template <> BOOST_QVM_INLINE_TRIVIAL double abs<double>( double x ) { return ::fabs(x); }
template <> BOOST_QVM_INLINE_TRIVIAL double floor<double>( double x ) { return ::floor(x); }
template <> BOOST_QVM_INLINE_TRIVIAL double ldexp<double>( double x, int y ) { return ::ldexp(x,y); }
template <> BOOST_QVM_INLINE_TRIVIAL double sign<double>( double x ) { return x<0 ? -1.0 : +1.0; }

template <> BOOST_QVM_INLINE_TRIVIAL long double acos<long double>( long double x ) { return ::acosl(x); }
template <> BOOST_QVM_INLINE_TRIVIAL long double asin<long double>( long double x ) { return ::asinl(x); }
template <> BOOST_QVM_INLINE_TRIVIAL long double atan<long double>( long double x ) { return ::atanl(x); }
template <> BOOST_QVM_INLINE_TRIVIAL long double atan2<long double>( long double x, long double y ) { return ::atan2l(x,y); }
template <> BOOST_QVM_INLINE_TRIVIAL long double cos<long double>( long double x ) { return ::cosl(x); }
template <> BOOST_QVM_INLINE_TRIVIAL long double sin<long double>( long double x ) { return ::sinl(x); }
template <> BOOST_QVM_INLINE_TRIVIAL long double tan<long double>( long double x ) { return ::tanl(x); }
template <> BOOST_QVM_INLINE_TRIVIAL long double cosh<long double>( long double x ) { return ::coshl(x); }
template <> BOOST_QVM_INLINE_TRIVIAL long double sinh<long double>( long double x ) { return ::sinhl(x); }
template <> BOOST_QVM_INLINE_TRIVIAL long double tanh<long double>( long double x ) { return ::tanhl(x); }
template <> BOOST_QVM_INLINE_TRIVIAL long double exp<long double>( long double x ) { return ::expl(x); }
template <> BOOST_QVM_INLINE_TRIVIAL long double log<long double>( long double x ) { return ::logl(x); }
template <> BOOST_QVM_INLINE_TRIVIAL long double log10<long double>( long double x ) { return ::log10l(x); }
template <> BOOST_QVM_INLINE_TRIVIAL long double mod<long double>( long double x, long double y ) { return ::fmodl(x,y); }
template <> BOOST_QVM_INLINE_TRIVIAL long double pow<long double>( long double x, long double y ) { return ::powl(x,y); }
template <> BOOST_QVM_INLINE_TRIVIAL long double sqrt<long double>( long double x ) { return ::sqrtl(x); }
template <> BOOST_QVM_INLINE_TRIVIAL long double ceil<long double>( long double x ) { return ::ceill(x); }
template <> BOOST_QVM_INLINE_TRIVIAL long double abs<long double>( long double x ) { return ::fabsl(x); }
template <> BOOST_QVM_INLINE_TRIVIAL long double floor<long double>( long double x ) { return ::floorl(x); }
template <> BOOST_QVM_INLINE_TRIVIAL long double ldexp<long double>( long double x, int y ) { return ::ldexpl(x,y); }
template <> BOOST_QVM_INLINE_TRIVIAL long double sign<long double>( long double x ) { return x<0 ? -1.0 : +1.0; }

} }

#endif
// <<< #include <boost/qvm/math.hpp>
// #line 14 "boost/qvm/mat_operations.hpp"
// >>> #include <boost/qvm/detail/determinant_impl.hpp>
#ifndef BOOST_QVM_DETAIL_DETERMINANT_IMPL_HPP_INCLUDED
#define BOOST_QVM_DETAIL_DETERMINANT_IMPL_HPP_INCLUDED

// #line 8 "boost/qvm/detail/determinant_impl.hpp"
// #include <boost/qvm/config.hpp> // Expanded at line 109
// >>> #include <boost/qvm/mat_traits_array.hpp>
#ifndef BOOST_QVM_MAT_TRAITS_ARRAY_HPP_INCLUDED
#define BOOST_QVM_MAT_TRAITS_ARRAY_HPP_INCLUDED

// #line 8 "boost/qvm/mat_traits_array.hpp"
// #include <boost/qvm/config.hpp> // Expanded at line 109
// #include <boost/qvm/deduce_mat.hpp> // Expanded at line 899
// #include <boost/qvm/assert.hpp> // Expanded at line 887

#if __cplusplus > 199711L

#include <array>

namespace boost { namespace qvm {

template <class T,std::size_t R,std::size_t Q,std::size_t C>
struct
mat_traits<std::array<std::array<std::array<T,R>,Q>,C> >
    {
    static int const rows=0;
    static int const cols=0;
    typedef void scalar_type;
    };

template <class T,std::size_t Rows,std::size_t Cols>
struct
mat_traits<std::array<std::array<T,Rows>,Cols> >
    {
    typedef std::array<std::array<T,Rows>,Cols> this_matrix;
    typedef T scalar_type;
    static int const rows=Rows;
    static int const cols=Cols;

    template <int Row,int Col>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_matrix const & x )
        {
        BOOST_QVM_STATIC_ASSERT(Row>=0);
        BOOST_QVM_STATIC_ASSERT(Row<rows);
        BOOST_QVM_STATIC_ASSERT(Col>=0);
        BOOST_QVM_STATIC_ASSERT(Col<cols);
        return x[Row][Col];
        }

    template <int Row,int Col>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type &
    write_element( this_matrix & x )
        {
        BOOST_QVM_STATIC_ASSERT(Row>=0);
        BOOST_QVM_STATIC_ASSERT(Row<rows);
        BOOST_QVM_STATIC_ASSERT(Col>=0);
        BOOST_QVM_STATIC_ASSERT(Col<cols);
        return x[Row][Col];
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int row, int col, this_matrix const & x )
        {
        BOOST_QVM_ASSERT(row>=0);
        BOOST_QVM_ASSERT(row<rows);
        BOOST_QVM_ASSERT(col>=0);
        BOOST_QVM_ASSERT(col<cols);
        return x[row][col];
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type &
    write_element_idx( int row, int col, this_matrix & x )
        {
        BOOST_QVM_ASSERT(row>=0);
        BOOST_QVM_ASSERT(row<rows);
        BOOST_QVM_ASSERT(col>=0);
        BOOST_QVM_ASSERT(col<cols);
        return x[row][col];
        }
    };

template <class T,std::size_t Rows,std::size_t Cols>
struct
mat_traits<std::array<std::array<T,Rows>,Cols> const>
    {
    typedef std::array<std::array<T,Rows>,Cols> const this_matrix;
    typedef T scalar_type;
    static int const rows=Rows;
    static int const cols=Cols;

    template <int Row,int Col>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_matrix & x )
        {
        BOOST_QVM_STATIC_ASSERT(Row>=0);
        BOOST_QVM_STATIC_ASSERT(Row<rows);
        BOOST_QVM_STATIC_ASSERT(Col>=0);
        BOOST_QVM_STATIC_ASSERT(Col<cols);
        return x[Row][Col];
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int row, int col, this_matrix & x )
        {
        BOOST_QVM_ASSERT(row>=0);
        BOOST_QVM_ASSERT(row<rows);
        BOOST_QVM_ASSERT(col>=0);
        BOOST_QVM_ASSERT(col<cols);
        return x[row][col];
        }
    };

template <class T,std::size_t Rows,std::size_t Cols,int R,int C>
struct
deduce_mat<std::array<std::array<T,Rows>,Cols>,R,C>
    {
    typedef mat<T,R,C> type;
    };

template <class T,std::size_t Rows,std::size_t Cols,int R,int C>
struct
deduce_mat<std::array<std::array<T,Rows>,Cols> const,R,C>
    {
    typedef mat<T,R,C> type;
    };

template <class T1,class T2,std::size_t Rows,std::size_t Cols,int R,int C>
struct
deduce_mat2<std::array<std::array<T1,Rows>,Cols>,std::array<std::array<T2,Rows>,Cols>,R,C>
    {
    typedef mat<typename deduce_scalar<T1,T2>::type,R,C> type;
    };

template <class T1,class T2,std::size_t Rows,std::size_t Cols,int R,int C>
struct
deduce_mat2<std::array<std::array<T1,Rows>,Cols> const,std::array<std::array<T2,Rows>,Cols>,R,C>
    {
    typedef mat<typename deduce_scalar<T1,T2>::type,R,C> type;
    };

template <class T1,class T2,std::size_t Rows,std::size_t Cols,int R,int C>
struct
deduce_mat2<std::array<std::array<T1,Rows>,Cols>,std::array<std::array<T2,Rows> const,Cols>,R,C>
    {
    typedef mat<typename deduce_scalar<T1,T2>::type,R,C> type;
    };

template <class T1,class T2,std::size_t Rows,std::size_t Cols,int R,int C>
struct
deduce_mat2<std::array<std::array<T1,Rows>,Cols> const,std::array<std::array<T2,Rows>,Cols> const,R,C>
    {
    typedef mat<typename deduce_scalar<T1,T2>::type,R,C> type;
    };

} }

#endif

namespace boost { namespace qvm {

template <class T,int R,int Q,int C>
struct
mat_traits<T[R][Q][C]>
    {
    static int const rows=0;
    static int const cols=0;
    typedef void scalar_type;
    };

template <class T,int Rows,int Cols>
struct
mat_traits<T[Rows][Cols]>
    {
    typedef T this_matrix[Rows][Cols];
    typedef T scalar_type;
    static int const rows=Rows;
    static int const cols=Cols;

    template <int Row,int Col>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_matrix const & x )
        {
        BOOST_QVM_STATIC_ASSERT(Row>=0);
        BOOST_QVM_STATIC_ASSERT(Row<rows);
        BOOST_QVM_STATIC_ASSERT(Col>=0);
        BOOST_QVM_STATIC_ASSERT(Col<cols);
        return x[Row][Col];
        }

    template <int Row,int Col>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type &
    write_element( this_matrix & x )
        {
        BOOST_QVM_STATIC_ASSERT(Row>=0);
        BOOST_QVM_STATIC_ASSERT(Row<rows);
        BOOST_QVM_STATIC_ASSERT(Col>=0);
        BOOST_QVM_STATIC_ASSERT(Col<cols);
        return x[Row][Col];
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int row, int col, this_matrix const & x )
        {
        BOOST_QVM_ASSERT(row>=0);
        BOOST_QVM_ASSERT(row<Rows);
        BOOST_QVM_ASSERT(col>=0);
        BOOST_QVM_ASSERT(col<Cols);
        return x[row][col];
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type &
    write_element_idx( int row, int col, this_matrix & x )
        {
        BOOST_QVM_ASSERT(row>=0);
        BOOST_QVM_ASSERT(row<Rows);
        BOOST_QVM_ASSERT(col>=0);
        BOOST_QVM_ASSERT(col<Cols);
        return x[row][col];
        }
    };

template <class T,int Rows,int Cols,int R,int C>
struct
deduce_mat<T[Rows][Cols],R,C>
    {
    typedef mat<T,R,C> type;
    };

template <class T,int Rows,int Cols,int R,int C>
struct
deduce_mat<T const[Rows][Cols],R,C>
    {
    typedef mat<T,R,C> type;
    };

template <class T1,class T2,int Rows,int Cols,int R,int C>
struct
deduce_mat2<T1[Rows][Cols],T2[Rows][Cols],R,C>
    {
    typedef mat<typename deduce_scalar<T1,T2>::type,R,C> type;
    };

template <int Rows,int Cols,class T>
T (&ptr_mref( T * ptr ))[Rows][Cols]
    {
    return *reinterpret_cast<T (*)[Rows][Cols]>(ptr);
    }

} }

#endif
// <<< #include <boost/qvm/mat_traits_array.hpp>
// #line 10 "boost/qvm/detail/determinant_impl.hpp"
// #include <boost/qvm/static_assert.hpp> // Expanded at line 1180

namespace boost { namespace qvm {

namespace
qvm_detail
    {
    template <int N>
    struct
    det_size
        {
        };

    template <class M>
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
    typename mat_traits<M>::scalar_type
    determinant_impl_( M const & a, det_size<2> )
        {
        return
            mat_traits<M>::template read_element<0,0>(a) * mat_traits<M>::template read_element<1,1>(a) -
            mat_traits<M>::template read_element<1,0>(a) * mat_traits<M>::template read_element<0,1>(a);
        }

    template <class M,int N>
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_RECURSION
    typename mat_traits<M>::scalar_type
    determinant_impl_( M const & a, det_size<N> )
        {
        typedef typename mat_traits<M>::scalar_type T;
        T m[N-1][N-1];
        T det=T(0);
        for( int j1=0; j1!=N; ++j1 )
            {
            for( int i=1; i!=N; ++i )
                {
                int j2 = 0;
                for( int j=0; j!=N; ++j )
                    {
                    if( j==j1 )
                        continue;
                    m[i-1][j2] = mat_traits<M>::read_element_idx(i,j,a);
                    ++j2;
                    }
                }
            T d=determinant_impl_(m,det_size<N-1>());
            if( j1&1 )
                d=-d;
            det += mat_traits<M>::read_element_idx(0,j1,a) * d;
            }
        return det;
        }

    template <class M>
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
    typename mat_traits<M>::scalar_type
    determinant_impl( M const & a )
        {
        BOOST_QVM_STATIC_ASSERT(mat_traits<M>::rows==mat_traits<M>::cols);
        return determinant_impl_(a,det_size<mat_traits<M>::rows>());
        }
    }

} }

#endif
// <<< #include <boost/qvm/detail/determinant_impl.hpp>
// #line 15 "boost/qvm/mat_operations.hpp"
// >>> #include <boost/qvm/detail/cofactor_impl.hpp>
#ifndef BOOST_QVM_DETAIL_COFACTOR_IMPL_HPP_INCLUDED
#define BOOST_QVM_DETAIL_COFACTOR_IMPL_HPP_INCLUDED

// #line 8 "boost/qvm/detail/cofactor_impl.hpp"
// #include <boost/qvm/detail/determinant_impl.hpp> // Expanded at line 7503
// #include <boost/qvm/mat_traits.hpp> // Expanded at line 303
// #include <boost/qvm/static_assert.hpp> // Expanded at line 1180

namespace boost { namespace qvm {

namespace
qvm_detail
    {
    template <class A>
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
    typename deduce_mat<A>::type
    cofactor_impl( A const & a )
        {
        BOOST_QVM_STATIC_ASSERT(mat_traits<A>::rows==mat_traits<A>::cols);
        int const N=mat_traits<A>::rows;
        typedef typename mat_traits<A>::scalar_type T;
        T c[N-1][N-1];
        typedef typename deduce_mat<A>::type R;
        R b;
        for( int j=0; j!=N; ++j )
            {
            for( int i=0; i!=N; ++i )
                {
                int i1=0;
                for( int ii=0; ii!=N; ++ii )
                    {
                    if( ii==i )
                        continue;
                    int j1=0;
                    for( int jj=0; jj!=N; ++jj )
                        {
                        if( jj==j )
                            continue;
                        c[i1][j1] = mat_traits<A>::read_element_idx(ii,jj,a);
                        ++j1;
                        }
                    ++i1;
                    }
                T det = determinant_impl(c);
                if( (i+j)&1 )
                    det=-det;
                write_mat_element_idx(i,j,b,det);
                }
            }
        return b;
        }
    }

} }

#endif
// <<< #include <boost/qvm/detail/cofactor_impl.hpp>
// #line 16 "boost/qvm/mat_operations.hpp"
// >>> #include <boost/qvm/detail/transp_impl.hpp>
#ifndef BOOST_QVM_DETAIL_TRANSP_IMPL_HPP_INCLUDED
#define BOOST_QVM_DETAIL_TRANSP_IMPL_HPP_INCLUDED

// #line 8 "boost/qvm/detail/transp_impl.hpp"
// #include <boost/qvm/deduce_mat.hpp> // Expanded at line 899
// #include <boost/qvm/static_assert.hpp> // Expanded at line 1180
// #include <boost/qvm/assert.hpp> // Expanded at line 887

namespace boost { namespace qvm {

namespace
qvm_detail
    {
    template <class OriginalMatrix>
    class
    transposed_
        {
        transposed_( transposed_ const & );
        transposed_ & operator=( transposed_ const & );
        ~transposed_();

        public:

        template <class T>
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        transposed_ &
        operator=( T const & x )
            {
            assign(*this,x);
            return *this;
            }

        template <class R
#if __cplusplus >= 201103L
            , class = typename enable_if<is_mat<R> >::type
#endif
        >
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        operator R() const
            {
            R r;
            assign(r,*this);
            return r;
            }
        };

    template <class OriginalMatrix,bool WriteElementRef=mat_write_element_ref<OriginalMatrix>::value>
    struct transposed_write_traits;

    template <class OriginalMatrix>
    struct
    transposed_write_traits<OriginalMatrix,true>
        {
        typedef typename mat_traits<OriginalMatrix>::scalar_type scalar_type;
        typedef transposed_<OriginalMatrix> this_matrix;
        static int const rows=mat_traits<OriginalMatrix>::cols;
        static int const cols=mat_traits<OriginalMatrix>::rows;

        template <int Row,int Col>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        scalar_type &
        write_element( this_matrix & x )
            {
            BOOST_QVM_STATIC_ASSERT(Row>=0);
            BOOST_QVM_STATIC_ASSERT(Row<rows);
            BOOST_QVM_STATIC_ASSERT(Col>=0);
            BOOST_QVM_STATIC_ASSERT(Col<cols);
            return mat_traits<OriginalMatrix>::template write_element<Col,Row>(reinterpret_cast<OriginalMatrix &>(x));
            }

        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        scalar_type &
        write_element_idx( int row, int col, this_matrix & x )
            {
            BOOST_QVM_ASSERT(row>=0);
            BOOST_QVM_ASSERT(row<rows);
            BOOST_QVM_ASSERT(col>=0);
            BOOST_QVM_ASSERT(col<cols);
            return mat_traits<OriginalMatrix>::write_element_idx(col,row,reinterpret_cast<OriginalMatrix &>(x));
            }
        };

    template <class OriginalMatrix>
    struct
    transposed_write_traits<OriginalMatrix,false>
        {
        typedef typename mat_traits<OriginalMatrix>::scalar_type scalar_type;
        typedef transposed_<OriginalMatrix> this_matrix;
        static int const rows=mat_traits<OriginalMatrix>::cols;
        static int const cols=mat_traits<OriginalMatrix>::rows;

        template <int Row,int Col>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        void
        write_element( this_matrix & x, scalar_type s )
            {
            BOOST_QVM_STATIC_ASSERT(Row>=0);
            BOOST_QVM_STATIC_ASSERT(Row<rows);
            BOOST_QVM_STATIC_ASSERT(Col>=0);
            BOOST_QVM_STATIC_ASSERT(Col<cols);
            mat_traits<OriginalMatrix>::template write_element<Col,Row>(reinterpret_cast<OriginalMatrix &>(x), s);
            }

        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        void
        write_element_idx( int row, int col, this_matrix & x, scalar_type s )
            {
            BOOST_QVM_ASSERT(row>=0);
            BOOST_QVM_ASSERT(row<rows);
            BOOST_QVM_ASSERT(col>=0);
            BOOST_QVM_ASSERT(col<cols);
            mat_traits<OriginalMatrix>::write_element_idx(col,row,reinterpret_cast<OriginalMatrix &>(x), s);
            }
        };
    }

template <class OriginalMatrix>
struct
mat_traits< qvm_detail::transposed_<OriginalMatrix> >:
    qvm_detail::transposed_write_traits<OriginalMatrix>
    {
    typedef typename mat_traits<OriginalMatrix>::scalar_type scalar_type;
    typedef qvm_detail::transposed_<OriginalMatrix> this_matrix;
    static int const rows=mat_traits<OriginalMatrix>::cols;
    static int const cols=mat_traits<OriginalMatrix>::rows;

    template <int Row,int Col>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_matrix const & x )
        {
        BOOST_QVM_STATIC_ASSERT(Row>=0);
        BOOST_QVM_STATIC_ASSERT(Row<rows);
        BOOST_QVM_STATIC_ASSERT(Col>=0);
        BOOST_QVM_STATIC_ASSERT(Col<cols);
        return mat_traits<OriginalMatrix>::template read_element<Col,Row>(reinterpret_cast<OriginalMatrix const &>(x));
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int row, int col, this_matrix const & x )
        {
        BOOST_QVM_ASSERT(row>=0);
        BOOST_QVM_ASSERT(row<rows);
        BOOST_QVM_ASSERT(col>=0);
        BOOST_QVM_ASSERT(col<cols);
        return mat_traits<OriginalMatrix>::read_element_idx(col,row,reinterpret_cast<OriginalMatrix const &>(x));
        }
    };

template <class OriginalMatrix,int R,int C>
struct
deduce_mat<qvm_detail::transposed_<OriginalMatrix>,R,C>
    {
    typedef mat<typename mat_traits<OriginalMatrix>::scalar_type,R,C> type;
    };

template <class OriginalMatrix,int R,int C>
struct
deduce_mat2<qvm_detail::transposed_<OriginalMatrix>,qvm_detail::transposed_<OriginalMatrix>,R,C>
    {
    typedef mat<typename mat_traits<OriginalMatrix>::scalar_type,R,C> type;
    };

} }

#endif
// <<< #include <boost/qvm/detail/transp_impl.hpp>
// #line 17 "boost/qvm/mat_operations.hpp"
// #include <boost/qvm/scalar_traits.hpp> // Expanded at line 909
// #include <boost/qvm/to_string.hpp> // Expanded at line 49

namespace boost { namespace qvm {

namespace
qvm_detail
    {
    BOOST_QVM_INLINE_CRITICAL
    void const *
    get_valid_ptr_mat_operations()
        {
        static int const obj=0;
        return &obj;
        }
    }

////////////////////////////////////////////////


namespace
qvm_detail
    {
    template <int R,int C>
    struct
    to_string_m_defined
        {
        static bool const value=false;
        };

    template <int I,int SizeMinusOne>
    struct
    to_string_matrix_elements
        {
        template <class A>
        static
        std::string
        f( A const & a )
            {
            using namespace qvm_to_string_detail;
            return
                ( (I%mat_traits<A>::cols)==0 ? '(' : ',' ) +
                to_string(mat_traits<A>::template read_element<I/mat_traits<A>::cols,I%mat_traits<A>::cols>(a)) +
                ( (I%mat_traits<A>::cols)==mat_traits<A>::cols-1 ? ")" : "" ) +
                to_string_matrix_elements<I+1,SizeMinusOne>::f(a);
            }
        };

    template <int SizeMinusOne>
    struct
    to_string_matrix_elements<SizeMinusOne,SizeMinusOne>
        {
        template <class A>
        static
        std::string
        f( A const & a )
            {
            using namespace qvm_to_string_detail;
            return
                ( (SizeMinusOne%mat_traits<A>::cols)==0 ? '(' : ',' ) +
                to_string(mat_traits<A>::template read_element<SizeMinusOne/mat_traits<A>::cols,SizeMinusOne%mat_traits<A>::cols>(a)) +
                ')';
            }
        };
    }

template <class A>
inline
typename enable_if_c<
    is_mat<A>::value  &&
    !qvm_detail::to_string_m_defined<mat_traits<A>::rows,mat_traits<A>::cols>::value,
    std::string>::type
to_string( A const & a )
    {
    return "("+qvm_detail::to_string_matrix_elements<0,mat_traits<A>::rows*mat_traits<A>::cols-1>::f(a)+')';
    }

////////////////////////////////////////////////

template <class A,class B,class Cmp>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value && is_mat<B>::value &&
    mat_traits<A>::rows==mat_traits<B>::rows &&
    mat_traits<A>::cols==mat_traits<B>::cols,
    bool>::type
cmp( A const & a, B const & b, Cmp f )
    {
    for( int i=0; i!=mat_traits<A>::rows; ++i )
        for( int j=0; j!=mat_traits<A>::cols; ++j )
            if( !f(
                mat_traits<A>::read_element_idx(i, j, a),
                mat_traits<B>::read_element_idx(i, j, b)) )
                return false;
    return true;
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int M,int N>
    struct
    convert_to_m_defined
        {
        static bool const value=false;
        };
    }

template <class R,class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
typename enable_if_c<
    is_mat<R>::value && is_mat<A>::value &&
    mat_traits<R>::rows==mat_traits<A>::rows &&
    mat_traits<R>::cols==mat_traits<A>::cols &&
    !qvm_detail::convert_to_m_defined<mat_traits<A>::rows,mat_traits<A>::cols>::value,
    R>::type
convert_to( A const & a )
    {
    R r; assign(r,a);
    return r;
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int D>
    struct
    determinant_defined
        {
        static bool const value=false;
        };
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
typename enable_if_c<
    is_mat<A>::value &&
    mat_traits<A>::rows==mat_traits<A>::cols &&
    !qvm_detail::determinant_defined<mat_traits<A>::rows>::value,
    typename mat_traits<A>::scalar_type>::type
determinant( A const & a )
    {
    return qvm_detail::determinant_impl(a);
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <class T,int Dim>
    class
    identity_mat_
        {
        identity_mat_( identity_mat_ const & );
        identity_mat_ & operator=( identity_mat_ const & );
        ~identity_mat_();

        public:

        template <class R
#if __cplusplus >= 201103L
            , class = typename enable_if<is_mat<R> >::type
#endif
        >
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        operator R() const
            {
            R r;
            assign(r,*this);
            return r;
            }
        };
    }

template <class T,int Dim>
struct
mat_traits< qvm_detail::identity_mat_<T,Dim> >
    {
    typedef qvm_detail::identity_mat_<T,Dim> this_matrix;
    typedef T scalar_type;
    static int const rows=Dim;
    static int const cols=Dim;

    template <int Row,int Col>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_matrix const & /*x*/ )
        {
        BOOST_QVM_STATIC_ASSERT(Row>=0);
        BOOST_QVM_STATIC_ASSERT(Row<Dim);
        BOOST_QVM_STATIC_ASSERT(Col>=0);
        BOOST_QVM_STATIC_ASSERT(Col<Dim);
        return scalar_traits<scalar_type>::value(Row==Col);
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int row, int col, this_matrix const & /*x*/ )
        {
        BOOST_QVM_ASSERT(row>=0);
        BOOST_QVM_ASSERT(row<Dim);
        BOOST_QVM_ASSERT(col>=0);
        BOOST_QVM_ASSERT(col<Dim);
        return scalar_traits<scalar_type>::value(row==col);
        }
    };

template <class T,int Dim>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
qvm_detail::identity_mat_<T,Dim> const &
identity_mat()
    {
    return *(qvm_detail::identity_mat_<T,Dim> const *)qvm_detail::get_valid_ptr_mat_operations();
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value &&
    mat_traits<A>::rows==mat_traits<A>::cols,
    void>::type
set_identity( A & a )
    {
    assign(a,identity_mat<typename mat_traits<A>::scalar_type,mat_traits<A>::rows>());
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <class T>
    struct
    projection_
        {
        T const _00;
        T const _11;
        T const _22;
        T const _23;
        T const _32;

        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        projection_( T _00, T _11, T _22, T _23, T _32 ):
            _00(_00),
            _11(_11),
            _22(_22),
            _23(_23),
            _32(_32)
            {
            }

        template <class R
#if __cplusplus >= 201103L
            , class = typename enable_if<is_mat<R> >::type
#endif
        >
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        operator R() const
            {
            R r;
            assign(r,*this);
            return r;
            }
        };

    template <int Row,int Col>
    struct
    projection_get
        {
        template <class T>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        T
        get( projection_<T> const & )
            {
            return scalar_traits<T>::value(0);
            }
        };

    template <> struct projection_get<0,0> { template <class T> static BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL T get( projection_<T> const & m ) { return m._00; } };
    template <> struct projection_get<1,1> { template <class T> static BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL T get( projection_<T> const & m ) { return m._11; } };
    template <> struct projection_get<2,2> { template <class T> static BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL T get( projection_<T> const & m ) { return m._22; } };
    template <> struct projection_get<2,3> { template <class T> static BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL T get( projection_<T> const & m ) { return m._23; } };
    template <> struct projection_get<3,2> { template <class T> static BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL T get( projection_<T> const & m ) { return m._32; } };
    }

template <class T>
struct
mat_traits< qvm_detail::projection_<T> >
    {
    typedef qvm_detail::projection_<T> this_matrix;
    typedef T scalar_type;
    static int const rows=4;
    static int const cols=4;

    template <int Row,int Col>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_matrix const & x )
        {
        BOOST_QVM_STATIC_ASSERT(Row>=0);
        BOOST_QVM_STATIC_ASSERT(Row<rows);
        BOOST_QVM_STATIC_ASSERT(Col>=0);
        BOOST_QVM_STATIC_ASSERT(Col<cols);
        return qvm_detail::projection_get<Row,Col>::get(x);
        }
    };

template <class T>
qvm_detail::projection_<T>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
perspective_lh( T fov_y, T aspect_ratio, T z_near, T z_far )
    {
    T const one = scalar_traits<T>::value(1);
    T const ys = one/tan(fov_y/scalar_traits<T>::value(2));
    T const xs = ys/aspect_ratio;
    T const zd = z_far-z_near;
    T const z1 = z_far/zd;
    T const z2 = -z_near*z1;
    return qvm_detail::projection_<T>(xs,ys,z1,z2,one);
    }

template <class T>
qvm_detail::projection_<T>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
perspective_rh( T fov_y, T aspect_ratio, T z_near, T z_far )
    {
    T const one = scalar_traits<T>::value(1);
    T const ys = one/tan(fov_y/scalar_traits<T>::value(2));
    T const xs = ys/aspect_ratio;
    T const zd = z_near-z_far;
    T const z1 = z_far/zd;
    T const z2 = z_near*z1;
    return qvm_detail::projection_<T>(xs,ys,z1,z2,-one);
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <class OriginalType,class Scalar>
    class
    matrix_scalar_cast_
        {
        matrix_scalar_cast_( matrix_scalar_cast_ const & );
        matrix_scalar_cast_ & operator=( matrix_scalar_cast_ const & );
        ~matrix_scalar_cast_();

        public:

        template <class T>
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        matrix_scalar_cast_ &
        operator=( T const & x )
            {
            assign(*this,x);
            return *this;
            }

        template <class R
#if __cplusplus >= 201103L
            , class = typename enable_if<is_mat<R> >::type
#endif
        >
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        operator R() const
            {
            R r;
            assign(r,*this);
            return r;
            }
        };

    template <bool> struct scalar_cast_matrix_filter { };
    template <> struct scalar_cast_matrix_filter<true> { typedef int type; };
    }

template <class OriginalType,class Scalar>
struct
mat_traits< qvm_detail::matrix_scalar_cast_<OriginalType,Scalar> >
    {
    typedef Scalar scalar_type;
    typedef qvm_detail::matrix_scalar_cast_<OriginalType,Scalar> this_matrix;
    static int const rows=mat_traits<OriginalType>::rows;
    static int const cols=mat_traits<OriginalType>::cols;

    template <int Row,int Col>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_matrix const & x )
        {
        BOOST_QVM_STATIC_ASSERT(Row>=0);
        BOOST_QVM_STATIC_ASSERT(Row<rows);
        BOOST_QVM_STATIC_ASSERT(Col>=0);
        BOOST_QVM_STATIC_ASSERT(Col<cols);
        return scalar_type(mat_traits<OriginalType>::template read_element<Row,Col>(reinterpret_cast<OriginalType const &>(x)));
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int row, int col, this_matrix const & x )
        {
        BOOST_QVM_ASSERT(row>=0);
        BOOST_QVM_ASSERT(row<rows);
        BOOST_QVM_ASSERT(col>=0);
        BOOST_QVM_ASSERT(col<cols);
        return scalar_type(mat_traits<OriginalType>::read_element_idx(col,row,reinterpret_cast<OriginalType const &>(x)));
        }
    };

template <class OriginalType,class Scalar,int R,int C>
struct
deduce_mat<qvm_detail::matrix_scalar_cast_<OriginalType,Scalar>,R,C>
    {
    typedef mat<Scalar,R,C> type;
    };

template <class Scalar,class T>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
qvm_detail::matrix_scalar_cast_<T,Scalar> const &
scalar_cast( T const & x, typename qvm_detail::scalar_cast_matrix_filter<is_mat<T>::value>::type=0 )
    {
    return reinterpret_cast<qvm_detail::matrix_scalar_cast_<T,Scalar> const &>(x);
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int M,int N>
    struct
    div_eq_ms_defined
        {
        static bool const value=false;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value && is_scalar<B>::value &&
    !qvm_detail::div_eq_ms_defined<mat_traits<A>::rows,mat_traits<A>::cols>::value,
    A &>::type
operator/=( A & a, B b )
    {
    for( int i=0; i!=mat_traits<A>::rows; ++i )
        for( int j=0; j!=mat_traits<A>::cols; ++j )
            write_mat_element_idx(i,j,a,mat_traits<A>::read_element_idx(i,j,a)/b);
    return a;
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int M,int N>
    struct
    div_ms_defined
        {
        static bool const value=false;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_mat<A>::value && is_scalar<B>::value &&
    !qvm_detail::div_ms_defined<mat_traits<A>::rows,mat_traits<A>::cols>::value,
    deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols> >::type
operator/( A const & a, B b )
    {
    typedef typename deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols>::type R;
    R r;
    for( int i=0; i!=mat_traits<A>::rows; ++i )
        for( int j=0; j!=mat_traits<A>::cols; ++j )
            write_mat_element_idx(i,j,r,mat_traits<A>::read_element_idx(i,j,a)/b);
    return r;
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int M,int N>
    struct
    eq_mm_defined
        {
        static bool const value=false;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value && is_mat<B>::value &&
    mat_traits<A>::rows==mat_traits<B>::rows &&
    mat_traits<A>::cols==mat_traits<B>::cols &&
    !qvm_detail::eq_mm_defined<mat_traits<A>::rows,mat_traits<A>::cols>::value,
    bool>::type
operator==( A const & a, B const & b )
    {
    for( int i=0; i!=mat_traits<A>::rows; ++i )
        for( int j=0; j!=mat_traits<A>::cols; ++j )
            if( mat_traits<A>::read_element_idx(i,j,a)!=mat_traits<B>::read_element_idx(i,j,b) )
                return false;
    return true;
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int M,int N>
    struct
    minus_eq_mm_defined
        {
        static bool const value=false;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value && is_mat<B>::value &&
    mat_traits<A>::rows==mat_traits<B>::rows &&
    mat_traits<A>::cols==mat_traits<B>::cols &&
    !qvm_detail::minus_eq_mm_defined<mat_traits<A>::rows,mat_traits<A>::cols>::value,
    A &>::type
operator-=( A & a, B const & b )
    {
    for( int i=0; i!=mat_traits<A>::rows; ++i )
        for( int j=0; j!=mat_traits<A>::cols; ++j )
            write_mat_element_idx(i,j,a,mat_traits<A>::read_element_idx(i,j,a)-mat_traits<B>::read_element_idx(i,j,b));
    return a;
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int M,int N>
    struct
    minus_m_defined
        {
        static bool const value=false;
        };
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_mat<A>::value &&
    !qvm_detail::minus_m_defined<mat_traits<A>::rows,mat_traits<A>::cols>::value,
    deduce_mat<A> >::type
operator-( A const & a )
    {
    typedef typename deduce_mat<A>::type R;
    R r;
    for( int i=0; i!=mat_traits<A>::rows; ++i )
        for( int j=0; j!=mat_traits<A>::cols; ++j )
            write_mat_element_idx(i,j,r,-mat_traits<A>::read_element_idx(i,j,a));
    return r;
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int M,int N>
    struct
    minus_mm_defined
        {
        static bool const value=false;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_mat<A>::value && is_mat<B>::value &&
    mat_traits<A>::rows==mat_traits<B>::rows &&
    mat_traits<A>::cols==mat_traits<B>::cols &&
    !qvm_detail::minus_mm_defined<mat_traits<A>::rows,mat_traits<A>::cols>::value,
    deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols> >::type
operator-( A const & a, B const & b )
    {
    typedef typename deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols>::type R;
    R r;
    for( int i=0; i!=mat_traits<A>::rows; ++i )
        for( int j=0; j!=mat_traits<A>::cols; ++j )
            write_mat_element_idx(i,j,r,mat_traits<A>::read_element_idx(i,j,a)-mat_traits<B>::read_element_idx(i,j,b));
    return r;
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int D>
    struct
    mul_eq_mm_defined
        {
        static bool const value=false;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value &&
    is_mat<B>::value &&
    mat_traits<A>::rows==mat_traits<A>::cols &&
    mat_traits<A>::rows==mat_traits<B>::rows &&
    mat_traits<A>::cols==mat_traits<B>::cols &&
    !qvm_detail::mul_eq_mm_defined<mat_traits<A>::rows>::value,
    A &>::type
operator*=( A & r, B const & b )
    {
    typedef typename mat_traits<A>::scalar_type Ta;
    Ta a[mat_traits<A>::rows][mat_traits<A>::cols];
    for( int i=0; i<mat_traits<A>::rows; ++i )
        for( int j=0; j<mat_traits<B>::cols; ++j )
            a[i][j]=mat_traits<A>::read_element_idx(i,j,r);
    for( int i=0; i<mat_traits<A>::rows; ++i )
        for( int j=0; j<mat_traits<B>::cols; ++j )
            {
            Ta x(scalar_traits<Ta>::value(0));
            for( int k=0; k<mat_traits<A>::cols; ++k )
                x += a[i][k]*mat_traits<B>::read_element_idx(k,j,b);
            write_mat_element_idx(i,j,r,x);
            }
    return r;
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int M,int N>
    struct
    mul_eq_ms_defined
        {
        static bool const value=false;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value && is_scalar<B>::value &&
    !qvm_detail::mul_eq_ms_defined<mat_traits<A>::rows,mat_traits<A>::cols>::value,
    A &>::type
operator*=( A & a, B b )
    {
    for( int i=0; i!=mat_traits<A>::rows; ++i )
        for( int j=0; j!=mat_traits<A>::cols; ++j )
            write_mat_element_idx(i,j,a,mat_traits<A>::read_element_idx(i,j,a)*b);
    return a;
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int R,int /*CR*/,int C>
    struct
    mul_mm_defined
        {
        static bool const value=false;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_mat<A>::value && is_mat<B>::value &&
    mat_traits<A>::cols==mat_traits<B>::rows &&
    !qvm_detail::mul_mm_defined<mat_traits<A>::rows,mat_traits<A>::cols,mat_traits<B>::cols>::value,
    deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<B>::cols> >::type
operator*( A const & a, B const & b )
    {
    typedef typename deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<B>::cols>::type R;
    R r;
    for( int i=0; i<mat_traits<A>::rows; ++i )
        for( int j=0; j<mat_traits<B>::cols; ++j )
            {
            typedef typename mat_traits<A>::scalar_type Ta;
            Ta x(scalar_traits<Ta>::value(0));
            for( int k=0; k<mat_traits<A>::cols; ++k )
                x += mat_traits<A>::read_element_idx(i,k,a)*mat_traits<B>::read_element_idx(k,j,b);
            write_mat_element_idx(i,j,r,x);
            }
    return r;
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int M,int N>
    struct
    mul_ms_defined
        {
        static bool const value=false;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_mat<A>::value && is_scalar<B>::value &&
    !qvm_detail::mul_ms_defined<mat_traits<A>::rows,mat_traits<A>::cols>::value,
    deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols> >::type
operator*( A const & a, B b )
    {
    typedef typename deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols>::type R;
    R r;
    for( int i=0; i!=mat_traits<A>::rows; ++i )
        for( int j=0; j!=mat_traits<A>::cols; ++j )
            write_mat_element_idx(i,j,r,mat_traits<A>::read_element_idx(i,j,a)*b);
    return r;
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int M,int N>
    struct
    mul_sm_defined
        {
        static bool const value=false;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_scalar<A>::value && is_mat<B>::value &&
    !qvm_detail::mul_sm_defined<mat_traits<B>::rows,mat_traits<B>::cols>::value,
    deduce_mat2<A,B,mat_traits<B>::rows,mat_traits<B>::cols> >::type
operator*( A a, B const & b )
    {
    typedef typename deduce_mat2<A,B,mat_traits<B>::rows,mat_traits<B>::cols>::type R;
    R r;
    for( int i=0; i!=mat_traits<B>::rows; ++i )
        for( int j=0; j!=mat_traits<B>::cols; ++j )
            write_mat_element_idx(i,j,r,a*mat_traits<B>::read_element_idx(i,j,b));
    return r;
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int M,int N>
    struct
    neq_mm_defined
        {
        static bool const value=false;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value && is_mat<B>::value &&
    mat_traits<A>::rows==mat_traits<B>::rows &&
    mat_traits<A>::cols==mat_traits<B>::cols &&
    !qvm_detail::neq_mm_defined<mat_traits<A>::rows,mat_traits<A>::cols>::value,
    bool>::type
operator!=( A const & a, B const & b )
    {
    for( int i=0; i!=mat_traits<A>::rows; ++i )
        for( int j=0; j!=mat_traits<A>::cols; ++j )
            if( mat_traits<A>::read_element_idx(i,j,a)!=mat_traits<B>::read_element_idx(i,j,b) )
                return true;
    return false;
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int M,int N>
    struct
    plus_eq_mm_defined
        {
        static bool const value=false;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value && is_mat<B>::value &&
    mat_traits<A>::rows==mat_traits<B>::rows &&
    mat_traits<A>::cols==mat_traits<B>::cols &&
    !qvm_detail::plus_eq_mm_defined<mat_traits<A>::rows,mat_traits<A>::cols>::value,
    A &>::type
operator+=( A & a, B const & b )
    {
    for( int i=0; i!=mat_traits<A>::rows; ++i )
        for( int j=0; j!=mat_traits<A>::cols; ++j )
            write_mat_element_idx(i,j,a,mat_traits<A>::read_element_idx(i,j,a)+mat_traits<B>::read_element_idx(i,j,b));
    return a;
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int M,int N>
    struct
    plus_mm_defined
        {
        static bool const value=false;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_mat<A>::value && is_mat<B>::value &&
    mat_traits<A>::rows==mat_traits<B>::rows &&
    mat_traits<A>::cols==mat_traits<B>::cols &&
    !qvm_detail::plus_mm_defined<mat_traits<A>::rows,mat_traits<A>::cols>::value,
    deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols> >::type
operator+( A const & a, B const & b )
    {
    typedef typename deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols>::type R;
    R r;
    for( int i=0; i!=mat_traits<A>::rows; ++i )
        for( int j=0; j!=mat_traits<A>::cols; ++j )
            write_mat_element_idx(i,j,r,mat_traits<A>::read_element_idx(i,j,a)+mat_traits<B>::read_element_idx(i,j,b));
    return r;
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <class T>
    class
    mref_
        {
        mref_( mref_ const & );
        mref_ & operator=( mref_ const & );
        ~mref_();

        public:

        template <class R>
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        mref_ &
        operator=( R const & x )
            {
            assign(*this,x);
            return *this;
            }

        template <class R
#if __cplusplus >= 201103L
            , class = typename enable_if<is_mat<R> >::type
#endif
        >
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        operator R() const
            {
            R r;
            assign(r,*this);
            return r;
            }
        };

    template <class M,bool WriteElementRef=mat_write_element_ref<M>::value>
    struct mref_write_traits;

    template <class M>
    struct
    mref_write_traits<M,true>
        {
        typedef typename mat_traits<M>::scalar_type scalar_type;
        typedef qvm_detail::mref_<M> this_matrix;
        static int const rows=mat_traits<M>::rows;
        static int const cols=mat_traits<M>::cols;

        template <int Row,int Col>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        scalar_type &
        write_element( this_matrix & x )
            {
            BOOST_QVM_STATIC_ASSERT(Row>=0);
            BOOST_QVM_STATIC_ASSERT(Row<rows);
            BOOST_QVM_STATIC_ASSERT(Col>=0);
            BOOST_QVM_STATIC_ASSERT(Col<cols);
            return mat_traits<M>::template write_element<Row,Col>(reinterpret_cast<M &>(x));
            }

        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        scalar_type &
        write_element_idx( int row, int col, this_matrix & x )
            {
            BOOST_QVM_ASSERT(row>=0);
            BOOST_QVM_ASSERT(row<rows);
            BOOST_QVM_ASSERT(col>=0);
            BOOST_QVM_ASSERT(col<cols);
            return mat_traits<M>::write_element_idx(row,col,reinterpret_cast<M &>(x));
            }
        };

    template <class M>
    struct
    mref_write_traits<M,false>
        {
        typedef typename mat_traits<M>::scalar_type scalar_type;
        typedef qvm_detail::mref_<M> this_matrix;
        static int const rows=mat_traits<M>::rows;
        static int const cols=mat_traits<M>::cols;

        template <int Row,int Col>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        void
        write_element( this_matrix & x, scalar_type s )
            {
            BOOST_QVM_STATIC_ASSERT(Row>=0);
            BOOST_QVM_STATIC_ASSERT(Row<rows);
            BOOST_QVM_STATIC_ASSERT(Col>=0);
            BOOST_QVM_STATIC_ASSERT(Col<cols);
            mat_traits<M>::template write_element<Row,Col>(reinterpret_cast<M &>(x), s);
            }

        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        void
        write_element_idx( int row, int col, this_matrix & x, scalar_type s )
            {
            BOOST_QVM_ASSERT(row>=0);
            BOOST_QVM_ASSERT(row<rows);
            BOOST_QVM_ASSERT(col>=0);
            BOOST_QVM_ASSERT(col<cols);
            mat_traits<M>::write_element_idx(row,col,reinterpret_cast<M &>(x), s);
            }
        };
    }

template <class M>
struct
mat_traits< qvm_detail::mref_<M> >:
    qvm_detail::mref_write_traits<M>
    {
    typedef typename mat_traits<M>::scalar_type scalar_type;
    typedef qvm_detail::mref_<M> this_matrix;
    static int const rows=mat_traits<M>::rows;
    static int const cols=mat_traits<M>::cols;

    template <int Row,int Col>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_matrix const & x )
        {
        BOOST_QVM_STATIC_ASSERT(Row>=0);
        BOOST_QVM_STATIC_ASSERT(Row<rows);
        BOOST_QVM_STATIC_ASSERT(Col>=0);
        BOOST_QVM_STATIC_ASSERT(Col<cols);
        return mat_traits<M>::template read_element<Row,Col>(reinterpret_cast<M const &>(x));
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int row, int col, this_matrix const & x )
        {
        BOOST_QVM_ASSERT(row>=0);
        BOOST_QVM_ASSERT(row<rows);
        BOOST_QVM_ASSERT(col>=0);
        BOOST_QVM_ASSERT(col<cols);
        return mat_traits<M>::read_element_idx(row,col,reinterpret_cast<M const &>(x));
        }
    };

template <class M,int R,int C>
struct
deduce_mat<qvm_detail::mref_<M>,R,C>
    {
    typedef mat<typename mat_traits<M>::scalar_type,R,C> type;
    };

template <class M>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
typename enable_if_c<
    is_mat<M>::value,
    qvm_detail::mref_<M> const &>::type
mref( M const & a )
    {
    return reinterpret_cast<qvm_detail::mref_<M> const &>(a);
    }

template <class M>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
typename enable_if_c<
    is_mat<M>::value,
    qvm_detail::mref_<M> &>::type
mref( M & a )
    {
    return reinterpret_cast<qvm_detail::mref_<M> &>(a);
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <class T,int Rows,int Cols>
    class
    zero_mat_
        {
        zero_mat_( zero_mat_ const & );
        zero_mat_ & operator=( zero_mat_ const & );
        ~zero_mat_();

        public:

        template <class R
#if __cplusplus >= 201103L
            , class = typename enable_if<is_mat<R> >::type
#endif
        >
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        operator R() const
            {
            R r;
            assign(r,*this);
            return r;
            }
        };
    }

template <class T,int Rows,int Cols>
struct
mat_traits< qvm_detail::zero_mat_<T,Rows,Cols> >
    {
    typedef qvm_detail::zero_mat_<T,Rows,Cols> this_matrix;
    typedef T scalar_type;
    static int const rows=Rows;
    static int const cols=Cols;

    template <int Row,int Col>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_matrix const & )
        {
        BOOST_QVM_STATIC_ASSERT(Row>=0);
        BOOST_QVM_STATIC_ASSERT(Row<rows);
        BOOST_QVM_STATIC_ASSERT(Col>=0);
        BOOST_QVM_STATIC_ASSERT(Col<cols);
        return scalar_traits<scalar_type>::value(0);
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int row, int col, this_matrix const & )
        {
        BOOST_QVM_ASSERT(row>=0); (void)row;
        BOOST_QVM_ASSERT(row<rows);
        BOOST_QVM_ASSERT(col>=0); (void)col;
        BOOST_QVM_ASSERT(col<cols);
        return scalar_traits<scalar_type>::value(0);
        }
    };

template <class T,int Rows,int Cols,int R,int C>
struct
deduce_mat<qvm_detail::zero_mat_<T,Rows,Cols>,R,C>
    {
    typedef mat<T,R,C> type;
    };

template <class T,int Rows,int Cols>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
qvm_detail::zero_mat_<T,Rows,Cols> const &
zero_mat()
    {
    return *(qvm_detail::zero_mat_<T,Rows,Cols> const *)qvm_detail::get_valid_ptr_mat_operations();
    }

template <class T,int Dim>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
qvm_detail::zero_mat_<T,Dim,Dim> const &
zero_mat()
    {
    return *(qvm_detail::zero_mat_<T,Dim,Dim> const *)qvm_detail::get_valid_ptr_mat_operations();
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value,
    void>::type
set_zero( A & a )
    {
    assign(a,zero_mat<typename mat_traits<A>::scalar_type,mat_traits<A>::rows,mat_traits<A>::cols>());
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int D,class S>
    struct
    rot_mat_
        {
        typedef S scalar_type;
        scalar_type a[3][3];

        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE
        rot_mat_(
                scalar_type a00, scalar_type a01, scalar_type a02,
                scalar_type a10, scalar_type a11, scalar_type a12,
                scalar_type a20, scalar_type a21, scalar_type a22 )
            {
            a[0][0] = a00;
            a[0][1] = a01;
            a[0][2] = a02;
            a[1][0] = a10;
            a[1][1] = a11;
            a[1][2] = a12;
            a[2][0] = a20;
            a[2][1] = a21;
            a[2][2] = a22;
            }

        template <class R
#if __cplusplus >= 201103L
            , class = typename enable_if<is_mat<R> >::type
#endif
        >
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        operator R() const
            {
            R r;
            assign(r,*this);
            return r;
            }
        };

    template <int Row,int Col>
    struct
    rot_m_get
        {
        template <class T>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        T
        get( T const (&)[3][3] )
            {
            return scalar_traits<T>::value(Row==Col);
            }
        };

    template <> struct rot_m_get<0,0> { template <class T> static BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL T get( T const (&a)[3][3] ) { return a[0][0]; } };
    template <> struct rot_m_get<0,1> { template <class T> static BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL T get( T const (&a)[3][3] ) { return a[0][1]; } };
    template <> struct rot_m_get<0,2> { template <class T> static BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL T get( T const (&a)[3][3] ) { return a[0][2]; } };
    template <> struct rot_m_get<1,0> { template <class T> static BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL T get( T const (&a)[3][3] ) { return a[1][0]; } };
    template <> struct rot_m_get<1,1> { template <class T> static BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL T get( T const (&a)[3][3] ) { return a[1][1]; } };
    template <> struct rot_m_get<1,2> { template <class T> static BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL T get( T const (&a)[3][3] ) { return a[1][2]; } };
    template <> struct rot_m_get<2,0> { template <class T> static BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL T get( T const (&a)[3][3] ) { return a[2][0]; } };
    template <> struct rot_m_get<2,1> { template <class T> static BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL T get( T const (&a)[3][3] ) { return a[2][1]; } };
    template <> struct rot_m_get<2,2> { template <class T> static BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL T get( T const (&a)[3][3] ) { return a[2][2]; } };
    }

template <class M>
struct mat_traits;

template <int D,class S>
struct
mat_traits< qvm_detail::rot_mat_<D,S> >
    {
    typedef qvm_detail::rot_mat_<D,S> this_matrix;
    typedef typename this_matrix::scalar_type scalar_type;
    static int const rows=D;
    static int const cols=D;

    template <int Row,int Col>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_matrix const & x )
        {
        BOOST_QVM_STATIC_ASSERT(Row>=0);
        BOOST_QVM_STATIC_ASSERT(Row<D);
        BOOST_QVM_STATIC_ASSERT(Col>=0);
        BOOST_QVM_STATIC_ASSERT(Col<D);
        return qvm_detail::rot_m_get<Row,Col>::get(x.a);
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int row, int col, this_matrix const & x )
        {
        BOOST_QVM_ASSERT(row>=0);
        BOOST_QVM_ASSERT(row<D);
        BOOST_QVM_ASSERT(col>=0);
        BOOST_QVM_ASSERT(col<D);
        return row<3 && col<3?
            x.a[row][col] :
            scalar_traits<scalar_type>::value(row==col);
        }
    };

template <int Dim,class V,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE
typename enable_if_c<
    is_vec<V>::value && vec_traits<V>::dim==3,
    qvm_detail::rot_mat_<Dim,Angle> >::type
rot_mat( V const & axis, Angle angle )
    {
    typedef Angle scalar_type;
    scalar_type const x=vec_traits<V>::template read_element<0>(axis);
    scalar_type const y=vec_traits<V>::template read_element<1>(axis);
    scalar_type const z=vec_traits<V>::template read_element<2>(axis);
    scalar_type const m2=x*x+y*y+z*z;
    if( m2==scalar_traits<scalar_type>::value(0) )
        BOOST_QVM_THROW_EXCEPTION(zero_magnitude_error());
    scalar_type const s = sin(angle);
    scalar_type const c = cos(angle);
    scalar_type const x2 = x*x;
    scalar_type const y2 = y*y;
    scalar_type const z2 = z*z;
    scalar_type const xy = x*y;
    scalar_type const xz = x*z;
    scalar_type const yz = y*z;
    scalar_type const xs = x*s;
    scalar_type const ys = y*s;
    scalar_type const zs = z*s;
    scalar_type const one = scalar_traits<scalar_type>::value(1);
    scalar_type const c1 = one-c;
    return qvm_detail::rot_mat_<Dim,Angle>(
        x2+(one-x2)*c, xy*c1-zs, xz*(one-c)+ys,
        xy*c1+zs, y2+(one-y2)*c, yz*c1-xs,
        xz*c1-ys, yz*c1+xs, z2+(one-z2)*c );
    }

template <class A,class B,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value &&
    mat_traits<A>::rows==mat_traits<A>::cols &&
    mat_traits<A>::rows>=3 &&
    is_vec<B>::value && vec_traits<B>::dim==3,
    void>::type
set_rot( A & a, B const & axis, Angle angle )
    {
    assign(a,rot_mat<mat_traits<A>::rows>(axis,angle));
    }

template <class A,class B,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value &&
    mat_traits<A>::rows==mat_traits<A>::cols &&
    mat_traits<A>::rows>=3 &&
    is_vec<B>::value && vec_traits<B>::dim==3,
    void>::type
rotate( A & a, B const & axis, Angle angle )
    {
    a *= rot_mat<mat_traits<A>::rows>(axis,angle);
    }

////////////////////////////////////////////////

template <int Dim,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE
qvm_detail::rot_mat_<Dim,Angle>
rot_mat_xzy( Angle x1, Angle z2, Angle y3 )
    {
    typedef Angle scalar_type;
    scalar_type const c1 = cos(x1);
    scalar_type const s1 = sin(x1);
    scalar_type const c2 = cos(z2);
    scalar_type const s2 = sin(z2);
    scalar_type const c3 = cos(y3);
    scalar_type const s3 = sin(y3);
    return qvm_detail::rot_mat_<Dim,Angle>(
        c2*c3, -s2, c2*s3,
        s1*s3 + c1*c3*s2, c1*c2, c1*s2*s3 - c3*s1,
        c3*s1*s2 - c1*s3, c2*s1, c1*c3 + s1*s2*s3 );
    }

template <class A,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value &&
    mat_traits<A>::rows==mat_traits<A>::cols &&
    mat_traits<A>::rows>=3,
    void>::type
set_rot_xzy( A & a, Angle x1, Angle z2, Angle y3 )
    {
    assign(a,rot_mat_xzy<mat_traits<A>::rows>(x1,z2,y3));
    }

template <class A,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value &&
    mat_traits<A>::rows==mat_traits<A>::cols &&
    mat_traits<A>::rows>=3,
    void>::type
rotate_xzy( A & a, Angle x1, Angle z2, Angle y3 )
    {
    a *= rot_mat_xzy<mat_traits<A>::rows>(x1,z2,y3);
    }

////////////////////////////////////////////////

template <int Dim,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE
qvm_detail::rot_mat_<Dim,Angle>
rot_mat_xyz( Angle x1, Angle y2, Angle z3 )
    {
    typedef Angle scalar_type;
    scalar_type const c1 = cos(x1);
    scalar_type const s1 = sin(x1);
    scalar_type const c2 = cos(y2);
    scalar_type const s2 = sin(y2);
    scalar_type const c3 = cos(z3);
    scalar_type const s3 = sin(z3);
    return qvm_detail::rot_mat_<Dim,Angle>(
        c2*c3, -c2*s3, s2,
        c1*s3 + c3*s1*s2, c1*c3 - s1*s2*s3, -c2*s1,
        s1*s3 - c1*c3*s2, c3*s1 + c1*s2*s3, c1*c2 );
    }

template <class A,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value &&
    mat_traits<A>::rows==mat_traits<A>::cols &&
    mat_traits<A>::rows>=3,
    void>::type
set_rot_xyz( A & a, Angle x1, Angle y2, Angle z3 )
    {
    assign(a,rot_mat_xyz<mat_traits<A>::rows>(x1,y2,z3));
    }

template <class A,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value &&
    mat_traits<A>::rows==mat_traits<A>::cols &&
    mat_traits<A>::rows>=3,
    void>::type
rotate_xyz( A & a, Angle x1, Angle y2, Angle z3 )
    {
    a *= rot_mat_xyz<mat_traits<A>::rows>(x1,y2,z3);
    }

////////////////////////////////////////////////

template <int Dim,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE
qvm_detail::rot_mat_<Dim,Angle>
rot_mat_yxz( Angle y1, Angle x2, Angle z3 )
    {
    typedef Angle scalar_type;
    scalar_type const c1 = cos(y1);
    scalar_type const s1 = sin(y1);
    scalar_type const c2 = cos(x2);
    scalar_type const s2 = sin(x2);
    scalar_type const c3 = cos(z3);
    scalar_type const s3 = sin(z3);
    return qvm_detail::rot_mat_<Dim,Angle>(
        c1*c3 + s1*s2*s3, c3*s1*s2 - c1*s3, c2*s1,
        c2*s3, c2*c3, -s2,
        c1*s2*s3 - c3*s1, c1*c3*s2 + s1*s3, c1*c2 );
    }

template <class A,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value &&
    mat_traits<A>::rows==mat_traits<A>::cols &&
    mat_traits<A>::rows>=3,
    void>::type
set_rot_yxz( A & a, Angle y1, Angle x2, Angle z3 )
    {
    assign(a,rot_mat_yxz<mat_traits<A>::rows>(y1,x2,z3));
    }

template <class A,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value &&
    mat_traits<A>::rows==mat_traits<A>::cols &&
    mat_traits<A>::rows>=3,
    void>::type
rotate_yxz( A & a, Angle y1, Angle x2, Angle z3 )
    {
    a *= rot_mat_yxz<mat_traits<A>::rows>(y1,x2,z3);
    }

////////////////////////////////////////////////

template <int Dim,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE
qvm_detail::rot_mat_<Dim,Angle>
rot_mat_yzx( Angle y1, Angle z2, Angle x3 )
    {
    typedef Angle scalar_type;
    scalar_type const c1 = cos(y1);
    scalar_type const s1 = sin(y1);
    scalar_type const c2 = cos(z2);
    scalar_type const s2 = sin(z2);
    scalar_type const c3 = cos(x3);
    scalar_type const s3 = sin(x3);
    return qvm_detail::rot_mat_<Dim,Angle>(
        c1*c2, s1*s3 - c1*c3*s2, c3*s1 + c1*s2*s3,
        s2, c2*c3, -c2*s3,
        -c2*s1, c1*s3 + c3*s1*s2, c1*c3 - s1*s2*s3 );
    }

template <class A,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value &&
    mat_traits<A>::rows==mat_traits<A>::cols &&
    mat_traits<A>::rows>=3,
    void>::type
set_rot_yzx( A & a, Angle y1, Angle z2, Angle x3 )
    {
    assign(a,rot_mat_yzx<mat_traits<A>::rows>(y1,z2,x3));
    }

template <class A,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value &&
    mat_traits<A>::rows==mat_traits<A>::cols &&
    mat_traits<A>::rows>=3,
    void>::type
rotate_yzx( A & a, Angle y1, Angle z2, Angle x3 )
    {
    a *= rot_mat_yzx<mat_traits<A>::rows>(y1,z2,x3);
    }

////////////////////////////////////////////////

template <int Dim,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE
qvm_detail::rot_mat_<Dim,Angle>
rot_mat_zyx( Angle z1, Angle y2, Angle x3 )
    {
    typedef Angle scalar_type;
    scalar_type const c1 = cos(z1);
    scalar_type const s1 = sin(z1);
    scalar_type const c2 = cos(y2);
    scalar_type const s2 = sin(y2);
    scalar_type const c3 = cos(x3);
    scalar_type const s3 = sin(x3);
    return qvm_detail::rot_mat_<Dim,Angle>(
        c1*c2, c1*s2*s3 - c3*s1, s1*s3 + c1*c3*s2,
        c2*s1, c1*c3 + s1*s2*s3, c3*s1*s2 - c1*s3,
        -s2, c2*s3, c2*c3 );
    }

template <class A,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value &&
    mat_traits<A>::rows==mat_traits<A>::cols &&
    mat_traits<A>::rows>=3,
    void>::type
set_rot_zyx( A & a, Angle z1, Angle y2, Angle x3 )
    {
    assign(a,rot_mat_zyx<mat_traits<A>::rows>(z1,y2,x3));
    }

template <class A,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value &&
    mat_traits<A>::rows==mat_traits<A>::cols &&
    mat_traits<A>::rows>=3,
    void>::type
rotate_zyx( A & a, Angle z1, Angle y2, Angle x3 )
    {
    a *= rot_mat_zyx<mat_traits<A>::rows>(z1,y2,x3);
    }

////////////////////////////////////////////////

template <int Dim,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE
qvm_detail::rot_mat_<Dim,Angle>
rot_mat_zxy( Angle z1, Angle x2, Angle y3 )
    {
    typedef Angle scalar_type;
    scalar_type const c1 = cos(z1);
    scalar_type const s1 = sin(z1);
    scalar_type const c2 = cos(x2);
    scalar_type const s2 = sin(x2);
    scalar_type const c3 = cos(y3);
    scalar_type const s3 = sin(y3);
    return qvm_detail::rot_mat_<Dim,Angle>(
        c1*c3 - s1*s2*s3, -c2*s1, c1*s3 + c3*s1*s2,
        c3*s1 + c1*s2*s3, c1*c2, s1*s3 - c1*c3*s2,
        -c2*s3, s2, c2*c3 );
    }

template <class A,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value &&
    mat_traits<A>::rows==mat_traits<A>::cols &&
    mat_traits<A>::rows>=3,
    void>::type
set_rot_zxy( A & a, Angle z1, Angle x2, Angle y3 )
    {
    assign(a,rot_mat_zxy<mat_traits<A>::rows>(z1,x2,y3));
    }

template <class A,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value &&
    mat_traits<A>::rows==mat_traits<A>::cols &&
    mat_traits<A>::rows>=3,
    void>::type
rotate_zxy( A & a, Angle z1, Angle x2, Angle y3 )
    {
    a *= rot_mat_zxy<mat_traits<A>::rows>(z1,x2,y3);
    }

////////////////////////////////////////////////

template <int Dim,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE
qvm_detail::rot_mat_<Dim,Angle>
rot_mat_xzx( Angle x1, Angle z2, Angle x3 )
    {
    typedef Angle scalar_type;
    scalar_type const c1 = cos(x1);
    scalar_type const s1 = sin(x1);
    scalar_type const c2 = cos(z2);
    scalar_type const s2 = sin(z2);
    scalar_type const c3 = cos(x3);
    scalar_type const s3 = sin(x3);
    return qvm_detail::rot_mat_<Dim,Angle>(
        c2, -c3*s2, s2*s3,
        c1*s2, c1*c2*c3 - s1*s3, -c3*s1 - c1*c2*s3,
        s1*s2, c1*s3 + c2*c3*s1, c1*c3 - c2*s1*s3 );
    }

template <class A,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value &&
    mat_traits<A>::rows==mat_traits<A>::cols &&
    mat_traits<A>::rows>=3,
    void>::type
set_rot_xzx( A & a, Angle x1, Angle z2, Angle x3 )
    {
    assign(a,rot_mat_xzx<mat_traits<A>::rows>(x1,z2,x3));
    }

template <class A,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value &&
    mat_traits<A>::rows==mat_traits<A>::cols &&
    mat_traits<A>::rows>=3,
    void>::type
rotate_xzx( A & a, Angle x1, Angle z2, Angle x3 )
    {
    a *= rot_mat_xzx<mat_traits<A>::rows>(x1,z2,x3);
    }

////////////////////////////////////////////////

template <int Dim,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE
qvm_detail::rot_mat_<Dim,Angle>
rot_mat_xyx( Angle x1, Angle y2, Angle x3 )
    {
    typedef Angle scalar_type;
    scalar_type const c1 = cos(x1);
    scalar_type const s1 = sin(x1);
    scalar_type const c2 = cos(y2);
    scalar_type const s2 = sin(y2);
    scalar_type const c3 = cos(x3);
    scalar_type const s3 = sin(x3);
    return qvm_detail::rot_mat_<Dim,Angle>(
        c2, s2*s3, c3*s2,
        s1*s2, c1*c3 - c2*s1*s3, -c1*s3 - c2*c3*s1,
        -c1*s2, c3*s1 + c1*c2*s3, c1*c2*c3 - s1*s3 );
    }

template <class A,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value &&
    mat_traits<A>::rows==mat_traits<A>::cols &&
    mat_traits<A>::rows>=3,
    void>::type
set_rot_xyx( A & a, Angle x1, Angle y2, Angle x3 )
    {
    assign(a,rot_mat_xyx<mat_traits<A>::rows>(x1,y2,x3));
    }

template <class A,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value &&
    mat_traits<A>::rows==mat_traits<A>::cols &&
    mat_traits<A>::rows>=3,
    void>::type
rotate_xyx( A & a, Angle x1, Angle y2, Angle x3 )
    {
    a *= rot_mat_xyx<mat_traits<A>::rows>(x1,y2,x3);
    }

////////////////////////////////////////////////

template <int Dim,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE
qvm_detail::rot_mat_<Dim,Angle>
rot_mat_yxy( Angle y1, Angle x2, Angle y3 )
    {
    typedef Angle scalar_type;
    scalar_type const c1 = cos(y1);
    scalar_type const s1 = sin(y1);
    scalar_type const c2 = cos(x2);
    scalar_type const s2 = sin(x2);
    scalar_type const c3 = cos(y3);
    scalar_type const s3 = sin(y3);
    return qvm_detail::rot_mat_<Dim,Angle>(
        c1*c3 - c2*s1*s3, s1*s2, c1*s3 + c2*c3*s1,
        s2*s3, c2, -c3*s2,
        -c3*s1 - c1*c2*s3, c1*s2, c1*c2*c3 - s1*s3 );
    }

template <class A,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value &&
    mat_traits<A>::rows==mat_traits<A>::cols &&
    mat_traits<A>::rows>=3,
    void>::type
set_rot_yxy( A & a, Angle y1, Angle x2, Angle y3 )
    {
    assign(a,rot_mat_yxy<mat_traits<A>::rows>(y1,x2,y3));
    }

template <class A,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value &&
    mat_traits<A>::rows==mat_traits<A>::cols &&
    mat_traits<A>::rows>=3,
    void>::type
rotate_yxy( A & a, Angle y1, Angle x2, Angle y3 )
    {
    a *= rot_mat_yxy<mat_traits<A>::rows>(y1,x2,y3);
    }

////////////////////////////////////////////////

template <int Dim,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE
qvm_detail::rot_mat_<Dim,Angle>
rot_mat_yzy( Angle y1, Angle z2, Angle y3 )
    {
    typedef Angle scalar_type;
    scalar_type const c1 = cos(y1);
    scalar_type const s1 = sin(y1);
    scalar_type const c2 = cos(z2);
    scalar_type const s2 = sin(z2);
    scalar_type const c3 = cos(y3);
    scalar_type const s3 = sin(y3);
    return qvm_detail::rot_mat_<Dim,Angle>(
        c1*c2*c3 - s1*s3, -c1*s2, c3*s1 + c1*c2*s3,
        c3*s2, c2, s2*s3,
        -c1*s3 - c2*c3*s1, s1*s2, c1*c3 - c2*s1*s3 );
    }

template <class A,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value &&
    mat_traits<A>::rows==mat_traits<A>::cols &&
    mat_traits<A>::rows>=3,
    void>::type
set_rot_yzy( A & a, Angle y1, Angle z2, Angle y3 )
    {
    assign(a,rot_mat_yzy<mat_traits<A>::rows>(y1,z2,y3));
    }

template <class A,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value &&
    mat_traits<A>::rows==mat_traits<A>::cols &&
    mat_traits<A>::rows>=3,
    void>::type
rotate_yzy( A & a, Angle y1, Angle z2, Angle y3 )
    {
    a *= rot_mat_yzy<mat_traits<A>::rows>(y1,z2,y3);
    }

////////////////////////////////////////////////

template <int Dim,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE
qvm_detail::rot_mat_<Dim,Angle>
rot_mat_zyz( Angle z1, Angle y2, Angle z3 )
    {
    typedef Angle scalar_type;
    scalar_type const c1 = cos(z1);
    scalar_type const s1 = sin(z1);
    scalar_type const c2 = cos(y2);
    scalar_type const s2 = sin(y2);
    scalar_type const c3 = cos(z3);
    scalar_type const s3 = sin(z3);
    return qvm_detail::rot_mat_<Dim,Angle>(
        c1*c2*c3 - s1*s3, -c3*s1 - c1*c2*s3, c1*s2,
        c1*s3 + c2*c3*s1, c1*c3 - c2*s1*s3, s1*s2,
        -c3*s2, s2*s3, c2 );
    }

template <class A,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value &&
    mat_traits<A>::rows==mat_traits<A>::cols &&
    mat_traits<A>::rows>=3,
    void>::type
set_rot_zyz( A & a, Angle z1, Angle y2, Angle z3 )
    {
    assign(a,rot_mat_zyz<mat_traits<A>::rows>(z1,y2,z3));
    }

template <class A,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value &&
    mat_traits<A>::rows==mat_traits<A>::cols &&
    mat_traits<A>::rows>=3,
    void>::type
rotate_zyz( A & a, Angle z1, Angle y2, Angle z3 )
    {
    a *= rot_mat_zyz<mat_traits<A>::rows>(z1,y2,z3);
    }

////////////////////////////////////////////////

template <int Dim,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE
qvm_detail::rot_mat_<Dim,Angle>
rot_mat_zxz( Angle z1, Angle x2, Angle z3 )
    {
    typedef Angle scalar_type;
    scalar_type const c1 = cos(z1);
    scalar_type const s1 = sin(z1);
    scalar_type const c2 = cos(x2);
    scalar_type const s2 = sin(x2);
    scalar_type const c3 = cos(z3);
    scalar_type const s3 = sin(z3);
    return qvm_detail::rot_mat_<Dim,Angle>(
        c1*c3 - c2*s1*s3, -c1*s3 - c2*c3*s1, s1*s2,
        c3*s1 + c1*c2*s3, c1*c2*c3 - s1*s3, -c1*s2,
        s2*s3, c3*s2, c2 );
    }

template <class A,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value &&
    mat_traits<A>::rows==mat_traits<A>::cols &&
    mat_traits<A>::rows>=3,
    void>::type
set_rot_zxz( A & a, Angle z1, Angle x2, Angle z3 )
    {
    assign(a,rot_mat_zxz<mat_traits<A>::rows>(z1,x2,z3));
    }

template <class A,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value &&
    mat_traits<A>::rows==mat_traits<A>::cols &&
    mat_traits<A>::rows>=3,
    void>::type
rotate_zxz( A & a, Angle z1, Angle x2, Angle z3 )
    {
    a *= rot_mat_zxz<mat_traits<A>::rows>(z1,x2,z3);
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int Dim,class Angle>
    struct
    rotx_mat_
        {
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        rotx_mat_()
            {
            }

        template <class R
#if __cplusplus >= 201103L
            , class = typename enable_if<is_mat<R> >::type
#endif
        >
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        operator R() const
            {
            R r;
            assign(r,*this);
            return r;
            }

        private:

        rotx_mat_( rotx_mat_ const & );
        rotx_mat_ & operator=( rotx_mat_ const & );
        ~rotx_mat_();
        };

    template <int Row,int Col>
    struct
    rotx_m_get
        {
        template <class T>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        T
        get( T const & )
            {
            return scalar_traits<T>::value(Row==Col);
            }
        };

    template <>
    struct
    rotx_m_get<1,1>
        {
        template <class T>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        T
        get( T const & angle )
            {
            return cos(angle);
            }
        };

    template <>
    struct
    rotx_m_get<1,2>
        {
        template <class T>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        T
        get( T const & angle )
            {
            return -sin(angle);
            }
        };

    template <>
    struct
    rotx_m_get<2,1>
        {
        template <class T>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        T
        get( T const & angle )
            {
            return sin(angle);
            }
        };

    template <>
    struct
    rotx_m_get<2,2>
        {
        template <class T>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        T
        get( T const & angle )
            {
            return cos(angle);
            }
        };
    }

template <int Dim,class Angle>
struct
mat_traits< qvm_detail::rotx_mat_<Dim,Angle> >
    {
    typedef qvm_detail::rotx_mat_<Dim,Angle> this_matrix;
    typedef Angle scalar_type;
    static int const rows=Dim;
    static int const cols=Dim;

    template <int Row,int Col>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_matrix const & x )
        {
        BOOST_QVM_STATIC_ASSERT(Row>=0);
        BOOST_QVM_STATIC_ASSERT(Col>=0);
        BOOST_QVM_STATIC_ASSERT(Row<Dim);
        BOOST_QVM_STATIC_ASSERT(Col<Dim);
        return qvm_detail::rotx_m_get<Row,Col>::get(reinterpret_cast<Angle const &>(x));
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int row, int col, this_matrix const & x )
        {
        BOOST_QVM_ASSERT(row>=0);
        BOOST_QVM_ASSERT(col>=0);
        BOOST_QVM_ASSERT(row<Dim);
        BOOST_QVM_ASSERT(col<Dim);
        Angle const & a=reinterpret_cast<Angle const &>(x);
        if( row==1 )
            {
            if( col==1 )
                return cos(a);
            if( col==2 )
                return -sin(a);
            }
        if( row==2 )
            {
            if( col==1 )
                return sin(a);
            if( col==2 )
                return cos(a);
            }
        return scalar_traits<scalar_type>::value(row==col);
        }
    };

template <int Dim,class Angle>
struct
deduce_mat<qvm_detail::rotx_mat_<Dim,Angle>,Dim,Dim>
    {
    typedef mat<Angle,Dim,Dim> type;
    };

template <int Dim,class Angle>
struct
deduce_mat2<qvm_detail::rotx_mat_<Dim,Angle>,qvm_detail::rotx_mat_<Dim,Angle>,Dim,Dim>
    {
    typedef mat<Angle,Dim,Dim> type;
    };

template <int Dim,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
qvm_detail::rotx_mat_<Dim,Angle> const &
rotx_mat( Angle const & angle )
    {
    BOOST_QVM_STATIC_ASSERT(Dim>=3);
    return reinterpret_cast<qvm_detail::rotx_mat_<Dim,Angle> const &>(angle);
    }

template <class A,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value &&
    mat_traits<A>::rows>=3 &&
    mat_traits<A>::rows==mat_traits<A>::cols,
    void>::type
set_rotx( A & a, Angle angle )
    {
    assign(a,rotx_mat<mat_traits<A>::rows>(angle));
    }

template <class A,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value &&
    mat_traits<A>::rows>=3 &&
    mat_traits<A>::rows==mat_traits<A>::cols,
    void>::type
rotate_x( A & a, Angle angle )
    {
    a *= rotx_mat<mat_traits<A>::rows>(angle);
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int Dim,class Angle>
    struct
    roty_mat_
        {
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        roty_mat_()
            {
            }

        template <class R
#if __cplusplus >= 201103L
            , class = typename enable_if<is_mat<R> >::type
#endif
        >
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        operator R() const
            {
            R r;
            assign(r,*this);
            return r;
            }

        private:

        roty_mat_( roty_mat_ const & );
        roty_mat_ & operator=( roty_mat_ const & );
        ~roty_mat_();
        };

    template <int Row,int Col>
    struct
    roty_m_get
        {
        template <class T>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        T
        get( T const & )
            {
            return scalar_traits<T>::value(Row==Col);
            }
        };

    template <>
    struct
    roty_m_get<0,0>
        {
        template <class T>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        T
        get( T const & angle )
            {
            return cos(angle);
            }
        };

    template <>
    struct
    roty_m_get<0,2>
        {
        template <class T>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        T
        get( T const & angle )
            {
            return sin(angle);
            }
        };

    template <>
    struct
    roty_m_get<2,0>
        {
        template <class T>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        T
        get( T const & angle )
            {
            return -sin(angle);
            }
        };

    template <>
    struct
    roty_m_get<2,2>
        {
        template <class T>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        T
        get( T const & angle )
            {
            return cos(angle);
            }
        };
    }

template <int Dim,class Angle>
struct
mat_traits< qvm_detail::roty_mat_<Dim,Angle> >
    {
    typedef qvm_detail::roty_mat_<Dim,Angle> this_matrix;
    typedef Angle scalar_type;
    static int const rows=Dim;
    static int const cols=Dim;

    template <int Row,int Col>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_matrix const & x )
        {
        BOOST_QVM_STATIC_ASSERT(Row>=0);
        BOOST_QVM_STATIC_ASSERT(Col>=0);
        BOOST_QVM_STATIC_ASSERT(Row<Dim);
        BOOST_QVM_STATIC_ASSERT(Col<Dim);
        return qvm_detail::roty_m_get<Row,Col>::get(reinterpret_cast<Angle const &>(x));
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int row, int col, this_matrix const & x )
        {
        BOOST_QVM_ASSERT(row>=0);
        BOOST_QVM_ASSERT(col>=0);
        BOOST_QVM_ASSERT(row<Dim);
        BOOST_QVM_ASSERT(col<Dim);
        Angle const & a=reinterpret_cast<Angle const &>(x);
        if( row==0 )
            {
            if( col==0 )
                return cos(a);
            if( col==2 )
                return sin(a);
            }
        if( row==2 )
            {
            if( col==0 )
                return -sin(a);
            if( col==2 )
                return cos(a);
            }
        return scalar_traits<scalar_type>::value(row==col);
        }
    };

template <int Dim,class Angle>
struct
deduce_mat<qvm_detail::roty_mat_<Dim,Angle>,Dim,Dim>
    {
    typedef mat<Angle,Dim,Dim> type;
    };

template <int Dim,class Angle>
struct
deduce_mat2<qvm_detail::roty_mat_<Dim,Angle>,qvm_detail::roty_mat_<Dim,Angle>,Dim,Dim>
    {
    typedef mat<Angle,Dim,Dim> type;
    };

template <int Dim,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
qvm_detail::roty_mat_<Dim,Angle> const &
roty_mat( Angle const & angle )
    {
    BOOST_QVM_STATIC_ASSERT(Dim>=3);
    return reinterpret_cast<qvm_detail::roty_mat_<Dim,Angle> const &>(angle);
    }

template <class A,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value &&
    mat_traits<A>::rows>=2 &&
    mat_traits<A>::rows==mat_traits<A>::cols,
    void>::type
set_roty( A & a, Angle angle )
    {
    assign(a,roty_mat<mat_traits<A>::rows>(angle));
    }

template <class A,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value &&
    mat_traits<A>::rows>=3 &&
    mat_traits<A>::rows==mat_traits<A>::cols,
    void>::type
rotate_y( A & a, Angle angle )
    {
    a *= roty_mat<mat_traits<A>::rows>(angle);
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int Dim,class Angle>
    struct
    rotz_mat_
        {
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        rotz_mat_()
            {
            }

        template <class R
#if __cplusplus >= 201103L
            , class = typename enable_if<is_mat<R> >::type
#endif
        >
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        operator R() const
            {
            R r;
            assign(r,*this);
            return r;
            }

        private:

        rotz_mat_( rotz_mat_ const & );
        rotz_mat_ & operator=( rotz_mat_ const & );
        ~rotz_mat_();
        };

    template <int Row,int Col>
    struct
    rotz_m_get
        {
        template <class T>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        T
        get( T const & )
            {
            return scalar_traits<T>::value(Row==Col);
            }
        };

    template <>
    struct
    rotz_m_get<0,0>
        {
        template <class T>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        T
        get( T const & angle )
            {
            return cos(angle);
            }
        };

    template <>
    struct
    rotz_m_get<0,1>
        {
        template <class T>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        T
        get( T const & angle )
            {
            return -sin(angle);
            }
        };

    template <>
    struct
    rotz_m_get<1,0>
        {
        template <class T>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        T
        get( T const & angle )
            {
            return sin(angle);
            }
        };

    template <>
    struct
    rotz_m_get<1,1>
        {
        template <class T>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        T
        get( T const & angle )
            {
            return cos(angle);
            }
        };
    }

template <int Dim,class Angle>
struct
mat_traits< qvm_detail::rotz_mat_<Dim,Angle> >
    {
    typedef qvm_detail::rotz_mat_<Dim,Angle> this_matrix;
    typedef Angle scalar_type;
    static int const rows=Dim;
    static int const cols=Dim;

    template <int Row,int Col>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_matrix const & x )
        {
        BOOST_QVM_STATIC_ASSERT(Row>=0);
        BOOST_QVM_STATIC_ASSERT(Col>=0);
        BOOST_QVM_STATIC_ASSERT(Row<Dim);
        BOOST_QVM_STATIC_ASSERT(Col<Dim);
        return qvm_detail::rotz_m_get<Row,Col>::get(reinterpret_cast<Angle const &>(x));
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int row, int col, this_matrix const & x )
        {
        BOOST_QVM_ASSERT(row>=0);
        BOOST_QVM_ASSERT(col>=0);
        BOOST_QVM_ASSERT(row<Dim);
        BOOST_QVM_ASSERT(col<Dim);
        Angle const & a=reinterpret_cast<Angle const &>(x);
        if( row==0 )
            {
            if( col==0 )
                return cos(a);
            if( col==1 )
                return -sin(a);
            }
        if( row==1 )
            {
            if( col==0 )
                return sin(a);
            if( col==1 )
                return cos(a);
            }
        return scalar_traits<scalar_type>::value(row==col);
        }
    };

template <int Dim,class Angle>
struct
deduce_mat<qvm_detail::rotz_mat_<Dim,Angle>,Dim,Dim>
    {
    typedef mat<Angle,Dim,Dim> type;
    };

template <int Dim,class Angle,int R,int C>
struct
deduce_mat2<qvm_detail::rotz_mat_<Dim,Angle>,qvm_detail::rotz_mat_<Dim,Angle>,R,C>
    {
    typedef mat<Angle,R,C> type;
    };

template <int Dim,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
qvm_detail::rotz_mat_<Dim,Angle> const &
rotz_mat( Angle const & angle )
    {
    BOOST_QVM_STATIC_ASSERT(Dim>=2);
    return reinterpret_cast<qvm_detail::rotz_mat_<Dim,Angle> const &>(angle);
    }

template <class A,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value &&
    mat_traits<A>::rows>=2 &&
    mat_traits<A>::rows==mat_traits<A>::cols,
    void>::type
set_rotz( A & a, Angle angle )
    {
    assign(a,rotz_mat<mat_traits<A>::rows>(angle));
    }

template <class A,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_mat<A>::value &&
    mat_traits<A>::rows>=2 &&
    mat_traits<A>::rows==mat_traits<A>::cols,
    void>::type
rotate_z( A & a, Angle angle )
    {
    a *= rotz_mat<mat_traits<A>::rows>(angle);
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int D>
    struct
    inverse_m_defined
        {
        static bool const value=false;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
typename lazy_enable_if_c<
    is_mat<A>::value && is_scalar<B>::value &&
    mat_traits<A>::rows==mat_traits<A>::cols &&
    !qvm_detail::inverse_m_defined<mat_traits<A>::rows>::value,
    deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols> >::type
inverse( A const & a, B det )
    {
    typedef typename mat_traits<A>::scalar_type T;
    BOOST_QVM_ASSERT(det!=scalar_traits<T>::value(0));
    T f=scalar_traits<T>::value(1)/det;
    typedef typename deduce_mat2<A,B,mat_traits<A>::rows,mat_traits<A>::cols>::type cofactor_return_type;
    cofactor_return_type c=qvm_detail::cofactor_impl(a);
    return reinterpret_cast<qvm_detail::transposed_<cofactor_return_type> const &>(c) * f;
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
typename lazy_enable_if_c<
    is_mat<A>::value &&
    mat_traits<A>::rows==mat_traits<A>::cols &&
    !qvm_detail::inverse_m_defined<mat_traits<A>::rows>::value,
    deduce_mat<A> >::type
inverse( A const & a )
    {
    typedef typename mat_traits<A>::scalar_type T;
    T det=determinant(a);
    if( det==scalar_traits<T>::value(0) )
        BOOST_QVM_THROW_EXCEPTION(zero_determinant_error());
    return inverse(a,det);
    }

////////////////////////////////////////////////

namespace
sfinae
    {
    using ::boost::qvm::to_string;
    using ::boost::qvm::assign;
    using ::boost::qvm::determinant;
    using ::boost::qvm::cmp;
    using ::boost::qvm::convert_to;
    using ::boost::qvm::set_identity;
    using ::boost::qvm::set_zero;
    using ::boost::qvm::scalar_cast;
    using ::boost::qvm::operator/=;
    using ::boost::qvm::operator/;
    using ::boost::qvm::operator==;
    using ::boost::qvm::operator-=;
    using ::boost::qvm::operator-;
    using ::boost::qvm::operator*=;
    using ::boost::qvm::operator*;
    using ::boost::qvm::operator!=;
    using ::boost::qvm::operator+=;
    using ::boost::qvm::operator+;
    using ::boost::qvm::mref;
    using ::boost::qvm::rot_mat;
    using ::boost::qvm::set_rot;
    using ::boost::qvm::rotate;
    using ::boost::qvm::set_rotx;
    using ::boost::qvm::rotate_x;
    using ::boost::qvm::set_roty;
    using ::boost::qvm::rotate_y;
    using ::boost::qvm::set_rotz;
    using ::boost::qvm::rotate_z;
    using ::boost::qvm::inverse;
    }

} }

#endif
// <<< #include <boost/qvm/mat_operations.hpp>
// #line 8 "boost/qvm/lite.hpp"
// >>> #include <boost/qvm/mat_access.hpp>
#ifndef BOOST_QVM_MAT_ACCESS_HPP_INCLUDED
#define BOOST_QVM_MAT_ACCESS_HPP_INCLUDED

// #line 8 "boost/qvm/mat_access.hpp"
// #include <boost/qvm/mat_traits.hpp> // Expanded at line 303
// #include <boost/qvm/config.hpp> // Expanded at line 109
// #include <boost/qvm/static_assert.hpp> // Expanded at line 1180
// #include <boost/qvm/enable_if.hpp> // Expanded at line 177

namespace boost { namespace qvm {

template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A00( M const & a ) { BOOST_QVM_STATIC_ASSERT(0<mat_traits<M>::rows && 0<mat_traits<M>::cols); return mat_traits<M>::template read_element<0,0>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A01( M const & a ) { BOOST_QVM_STATIC_ASSERT(0<mat_traits<M>::rows && 1<mat_traits<M>::cols); return mat_traits<M>::template read_element<0,1>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A02( M const & a ) { BOOST_QVM_STATIC_ASSERT(0<mat_traits<M>::rows && 2<mat_traits<M>::cols); return mat_traits<M>::template read_element<0,2>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A03( M const & a ) { BOOST_QVM_STATIC_ASSERT(0<mat_traits<M>::rows && 3<mat_traits<M>::cols); return mat_traits<M>::template read_element<0,3>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A04( M const & a ) { BOOST_QVM_STATIC_ASSERT(0<mat_traits<M>::rows && 4<mat_traits<M>::cols); return mat_traits<M>::template read_element<0,4>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A05( M const & a ) { BOOST_QVM_STATIC_ASSERT(0<mat_traits<M>::rows && 5<mat_traits<M>::cols); return mat_traits<M>::template read_element<0,5>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A06( M const & a ) { BOOST_QVM_STATIC_ASSERT(0<mat_traits<M>::rows && 6<mat_traits<M>::cols); return mat_traits<M>::template read_element<0,6>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A07( M const & a ) { BOOST_QVM_STATIC_ASSERT(0<mat_traits<M>::rows && 7<mat_traits<M>::cols); return mat_traits<M>::template read_element<0,7>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A08( M const & a ) { BOOST_QVM_STATIC_ASSERT(0<mat_traits<M>::rows && 8<mat_traits<M>::cols); return mat_traits<M>::template read_element<0,8>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A09( M const & a ) { BOOST_QVM_STATIC_ASSERT(0<mat_traits<M>::rows && 9<mat_traits<M>::cols); return mat_traits<M>::template read_element<0,9>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A10( M const & a ) { BOOST_QVM_STATIC_ASSERT(1<mat_traits<M>::rows && 0<mat_traits<M>::cols); return mat_traits<M>::template read_element<1,0>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A11( M const & a ) { BOOST_QVM_STATIC_ASSERT(1<mat_traits<M>::rows && 1<mat_traits<M>::cols); return mat_traits<M>::template read_element<1,1>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A12( M const & a ) { BOOST_QVM_STATIC_ASSERT(1<mat_traits<M>::rows && 2<mat_traits<M>::cols); return mat_traits<M>::template read_element<1,2>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A13( M const & a ) { BOOST_QVM_STATIC_ASSERT(1<mat_traits<M>::rows && 3<mat_traits<M>::cols); return mat_traits<M>::template read_element<1,3>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A14( M const & a ) { BOOST_QVM_STATIC_ASSERT(1<mat_traits<M>::rows && 4<mat_traits<M>::cols); return mat_traits<M>::template read_element<1,4>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A15( M const & a ) { BOOST_QVM_STATIC_ASSERT(1<mat_traits<M>::rows && 5<mat_traits<M>::cols); return mat_traits<M>::template read_element<1,5>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A16( M const & a ) { BOOST_QVM_STATIC_ASSERT(1<mat_traits<M>::rows && 6<mat_traits<M>::cols); return mat_traits<M>::template read_element<1,6>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A17( M const & a ) { BOOST_QVM_STATIC_ASSERT(1<mat_traits<M>::rows && 7<mat_traits<M>::cols); return mat_traits<M>::template read_element<1,7>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A18( M const & a ) { BOOST_QVM_STATIC_ASSERT(1<mat_traits<M>::rows && 8<mat_traits<M>::cols); return mat_traits<M>::template read_element<1,8>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A19( M const & a ) { BOOST_QVM_STATIC_ASSERT(1<mat_traits<M>::rows && 9<mat_traits<M>::cols); return mat_traits<M>::template read_element<1,9>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A20( M const & a ) { BOOST_QVM_STATIC_ASSERT(2<mat_traits<M>::rows && 0<mat_traits<M>::cols); return mat_traits<M>::template read_element<2,0>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A21( M const & a ) { BOOST_QVM_STATIC_ASSERT(2<mat_traits<M>::rows && 1<mat_traits<M>::cols); return mat_traits<M>::template read_element<2,1>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A22( M const & a ) { BOOST_QVM_STATIC_ASSERT(2<mat_traits<M>::rows && 2<mat_traits<M>::cols); return mat_traits<M>::template read_element<2,2>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A23( M const & a ) { BOOST_QVM_STATIC_ASSERT(2<mat_traits<M>::rows && 3<mat_traits<M>::cols); return mat_traits<M>::template read_element<2,3>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A24( M const & a ) { BOOST_QVM_STATIC_ASSERT(2<mat_traits<M>::rows && 4<mat_traits<M>::cols); return mat_traits<M>::template read_element<2,4>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A25( M const & a ) { BOOST_QVM_STATIC_ASSERT(2<mat_traits<M>::rows && 5<mat_traits<M>::cols); return mat_traits<M>::template read_element<2,5>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A26( M const & a ) { BOOST_QVM_STATIC_ASSERT(2<mat_traits<M>::rows && 6<mat_traits<M>::cols); return mat_traits<M>::template read_element<2,6>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A27( M const & a ) { BOOST_QVM_STATIC_ASSERT(2<mat_traits<M>::rows && 7<mat_traits<M>::cols); return mat_traits<M>::template read_element<2,7>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A28( M const & a ) { BOOST_QVM_STATIC_ASSERT(2<mat_traits<M>::rows && 8<mat_traits<M>::cols); return mat_traits<M>::template read_element<2,8>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A29( M const & a ) { BOOST_QVM_STATIC_ASSERT(2<mat_traits<M>::rows && 9<mat_traits<M>::cols); return mat_traits<M>::template read_element<2,9>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A30( M const & a ) { BOOST_QVM_STATIC_ASSERT(3<mat_traits<M>::rows && 0<mat_traits<M>::cols); return mat_traits<M>::template read_element<3,0>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A31( M const & a ) { BOOST_QVM_STATIC_ASSERT(3<mat_traits<M>::rows && 1<mat_traits<M>::cols); return mat_traits<M>::template read_element<3,1>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A32( M const & a ) { BOOST_QVM_STATIC_ASSERT(3<mat_traits<M>::rows && 2<mat_traits<M>::cols); return mat_traits<M>::template read_element<3,2>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A33( M const & a ) { BOOST_QVM_STATIC_ASSERT(3<mat_traits<M>::rows && 3<mat_traits<M>::cols); return mat_traits<M>::template read_element<3,3>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A34( M const & a ) { BOOST_QVM_STATIC_ASSERT(3<mat_traits<M>::rows && 4<mat_traits<M>::cols); return mat_traits<M>::template read_element<3,4>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A35( M const & a ) { BOOST_QVM_STATIC_ASSERT(3<mat_traits<M>::rows && 5<mat_traits<M>::cols); return mat_traits<M>::template read_element<3,5>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A36( M const & a ) { BOOST_QVM_STATIC_ASSERT(3<mat_traits<M>::rows && 6<mat_traits<M>::cols); return mat_traits<M>::template read_element<3,6>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A37( M const & a ) { BOOST_QVM_STATIC_ASSERT(3<mat_traits<M>::rows && 7<mat_traits<M>::cols); return mat_traits<M>::template read_element<3,7>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A38( M const & a ) { BOOST_QVM_STATIC_ASSERT(3<mat_traits<M>::rows && 8<mat_traits<M>::cols); return mat_traits<M>::template read_element<3,8>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A39( M const & a ) { BOOST_QVM_STATIC_ASSERT(3<mat_traits<M>::rows && 9<mat_traits<M>::cols); return mat_traits<M>::template read_element<3,9>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A40( M const & a ) { BOOST_QVM_STATIC_ASSERT(4<mat_traits<M>::rows && 0<mat_traits<M>::cols); return mat_traits<M>::template read_element<4,0>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A41( M const & a ) { BOOST_QVM_STATIC_ASSERT(4<mat_traits<M>::rows && 1<mat_traits<M>::cols); return mat_traits<M>::template read_element<4,1>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A42( M const & a ) { BOOST_QVM_STATIC_ASSERT(4<mat_traits<M>::rows && 2<mat_traits<M>::cols); return mat_traits<M>::template read_element<4,2>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A43( M const & a ) { BOOST_QVM_STATIC_ASSERT(4<mat_traits<M>::rows && 3<mat_traits<M>::cols); return mat_traits<M>::template read_element<4,3>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A44( M const & a ) { BOOST_QVM_STATIC_ASSERT(4<mat_traits<M>::rows && 4<mat_traits<M>::cols); return mat_traits<M>::template read_element<4,4>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A45( M const & a ) { BOOST_QVM_STATIC_ASSERT(4<mat_traits<M>::rows && 5<mat_traits<M>::cols); return mat_traits<M>::template read_element<4,5>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A46( M const & a ) { BOOST_QVM_STATIC_ASSERT(4<mat_traits<M>::rows && 6<mat_traits<M>::cols); return mat_traits<M>::template read_element<4,6>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A47( M const & a ) { BOOST_QVM_STATIC_ASSERT(4<mat_traits<M>::rows && 7<mat_traits<M>::cols); return mat_traits<M>::template read_element<4,7>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A48( M const & a ) { BOOST_QVM_STATIC_ASSERT(4<mat_traits<M>::rows && 8<mat_traits<M>::cols); return mat_traits<M>::template read_element<4,8>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A49( M const & a ) { BOOST_QVM_STATIC_ASSERT(4<mat_traits<M>::rows && 9<mat_traits<M>::cols); return mat_traits<M>::template read_element<4,9>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A50( M const & a ) { BOOST_QVM_STATIC_ASSERT(5<mat_traits<M>::rows && 0<mat_traits<M>::cols); return mat_traits<M>::template read_element<5,0>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A51( M const & a ) { BOOST_QVM_STATIC_ASSERT(5<mat_traits<M>::rows && 1<mat_traits<M>::cols); return mat_traits<M>::template read_element<5,1>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A52( M const & a ) { BOOST_QVM_STATIC_ASSERT(5<mat_traits<M>::rows && 2<mat_traits<M>::cols); return mat_traits<M>::template read_element<5,2>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A53( M const & a ) { BOOST_QVM_STATIC_ASSERT(5<mat_traits<M>::rows && 3<mat_traits<M>::cols); return mat_traits<M>::template read_element<5,3>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A54( M const & a ) { BOOST_QVM_STATIC_ASSERT(5<mat_traits<M>::rows && 4<mat_traits<M>::cols); return mat_traits<M>::template read_element<5,4>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A55( M const & a ) { BOOST_QVM_STATIC_ASSERT(5<mat_traits<M>::rows && 5<mat_traits<M>::cols); return mat_traits<M>::template read_element<5,5>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A56( M const & a ) { BOOST_QVM_STATIC_ASSERT(5<mat_traits<M>::rows && 6<mat_traits<M>::cols); return mat_traits<M>::template read_element<5,6>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A57( M const & a ) { BOOST_QVM_STATIC_ASSERT(5<mat_traits<M>::rows && 7<mat_traits<M>::cols); return mat_traits<M>::template read_element<5,7>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A58( M const & a ) { BOOST_QVM_STATIC_ASSERT(5<mat_traits<M>::rows && 8<mat_traits<M>::cols); return mat_traits<M>::template read_element<5,8>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A59( M const & a ) { BOOST_QVM_STATIC_ASSERT(5<mat_traits<M>::rows && 9<mat_traits<M>::cols); return mat_traits<M>::template read_element<5,9>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A60( M const & a ) { BOOST_QVM_STATIC_ASSERT(6<mat_traits<M>::rows && 0<mat_traits<M>::cols); return mat_traits<M>::template read_element<6,0>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A61( M const & a ) { BOOST_QVM_STATIC_ASSERT(6<mat_traits<M>::rows && 1<mat_traits<M>::cols); return mat_traits<M>::template read_element<6,1>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A62( M const & a ) { BOOST_QVM_STATIC_ASSERT(6<mat_traits<M>::rows && 2<mat_traits<M>::cols); return mat_traits<M>::template read_element<6,2>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A63( M const & a ) { BOOST_QVM_STATIC_ASSERT(6<mat_traits<M>::rows && 3<mat_traits<M>::cols); return mat_traits<M>::template read_element<6,3>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A64( M const & a ) { BOOST_QVM_STATIC_ASSERT(6<mat_traits<M>::rows && 4<mat_traits<M>::cols); return mat_traits<M>::template read_element<6,4>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A65( M const & a ) { BOOST_QVM_STATIC_ASSERT(6<mat_traits<M>::rows && 5<mat_traits<M>::cols); return mat_traits<M>::template read_element<6,5>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A66( M const & a ) { BOOST_QVM_STATIC_ASSERT(6<mat_traits<M>::rows && 6<mat_traits<M>::cols); return mat_traits<M>::template read_element<6,6>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A67( M const & a ) { BOOST_QVM_STATIC_ASSERT(6<mat_traits<M>::rows && 7<mat_traits<M>::cols); return mat_traits<M>::template read_element<6,7>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A68( M const & a ) { BOOST_QVM_STATIC_ASSERT(6<mat_traits<M>::rows && 8<mat_traits<M>::cols); return mat_traits<M>::template read_element<6,8>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A69( M const & a ) { BOOST_QVM_STATIC_ASSERT(6<mat_traits<M>::rows && 9<mat_traits<M>::cols); return mat_traits<M>::template read_element<6,9>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A70( M const & a ) { BOOST_QVM_STATIC_ASSERT(7<mat_traits<M>::rows && 0<mat_traits<M>::cols); return mat_traits<M>::template read_element<7,0>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A71( M const & a ) { BOOST_QVM_STATIC_ASSERT(7<mat_traits<M>::rows && 1<mat_traits<M>::cols); return mat_traits<M>::template read_element<7,1>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A72( M const & a ) { BOOST_QVM_STATIC_ASSERT(7<mat_traits<M>::rows && 2<mat_traits<M>::cols); return mat_traits<M>::template read_element<7,2>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A73( M const & a ) { BOOST_QVM_STATIC_ASSERT(7<mat_traits<M>::rows && 3<mat_traits<M>::cols); return mat_traits<M>::template read_element<7,3>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A74( M const & a ) { BOOST_QVM_STATIC_ASSERT(7<mat_traits<M>::rows && 4<mat_traits<M>::cols); return mat_traits<M>::template read_element<7,4>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A75( M const & a ) { BOOST_QVM_STATIC_ASSERT(7<mat_traits<M>::rows && 5<mat_traits<M>::cols); return mat_traits<M>::template read_element<7,5>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A76( M const & a ) { BOOST_QVM_STATIC_ASSERT(7<mat_traits<M>::rows && 6<mat_traits<M>::cols); return mat_traits<M>::template read_element<7,6>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A77( M const & a ) { BOOST_QVM_STATIC_ASSERT(7<mat_traits<M>::rows && 7<mat_traits<M>::cols); return mat_traits<M>::template read_element<7,7>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A78( M const & a ) { BOOST_QVM_STATIC_ASSERT(7<mat_traits<M>::rows && 8<mat_traits<M>::cols); return mat_traits<M>::template read_element<7,8>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A79( M const & a ) { BOOST_QVM_STATIC_ASSERT(7<mat_traits<M>::rows && 9<mat_traits<M>::cols); return mat_traits<M>::template read_element<7,9>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A80( M const & a ) { BOOST_QVM_STATIC_ASSERT(8<mat_traits<M>::rows && 0<mat_traits<M>::cols); return mat_traits<M>::template read_element<8,0>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A81( M const & a ) { BOOST_QVM_STATIC_ASSERT(8<mat_traits<M>::rows && 1<mat_traits<M>::cols); return mat_traits<M>::template read_element<8,1>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A82( M const & a ) { BOOST_QVM_STATIC_ASSERT(8<mat_traits<M>::rows && 2<mat_traits<M>::cols); return mat_traits<M>::template read_element<8,2>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A83( M const & a ) { BOOST_QVM_STATIC_ASSERT(8<mat_traits<M>::rows && 3<mat_traits<M>::cols); return mat_traits<M>::template read_element<8,3>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A84( M const & a ) { BOOST_QVM_STATIC_ASSERT(8<mat_traits<M>::rows && 4<mat_traits<M>::cols); return mat_traits<M>::template read_element<8,4>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A85( M const & a ) { BOOST_QVM_STATIC_ASSERT(8<mat_traits<M>::rows && 5<mat_traits<M>::cols); return mat_traits<M>::template read_element<8,5>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A86( M const & a ) { BOOST_QVM_STATIC_ASSERT(8<mat_traits<M>::rows && 6<mat_traits<M>::cols); return mat_traits<M>::template read_element<8,6>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A87( M const & a ) { BOOST_QVM_STATIC_ASSERT(8<mat_traits<M>::rows && 7<mat_traits<M>::cols); return mat_traits<M>::template read_element<8,7>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A88( M const & a ) { BOOST_QVM_STATIC_ASSERT(8<mat_traits<M>::rows && 8<mat_traits<M>::cols); return mat_traits<M>::template read_element<8,8>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A89( M const & a ) { BOOST_QVM_STATIC_ASSERT(8<mat_traits<M>::rows && 9<mat_traits<M>::cols); return mat_traits<M>::template read_element<8,9>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A90( M const & a ) { BOOST_QVM_STATIC_ASSERT(9<mat_traits<M>::rows && 0<mat_traits<M>::cols); return mat_traits<M>::template read_element<9,0>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A91( M const & a ) { BOOST_QVM_STATIC_ASSERT(9<mat_traits<M>::rows && 1<mat_traits<M>::cols); return mat_traits<M>::template read_element<9,1>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A92( M const & a ) { BOOST_QVM_STATIC_ASSERT(9<mat_traits<M>::rows && 2<mat_traits<M>::cols); return mat_traits<M>::template read_element<9,2>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A93( M const & a ) { BOOST_QVM_STATIC_ASSERT(9<mat_traits<M>::rows && 3<mat_traits<M>::cols); return mat_traits<M>::template read_element<9,3>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A94( M const & a ) { BOOST_QVM_STATIC_ASSERT(9<mat_traits<M>::rows && 4<mat_traits<M>::cols); return mat_traits<M>::template read_element<9,4>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A95( M const & a ) { BOOST_QVM_STATIC_ASSERT(9<mat_traits<M>::rows && 5<mat_traits<M>::cols); return mat_traits<M>::template read_element<9,5>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A96( M const & a ) { BOOST_QVM_STATIC_ASSERT(9<mat_traits<M>::rows && 6<mat_traits<M>::cols); return mat_traits<M>::template read_element<9,6>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A97( M const & a ) { BOOST_QVM_STATIC_ASSERT(9<mat_traits<M>::rows && 7<mat_traits<M>::cols); return mat_traits<M>::template read_element<9,7>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A98( M const & a ) { BOOST_QVM_STATIC_ASSERT(9<mat_traits<M>::rows && 8<mat_traits<M>::cols); return mat_traits<M>::template read_element<9,8>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value,typename mat_traits<M>::scalar_type>::type A99( M const & a ) { BOOST_QVM_STATIC_ASSERT(9<mat_traits<M>::rows && 9<mat_traits<M>::cols); return mat_traits<M>::template read_element<9,9>(a); }

namespace
qvm_detail
    {
    template <int R,int C,class M>
    struct
    m_element_access
        {
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        void
        operator=( typename mat_traits<M>::scalar_type s )
            {
            mat_traits<M>::template write_element<R,C>(*reinterpret_cast<M *>(this), s);
            }

        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        operator typename mat_traits<M>::scalar_type() const
            {
            return mat_traits<M>::template read_element<R,C>(*reinterpret_cast<M const *>(this));
            }
        };
    }

template <int R,int C,class M>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
typename enable_if_c<
    is_mat<M>::value,
    typename mat_traits<M>::scalar_type>::type
A( M const & a )
    {
    BOOST_QVM_STATIC_ASSERT(R>=0);
    BOOST_QVM_STATIC_ASSERT(R<mat_traits<M>::rows);
    BOOST_QVM_STATIC_ASSERT(C>=0);
    BOOST_QVM_STATIC_ASSERT(C<mat_traits<M>::cols);
    return mat_traits<M>::template read_element<R,C>(a);
    }

template <int R,int C,class M>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
typename enable_if_c<
    is_mat<M>::value && mat_write_element_ref<M>::value,
    typename mat_traits<M>::scalar_type &>::type
A( M & a )
    {
    BOOST_QVM_STATIC_ASSERT(R>=0);
    BOOST_QVM_STATIC_ASSERT(R<mat_traits<M>::rows);
    BOOST_QVM_STATIC_ASSERT(C>=0);
    BOOST_QVM_STATIC_ASSERT(C<mat_traits<M>::cols);
    return mat_traits<M>::template write_element<R,C>(a);
    }

template <int R,int C,class M>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
typename enable_if_c<
    is_mat<M>::value && !mat_write_element_ref<M>::value,
    qvm_detail::m_element_access<R,C,M> &>::type
A( M & a )
    {
    BOOST_QVM_STATIC_ASSERT(R>=0);
    BOOST_QVM_STATIC_ASSERT(R<mat_traits<M>::rows);
    BOOST_QVM_STATIC_ASSERT(C>=0);
    BOOST_QVM_STATIC_ASSERT(C<mat_traits<M>::cols);
    return *reinterpret_cast<qvm_detail::m_element_access<R,C,M> *>(&a);
    }

////////////////////////////////////////////////

template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A00( M & a ) { BOOST_QVM_STATIC_ASSERT(0<mat_traits<M>::rows && 0<mat_traits<M>::cols); return mat_traits<M>::template write_element<0,0>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A01( M & a ) { BOOST_QVM_STATIC_ASSERT(0<mat_traits<M>::rows && 1<mat_traits<M>::cols); return mat_traits<M>::template write_element<0,1>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A02( M & a ) { BOOST_QVM_STATIC_ASSERT(0<mat_traits<M>::rows && 2<mat_traits<M>::cols); return mat_traits<M>::template write_element<0,2>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A03( M & a ) { BOOST_QVM_STATIC_ASSERT(0<mat_traits<M>::rows && 3<mat_traits<M>::cols); return mat_traits<M>::template write_element<0,3>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A04( M & a ) { BOOST_QVM_STATIC_ASSERT(0<mat_traits<M>::rows && 4<mat_traits<M>::cols); return mat_traits<M>::template write_element<0,4>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A05( M & a ) { BOOST_QVM_STATIC_ASSERT(0<mat_traits<M>::rows && 5<mat_traits<M>::cols); return mat_traits<M>::template write_element<0,5>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A06( M & a ) { BOOST_QVM_STATIC_ASSERT(0<mat_traits<M>::rows && 6<mat_traits<M>::cols); return mat_traits<M>::template write_element<0,6>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A07( M & a ) { BOOST_QVM_STATIC_ASSERT(0<mat_traits<M>::rows && 7<mat_traits<M>::cols); return mat_traits<M>::template write_element<0,7>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A08( M & a ) { BOOST_QVM_STATIC_ASSERT(0<mat_traits<M>::rows && 8<mat_traits<M>::cols); return mat_traits<M>::template write_element<0,8>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A09( M & a ) { BOOST_QVM_STATIC_ASSERT(0<mat_traits<M>::rows && 9<mat_traits<M>::cols); return mat_traits<M>::template write_element<0,9>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A10( M & a ) { BOOST_QVM_STATIC_ASSERT(1<mat_traits<M>::rows && 0<mat_traits<M>::cols); return mat_traits<M>::template write_element<1,0>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A11( M & a ) { BOOST_QVM_STATIC_ASSERT(1<mat_traits<M>::rows && 1<mat_traits<M>::cols); return mat_traits<M>::template write_element<1,1>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A12( M & a ) { BOOST_QVM_STATIC_ASSERT(1<mat_traits<M>::rows && 2<mat_traits<M>::cols); return mat_traits<M>::template write_element<1,2>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A13( M & a ) { BOOST_QVM_STATIC_ASSERT(1<mat_traits<M>::rows && 3<mat_traits<M>::cols); return mat_traits<M>::template write_element<1,3>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A14( M & a ) { BOOST_QVM_STATIC_ASSERT(1<mat_traits<M>::rows && 4<mat_traits<M>::cols); return mat_traits<M>::template write_element<1,4>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A15( M & a ) { BOOST_QVM_STATIC_ASSERT(1<mat_traits<M>::rows && 5<mat_traits<M>::cols); return mat_traits<M>::template write_element<1,5>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A16( M & a ) { BOOST_QVM_STATIC_ASSERT(1<mat_traits<M>::rows && 6<mat_traits<M>::cols); return mat_traits<M>::template write_element<1,6>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A17( M & a ) { BOOST_QVM_STATIC_ASSERT(1<mat_traits<M>::rows && 7<mat_traits<M>::cols); return mat_traits<M>::template write_element<1,7>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A18( M & a ) { BOOST_QVM_STATIC_ASSERT(1<mat_traits<M>::rows && 8<mat_traits<M>::cols); return mat_traits<M>::template write_element<1,8>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A19( M & a ) { BOOST_QVM_STATIC_ASSERT(1<mat_traits<M>::rows && 9<mat_traits<M>::cols); return mat_traits<M>::template write_element<1,9>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A20( M & a ) { BOOST_QVM_STATIC_ASSERT(2<mat_traits<M>::rows && 0<mat_traits<M>::cols); return mat_traits<M>::template write_element<2,0>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A21( M & a ) { BOOST_QVM_STATIC_ASSERT(2<mat_traits<M>::rows && 1<mat_traits<M>::cols); return mat_traits<M>::template write_element<2,1>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A22( M & a ) { BOOST_QVM_STATIC_ASSERT(2<mat_traits<M>::rows && 2<mat_traits<M>::cols); return mat_traits<M>::template write_element<2,2>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A23( M & a ) { BOOST_QVM_STATIC_ASSERT(2<mat_traits<M>::rows && 3<mat_traits<M>::cols); return mat_traits<M>::template write_element<2,3>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A24( M & a ) { BOOST_QVM_STATIC_ASSERT(2<mat_traits<M>::rows && 4<mat_traits<M>::cols); return mat_traits<M>::template write_element<2,4>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A25( M & a ) { BOOST_QVM_STATIC_ASSERT(2<mat_traits<M>::rows && 5<mat_traits<M>::cols); return mat_traits<M>::template write_element<2,5>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A26( M & a ) { BOOST_QVM_STATIC_ASSERT(2<mat_traits<M>::rows && 6<mat_traits<M>::cols); return mat_traits<M>::template write_element<2,6>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A27( M & a ) { BOOST_QVM_STATIC_ASSERT(2<mat_traits<M>::rows && 7<mat_traits<M>::cols); return mat_traits<M>::template write_element<2,7>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A28( M & a ) { BOOST_QVM_STATIC_ASSERT(2<mat_traits<M>::rows && 8<mat_traits<M>::cols); return mat_traits<M>::template write_element<2,8>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A29( M & a ) { BOOST_QVM_STATIC_ASSERT(2<mat_traits<M>::rows && 9<mat_traits<M>::cols); return mat_traits<M>::template write_element<2,9>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A30( M & a ) { BOOST_QVM_STATIC_ASSERT(3<mat_traits<M>::rows && 0<mat_traits<M>::cols); return mat_traits<M>::template write_element<3,0>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A31( M & a ) { BOOST_QVM_STATIC_ASSERT(3<mat_traits<M>::rows && 1<mat_traits<M>::cols); return mat_traits<M>::template write_element<3,1>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A32( M & a ) { BOOST_QVM_STATIC_ASSERT(3<mat_traits<M>::rows && 2<mat_traits<M>::cols); return mat_traits<M>::template write_element<3,2>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A33( M & a ) { BOOST_QVM_STATIC_ASSERT(3<mat_traits<M>::rows && 3<mat_traits<M>::cols); return mat_traits<M>::template write_element<3,3>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A34( M & a ) { BOOST_QVM_STATIC_ASSERT(3<mat_traits<M>::rows && 4<mat_traits<M>::cols); return mat_traits<M>::template write_element<3,4>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A35( M & a ) { BOOST_QVM_STATIC_ASSERT(3<mat_traits<M>::rows && 5<mat_traits<M>::cols); return mat_traits<M>::template write_element<3,5>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A36( M & a ) { BOOST_QVM_STATIC_ASSERT(3<mat_traits<M>::rows && 6<mat_traits<M>::cols); return mat_traits<M>::template write_element<3,6>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A37( M & a ) { BOOST_QVM_STATIC_ASSERT(3<mat_traits<M>::rows && 7<mat_traits<M>::cols); return mat_traits<M>::template write_element<3,7>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A38( M & a ) { BOOST_QVM_STATIC_ASSERT(3<mat_traits<M>::rows && 8<mat_traits<M>::cols); return mat_traits<M>::template write_element<3,8>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A39( M & a ) { BOOST_QVM_STATIC_ASSERT(3<mat_traits<M>::rows && 9<mat_traits<M>::cols); return mat_traits<M>::template write_element<3,9>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A40( M & a ) { BOOST_QVM_STATIC_ASSERT(4<mat_traits<M>::rows && 0<mat_traits<M>::cols); return mat_traits<M>::template write_element<4,0>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A41( M & a ) { BOOST_QVM_STATIC_ASSERT(4<mat_traits<M>::rows && 1<mat_traits<M>::cols); return mat_traits<M>::template write_element<4,1>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A42( M & a ) { BOOST_QVM_STATIC_ASSERT(4<mat_traits<M>::rows && 2<mat_traits<M>::cols); return mat_traits<M>::template write_element<4,2>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A43( M & a ) { BOOST_QVM_STATIC_ASSERT(4<mat_traits<M>::rows && 3<mat_traits<M>::cols); return mat_traits<M>::template write_element<4,3>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A44( M & a ) { BOOST_QVM_STATIC_ASSERT(4<mat_traits<M>::rows && 4<mat_traits<M>::cols); return mat_traits<M>::template write_element<4,4>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A45( M & a ) { BOOST_QVM_STATIC_ASSERT(4<mat_traits<M>::rows && 5<mat_traits<M>::cols); return mat_traits<M>::template write_element<4,5>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A46( M & a ) { BOOST_QVM_STATIC_ASSERT(4<mat_traits<M>::rows && 6<mat_traits<M>::cols); return mat_traits<M>::template write_element<4,6>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A47( M & a ) { BOOST_QVM_STATIC_ASSERT(4<mat_traits<M>::rows && 7<mat_traits<M>::cols); return mat_traits<M>::template write_element<4,7>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A48( M & a ) { BOOST_QVM_STATIC_ASSERT(4<mat_traits<M>::rows && 8<mat_traits<M>::cols); return mat_traits<M>::template write_element<4,8>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A49( M & a ) { BOOST_QVM_STATIC_ASSERT(4<mat_traits<M>::rows && 9<mat_traits<M>::cols); return mat_traits<M>::template write_element<4,9>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A50( M & a ) { BOOST_QVM_STATIC_ASSERT(5<mat_traits<M>::rows && 0<mat_traits<M>::cols); return mat_traits<M>::template write_element<5,0>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A51( M & a ) { BOOST_QVM_STATIC_ASSERT(5<mat_traits<M>::rows && 1<mat_traits<M>::cols); return mat_traits<M>::template write_element<5,1>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A52( M & a ) { BOOST_QVM_STATIC_ASSERT(5<mat_traits<M>::rows && 2<mat_traits<M>::cols); return mat_traits<M>::template write_element<5,2>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A53( M & a ) { BOOST_QVM_STATIC_ASSERT(5<mat_traits<M>::rows && 3<mat_traits<M>::cols); return mat_traits<M>::template write_element<5,3>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A54( M & a ) { BOOST_QVM_STATIC_ASSERT(5<mat_traits<M>::rows && 4<mat_traits<M>::cols); return mat_traits<M>::template write_element<5,4>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A55( M & a ) { BOOST_QVM_STATIC_ASSERT(5<mat_traits<M>::rows && 5<mat_traits<M>::cols); return mat_traits<M>::template write_element<5,5>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A56( M & a ) { BOOST_QVM_STATIC_ASSERT(5<mat_traits<M>::rows && 6<mat_traits<M>::cols); return mat_traits<M>::template write_element<5,6>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A57( M & a ) { BOOST_QVM_STATIC_ASSERT(5<mat_traits<M>::rows && 7<mat_traits<M>::cols); return mat_traits<M>::template write_element<5,7>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A58( M & a ) { BOOST_QVM_STATIC_ASSERT(5<mat_traits<M>::rows && 8<mat_traits<M>::cols); return mat_traits<M>::template write_element<5,8>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A59( M & a ) { BOOST_QVM_STATIC_ASSERT(5<mat_traits<M>::rows && 9<mat_traits<M>::cols); return mat_traits<M>::template write_element<5,9>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A60( M & a ) { BOOST_QVM_STATIC_ASSERT(6<mat_traits<M>::rows && 0<mat_traits<M>::cols); return mat_traits<M>::template write_element<6,0>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A61( M & a ) { BOOST_QVM_STATIC_ASSERT(6<mat_traits<M>::rows && 1<mat_traits<M>::cols); return mat_traits<M>::template write_element<6,1>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A62( M & a ) { BOOST_QVM_STATIC_ASSERT(6<mat_traits<M>::rows && 2<mat_traits<M>::cols); return mat_traits<M>::template write_element<6,2>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A63( M & a ) { BOOST_QVM_STATIC_ASSERT(6<mat_traits<M>::rows && 3<mat_traits<M>::cols); return mat_traits<M>::template write_element<6,3>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A64( M & a ) { BOOST_QVM_STATIC_ASSERT(6<mat_traits<M>::rows && 4<mat_traits<M>::cols); return mat_traits<M>::template write_element<6,4>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A65( M & a ) { BOOST_QVM_STATIC_ASSERT(6<mat_traits<M>::rows && 5<mat_traits<M>::cols); return mat_traits<M>::template write_element<6,5>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A66( M & a ) { BOOST_QVM_STATIC_ASSERT(6<mat_traits<M>::rows && 6<mat_traits<M>::cols); return mat_traits<M>::template write_element<6,6>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A67( M & a ) { BOOST_QVM_STATIC_ASSERT(6<mat_traits<M>::rows && 7<mat_traits<M>::cols); return mat_traits<M>::template write_element<6,7>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A68( M & a ) { BOOST_QVM_STATIC_ASSERT(6<mat_traits<M>::rows && 8<mat_traits<M>::cols); return mat_traits<M>::template write_element<6,8>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A69( M & a ) { BOOST_QVM_STATIC_ASSERT(6<mat_traits<M>::rows && 9<mat_traits<M>::cols); return mat_traits<M>::template write_element<6,9>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A70( M & a ) { BOOST_QVM_STATIC_ASSERT(7<mat_traits<M>::rows && 0<mat_traits<M>::cols); return mat_traits<M>::template write_element<7,0>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A71( M & a ) { BOOST_QVM_STATIC_ASSERT(7<mat_traits<M>::rows && 1<mat_traits<M>::cols); return mat_traits<M>::template write_element<7,1>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A72( M & a ) { BOOST_QVM_STATIC_ASSERT(7<mat_traits<M>::rows && 2<mat_traits<M>::cols); return mat_traits<M>::template write_element<7,2>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A73( M & a ) { BOOST_QVM_STATIC_ASSERT(7<mat_traits<M>::rows && 3<mat_traits<M>::cols); return mat_traits<M>::template write_element<7,3>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A74( M & a ) { BOOST_QVM_STATIC_ASSERT(7<mat_traits<M>::rows && 4<mat_traits<M>::cols); return mat_traits<M>::template write_element<7,4>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A75( M & a ) { BOOST_QVM_STATIC_ASSERT(7<mat_traits<M>::rows && 5<mat_traits<M>::cols); return mat_traits<M>::template write_element<7,5>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A76( M & a ) { BOOST_QVM_STATIC_ASSERT(7<mat_traits<M>::rows && 6<mat_traits<M>::cols); return mat_traits<M>::template write_element<7,6>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A77( M & a ) { BOOST_QVM_STATIC_ASSERT(7<mat_traits<M>::rows && 7<mat_traits<M>::cols); return mat_traits<M>::template write_element<7,7>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A78( M & a ) { BOOST_QVM_STATIC_ASSERT(7<mat_traits<M>::rows && 8<mat_traits<M>::cols); return mat_traits<M>::template write_element<7,8>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A79( M & a ) { BOOST_QVM_STATIC_ASSERT(7<mat_traits<M>::rows && 9<mat_traits<M>::cols); return mat_traits<M>::template write_element<7,9>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A80( M & a ) { BOOST_QVM_STATIC_ASSERT(8<mat_traits<M>::rows && 0<mat_traits<M>::cols); return mat_traits<M>::template write_element<8,0>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A81( M & a ) { BOOST_QVM_STATIC_ASSERT(8<mat_traits<M>::rows && 1<mat_traits<M>::cols); return mat_traits<M>::template write_element<8,1>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A82( M & a ) { BOOST_QVM_STATIC_ASSERT(8<mat_traits<M>::rows && 2<mat_traits<M>::cols); return mat_traits<M>::template write_element<8,2>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A83( M & a ) { BOOST_QVM_STATIC_ASSERT(8<mat_traits<M>::rows && 3<mat_traits<M>::cols); return mat_traits<M>::template write_element<8,3>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A84( M & a ) { BOOST_QVM_STATIC_ASSERT(8<mat_traits<M>::rows && 4<mat_traits<M>::cols); return mat_traits<M>::template write_element<8,4>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A85( M & a ) { BOOST_QVM_STATIC_ASSERT(8<mat_traits<M>::rows && 5<mat_traits<M>::cols); return mat_traits<M>::template write_element<8,5>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A86( M & a ) { BOOST_QVM_STATIC_ASSERT(8<mat_traits<M>::rows && 6<mat_traits<M>::cols); return mat_traits<M>::template write_element<8,6>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A87( M & a ) { BOOST_QVM_STATIC_ASSERT(8<mat_traits<M>::rows && 7<mat_traits<M>::cols); return mat_traits<M>::template write_element<8,7>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A88( M & a ) { BOOST_QVM_STATIC_ASSERT(8<mat_traits<M>::rows && 8<mat_traits<M>::cols); return mat_traits<M>::template write_element<8,8>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A89( M & a ) { BOOST_QVM_STATIC_ASSERT(8<mat_traits<M>::rows && 9<mat_traits<M>::cols); return mat_traits<M>::template write_element<8,9>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A90( M & a ) { BOOST_QVM_STATIC_ASSERT(9<mat_traits<M>::rows && 0<mat_traits<M>::cols); return mat_traits<M>::template write_element<9,0>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A91( M & a ) { BOOST_QVM_STATIC_ASSERT(9<mat_traits<M>::rows && 1<mat_traits<M>::cols); return mat_traits<M>::template write_element<9,1>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A92( M & a ) { BOOST_QVM_STATIC_ASSERT(9<mat_traits<M>::rows && 2<mat_traits<M>::cols); return mat_traits<M>::template write_element<9,2>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A93( M & a ) { BOOST_QVM_STATIC_ASSERT(9<mat_traits<M>::rows && 3<mat_traits<M>::cols); return mat_traits<M>::template write_element<9,3>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A94( M & a ) { BOOST_QVM_STATIC_ASSERT(9<mat_traits<M>::rows && 4<mat_traits<M>::cols); return mat_traits<M>::template write_element<9,4>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A95( M & a ) { BOOST_QVM_STATIC_ASSERT(9<mat_traits<M>::rows && 5<mat_traits<M>::cols); return mat_traits<M>::template write_element<9,5>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A96( M & a ) { BOOST_QVM_STATIC_ASSERT(9<mat_traits<M>::rows && 6<mat_traits<M>::cols); return mat_traits<M>::template write_element<9,6>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A97( M & a ) { BOOST_QVM_STATIC_ASSERT(9<mat_traits<M>::rows && 7<mat_traits<M>::cols); return mat_traits<M>::template write_element<9,7>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A98( M & a ) { BOOST_QVM_STATIC_ASSERT(9<mat_traits<M>::rows && 8<mat_traits<M>::cols); return mat_traits<M>::template write_element<9,8>(a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && mat_write_element_ref<M>::value,typename mat_traits<M>::scalar_type &>::type A99( M & a ) { BOOST_QVM_STATIC_ASSERT(9<mat_traits<M>::rows && 9<mat_traits<M>::cols); return mat_traits<M>::template write_element<9,9>(a); }

template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<0,0,M> &>::type A00( M & a ) { BOOST_QVM_STATIC_ASSERT(0<mat_traits<M>::rows && 0<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<0,0, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<0,1,M> &>::type A01( M & a ) { BOOST_QVM_STATIC_ASSERT(0<mat_traits<M>::rows && 1<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<0,1, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<0,2,M> &>::type A02( M & a ) { BOOST_QVM_STATIC_ASSERT(0<mat_traits<M>::rows && 2<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<0,2, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<0,3,M> &>::type A03( M & a ) { BOOST_QVM_STATIC_ASSERT(0<mat_traits<M>::rows && 3<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<0,3, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<0,4,M> &>::type A04( M & a ) { BOOST_QVM_STATIC_ASSERT(0<mat_traits<M>::rows && 4<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<0,4, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<0,5,M> &>::type A05( M & a ) { BOOST_QVM_STATIC_ASSERT(0<mat_traits<M>::rows && 5<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<0,5, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<0,6,M> &>::type A06( M & a ) { BOOST_QVM_STATIC_ASSERT(0<mat_traits<M>::rows && 6<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<0,6, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<0,7,M> &>::type A07( M & a ) { BOOST_QVM_STATIC_ASSERT(0<mat_traits<M>::rows && 7<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<0,7, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<0,8,M> &>::type A08( M & a ) { BOOST_QVM_STATIC_ASSERT(0<mat_traits<M>::rows && 8<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<0,8, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<0,9,M> &>::type A09( M & a ) { BOOST_QVM_STATIC_ASSERT(0<mat_traits<M>::rows && 9<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<0,9, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<1,0,M> &>::type A10( M & a ) { BOOST_QVM_STATIC_ASSERT(1<mat_traits<M>::rows && 0<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<1,0, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<1,1,M> &>::type A11( M & a ) { BOOST_QVM_STATIC_ASSERT(1<mat_traits<M>::rows && 1<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<1,1, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<1,2,M> &>::type A12( M & a ) { BOOST_QVM_STATIC_ASSERT(1<mat_traits<M>::rows && 2<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<1,2, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<1,3,M> &>::type A13( M & a ) { BOOST_QVM_STATIC_ASSERT(1<mat_traits<M>::rows && 3<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<1,3, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<1,4,M> &>::type A14( M & a ) { BOOST_QVM_STATIC_ASSERT(1<mat_traits<M>::rows && 4<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<1,4, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<1,5,M> &>::type A15( M & a ) { BOOST_QVM_STATIC_ASSERT(1<mat_traits<M>::rows && 5<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<1,5, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<1,6,M> &>::type A16( M & a ) { BOOST_QVM_STATIC_ASSERT(1<mat_traits<M>::rows && 6<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<1,6, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<1,7,M> &>::type A17( M & a ) { BOOST_QVM_STATIC_ASSERT(1<mat_traits<M>::rows && 7<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<1,7, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<1,8,M> &>::type A18( M & a ) { BOOST_QVM_STATIC_ASSERT(1<mat_traits<M>::rows && 8<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<1,8, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<1,9,M> &>::type A19( M & a ) { BOOST_QVM_STATIC_ASSERT(1<mat_traits<M>::rows && 9<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<1,9, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<2,0,M> &>::type A20( M & a ) { BOOST_QVM_STATIC_ASSERT(2<mat_traits<M>::rows && 0<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<2,0, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<2,1,M> &>::type A21( M & a ) { BOOST_QVM_STATIC_ASSERT(2<mat_traits<M>::rows && 1<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<2,1, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<2,2,M> &>::type A22( M & a ) { BOOST_QVM_STATIC_ASSERT(2<mat_traits<M>::rows && 2<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<2,2, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<2,3,M> &>::type A23( M & a ) { BOOST_QVM_STATIC_ASSERT(2<mat_traits<M>::rows && 3<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<2,3, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<2,4,M> &>::type A24( M & a ) { BOOST_QVM_STATIC_ASSERT(2<mat_traits<M>::rows && 4<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<2,4, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<2,5,M> &>::type A25( M & a ) { BOOST_QVM_STATIC_ASSERT(2<mat_traits<M>::rows && 5<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<2,5, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<2,6,M> &>::type A26( M & a ) { BOOST_QVM_STATIC_ASSERT(2<mat_traits<M>::rows && 6<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<2,6, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<2,7,M> &>::type A27( M & a ) { BOOST_QVM_STATIC_ASSERT(2<mat_traits<M>::rows && 7<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<2,7, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<2,8,M> &>::type A28( M & a ) { BOOST_QVM_STATIC_ASSERT(2<mat_traits<M>::rows && 8<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<2,8, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<2,9,M> &>::type A29( M & a ) { BOOST_QVM_STATIC_ASSERT(2<mat_traits<M>::rows && 9<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<2,9, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<3,0,M> &>::type A30( M & a ) { BOOST_QVM_STATIC_ASSERT(3<mat_traits<M>::rows && 0<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<3,0, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<3,1,M> &>::type A31( M & a ) { BOOST_QVM_STATIC_ASSERT(3<mat_traits<M>::rows && 1<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<3,1, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<3,2,M> &>::type A32( M & a ) { BOOST_QVM_STATIC_ASSERT(3<mat_traits<M>::rows && 2<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<3,2, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<3,3,M> &>::type A33( M & a ) { BOOST_QVM_STATIC_ASSERT(3<mat_traits<M>::rows && 3<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<3,3, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<3,4,M> &>::type A34( M & a ) { BOOST_QVM_STATIC_ASSERT(3<mat_traits<M>::rows && 4<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<3,4, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<3,5,M> &>::type A35( M & a ) { BOOST_QVM_STATIC_ASSERT(3<mat_traits<M>::rows && 5<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<3,5, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<3,6,M> &>::type A36( M & a ) { BOOST_QVM_STATIC_ASSERT(3<mat_traits<M>::rows && 6<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<3,6, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<3,7,M> &>::type A37( M & a ) { BOOST_QVM_STATIC_ASSERT(3<mat_traits<M>::rows && 7<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<3,7, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<3,8,M> &>::type A38( M & a ) { BOOST_QVM_STATIC_ASSERT(3<mat_traits<M>::rows && 8<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<3,8, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<3,9,M> &>::type A39( M & a ) { BOOST_QVM_STATIC_ASSERT(3<mat_traits<M>::rows && 9<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<3,9, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<4,0,M> &>::type A40( M & a ) { BOOST_QVM_STATIC_ASSERT(4<mat_traits<M>::rows && 0<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<4,0, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<4,1,M> &>::type A41( M & a ) { BOOST_QVM_STATIC_ASSERT(4<mat_traits<M>::rows && 1<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<4,1, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<4,2,M> &>::type A42( M & a ) { BOOST_QVM_STATIC_ASSERT(4<mat_traits<M>::rows && 2<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<4,2, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<4,3,M> &>::type A43( M & a ) { BOOST_QVM_STATIC_ASSERT(4<mat_traits<M>::rows && 3<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<4,3, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<4,4,M> &>::type A44( M & a ) { BOOST_QVM_STATIC_ASSERT(4<mat_traits<M>::rows && 4<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<4,4, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<4,5,M> &>::type A45( M & a ) { BOOST_QVM_STATIC_ASSERT(4<mat_traits<M>::rows && 5<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<4,5, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<4,6,M> &>::type A46( M & a ) { BOOST_QVM_STATIC_ASSERT(4<mat_traits<M>::rows && 6<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<4,6, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<4,7,M> &>::type A47( M & a ) { BOOST_QVM_STATIC_ASSERT(4<mat_traits<M>::rows && 7<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<4,7, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<4,8,M> &>::type A48( M & a ) { BOOST_QVM_STATIC_ASSERT(4<mat_traits<M>::rows && 8<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<4,8, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<4,9,M> &>::type A49( M & a ) { BOOST_QVM_STATIC_ASSERT(4<mat_traits<M>::rows && 9<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<4,9, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<5,0,M> &>::type A50( M & a ) { BOOST_QVM_STATIC_ASSERT(5<mat_traits<M>::rows && 0<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<5,0, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<5,1,M> &>::type A51( M & a ) { BOOST_QVM_STATIC_ASSERT(5<mat_traits<M>::rows && 1<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<5,1, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<5,2,M> &>::type A52( M & a ) { BOOST_QVM_STATIC_ASSERT(5<mat_traits<M>::rows && 2<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<5,2, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<5,3,M> &>::type A53( M & a ) { BOOST_QVM_STATIC_ASSERT(5<mat_traits<M>::rows && 3<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<5,3, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<5,4,M> &>::type A54( M & a ) { BOOST_QVM_STATIC_ASSERT(5<mat_traits<M>::rows && 4<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<5,4, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<5,5,M> &>::type A55( M & a ) { BOOST_QVM_STATIC_ASSERT(5<mat_traits<M>::rows && 5<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<5,5, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<5,6,M> &>::type A56( M & a ) { BOOST_QVM_STATIC_ASSERT(5<mat_traits<M>::rows && 6<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<5,6, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<5,7,M> &>::type A57( M & a ) { BOOST_QVM_STATIC_ASSERT(5<mat_traits<M>::rows && 7<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<5,7, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<5,8,M> &>::type A58( M & a ) { BOOST_QVM_STATIC_ASSERT(5<mat_traits<M>::rows && 8<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<5,8, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<5,9,M> &>::type A59( M & a ) { BOOST_QVM_STATIC_ASSERT(5<mat_traits<M>::rows && 9<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<5,9, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<6,0,M> &>::type A60( M & a ) { BOOST_QVM_STATIC_ASSERT(6<mat_traits<M>::rows && 0<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<6,0, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<6,1,M> &>::type A61( M & a ) { BOOST_QVM_STATIC_ASSERT(6<mat_traits<M>::rows && 1<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<6,1, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<6,2,M> &>::type A62( M & a ) { BOOST_QVM_STATIC_ASSERT(6<mat_traits<M>::rows && 2<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<6,2, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<6,3,M> &>::type A63( M & a ) { BOOST_QVM_STATIC_ASSERT(6<mat_traits<M>::rows && 3<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<6,3, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<6,4,M> &>::type A64( M & a ) { BOOST_QVM_STATIC_ASSERT(6<mat_traits<M>::rows && 4<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<6,4, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<6,5,M> &>::type A65( M & a ) { BOOST_QVM_STATIC_ASSERT(6<mat_traits<M>::rows && 5<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<6,5, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<6,6,M> &>::type A66( M & a ) { BOOST_QVM_STATIC_ASSERT(6<mat_traits<M>::rows && 6<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<6,6, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<6,7,M> &>::type A67( M & a ) { BOOST_QVM_STATIC_ASSERT(6<mat_traits<M>::rows && 7<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<6,7, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<6,8,M> &>::type A68( M & a ) { BOOST_QVM_STATIC_ASSERT(6<mat_traits<M>::rows && 8<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<6,8, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<6,9,M> &>::type A69( M & a ) { BOOST_QVM_STATIC_ASSERT(6<mat_traits<M>::rows && 9<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<6,9, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<7,0,M> &>::type A70( M & a ) { BOOST_QVM_STATIC_ASSERT(7<mat_traits<M>::rows && 0<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<7,0, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<7,1,M> &>::type A71( M & a ) { BOOST_QVM_STATIC_ASSERT(7<mat_traits<M>::rows && 1<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<7,1, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<7,2,M> &>::type A72( M & a ) { BOOST_QVM_STATIC_ASSERT(7<mat_traits<M>::rows && 2<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<7,2, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<7,3,M> &>::type A73( M & a ) { BOOST_QVM_STATIC_ASSERT(7<mat_traits<M>::rows && 3<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<7,3, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<7,4,M> &>::type A74( M & a ) { BOOST_QVM_STATIC_ASSERT(7<mat_traits<M>::rows && 4<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<7,4, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<7,5,M> &>::type A75( M & a ) { BOOST_QVM_STATIC_ASSERT(7<mat_traits<M>::rows && 5<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<7,5, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<7,6,M> &>::type A76( M & a ) { BOOST_QVM_STATIC_ASSERT(7<mat_traits<M>::rows && 6<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<7,6, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<7,7,M> &>::type A77( M & a ) { BOOST_QVM_STATIC_ASSERT(7<mat_traits<M>::rows && 7<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<7,7, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<7,8,M> &>::type A78( M & a ) { BOOST_QVM_STATIC_ASSERT(7<mat_traits<M>::rows && 8<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<7,8, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<7,9,M> &>::type A79( M & a ) { BOOST_QVM_STATIC_ASSERT(7<mat_traits<M>::rows && 9<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<7,9, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<8,0,M> &>::type A80( M & a ) { BOOST_QVM_STATIC_ASSERT(8<mat_traits<M>::rows && 0<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<8,0, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<8,1,M> &>::type A81( M & a ) { BOOST_QVM_STATIC_ASSERT(8<mat_traits<M>::rows && 1<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<8,1, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<8,2,M> &>::type A82( M & a ) { BOOST_QVM_STATIC_ASSERT(8<mat_traits<M>::rows && 2<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<8,2, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<8,3,M> &>::type A83( M & a ) { BOOST_QVM_STATIC_ASSERT(8<mat_traits<M>::rows && 3<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<8,3, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<8,4,M> &>::type A84( M & a ) { BOOST_QVM_STATIC_ASSERT(8<mat_traits<M>::rows && 4<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<8,4, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<8,5,M> &>::type A85( M & a ) { BOOST_QVM_STATIC_ASSERT(8<mat_traits<M>::rows && 5<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<8,5, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<8,6,M> &>::type A86( M & a ) { BOOST_QVM_STATIC_ASSERT(8<mat_traits<M>::rows && 6<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<8,6, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<8,7,M> &>::type A87( M & a ) { BOOST_QVM_STATIC_ASSERT(8<mat_traits<M>::rows && 7<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<8,7, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<8,8,M> &>::type A88( M & a ) { BOOST_QVM_STATIC_ASSERT(8<mat_traits<M>::rows && 8<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<8,8, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<8,9,M> &>::type A89( M & a ) { BOOST_QVM_STATIC_ASSERT(8<mat_traits<M>::rows && 9<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<8,9, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<9,0,M> &>::type A90( M & a ) { BOOST_QVM_STATIC_ASSERT(9<mat_traits<M>::rows && 0<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<9,0, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<9,1,M> &>::type A91( M & a ) { BOOST_QVM_STATIC_ASSERT(9<mat_traits<M>::rows && 1<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<9,1, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<9,2,M> &>::type A92( M & a ) { BOOST_QVM_STATIC_ASSERT(9<mat_traits<M>::rows && 2<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<9,2, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<9,3,M> &>::type A93( M & a ) { BOOST_QVM_STATIC_ASSERT(9<mat_traits<M>::rows && 3<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<9,3, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<9,4,M> &>::type A94( M & a ) { BOOST_QVM_STATIC_ASSERT(9<mat_traits<M>::rows && 4<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<9,4, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<9,5,M> &>::type A95( M & a ) { BOOST_QVM_STATIC_ASSERT(9<mat_traits<M>::rows && 5<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<9,5, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<9,6,M> &>::type A96( M & a ) { BOOST_QVM_STATIC_ASSERT(9<mat_traits<M>::rows && 6<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<9,6, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<9,7,M> &>::type A97( M & a ) { BOOST_QVM_STATIC_ASSERT(9<mat_traits<M>::rows && 7<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<9,7, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<9,8,M> &>::type A98( M & a ) { BOOST_QVM_STATIC_ASSERT(9<mat_traits<M>::rows && 8<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<9,8, M> *>(&a); }
template <class M> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_mat<M>::value && !mat_write_element_ref<M>::value,qvm_detail::m_element_access<9,9,M> &>::type A99( M & a ) { BOOST_QVM_STATIC_ASSERT(9<mat_traits<M>::rows && 9<mat_traits<M>::cols); return *reinterpret_cast<qvm_detail::m_element_access<9,9, M> *>(&a); }

} }

#endif
// <<< #include <boost/qvm/mat_access.hpp>
// #line 9 "boost/qvm/lite.hpp"
// >>> #include <boost/qvm/mat_traits_defaults.hpp>
#ifndef BOOST_QVM_MAT_TRAITS_DEFAULTS_HPP_INCLUDED
#define BOOST_QVM_MAT_TRAITS_DEFAULTS_HPP_INCLUDED

// #line 8 "boost/qvm/mat_traits_defaults.hpp"
// #include <boost/qvm/config.hpp> // Expanded at line 109
// #include <boost/qvm/assert.hpp> // Expanded at line 887

namespace boost { namespace qvm {

template <class>
struct mat_traits;

namespace
qvm_detail
    {
    template <int I,int N>
    struct
    matrix_w
        {
        template <class A>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        typename mat_traits<A>::scalar_type &
        write_element_idx( int r, int c, A & a )
            {
            return (I/mat_traits<A>::cols)==r && (I%mat_traits<A>::cols)==c?
                mat_traits<A>::template write_element<I/mat_traits<A>::cols,I%mat_traits<A>::cols>(a) :
                matrix_w<I+1,N>::write_element_idx(r,c,a);
            }
        };

    template <int N>
    struct
    matrix_w<N,N>
        {
        template <class A>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        typename mat_traits<A>::scalar_type &
        write_element_idx( int, int, A & a )
            {
            BOOST_QVM_ASSERT(0);
            return mat_traits<A>::template write_element<0,0>(a);
            }
        };
    }

template <class MatType,class ScalarType,int Rows,int Cols>
struct
mat_traits_defaults
    {
    typedef MatType mat_type;
    typedef ScalarType scalar_type;
    static int const rows=Rows;
    static int const cols=Cols;

    template <int Row,int Col>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( mat_type const & x )
        {
        return mat_traits<mat_type>::template write_element<Row,Col>(const_cast<mat_type &>(x));
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int r, int c, mat_type const & x )
        {
        return mat_traits<mat_type>::write_element_idx(r,c,const_cast<mat_type &>(x));
        }

    protected:

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
    scalar_type &
    write_element_idx( int r, int c, mat_type & m )
        {
        return qvm_detail::matrix_w<0,mat_traits<mat_type>::rows*mat_traits<mat_type>::cols>::write_element_idx(r,c,m);
        }
    };

} }

#endif
// <<< #include <boost/qvm/mat_traits_defaults.hpp>
// #line 10 "boost/qvm/lite.hpp"
// #include <boost/qvm/mat_traits_array.hpp> // Expanded at line 7509
// >>> #include <boost/qvm/mat.hpp>
#ifndef BOOST_QVM_MAT_HPP_INCLUDED
#define BOOST_QVM_MAT_HPP_INCLUDED

// #line 8 "boost/qvm/mat.hpp"
// #include <boost/qvm/detail/mat_assign.hpp> // Expanded at line 99
// #include <boost/qvm/assert.hpp> // Expanded at line 887
// #include <boost/qvm/static_assert.hpp> // Expanded at line 1180

namespace boost { namespace qvm {

template <class T,int Rows,int Cols>
struct
mat
    {
    T a[Rows][Cols];
    template <class R
#if __cplusplus >= 201103L
        , class = typename enable_if<is_mat<R> >::type
#endif
    >
    operator R() const
        {
        R r;
        assign(r,*this);
        return r;
        }
    };

template <class M>
struct mat_traits;

template <class T,int Rows,int Cols>
struct
mat_traits< mat<T,Rows,Cols> >
    {
    typedef mat<T,Rows,Cols> this_matrix;
    typedef T scalar_type;
    static int const rows=Rows;
    static int const cols=Cols;

    template <int Row,int Col>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_matrix const & x )
        {
        BOOST_QVM_STATIC_ASSERT(Row>=0);
        BOOST_QVM_STATIC_ASSERT(Row<rows);
        BOOST_QVM_STATIC_ASSERT(Col>=0);
        BOOST_QVM_STATIC_ASSERT(Col<cols);
        return x.a[Row][Col];
        }

    template <int Row,int Col>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type &
    write_element( this_matrix & x )
        {
        BOOST_QVM_STATIC_ASSERT(Row>=0);
        BOOST_QVM_STATIC_ASSERT(Row<rows);
        BOOST_QVM_STATIC_ASSERT(Col>=0);
        BOOST_QVM_STATIC_ASSERT(Col<cols);
        return x.a[Row][Col];
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int row, int col, this_matrix const & x )
        {
        BOOST_QVM_ASSERT(row>=0);
        BOOST_QVM_ASSERT(row<Rows);
        BOOST_QVM_ASSERT(col>=0);
        BOOST_QVM_ASSERT(col<Cols);
        return x.a[row][col];
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type &
    write_element_idx( int row, int col, this_matrix & x )
        {
        BOOST_QVM_ASSERT(row>=0);
        BOOST_QVM_ASSERT(row<Rows);
        BOOST_QVM_ASSERT(col>=0);
        BOOST_QVM_ASSERT(col<Cols);
        return x.a[row][col];
        }
    };

} }

#endif
// <<< #include <boost/qvm/mat.hpp>
// #line 12 "boost/qvm/lite.hpp"
// #include <boost/qvm/math.hpp> // Expanded at line 7399
// >>> #include <boost/qvm/quat_operations.hpp>
#ifndef BOOST_QVM_QUAT_OPERATIONS
#define BOOST_QVM_QUAT_OPERATIONS

// #line 8 "boost/qvm/quat_operations.hpp"
// >>> #include <boost/qvm/detail/quat_assign.hpp>
#ifndef BOOST_QVM_DETAIL_QUAT_ASSIGN_HPP_INCLUDED
#define BOOST_QVM_DETAIL_QUAT_ASSIGN_HPP_INCLUDED

// #line 8 "boost/qvm/detail/quat_assign.hpp"
// #include <boost/qvm/config.hpp> // Expanded at line 109
// #include <boost/qvm/enable_if.hpp> // Expanded at line 177
// #include <boost/qvm/quat_traits.hpp> // Expanded at line 914

namespace boost { namespace qvm {

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_quat<A>::value && is_quat<B>::value,
    A &>::type
assign( A & a, B const & b )
    {
    write_quat_element<0>(a,quat_traits<B>::template read_element<0>(b));
    write_quat_element<1>(a,quat_traits<B>::template read_element<1>(b));
    write_quat_element<2>(a,quat_traits<B>::template read_element<2>(b));
    write_quat_element<3>(a,quat_traits<B>::template read_element<3>(b));
    return a;
    }

} }

#endif
// <<< #include <boost/qvm/detail/quat_assign.hpp>
// #line 9 "boost/qvm/quat_operations.hpp"
// >>> #include <boost/qvm/deduce_quat.hpp>
#ifndef BOOST_QVM_DEDUCE_QUAT_HPP_INCLUDED
#define BOOST_QVM_DEDUCE_QUAT_HPP_INCLUDED

// #line 8 "boost/qvm/deduce_quat.hpp"
// #include <boost/qvm/deduce_scalar.hpp> // Expanded at line 904
// #include <boost/qvm/quat_traits.hpp> // Expanded at line 914
// #include <boost/qvm/static_assert.hpp> // Expanded at line 1180

namespace boost { namespace qvm {

template <class T>
struct quat;

namespace
qvm_detail
    {
    template <class Q,class S,
        class QS=typename quat_traits<Q>::scalar_type>
    struct
    deduce_q_default
        {
        BOOST_QVM_STATIC_ASSERT(is_quat<Q>::value);
        typedef quat<typename quat_traits<Q>::scalar_type> type;
        };

    template <class Q,class S>
    struct
    deduce_q_default<Q,S,S>
        {
        BOOST_QVM_STATIC_ASSERT(is_quat<Q>::value);
        typedef Q type;
        };
    }

template <class Q,class S=typename quat_traits<Q>::scalar_type>
struct
deduce_quat
    {
    BOOST_QVM_STATIC_ASSERT(is_quat<Q>::value);
    typedef typename qvm_detail::deduce_q_default<Q,S>::type type;
    };

namespace
qvm_detail
    {
    template <class A,class B,class S,
        bool IsScalarA=is_scalar<A>::value,
        bool IsScalarB=is_scalar<B>::value>
    struct
    deduce_q2_default
        {
        typedef quat<S> type;
        };

    template <class Q,class S>
    struct
    deduce_q2_default<Q,Q,S,false,false>
        {
        BOOST_QVM_STATIC_ASSERT(is_quat<Q>::value);
        typedef Q type;
        };

    template <class A,class B,class S>
    struct
    deduce_q2_default<A,B,S,false,true>
        {
        BOOST_QVM_STATIC_ASSERT(is_quat<A>::value);
        typedef typename deduce_quat<A,S>::type type;
        };

    template <class A,class B,class S>
    struct
    deduce_q2_default<A,B,S,true,false>
        {
        BOOST_QVM_STATIC_ASSERT(is_quat<B>::value);
        typedef typename deduce_quat<B,S>::type type;
        };
    }

template <class A,class B,class S=typename deduce_scalar<typename scalar<A>::type,typename scalar<B>::type>::type>
struct
deduce_quat2
    {
    BOOST_QVM_STATIC_ASSERT(is_quat<A>::value || is_quat<B>::value);
    typedef typename qvm_detail::deduce_q2_default<A,B,S>::type type;
    };

} }

#endif
// <<< #include <boost/qvm/deduce_quat.hpp>
// #line 10 "boost/qvm/quat_operations.hpp"
// #include <boost/qvm/mat_traits.hpp> // Expanded at line 303
// #include <boost/qvm/scalar_traits.hpp> // Expanded at line 909
// #include <boost/qvm/math.hpp> // Expanded at line 7399
// #include <boost/qvm/assert.hpp> // Expanded at line 887
// #include <boost/qvm/error.hpp> // Expanded at line 17
// #include <boost/qvm/throw_exception.hpp> // Expanded at line 1546
// #include <boost/qvm/to_string.hpp> // Expanded at line 49

namespace boost { namespace qvm {

namespace
qvm_detail
    {
    BOOST_QVM_INLINE_CRITICAL
    void const *
    get_valid_ptr_quat_operations()
        {
        static int const obj=0;
        return &obj;
        }
    }

////////////////////////////////////////////////

namespace
msvc_parse_bug_workaround
    {
    template <class A,class B>
    struct
    quats
        {
        static bool const value=is_quat<A>::value && is_quat<B>::value;
        };
    }


template <class A>
inline
typename enable_if_c<
    is_quat<A>::value,
    std::string>::type
to_string( A const & a )
    {
    using namespace qvm_to_string_detail;
    return '('+
        to_string(quat_traits<A>::template read_element<0>(a))+','+
        to_string(quat_traits<A>::template read_element<1>(a))+','+
        to_string(quat_traits<A>::template read_element<2>(a))+','+
        to_string(quat_traits<A>::template read_element<3>(a))+')';
    }

////////////////////////////////////////////////

template <class A,class B,class Cmp>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_quat<A>::value && is_quat<B>::value,
    bool>::type
cmp( A const & a, B const & b, Cmp f )
    {
    typedef typename quat_traits<A>::scalar_type T;
    typedef typename quat_traits<B>::scalar_type U;
    T q1[4] =
        {
        quat_traits<A>::template read_element<0>(a),
        quat_traits<A>::template read_element<1>(a),
        quat_traits<A>::template read_element<2>(a),
        quat_traits<A>::template read_element<3>(a)
        };
    U q2[4] =
        {
        quat_traits<B>::template read_element<0>(b),
        quat_traits<B>::template read_element<1>(b),
        quat_traits<B>::template read_element<2>(b),
        quat_traits<B>::template read_element<3>(b)
        };
    int i=0;
    for( ; i!=4; ++i )
        if( !f(q1[i],q2[i]) )
            break;
    if( i==4 )
        return true;
    for( i=0; i!=4; ++i )
        if( !f(q1[i],-q2[i]) )
            return false;
    return true;
    }

////////////////////////////////////////////////

template <class R,class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
typename enable_if_c<
    is_quat<R>::value && is_quat<A>::value,
    R>::type
convert_to( A const & a )
    {
    R r;
    write_quat_element<0>(r,quat_traits<A>::template read_element<0>(a));
    write_quat_element<1>(r,quat_traits<A>::template read_element<1>(a));
    write_quat_element<2>(r,quat_traits<A>::template read_element<2>(a));
    write_quat_element<3>(r,quat_traits<A>::template read_element<3>(a));
    return r;
    }

template <class R,class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_quat<R>::value && is_mat<A>::value &&
    mat_traits<A>::rows==3 && mat_traits<A>::cols==3,
    R>::type
convert_to( A const & a )
    {
    typedef typename mat_traits<A>::scalar_type T;
    T const mat[3][3] =
        {
            { mat_traits<A>::template read_element<0,0>(a), mat_traits<A>::template read_element<0,1>(a), mat_traits<A>::template read_element<0,2>(a) },
            { mat_traits<A>::template read_element<1,0>(a), mat_traits<A>::template read_element<1,1>(a), mat_traits<A>::template read_element<1,2>(a) },
            { mat_traits<A>::template read_element<2,0>(a), mat_traits<A>::template read_element<2,1>(a), mat_traits<A>::template read_element<2,2>(a) }
        };
    R r;
    if( mat[0][0]+mat[1][1]+mat[2][2] > scalar_traits<T>::value(0) )
        {
        T t = mat[0][0] + mat[1][1] + mat[2][2] + scalar_traits<T>::value(1);
        T s = (scalar_traits<T>::value(1)/sqrt(t))/2;
        write_quat_element<0>(r,s*t);
        write_quat_element<1>(r,(mat[2][1]-mat[1][2])*s);
        write_quat_element<2>(r,(mat[0][2]-mat[2][0])*s);
        write_quat_element<3>(r,(mat[1][0]-mat[0][1])*s);
        }
    else if( mat[0][0]>mat[1][1] && mat[0][0]>mat[2][2] )
        {
        T t = mat[0][0] - mat[1][1] - mat[2][2] + scalar_traits<T>::value(1);
        T s = (scalar_traits<T>::value(1)/sqrt(t))/2;
        write_quat_element<0>(r,(mat[2][1]-mat[1][2])*s);
        write_quat_element<1>(r,s*t);
        write_quat_element<2>(r,(mat[1][0]+mat[0][1])*s);
        write_quat_element<3>(r,(mat[0][2]+mat[2][0])*s);
        }
    else if( mat[1][1]>mat[2][2] )
        {
        T t = - mat[0][0] + mat[1][1] - mat[2][2] + scalar_traits<T>::value(1);
        T s = (scalar_traits<T>::value(1)/sqrt(t))/2;
        write_quat_element<0>(r,(mat[0][2]-mat[2][0])*s);
        write_quat_element<1>(r,(mat[1][0]+mat[0][1])*s);
        write_quat_element<2>(r,s*t);
        write_quat_element<3>(r,(mat[2][1]+mat[1][2])*s);
        }
    else
        {
        T t = - mat[0][0] - mat[1][1] + mat[2][2] + scalar_traits<T>::value(1);
        T s = (scalar_traits<T>::value(1)/sqrt(t))/2;
        write_quat_element<0>(r,(mat[1][0]-mat[0][1])*s);
        write_quat_element<1>(r,(mat[0][2]+mat[2][0])*s);
        write_quat_element<2>(r,(mat[2][1]+mat[1][2])*s);
        write_quat_element<3>(r,s*t);
        }
    return r;
    }

////////////////////////////////////////////////

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_quat<A>::value,
    deduce_quat<A> >::type
conjugate( A const & a )
    {
    typedef typename deduce_quat<A>::type R;
    R r;
    write_quat_element<0>(r,quat_traits<A>::template read_element<0>(a));
    write_quat_element<1>(r,-quat_traits<A>::template read_element<1>(a));
    write_quat_element<2>(r,-quat_traits<A>::template read_element<2>(a));
    write_quat_element<3>(r,-quat_traits<A>::template read_element<3>(a));
    return r;
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <class T>
    class
    identity_quat_
        {
        identity_quat_( identity_quat_ const & );
        identity_quat_ & operator=( identity_quat_ const & );
        ~identity_quat_();

        public:

        template <class R
#if __cplusplus >= 201103L
            , class = typename enable_if<is_quat<R> >::type
#endif
        >
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        operator R() const
            {
            R r;
            assign(r,*this);
            return r;
            }
        };
    }

template <class T>
struct
quat_traits< qvm_detail::identity_quat_<T> >
    {
    typedef qvm_detail::identity_quat_<T> this_quaternion;
    typedef T scalar_type;

    template <int I>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_quaternion const & )
        {
        BOOST_QVM_STATIC_ASSERT(I>=0);
        BOOST_QVM_STATIC_ASSERT(I<4);
        return scalar_traits<T>::value(I==0);
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int i, this_quaternion const & )
        {
        BOOST_QVM_ASSERT(i>=0);
        BOOST_QVM_ASSERT(i<4);
        return scalar_traits<T>::value(i==0);
        }
    };

template <class T>
struct
deduce_quat< qvm_detail::identity_quat_<T> >
    {
    typedef quat<T> type;
    };

template <class T>
struct
deduce_quat2< qvm_detail::identity_quat_<T>, qvm_detail::identity_quat_<T> >
    {
    typedef quat<T> type;
    };

template <class T>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
qvm_detail::identity_quat_<T> const &
identity_quat()
    {
    return *(qvm_detail::identity_quat_<T> const *)qvm_detail::get_valid_ptr_quat_operations();
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_quat<A>::value,
    void>::type
set_identity( A & a )
    {
    typedef typename quat_traits<A>::scalar_type T;
    T const zero=scalar_traits<T>::value(0);
    T const one=scalar_traits<T>::value(1);
    write_quat_element<0>(a,one);
    write_quat_element<1>(a,zero);
    write_quat_element<2>(a,zero);
    write_quat_element<3>(a,zero);
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <class OriginalType,class Scalar>
    class
    quaternion_scalar_cast_
        {
        quaternion_scalar_cast_( quaternion_scalar_cast_ const & );
        quaternion_scalar_cast_ & operator=( quaternion_scalar_cast_ const & );
        ~quaternion_scalar_cast_();

        public:

        template <class T>
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        quaternion_scalar_cast_ &
        operator=( T const & x )
            {
            assign(*this,x);
            return *this;
            }

        template <class R
#if __cplusplus >= 201103L
            , class = typename enable_if<is_quat<R> >::type
#endif
        >
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        operator R() const
            {
            R r;
            assign(r,*this);
            return r;
            }
        };

    template <bool> struct scalar_cast_quaternion_filter { };
    template <> struct scalar_cast_quaternion_filter<true> { typedef int type; };
    }

template <class OriginalType,class Scalar>
struct
quat_traits< qvm_detail::quaternion_scalar_cast_<OriginalType,Scalar> >
    {
    typedef Scalar scalar_type;
    typedef qvm_detail::quaternion_scalar_cast_<OriginalType,Scalar> this_quaternion;

    template <int I>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_quaternion const & x )
        {
        BOOST_QVM_STATIC_ASSERT(I>=0);
        BOOST_QVM_STATIC_ASSERT(I<4);
        return scalar_type(quat_traits<OriginalType>::template read_element<I>(reinterpret_cast<OriginalType const &>(x)));
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int i, this_quaternion const & x )
        {
        BOOST_QVM_ASSERT(i>=0);
        BOOST_QVM_ASSERT(i<4);
        return scalar_type(quat_traits<OriginalType>::read_element_idx(i,reinterpret_cast<OriginalType const &>(x)));
        }
    };

template <class Scalar,class T>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
qvm_detail::quaternion_scalar_cast_<T,Scalar> const &
scalar_cast( T const & x, typename qvm_detail::scalar_cast_quaternion_filter<is_quat<T>::value>::type=0 )
    {
    return reinterpret_cast<qvm_detail::quaternion_scalar_cast_<T,Scalar> const &>(x);
    }

////////////////////////////////////////////////

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_quat<A>::value && is_scalar<B>::value,
    A &>::type
operator/=( A & a, B b )
    {
    write_quat_element<0>(a,quat_traits<A>::template read_element<0>(a)/b);
    write_quat_element<1>(a,quat_traits<A>::template read_element<1>(a)/b);
    write_quat_element<2>(a,quat_traits<A>::template read_element<2>(a)/b);
    write_quat_element<3>(a,quat_traits<A>::template read_element<3>(a)/b);
    return a;
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_quat<A>::value && is_scalar<B>::value,
    deduce_quat2<A,B> >::type
operator/( A const & a, B b )
    {
    typedef typename deduce_quat2<A,B>::type R;
    R r;
    write_quat_element<0>(r,quat_traits<A>::template read_element<0>(a)/b);
    write_quat_element<1>(r,quat_traits<A>::template read_element<1>(a)/b);
    write_quat_element<2>(r,quat_traits<A>::template read_element<2>(a)/b);
    write_quat_element<3>(r,quat_traits<A>::template read_element<3>(a)/b);
    return r;
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_quat<A>::value && is_quat<B>::value,
    deduce_scalar<typename quat_traits<A>::scalar_type,typename quat_traits<B>::scalar_type> >::type
dot( A const & a, B const & b )
    {
    typedef typename quat_traits<A>::scalar_type Ta;
    typedef typename quat_traits<B>::scalar_type Tb;
    typedef typename deduce_scalar<Ta,Tb>::type Tr;
    Ta const a0=quat_traits<A>::template read_element<0>(a);
    Ta const a1=quat_traits<A>::template read_element<1>(a);
    Ta const a2=quat_traits<A>::template read_element<2>(a);
    Ta const a3=quat_traits<A>::template read_element<3>(a);
    Tb const b0=quat_traits<B>::template read_element<0>(b);
    Tb const b1=quat_traits<B>::template read_element<1>(b);
    Tb const b2=quat_traits<B>::template read_element<2>(b);
    Tb const b3=quat_traits<B>::template read_element<3>(b);
    Tr const dp=a0*b0+a1*b1+a2*b2+a3*b3;
    return dp;
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_quat<A>::value && is_quat<B>::value,
    bool>::type
operator==( A const & a, B const & b )
    {
    return
        quat_traits<A>::template read_element<0>(a)==quat_traits<B>::template read_element<0>(b) &&
        quat_traits<A>::template read_element<1>(a)==quat_traits<B>::template read_element<1>(b) &&
        quat_traits<A>::template read_element<2>(a)==quat_traits<B>::template read_element<2>(b) &&
        quat_traits<A>::template read_element<3>(a)==quat_traits<B>::template read_element<3>(b);
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_quat<A>::value,
    deduce_quat<A> >::type
inverse( A const & a )
    {
    typedef typename deduce_quat<A>::type R;
    typedef typename quat_traits<A>::scalar_type TA;
    TA aa = quat_traits<A>::template read_element<0>(a);
    TA ab = quat_traits<A>::template read_element<1>(a);
    TA ac = quat_traits<A>::template read_element<2>(a);
    TA ad = quat_traits<A>::template read_element<3>(a);
    TA m2 = ab*ab + ac*ac + ad*ad + aa*aa;
    if( m2==scalar_traits<TA>::value(0) )
        BOOST_QVM_THROW_EXCEPTION(zero_magnitude_error());
    TA rm=scalar_traits<TA>::value(1)/m2;
    R r;
    write_quat_element<0>(r,aa*rm);
    write_quat_element<1>(r,-ab*rm);
    write_quat_element<2>(r,-ac*rm);
    write_quat_element<3>(r,-ad*rm);
    return r;
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_quat<A>::value,
    typename quat_traits<A>::scalar_type>::type
mag_sqr( A const & a )
    {
    typedef typename quat_traits<A>::scalar_type T;
    T x=quat_traits<A>::template read_element<0>(a);
    T y=quat_traits<A>::template read_element<1>(a);
    T z=quat_traits<A>::template read_element<2>(a);
    T w=quat_traits<A>::template read_element<3>(a);
    return x*x+y*y+z*z+w*w;
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_quat<A>::value,
    typename quat_traits<A>::scalar_type>::type
mag( A const & a )
    {
    typedef typename quat_traits<A>::scalar_type T;
    T x=quat_traits<A>::template read_element<0>(a);
    T y=quat_traits<A>::template read_element<1>(a);
    T z=quat_traits<A>::template read_element<2>(a);
    T w=quat_traits<A>::template read_element<3>(a);
    return sqrt(x*x+y*y+z*z+w*w);
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if<
    msvc_parse_bug_workaround::quats<A,B>,
    A &>::type
operator-=( A & a, B const & b )
    {
    write_quat_element<0>(a,quat_traits<A>::template read_element<0>(a)-quat_traits<B>::template read_element<0>(b));
    write_quat_element<1>(a,quat_traits<A>::template read_element<1>(a)-quat_traits<B>::template read_element<1>(b));
    write_quat_element<2>(a,quat_traits<A>::template read_element<2>(a)-quat_traits<B>::template read_element<2>(b));
    write_quat_element<3>(a,quat_traits<A>::template read_element<3>(a)-quat_traits<B>::template read_element<3>(b));
    return a;
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_quat<A>::value && is_quat<B>::value,
    deduce_quat2<A,B> >::type
operator-( A const & a, B const & b )
    {
    typedef typename deduce_quat2<A,B>::type R;
    R r;
    write_quat_element<0>(r,quat_traits<A>::template read_element<0>(a)-quat_traits<B>::template read_element<0>(b));
    write_quat_element<1>(r,quat_traits<A>::template read_element<1>(a)-quat_traits<B>::template read_element<1>(b));
    write_quat_element<2>(r,quat_traits<A>::template read_element<2>(a)-quat_traits<B>::template read_element<2>(b));
    write_quat_element<3>(r,quat_traits<A>::template read_element<3>(a)-quat_traits<B>::template read_element<3>(b));
    return r;
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_quat<A>::value,
    deduce_quat<A> >::type
operator-( A const & a )
    {
    typedef typename deduce_quat<A>::type R;
    R r;
    write_quat_element<0>(r,-quat_traits<A>::template read_element<0>(a));
    write_quat_element<1>(r,-quat_traits<A>::template read_element<1>(a));
    write_quat_element<2>(r,-quat_traits<A>::template read_element<2>(a));
    write_quat_element<3>(r,-quat_traits<A>::template read_element<3>(a));
    return r;
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if<
    msvc_parse_bug_workaround::quats<A,B>,
    A &>::type
operator*=( A & a, B const & b )
    {
    typedef typename quat_traits<A>::scalar_type TA;
    typedef typename quat_traits<B>::scalar_type TB;
    TA const aa=quat_traits<A>::template read_element<0>(a);
    TA const ab=quat_traits<A>::template read_element<1>(a);
    TA const ac=quat_traits<A>::template read_element<2>(a);
    TA const ad=quat_traits<A>::template read_element<3>(a);
    TB const ba=quat_traits<B>::template read_element<0>(b);
    TB const bb=quat_traits<B>::template read_element<1>(b);
    TB const bc=quat_traits<B>::template read_element<2>(b);
    TB const bd=quat_traits<B>::template read_element<3>(b);
    write_quat_element<0>(a,aa*ba - ab*bb - ac*bc - ad*bd);
    write_quat_element<1>(a,aa*bb + ab*ba + ac*bd - ad*bc);
    write_quat_element<2>(a,aa*bc + ac*ba + ad*bb - ab*bd);
    write_quat_element<3>(a,aa*bd + ad*ba + ab*bc - ac*bb);
    return a;
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_quat<A>::value && is_scalar<B>::value,
    A &>::type
operator*=( A & a, B b )
    {
    write_quat_element<0>(a, quat_traits<A>::template read_element<0>(a)*b);
    write_quat_element<1>(a, quat_traits<A>::template read_element<1>(a)*b);
    write_quat_element<2>(a, quat_traits<A>::template read_element<2>(a)*b);
    write_quat_element<3>(a, quat_traits<A>::template read_element<3>(a)*b);
    return a;
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_quat<A>::value && is_quat<B>::value,
    deduce_quat2<A,B> >::type
operator*( A const & a, B const & b )
    {
    typedef typename deduce_quat2<A,B>::type R;
    typedef typename quat_traits<A>::scalar_type TA;
    typedef typename quat_traits<B>::scalar_type TB;
    TA const aa=quat_traits<A>::template read_element<0>(a);
    TA const ab=quat_traits<A>::template read_element<1>(a);
    TA const ac=quat_traits<A>::template read_element<2>(a);
    TA const ad=quat_traits<A>::template read_element<3>(a);
    TB const ba=quat_traits<B>::template read_element<0>(b);
    TB const bb=quat_traits<B>::template read_element<1>(b);
    TB const bc=quat_traits<B>::template read_element<2>(b);
    TB const bd=quat_traits<B>::template read_element<3>(b);
    R r;
    write_quat_element<0>(r,aa*ba - ab*bb - ac*bc - ad*bd);
    write_quat_element<1>(r,aa*bb + ab*ba + ac*bd - ad*bc);
    write_quat_element<2>(r,aa*bc + ac*ba + ad*bb - ab*bd);
    write_quat_element<3>(r,aa*bd + ad*ba + ab*bc - ac*bb);
    return r;
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c2<
    is_quat<A>::value && is_scalar<B>::value,
    deduce_quat2<A,B> >::type
operator*( A const & a, B b )
    {
    typedef typename deduce_quat2<A,B>::type R;
    R r;
    write_quat_element<0>(r,quat_traits<A>::template read_element<0>(a)*b);
    write_quat_element<1>(r,quat_traits<A>::template read_element<1>(a)*b);
    write_quat_element<2>(r,quat_traits<A>::template read_element<2>(a)*b);
    write_quat_element<3>(r,quat_traits<A>::template read_element<3>(a)*b);
    return r;
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_quat<A>::value && is_quat<B>::value,
    bool>::type
operator!=( A const & a, B const & b )
    {
    return
        quat_traits<A>::template read_element<0>(a)!=quat_traits<B>::template read_element<0>(b) ||
        quat_traits<A>::template read_element<1>(a)!=quat_traits<B>::template read_element<1>(b) ||
        quat_traits<A>::template read_element<2>(a)!=quat_traits<B>::template read_element<2>(b) ||
        quat_traits<A>::template read_element<3>(a)!=quat_traits<B>::template read_element<3>(b);
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_quat<A>::value,
    deduce_quat<A> >::type
normalized( A const & a )
    {
    typedef typename quat_traits<A>::scalar_type T;
    T const a0=quat_traits<A>::template read_element<0>(a);
    T const a1=quat_traits<A>::template read_element<1>(a);
    T const a2=quat_traits<A>::template read_element<2>(a);
    T const a3=quat_traits<A>::template read_element<3>(a);
    T const m2=a0*a0+a1*a1+a2*a2+a3*a3;
    if( m2==scalar_traits<typename quat_traits<A>::scalar_type>::value(0) )
        BOOST_QVM_THROW_EXCEPTION(zero_magnitude_error());
    T const rm=scalar_traits<T>::value(1)/sqrt(m2);
    typedef typename deduce_quat<A>::type R;
    R r;
    write_quat_element<0>(r,a0*rm);
    write_quat_element<1>(r,a1*rm);
    write_quat_element<2>(r,a2*rm);
    write_quat_element<3>(r,a3*rm);
    return r;
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_quat<A>::value,
    void>::type
normalize( A & a )
    {
    typedef typename quat_traits<A>::scalar_type T;
    T const a0=quat_traits<A>::template read_element<0>(a);
    T const a1=quat_traits<A>::template read_element<1>(a);
    T const a2=quat_traits<A>::template read_element<2>(a);
    T const a3=quat_traits<A>::template read_element<3>(a);
    T const m2=a0*a0+a1*a1+a2*a2+a3*a3;
    if( m2==scalar_traits<typename quat_traits<A>::scalar_type>::value(0) )
        BOOST_QVM_THROW_EXCEPTION(zero_magnitude_error());
    T const rm=scalar_traits<T>::value(1)/sqrt(m2);
    write_quat_element<0>(a,quat_traits<A>::template read_element<0>(a)*rm);
    write_quat_element<1>(a,quat_traits<A>::template read_element<1>(a)*rm);
    write_quat_element<2>(a,quat_traits<A>::template read_element<2>(a)*rm);
    write_quat_element<3>(a,quat_traits<A>::template read_element<3>(a)*rm);
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if<
    msvc_parse_bug_workaround::quats<A,B>,
    A &>::type
operator+=( A & a, B const & b )
    {
    write_quat_element<0>(a,quat_traits<A>::template read_element<0>(a)+quat_traits<B>::template read_element<0>(b));
    write_quat_element<1>(a,quat_traits<A>::template read_element<1>(a)+quat_traits<B>::template read_element<1>(b));
    write_quat_element<2>(a,quat_traits<A>::template read_element<2>(a)+quat_traits<B>::template read_element<2>(b));
    write_quat_element<3>(a,quat_traits<A>::template read_element<3>(a)+quat_traits<B>::template read_element<3>(b));
    return a;
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_quat<A>::value && is_quat<B>::value,
    deduce_quat2<A,B> >::type
operator+( A const & a, B const & b )
    {
    typedef typename deduce_quat2<A,B>::type R;
    R r;
    write_quat_element<0>(r,quat_traits<A>::template read_element<0>(a)+quat_traits<B>::template read_element<0>(b));
    write_quat_element<1>(r,quat_traits<A>::template read_element<1>(a)+quat_traits<B>::template read_element<1>(b));
    write_quat_element<2>(r,quat_traits<A>::template read_element<2>(a)+quat_traits<B>::template read_element<2>(b));
    write_quat_element<3>(r,quat_traits<A>::template read_element<3>(a)+quat_traits<B>::template read_element<3>(b));
    return r;
    }

template <class A,class B,class C>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_quat<A>::value && is_quat<B>::value && is_scalar<C>::value,
    deduce_quat2<A,B> >::type
slerp360( A const & a, B const & b, C t )
    {
    typedef typename deduce_quat2<A,B>::type R;
    typedef typename quat_traits<R>::scalar_type TR;
    TR const one = scalar_traits<TR>::value(1);
    TR const threshold = one - one / scalar_traits<TR>::value(2000); //0.9995
    TR const dp = dot(a,b);
    TR const abs_dp = abs(dp);
    if( abs_dp > threshold )
        return a*(one-t) + b*t;
    TR const th = acos(dp);
    TR const invsinth = one / sin(th);
    return a * (sin(th * (one-t)) * invsinth) + b * (sin(th * t) * invsinth);
    }

template <class A,class B,class C>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_quat<A>::value && is_quat<B>::value && is_scalar<C>::value,
    deduce_quat2<A,B> >::type
slerp180( A const & a, B const & b, C t )
    {
    typedef typename deduce_quat2<A,B>::type R;
    typedef typename quat_traits<R>::scalar_type TR;
    TR const one = scalar_traits<TR>::value(1);
    TR const threshold = one - one / scalar_traits<TR>::value(2000); //0.9995
    TR const dp = dot(a,b);
    TR const abs_dp = abs(dp);
    if( abs_dp > threshold )
        return a*(one-t)*sign(dp) + b*t;
    TR const th = acos(abs_dp);
    TR const invsinth = one / sin(th);
    return a * (sin(th * (one-t)) * invsinth * sign(dp)) + b * (sin(th * t) * invsinth);
    }

template <class A,class B,class C>
BOOST_QVM_DEPRECATED("please use slerp180 or slerp360")
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_quat<A>::value && is_quat<B>::value && is_scalar<C>::value,
    deduce_quat2<A,B> >::type
slerp( A const & a, B const & b, C t )
    {
    return slerp360(a, b, t);
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <class T>
    class
    qref_
        {
        qref_( qref_ const & );
        qref_ & operator=( qref_ const & );
        ~qref_();

        public:

        template <class R>
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        qref_ &
        operator=( R const & x )
            {
            assign(*this,x);
            return *this;
            }

        template <class R
#if __cplusplus >= 201103L
            , class = typename enable_if<is_quat<R> >::type
#endif
        >
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        operator R() const
            {
            R r;
            assign(r,*this);
            return r;
            }
        };

    template <class Q,bool WriteElementRef=quat_write_element_ref<Q>::value>
    struct qref_write_traits;

    template <class Q>
    struct
    qref_write_traits<Q,true>
        {
        typedef typename quat_traits<Q>::scalar_type scalar_type;
        typedef qvm_detail::qref_<Q> this_quaternion;

        template <int I>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        scalar_type &
        write_element( this_quaternion & x )
            {
            BOOST_QVM_STATIC_ASSERT(I>=0);
            BOOST_QVM_STATIC_ASSERT(I<4);
            return quat_traits<Q>::template write_element<I>(reinterpret_cast<Q &>(x));
            }
        };

    template <class Q>
    struct
    qref_write_traits<Q,false>
        {
        typedef typename quat_traits<Q>::scalar_type scalar_type;
        typedef qvm_detail::qref_<Q> this_quaternion;

        template <int I>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        void
        write_element( this_quaternion & x, scalar_type s )
            {
            BOOST_QVM_STATIC_ASSERT(I>=0);
            BOOST_QVM_STATIC_ASSERT(I<4);
            quat_traits<Q>::template write_element<I>(reinterpret_cast<Q &>(x), s);
            }
        };
    }

template <class Q>
struct quat_traits;

template <class Q>
struct
quat_traits< qvm_detail::qref_<Q> >:
    qvm_detail::qref_write_traits<Q>
    {
    typedef typename quat_traits<Q>::scalar_type scalar_type;
    typedef qvm_detail::qref_<Q> this_quaternion;

    template <int I>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_quaternion const & x )
        {
        BOOST_QVM_STATIC_ASSERT(I>=0);
        BOOST_QVM_STATIC_ASSERT(I<4);
        return quat_traits<Q>::template read_element<I>(reinterpret_cast<Q const &>(x));
        }
    };

template <class Q>
struct
deduce_quat< qvm_detail::qref_<Q> >
    {
    typedef quat<typename quat_traits<Q>::scalar_type> type;
    };

template <class Q>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
typename enable_if_c<
    is_quat<Q>::value,
    qvm_detail::qref_<Q> const &>::type
qref( Q const & a )
    {
    return reinterpret_cast<qvm_detail::qref_<Q> const &>(a);
    }

template <class Q>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
typename enable_if_c<
    is_quat<Q>::value,
    qvm_detail::qref_<Q> &>::type
qref( Q & a )
    {
    return reinterpret_cast<qvm_detail::qref_<Q> &>(a);
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <class T>
    class
    zero_q_
        {
        zero_q_( zero_q_ const & );
        zero_q_ & operator=( zero_q_ const & );
        ~zero_q_();

        public:

        template <class R
#if __cplusplus >= 201103L
            , class = typename enable_if<is_quat<R> >::type
#endif
        >
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        operator R() const
            {
            R r;
            assign(r,*this);
            return r;
            }
        };
    }

template <class T>
struct
quat_traits< qvm_detail::zero_q_<T> >
    {
    typedef qvm_detail::zero_q_<T> this_quaternion;
    typedef T scalar_type;

    template <int I>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_quaternion const & )
        {
        BOOST_QVM_STATIC_ASSERT(I>=0);
        BOOST_QVM_STATIC_ASSERT(I<4);
        return scalar_traits<scalar_type>::value(0);
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int i, this_quaternion const & )
        {
        BOOST_QVM_ASSERT(i>=0); (void)i;
        BOOST_QVM_ASSERT(i<4);
        return scalar_traits<scalar_type>::value(0);
        }
    };

template <class T>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
qvm_detail::zero_q_<T> const &
zero_quat()
    {
    return *(qvm_detail::zero_q_<T> const *)qvm_detail::get_valid_ptr_quat_operations();
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_quat<A>::value,
    void>::type
set_zero( A & a )
    {
    typedef typename quat_traits<A>::scalar_type T;
    T const zero=scalar_traits<T>::value(0);
    write_quat_element<0>(a,zero);
    write_quat_element<1>(a,zero);
    write_quat_element<2>(a,zero);
    write_quat_element<3>(a,zero);
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <class V>
    struct
    rot_quat_
        {
        typedef typename vec_traits<V>::scalar_type scalar_type;
        scalar_type a[4];

        template <class Angle>
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE
        rot_quat_( V const & axis, Angle angle )
            {
            scalar_type const x=vec_traits<V>::template read_element<0>(axis);
            scalar_type const y=vec_traits<V>::template read_element<1>(axis);
            scalar_type const z=vec_traits<V>::template read_element<2>(axis);
            scalar_type const m2=x*x+y*y+z*z;
            if( m2==scalar_traits<scalar_type>::value(0) )
                BOOST_QVM_THROW_EXCEPTION(zero_magnitude_error());
            scalar_type const rm=scalar_traits<scalar_type>::value(1)/sqrt(m2);
            angle/=2;
            scalar_type const s=sin(angle);
            a[0] = cos(angle);
            a[1] = rm*x*s;
            a[2] = rm*y*s;
            a[3] = rm*z*s;
            }

        template <class R
#if __cplusplus >= 201103L
            , class = typename enable_if<is_quat<R> >::type
#endif
        >
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        operator R() const
            {
            R r;
            assign(r,*this);
            return r;
            }
        };
    }

template <class V>
struct
quat_traits< qvm_detail::rot_quat_<V> >
    {
    typedef qvm_detail::rot_quat_<V> this_quaternion;
    typedef typename this_quaternion::scalar_type scalar_type;

    template <int I>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_quaternion const & x )
        {
        BOOST_QVM_STATIC_ASSERT(I>=0);
        BOOST_QVM_STATIC_ASSERT(I<4);
        return x.a[I];
        }
    };

template <class V>
struct
deduce_quat< qvm_detail::rot_quat_<V> >
    {
    typedef quat<typename vec_traits<V>::scalar_type> type;
    };

template <class A,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE
typename enable_if_c<
    is_vec<A>::value && vec_traits<A>::dim==3,
    qvm_detail::rot_quat_<A> >::type
rot_quat( A const & axis, Angle angle )
    {
    return qvm_detail::rot_quat_<A>(axis,angle);
    }

template <class A,class B,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_quat<A>::value &&
    is_vec<B>::value && vec_traits<B>::dim==3,
    void>::type
set_rot( A & a, B const & axis, Angle angle )
    {
    assign(a,rot_quat(axis,angle));
    }

template <class A,class B,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_quat<A>::value &&
    is_vec<B>::value && vec_traits<B>::dim==3,
    void>::type
rotate( A & a, B const & axis, Angle angle )
    {
    a *= rot_quat(axis,angle);
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <class T>
    struct
    rotx_quat_
        {
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        rotx_quat_()
            {
            }

        template <class R
#if __cplusplus >= 201103L
            , class = typename enable_if<is_quat<R> >::type
#endif
        >
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        operator R() const
            {
            R r;
            assign(r,*this);
            return r;
            }

        private:

        rotx_quat_( rotx_quat_ const & );
        rotx_quat_ & operator=( rotx_quat_ const & );
        ~rotx_quat_();
        };

    template <int I>
    struct
    rotx_q_get
        {
        template <class T>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        T
        get( T const & )
            {
            return scalar_traits<T>::value(0);
            }
        };

    template <>
    struct
    rotx_q_get<1>
        {
        template <class T>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        T
        get( T const & angle )
            {
            return sin(angle/2);
            }
        };

    template <>
    struct
    rotx_q_get<0>
        {
        template <class T>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        T
        get( T const & angle )
            {
            return cos(angle/2);
            }
        };
    }

template <class Angle>
struct
quat_traits< qvm_detail::rotx_quat_<Angle> >
    {
    typedef qvm_detail::rotx_quat_<Angle> this_quaternion;
    typedef Angle scalar_type;

    template <int I>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_quaternion const & x )
        {
        BOOST_QVM_STATIC_ASSERT(I>=0);
        BOOST_QVM_STATIC_ASSERT(I<4);
        return qvm_detail::rotx_q_get<I>::get(reinterpret_cast<Angle const &>(x));
        }
    };

template <class Angle>
struct
deduce_quat< qvm_detail::rotx_quat_<Angle> >
    {
    typedef quat<Angle> type;
    };

template <class Angle>
struct
deduce_quat2< qvm_detail::rotx_quat_<Angle>, qvm_detail::rotx_quat_<Angle> >
    {
    typedef quat<Angle> type;
    };

template <class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
qvm_detail::rotx_quat_<Angle> const &
rotx_quat( Angle const & angle )
    {
    return reinterpret_cast<qvm_detail::rotx_quat_<Angle> const &>(angle);
    }

template <class A,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_quat<A>::value,
    void>::type
set_rotx( A & a, Angle angle )
    {
    assign(a,rotx_quat(angle));
    }

template <class A,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_quat<A>::value,
    void>::type
rotate_x( A & a, Angle angle )
    {
    a *= rotx_quat(angle);
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <class T>
    struct
    roty_quat_
        {
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        roty_quat_()
            {
            }

        template <class R
#if __cplusplus >= 201103L
            , class = typename enable_if<is_quat<R> >::type
#endif
        >
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        operator R() const
            {
            R r;
            assign(r,*this);
            return r;
            }

        private:

        roty_quat_( roty_quat_ const & );
        roty_quat_ & operator=( roty_quat_ const & );
        ~roty_quat_();
        };

    template <int I>
    struct
    roty_q_get
        {
        template <class T>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        T
        get( T const & )
            {
            return scalar_traits<T>::value(0);
            }
        };

    template <>
    struct
    roty_q_get<2>
        {
        template <class T>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        T
        get( T const & angle )
            {
            return sin(angle/2);
            }
        };

    template <>
    struct
    roty_q_get<0>
        {
        template <class T>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        T
        get( T const & angle )
            {
            return cos(angle/2);
            }
        };
    }

template <class Angle>
struct
quat_traits< qvm_detail::roty_quat_<Angle> >
    {
    typedef qvm_detail::roty_quat_<Angle> this_quaternion;
    typedef Angle scalar_type;

    template <int I>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_quaternion const & x )
        {
        BOOST_QVM_STATIC_ASSERT(I>=0);
        BOOST_QVM_STATIC_ASSERT(I<4);
        return qvm_detail::roty_q_get<I>::get(reinterpret_cast<Angle const &>(x));
        }
    };

template <class Angle>
struct
deduce_quat< qvm_detail::roty_quat_<Angle> >
    {
    typedef quat<Angle> type;
    };

template <class Angle>
struct
deduce_quat2< qvm_detail::roty_quat_<Angle>, qvm_detail::roty_quat_<Angle> >
    {
    typedef quat<Angle> type;
    };

template <class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
qvm_detail::roty_quat_<Angle> const &
roty_quat( Angle const & angle )
    {
    return reinterpret_cast<qvm_detail::roty_quat_<Angle> const &>(angle);
    }

template <class A,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_quat<A>::value,
    void>::type
set_roty( A & a, Angle angle )
    {
    assign(a,roty_quat(angle));
    }

template <class A,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_quat<A>::value,
    void>::type
rotate_y( A & a, Angle angle )
    {
    a *= roty_quat(angle);
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <class T>
    struct
    rotz_quat_
        {
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        rotz_quat_()
            {
            }

        template <class R
#if __cplusplus >= 201103L
            , class = typename enable_if<is_quat<R> >::type
#endif
        >
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        operator R() const
            {
            R r;
            assign(r,*this);
            return r;
            }

        private:

        rotz_quat_( rotz_quat_ const & );
        rotz_quat_ & operator=( rotz_quat_ const & );
        ~rotz_quat_();
        };

    template <int I>
    struct
    rotz_q_get
        {
        template <class T>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        T
        get( T const & )
            {
            return scalar_traits<T>::value(0);
            }
        };

    template <>
    struct
    rotz_q_get<3>
        {
        template <class T>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        T
        get( T const & angle )
            {
            return sin(angle/2);
            }
        };

    template <>
    struct
    rotz_q_get<0>
        {
        template <class T>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        T
        get( T const & angle )
            {
            return cos(angle/2);
            }
        };
    }

template <class Angle>
struct
quat_traits< qvm_detail::rotz_quat_<Angle> >
    {
    typedef qvm_detail::rotz_quat_<Angle> this_quaternion;
    typedef Angle scalar_type;

    template <int I>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_quaternion const & x )
        {
        BOOST_QVM_STATIC_ASSERT(I>=0);
        BOOST_QVM_STATIC_ASSERT(I<4);
        return qvm_detail::rotz_q_get<I>::get(reinterpret_cast<Angle const &>(x));
        }
    };

template <class Angle>
struct
deduce_quat< qvm_detail::rotz_quat_<Angle> >
    {
    typedef quat<Angle> type;
    };

template <class Angle>
struct
deduce_quat2< qvm_detail::rotz_quat_<Angle>, qvm_detail::rotz_quat_<Angle> >
    {
    typedef quat<Angle> type;
    };

template <class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
qvm_detail::rotz_quat_<Angle> const &
rotz_quat( Angle const & angle )
    {
    return reinterpret_cast<qvm_detail::rotz_quat_<Angle> const &>(angle);
    }

template <class A,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_quat<A>::value,
    void>::type
set_rotz( A & a, Angle angle )
    {
    assign(a,rotz_quat(angle));
    }

template <class A,class Angle>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_quat<A>::value,
    void>::type
rotate_z( A & a, Angle angle )
    {
    a *= rotz_quat(angle);
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_quat<A>::value && is_vec<B>::value && vec_traits<B>::dim==3,
    typename quat_traits<A>::scalar_type>::type
axis_angle( A const & a, B & b )
    {
    typedef typename quat_traits<A>::scalar_type T;
    T a0=quat_traits<A>::template read_element<0>(a);
    T a1=quat_traits<A>::template read_element<1>(a);
    T a2=quat_traits<A>::template read_element<2>(a);
    T a3=quat_traits<A>::template read_element<3>(a);
    if( a0>1 )
        {
        T const m2=a0*a0+a1*a1+a2*a2+a3*a3;
        if( m2==scalar_traits<T>::value(0) )
            BOOST_QVM_THROW_EXCEPTION(zero_magnitude_error());
        T const s=sqrt(m2);
        a0/=s;
        a1/=s;
        a2/=s;
        a3/=s;
        }
    if( T s=sqrt(1-a0*a0) )
        {
        write_vec_element<0>(b, a1/s);
        write_vec_element<1>(b, a2/s);
        write_vec_element<2>(b, a3/s);
        }
    else
        {
        typedef typename vec_traits<B>::scalar_type U;
        write_vec_element<0>(b, scalar_traits<U>::value(1));
        write_vec_element<1>(b, scalar_traits<U>::value(0));
        write_vec_element<2>(b, scalar_traits<U>::value(0));
        }
    return scalar_traits<T>::value(2) * qvm::acos(a0);
    }

////////////////////////////////////////////////

namespace
sfinae
    {
    using ::boost::qvm::assign;
    using ::boost::qvm::cmp;
    using ::boost::qvm::convert_to;
    using ::boost::qvm::conjugate;
    using ::boost::qvm::set_identity;
    using ::boost::qvm::set_zero;
    using ::boost::qvm::scalar_cast;
    using ::boost::qvm::operator/=;
    using ::boost::qvm::operator/;
    using ::boost::qvm::dot;
    using ::boost::qvm::operator==;
    using ::boost::qvm::inverse;
    using ::boost::qvm::mag_sqr;
    using ::boost::qvm::mag;
    using ::boost::qvm::slerp360;
    using ::boost::qvm::slerp180;
    using ::boost::qvm::slerp;
    using ::boost::qvm::operator-=;
    using ::boost::qvm::operator-;
    using ::boost::qvm::operator*=;
    using ::boost::qvm::operator*;
    using ::boost::qvm::operator!=;
    using ::boost::qvm::normalized;
    using ::boost::qvm::normalize;
    using ::boost::qvm::operator+=;
    using ::boost::qvm::operator+;
    using ::boost::qvm::qref;
    using ::boost::qvm::rot_quat;
    using ::boost::qvm::set_rot;
    using ::boost::qvm::rotate;
    using ::boost::qvm::rotx_quat;
    using ::boost::qvm::set_rotx;
    using ::boost::qvm::rotate_x;
    using ::boost::qvm::roty_quat;
    using ::boost::qvm::set_roty;
    using ::boost::qvm::rotate_y;
    using ::boost::qvm::rotz_quat;
    using ::boost::qvm::set_rotz;
    using ::boost::qvm::rotate_z;
    }

} }

#endif
// <<< #include <boost/qvm/quat_operations.hpp>
// #line 14 "boost/qvm/lite.hpp"
// >>> #include <boost/qvm/quat_access.hpp>
#ifndef BOOST_QVM_QUAT_ACCESS_HPP_INCLUDED
#define BOOST_QVM_QUAT_ACCESS_HPP_INCLUDED

// #line 8 "boost/qvm/quat_access.hpp"
// #include <boost/qvm/config.hpp> // Expanded at line 109
// #include <boost/qvm/quat_traits.hpp> // Expanded at line 914
// #include <boost/qvm/deduce_vec.hpp> // Expanded at line 1450
// #include <boost/qvm/static_assert.hpp> // Expanded at line 1180
// #include <boost/qvm/enable_if.hpp> // Expanded at line 177

namespace boost { namespace qvm {

template <class Q> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_quat<Q>::value,typename quat_traits<Q>::scalar_type>::type S( Q const & a ) { return quat_traits<Q>::template read_element<0>(a); }
template <class Q> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_quat<Q>::value,typename quat_traits<Q>::scalar_type>::type X( Q const & a ) { return quat_traits<Q>::template read_element<1>(a); }
template <class Q> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_quat<Q>::value,typename quat_traits<Q>::scalar_type>::type Y( Q const & a ) { return quat_traits<Q>::template read_element<2>(a); }
template <class Q> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_quat<Q>::value,typename quat_traits<Q>::scalar_type>::type Z( Q const & a ) { return quat_traits<Q>::template read_element<3>(a); }

namespace
qvm_detail
    {
    template <int I,class Q>
    struct
    q_element_access
        {
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        void
        operator=( typename quat_traits<Q>::scalar_type s )
            {
            quat_traits<Q>::template write_element<I>(*reinterpret_cast<Q *>(this), s);
            }

        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        operator typename vec_traits<Q>::scalar_type() const
            {
            return quat_traits<Q>::template read_element<I>(*reinterpret_cast<Q const *>(this));
            }
        };

    template <class Q>
    struct
    quat_v_
        {
        template <class R
#if __cplusplus >= 201103L
            , class = typename enable_if<is_vec<R> >::type
#endif
        >
        operator R() const
            {
            R r;
            assign(r,*this);
            return r;
            }

        private:

        quat_v_( quat_v_ const & );
        quat_v_ const & operator=( quat_v_ const & );
        ~quat_v_();
        };

    template <class Q,bool WriteElementRef=quat_write_element_ref<Q>::value>
    struct quat_v_write_traits;

    template <class Q>
    struct
    quat_v_write_traits<Q,true>
        {
        typedef qvm_detail::quat_v_<Q> this_vector;
        typedef typename quat_traits<Q>::scalar_type scalar_type;
        static int const dim=3;

        template <int I>
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        static
        scalar_type &
        write_element( this_vector & q )
            {
            BOOST_QVM_STATIC_ASSERT(I>=0);
            BOOST_QVM_STATIC_ASSERT(I<dim);
            return quat_traits<Q>::template write_element<I+1>( reinterpret_cast<Q &>(q) );
            }
        };

    template <class Q>
    struct
    quat_v_write_traits<Q,false>
        {
        typedef qvm_detail::quat_v_<Q> this_vector;
        typedef typename quat_traits<Q>::scalar_type scalar_type;
        static int const dim=3;

        template <int I>
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        static
        void
        write_element( this_vector & q, scalar_type s )
            {
            BOOST_QVM_STATIC_ASSERT(I>=0);
            BOOST_QVM_STATIC_ASSERT(I<dim);
            quat_traits<Q>::template write_element<I+1>( reinterpret_cast<Q &>(q), s );
            }
        };
    }

template <class V>
struct vec_traits;

template <class Q>
struct
vec_traits< qvm_detail::quat_v_<Q> >:
    qvm_detail::quat_v_write_traits<Q>
    {
    typedef qvm_detail::quat_v_<Q> this_vector;
    typedef typename quat_traits<Q>::scalar_type scalar_type;
    static int const dim=3;

    template <int I>
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    static
    scalar_type
    read_element( this_vector const & q )
        {
        BOOST_QVM_STATIC_ASSERT(I>=0);
        BOOST_QVM_STATIC_ASSERT(I<dim);
        return quat_traits<Q>::template read_element<I+1>( reinterpret_cast<Q const &>(q) );
        }
    };

template <class Q,int D>
struct
deduce_vec<qvm_detail::quat_v_<Q>,D>
    {
    typedef vec<typename quat_traits<Q>::scalar_type,D> type;
    };

template <class Q,int D>
struct
deduce_vec2<qvm_detail::quat_v_<Q>,qvm_detail::quat_v_<Q>,D>
    {
    typedef vec<typename quat_traits<Q>::scalar_type,D> type;
    };

template <class Q>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
typename enable_if_c<
    is_quat<Q>::value,
    qvm_detail::quat_v_<Q> const &>::type
V( Q const & a )
    {
    return reinterpret_cast<qvm_detail::quat_v_<Q> const &>(a);
    }

template <class Q>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
typename enable_if_c<
    is_quat<Q>::value,
    qvm_detail::quat_v_<Q> &>::type
V( Q & a )
    {
    return reinterpret_cast<qvm_detail::quat_v_<Q> &>(a);
    }

template <class Q> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_quat<Q>::value && quat_write_element_ref<Q>::value,typename quat_traits<Q>::scalar_type &>::type S( Q & a ) { return quat_traits<Q>::template write_element<0>(a); }
template <class Q> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_quat<Q>::value && quat_write_element_ref<Q>::value,typename quat_traits<Q>::scalar_type &>::type X( Q & a ) { return quat_traits<Q>::template write_element<1>(a); }
template <class Q> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_quat<Q>::value && quat_write_element_ref<Q>::value,typename quat_traits<Q>::scalar_type &>::type Y( Q & a ) { return quat_traits<Q>::template write_element<2>(a); }
template <class Q> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_quat<Q>::value && quat_write_element_ref<Q>::value,typename quat_traits<Q>::scalar_type &>::type Z( Q & a ) { return quat_traits<Q>::template write_element<3>(a); }

template <class Q> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_quat<Q>::value && !quat_write_element_ref<Q>::value,qvm_detail::q_element_access<0,Q> &>::type S( Q & a ) { return *reinterpret_cast<qvm_detail::q_element_access<0, Q> *>(&a); }
template <class Q> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_quat<Q>::value && !quat_write_element_ref<Q>::value,qvm_detail::q_element_access<1,Q> &>::type X( Q & a ) { return *reinterpret_cast<qvm_detail::q_element_access<1, Q> *>(&a); }
template <class Q> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_quat<Q>::value && !quat_write_element_ref<Q>::value,qvm_detail::q_element_access<2,Q> &>::type Y( Q & a ) { return *reinterpret_cast<qvm_detail::q_element_access<2, Q> *>(&a); }
template <class Q> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_quat<Q>::value && !quat_write_element_ref<Q>::value,qvm_detail::q_element_access<3,Q> &>::type Z( Q & a ) { return *reinterpret_cast<qvm_detail::q_element_access<3, Q> *>(&a); }

} }

#endif
// <<< #include <boost/qvm/quat_access.hpp>
// #line 15 "boost/qvm/lite.hpp"
// >>> #include <boost/qvm/quat_traits_defaults.hpp>
#ifndef BOOST_QVM_QUAT_TRAITS_DEFAULTS_HPP_INCLUDED
#define BOOST_QVM_QUAT_TRAITS_DEFAULTS_HPP_INCLUDED

// #line 8 "boost/qvm/quat_traits_defaults.hpp"
// #include <boost/qvm/config.hpp> // Expanded at line 109
// #include <boost/qvm/assert.hpp> // Expanded at line 887

namespace boost { namespace qvm {

template <class>
struct quat_traits;

template <class QuatType,class ScalarType>
struct
quat_traits_defaults
    {
    typedef QuatType quat_type;
    typedef ScalarType scalar_type;

    template <int I>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( quat_type const & x )
        {
        return quat_traits<quat_type>::template write_element<I>(const_cast<quat_type &>(x));
        }
    };

} }

#endif
// <<< #include <boost/qvm/quat_traits_defaults.hpp>
// #line 16 "boost/qvm/lite.hpp"
// #include <boost/qvm/quat_traits.hpp> // Expanded at line 914
// >>> #include <boost/qvm/quat.hpp>
#ifndef BOOST_QVM_QUAT_HPP_INCLUDED
#define BOOST_QVM_QUAT_HPP_INCLUDED

// #line 8 "boost/qvm/quat.hpp"
// #include <boost/qvm/detail/quat_assign.hpp> // Expanded at line 11183
// #include <boost/qvm/assert.hpp> // Expanded at line 887
// #include <boost/qvm/static_assert.hpp> // Expanded at line 1180

namespace boost { namespace qvm {

template <class T>
struct
quat
    {
    T a[4];
    template <class R
#if __cplusplus >= 201103L
        , class = typename enable_if<is_quat<R> >::type
#endif
    >
    operator R() const
        {
        R r;
        assign(r,*this);
        return r;
        }
    };

template <class Q>
struct quat_traits;

template <class T>
struct
quat_traits< quat<T> >
    {
    typedef quat<T> this_quaternion;
    typedef T scalar_type;

    template <int I>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_quaternion const & x )
        {
        BOOST_QVM_STATIC_ASSERT(I>=0);
        BOOST_QVM_STATIC_ASSERT(I<4);
        return x.a[I];
        }

    template <int I>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type &
    write_element( this_quaternion & x )
        {
        BOOST_QVM_STATIC_ASSERT(I>=0);
        BOOST_QVM_STATIC_ASSERT(I<4);
        return x.a[I];
        }
    };

} }

#endif
// <<< #include <boost/qvm/quat.hpp>
// #line 18 "boost/qvm/lite.hpp"
// >>> #include <boost/qvm/quat_vec_operations.hpp>
#ifndef BOOST_QVM_QUAT_VEC_OPERATIONS_HPP_INCLUDED
#define BOOST_QVM_QUAT_VEC_OPERATIONS_HPP_INCLUDED

// #line 8 "boost/qvm/quat_vec_operations.hpp"
// #include <boost/qvm/quat_traits.hpp> // Expanded at line 914
// #include <boost/qvm/deduce_vec.hpp> // Expanded at line 1450
// #include <boost/qvm/config.hpp> // Expanded at line 109
// #include <boost/qvm/enable_if.hpp> // Expanded at line 177

namespace boost { namespace qvm {

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_quat<A>::value &&
    is_vec<B>::value && vec_traits<B>::dim==3,
    deduce_vec2<A,B,3> >::type
operator*( A const & a, B const & b )
    {
    typedef typename deduce_vec2<A,B,3>::type R;
    typedef typename quat_traits<A>::scalar_type TA;
    typedef typename vec_traits<B>::scalar_type TB;
    TA const aa = quat_traits<A>::template read_element<0>(a);
    TA const ab = quat_traits<A>::template read_element<1>(a);
    TA const ac = quat_traits<A>::template read_element<2>(a);
    TA const ad = quat_traits<A>::template read_element<3>(a);
    TA const t2 = aa*ab;
    TA const t3 = aa*ac;
    TA const t4 = aa*ad;
    TA const t5 = -ab*ab;
    TA const t6 = ab*ac;
    TA const t7 = ab*ad;
    TA const t8 = -ac*ac;
    TA const t9 = ac*ad;
    TA const t10     = -ad*ad;
    TB const bx = vec_traits<B>::template read_element<0>(b);
    TB const by = vec_traits<B>::template read_element<1>(b);
    TB const bz = vec_traits<B>::template read_element<2>(b);
    R r;
    write_vec_element<0>(r, 2*((t8+t10)*bx + (t6-t4)*by + (t3+t7)*bz) + bx);
    write_vec_element<1>(r, 2*((t4+t6)*bx + (t5+t10)*by + (t9-t2)*bz) + by);
    write_vec_element<2>(r, 2*((t7-t3)*bx + (t2+t9)*by + (t5+t8)*bz) + bz);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

} }

#endif
// <<< #include <boost/qvm/quat_vec_operations.hpp>
// #line 19 "boost/qvm/lite.hpp"
// >>> #include <boost/qvm/vec_operations.hpp>
#ifndef BOOST_QVM_VEC_OPERATIONS_HPP_INCLUDED
#define BOOST_QVM_VEC_OPERATIONS_HPP_INCLUDED

// #line 9 "boost/qvm/vec_operations.hpp"
// >>> #include <boost/qvm/detail/vec_assign.hpp>
#ifndef BOOST_QVM_DETAIL_VEC_ASSIGN_HPP_INCLUDED
#define BOOST_QVM_DETAIL_VEC_ASSIGN_HPP_INCLUDED

// #line 8 "boost/qvm/detail/vec_assign.hpp"
// >>> #include <boost/qvm/gen/vec_assign2.hpp>
#ifndef BOOST_QVM_GEN_VEC_ASSIGN2_HPP_INCLUDED
#define BOOST_QVM_GEN_VEC_ASSIGN2_HPP_INCLUDED

// #line 9 "boost/qvm/gen/vec_assign2.hpp"
// #include <boost/qvm/config.hpp> // Expanded at line 109
// #include <boost/qvm/enable_if.hpp> // Expanded at line 177
// #include <boost/qvm/vec_traits.hpp> // Expanded at line 1010

namespace boost { namespace qvm {

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    vec_traits<A>::dim==2 && vec_traits<B>::dim==2,
    A &>::type
assign( A & a, B const & b )
    {
    write_vec_element<0>(a,vec_traits<B>::template read_element<0>(b));
    write_vec_element<1>(a,vec_traits<B>::template read_element<1>(b));
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::assign;
    }

namespace
qvm_detail
    {
    template <int D>
    struct assign_vv_defined;

    template <>
    struct
    assign_vv_defined<2>
        {
        static bool const value=true;
        };
    }

} }

#endif
// <<< #include <boost/qvm/gen/vec_assign2.hpp>
// #line 9 "boost/qvm/detail/vec_assign.hpp"
// >>> #include <boost/qvm/gen/vec_assign3.hpp>
#ifndef BOOST_QVM_GEN_VEC_ASSIGN3_HPP_INCLUDED
#define BOOST_QVM_GEN_VEC_ASSIGN3_HPP_INCLUDED

// #line 9 "boost/qvm/gen/vec_assign3.hpp"
// #include <boost/qvm/config.hpp> // Expanded at line 109
// #include <boost/qvm/enable_if.hpp> // Expanded at line 177
// #include <boost/qvm/vec_traits.hpp> // Expanded at line 1010

namespace boost { namespace qvm {

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    vec_traits<A>::dim==3 && vec_traits<B>::dim==3,
    A &>::type
assign( A & a, B const & b )
    {
    write_vec_element<0>(a,vec_traits<B>::template read_element<0>(b));
    write_vec_element<1>(a,vec_traits<B>::template read_element<1>(b));
    write_vec_element<2>(a,vec_traits<B>::template read_element<2>(b));
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::assign;
    }

namespace
qvm_detail
    {
    template <int D>
    struct assign_vv_defined;

    template <>
    struct
    assign_vv_defined<3>
        {
        static bool const value=true;
        };
    }

} }

#endif
// <<< #include <boost/qvm/gen/vec_assign3.hpp>
// #line 10 "boost/qvm/detail/vec_assign.hpp"
// >>> #include <boost/qvm/gen/vec_assign4.hpp>
#ifndef BOOST_QVM_GEN_VEC_ASSIGN4_HPP_INCLUDED
#define BOOST_QVM_GEN_VEC_ASSIGN4_HPP_INCLUDED

// #line 9 "boost/qvm/gen/vec_assign4.hpp"
// #include <boost/qvm/config.hpp> // Expanded at line 109
// #include <boost/qvm/enable_if.hpp> // Expanded at line 177
// #include <boost/qvm/vec_traits.hpp> // Expanded at line 1010

namespace boost { namespace qvm {

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    vec_traits<A>::dim==4 && vec_traits<B>::dim==4,
    A &>::type
assign( A & a, B const & b )
    {
    write_vec_element<0>(a,vec_traits<B>::template read_element<0>(b));
    write_vec_element<1>(a,vec_traits<B>::template read_element<1>(b));
    write_vec_element<2>(a,vec_traits<B>::template read_element<2>(b));
    write_vec_element<3>(a,vec_traits<B>::template read_element<3>(b));
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::assign;
    }

namespace
qvm_detail
    {
    template <int D>
    struct assign_vv_defined;

    template <>
    struct
    assign_vv_defined<4>
        {
        static bool const value=true;
        };
    }

} }

#endif
// <<< #include <boost/qvm/gen/vec_assign4.hpp>
// #line 11 "boost/qvm/detail/vec_assign.hpp"

namespace boost { namespace qvm {

namespace
qvm_detail
    {
    template <int D>
    struct
    assign_vv_defined
        {
        static bool const value=false;
        };

    template <int I,int N>
    struct
    copy_vector_elements
        {
        template <class A,class B>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        typename enable_if_c<
            vec_write_element_ref<A>::value,
            void>::type
        f( A & a, B const & b )
            {
            vec_traits<A>::template write_element<I>(a) = vec_traits<B>::template read_element<I>(b);
            copy_vector_elements<I+1,N>::f(a,b);
            }

        template <class A,class B>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        typename enable_if_c<
            !vec_write_element_ref<A>::value,
            void>::type
        f( A & a, B const & b )
            {
            vec_traits<A>::template write_element<I>(a, vec_traits<B>::template read_element<I>(b));
            copy_vector_elements<I+1,N>::f(a,b);
            }
        };

    template <int N>
    struct
    copy_vector_elements<N,N>
        {
        template <class A,class B>
        static
        void
        f( A &, B const & )
            {
            }
        };
    }

template <class A,class B>
inline
typename enable_if_c<
    is_vec<A>::value && is_vec<B>::value &&
    vec_traits<A>::dim==vec_traits<B>::dim &&
    !qvm_detail::assign_vv_defined<vec_traits<A>::dim>::value,
    A &>::type
assign( A & a, B const & b )
    {
    qvm_detail::copy_vector_elements<0,vec_traits<A>::dim>::f(a,b);
    return a;
    }

} }

#endif
// <<< #include <boost/qvm/detail/vec_assign.hpp>
// #line 10 "boost/qvm/vec_operations.hpp"
// >>> #include <boost/qvm/vec_operations2.hpp>
// #line 5 "boost/qvm/vec_operations2.hpp"
// >>> #include <boost/qvm/gen/vec_operations2.hpp>
#ifndef BOOST_QVM_GEN_VEC_OPERATIONS2_HPP_INCLUDED
#define BOOST_QVM_GEN_VEC_OPERATIONS2_HPP_INCLUDED

// #line 9 "boost/qvm/gen/vec_operations2.hpp"
// #include <boost/qvm/deduce_scalar.hpp> // Expanded at line 904
// #include <boost/qvm/deduce_vec.hpp> // Expanded at line 1450
// #include <boost/qvm/error.hpp> // Expanded at line 17
// #include <boost/qvm/gen/vec_assign2.hpp> // Expanded at line 13220
// #include <boost/qvm/math.hpp> // Expanded at line 7399
// #include <boost/qvm/static_assert.hpp> // Expanded at line 1180
// #include <boost/qvm/throw_exception.hpp> // Expanded at line 1546

namespace boost { namespace qvm {

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    vec_traits<A>::dim==2 && vec_traits<B>::dim==2,
    deduce_vec2<A,B,2> >::type
operator+( A const & a, B const & b )
    {
    typedef typename deduce_vec2<A,B,2>::type R;
    BOOST_QVM_STATIC_ASSERT(vec_traits<R>::dim==2);
    R r;
    write_vec_element<0>(r,vec_traits<A>::template read_element<0>(a)+vec_traits<B>::template read_element<0>(b));
    write_vec_element<1>(r,vec_traits<A>::template read_element<1>(a)+vec_traits<B>::template read_element<1>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator+;
    }

namespace
qvm_detail
    {
    template <int D>
    struct plus_vv_defined;

    template <>
    struct
    plus_vv_defined<2>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    vec_traits<A>::dim==2 && vec_traits<B>::dim==2,
    deduce_vec2<A,B,2> >::type
operator-( A const & a, B const & b )
    {
    typedef typename deduce_vec2<A,B,2>::type R;
    BOOST_QVM_STATIC_ASSERT(vec_traits<R>::dim==2);
    R r;
    write_vec_element<0>(r,vec_traits<A>::template read_element<0>(a)-vec_traits<B>::template read_element<0>(b));
    write_vec_element<1>(r,vec_traits<A>::template read_element<1>(a)-vec_traits<B>::template read_element<1>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator-;
    }

namespace
qvm_detail
    {
    template <int D>
    struct minus_vv_defined;

    template <>
    struct
    minus_vv_defined<2>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    vec_traits<A>::dim==2 && vec_traits<B>::dim==2,
    A &>::type
operator+=( A & a, B const & b )
    {
    write_vec_element<0>(a,vec_traits<A>::template read_element<0>(a)+vec_traits<B>::template read_element<0>(b));
    write_vec_element<1>(a,vec_traits<A>::template read_element<1>(a)+vec_traits<B>::template read_element<1>(b));
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator+=;
    }

namespace
qvm_detail
    {
    template <int D>
    struct plus_eq_vv_defined;

    template <>
    struct
    plus_eq_vv_defined<2>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    vec_traits<A>::dim==2 && vec_traits<B>::dim==2,
    A &>::type
operator-=( A & a, B const & b )
    {
    write_vec_element<0>(a,vec_traits<A>::template read_element<0>(a)-vec_traits<B>::template read_element<0>(b));
    write_vec_element<1>(a,vec_traits<A>::template read_element<1>(a)-vec_traits<B>::template read_element<1>(b));
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator-=;
    }

namespace
qvm_detail
    {
    template <int D>
    struct minus_eq_vv_defined;

    template <>
    struct
    minus_eq_vv_defined<2>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    vec_traits<A>::dim==2 && is_scalar<B>::value,
    deduce_vec2<A,B,vec_traits<A>::dim> >::type
operator*( A const & a, B b )
    {
    typedef typename deduce_vec2<A,B,vec_traits<A>::dim>::type R;
    R r;
    write_vec_element<0>(r,vec_traits<A>::template read_element<0>(a)*b);
    write_vec_element<1>(r,vec_traits<A>::template read_element<1>(a)*b);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

namespace
qvm_detail
    {
    template <int D>
    struct mul_vs_defined;

    template <>
    struct
    mul_vs_defined<2>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_scalar<A>::value && vec_traits<B>::dim==2,
    deduce_vec2<A,B,vec_traits<B>::dim> >::type
operator*( A a, B const & b )
    {
    typedef typename deduce_vec2<A,B,vec_traits<B>::dim>::type R;
    R r;
    write_vec_element<0>(r,a*vec_traits<B>::template read_element<0>(b));
    write_vec_element<1>(r,a*vec_traits<B>::template read_element<1>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

namespace
qvm_detail
    {
    template <int D>
    struct mul_sv_defined;

    template <>
    struct
    mul_sv_defined<2>
        {
        static bool const value=true;
        };
    }

template <class A,class  B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    vec_traits<A>::dim==2 && is_scalar<B>::value,
    A &>::type
operator*=( A & a, B b )
    {
    write_vec_element<0>(a, vec_traits<A>::template read_element<0>(a)*b);
    write_vec_element<1>(a, vec_traits<A>::template read_element<1>(a)*b);
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*=;
    }

namespace
qvm_detail
    {
    template <int D>
    struct mul_eq_vs_defined;

    template <>
    struct
    mul_eq_vs_defined<2>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    vec_traits<A>::dim==2 && is_scalar<B>::value,
    deduce_vec2<A,B,vec_traits<A>::dim> >::type
operator/( A const & a, B b )
    {
    typedef typename deduce_vec2<A,B,vec_traits<A>::dim>::type R;
    R r;
    write_vec_element<0>(r,vec_traits<A>::template read_element<0>(a)/b);
    write_vec_element<1>(r,vec_traits<A>::template read_element<1>(a)/b);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator/;
    }

namespace
qvm_detail
    {
    template <int D>
    struct div_vs_defined;

    template <>
    struct
    div_vs_defined<2>
        {
        static bool const value=true;
        };
    }

template <class A,class  B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    vec_traits<A>::dim==2 && is_scalar<B>::value,
    A &>::type
operator/=( A & a, B b )
    {
    write_vec_element<0>(a, vec_traits<A>::template read_element<0>(a)/b);
    write_vec_element<1>(a, vec_traits<A>::template read_element<1>(a)/b);
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator/=;
    }

namespace
qvm_detail
    {
    template <int D>
    struct div_eq_vs_defined;

    template <>
    struct
    div_eq_vs_defined<2>
        {
        static bool const value=true;
        };
    }

template <class R,class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_vec<A>::value &&
    vec_traits<R>::dim==2 && vec_traits<A>::dim==2,
    R>::type
convert_to( A const & a )
    {
    R r;
    write_vec_element<0>(r,vec_traits<A>::template read_element<0>(a));
    write_vec_element<1>(r,vec_traits<A>::template read_element<1>(a));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::convert_to;
    }

namespace
qvm_detail
    {
    template <int D>
    struct convert_to_v_defined;

    template <>
    struct
    convert_to_v_defined<2>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    vec_traits<A>::dim==2 && vec_traits<B>::dim==2,
bool>::type
operator==( A const & a, B const & b )
    {
    return
        vec_traits<A>::template read_element<0>(a)==vec_traits<B>::template read_element<0>(b) &&
        vec_traits<A>::template read_element<1>(a)==vec_traits<B>::template read_element<1>(b);
    }

namespace
sfinae
    {
    using ::boost::qvm::operator==;
    }

namespace
qvm_detail
    {
    template <int D>
    struct eq_vv_defined;

    template <>
    struct
    eq_vv_defined<2>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    vec_traits<A>::dim==2 && vec_traits<B>::dim==2,
bool>::type
operator!=( A const & a, B const & b )
    {
    return
        !(vec_traits<A>::template read_element<0>(a)==vec_traits<B>::template read_element<0>(b)) ||
        !(vec_traits<A>::template read_element<1>(a)==vec_traits<B>::template read_element<1>(b));
    }

namespace
sfinae
    {
    using ::boost::qvm::operator!=;
    }

namespace
qvm_detail
    {
    template <int D>
    struct neq_vv_defined;

    template <>
    struct
    neq_vv_defined<2>
        {
        static bool const value=true;
        };
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    vec_traits<A>::dim==2,
    deduce_vec<A> >::type
operator-( A const & a )
    {
    typedef typename deduce_vec<A>::type R;
    R r;
    write_vec_element<0>(r,-vec_traits<A>::template read_element<0>(a));
    write_vec_element<1>(r,-vec_traits<A>::template read_element<1>(a));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator-;
    }

namespace
qvm_detail
    {
    template <int D>
    struct minus_v_defined;

    template <>
    struct
    minus_v_defined<2>
        {
        static bool const value=true;
        };
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_vec<A>::value && vec_traits<A>::dim==2,
    typename vec_traits<A>::scalar_type>::type
mag( A const & a )
    {
    typedef typename vec_traits<A>::scalar_type T;
    T const a0=vec_traits<A>::template read_element<0>(a);
    T const a1=vec_traits<A>::template read_element<1>(a);
    T const m2=a0*a0+a1*a1;
    T const mag=sqrt(m2);
    return mag;
    }

namespace
sfinae
    {
    using ::boost::qvm::mag;
    }

namespace
qvm_detail
    {
    template <int D>
    struct mag_v_defined;

    template <>
    struct
    mag_v_defined<2>
        {
        static bool const value=true;
        };
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_vec<A>::value && vec_traits<A>::dim==2,
    typename vec_traits<A>::scalar_type>::type
mag_sqr( A const & a )
    {
    typedef typename vec_traits<A>::scalar_type T;
    T const a0=vec_traits<A>::template read_element<0>(a);
    T const a1=vec_traits<A>::template read_element<1>(a);
    T const m2=a0*a0+a1*a1;
    return m2;
    }

namespace
sfinae
    {
    using ::boost::qvm::mag_sqr;
    }

namespace
qvm_detail
    {
    template <int D>
    struct mag_sqr_v_defined;

    template <>
    struct
    mag_sqr_v_defined<2>
        {
        static bool const value=true;
        };
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    vec_traits<A>::dim==2,
    deduce_vec<A> >::type
normalized( A const & a )
    {
    typedef typename vec_traits<A>::scalar_type T;
    T const a0=vec_traits<A>::template read_element<0>(a);
    T const a1=vec_traits<A>::template read_element<1>(a);
    T const m2=a0*a0+a1*a1;
    if( m2==scalar_traits<typename vec_traits<A>::scalar_type>::value(0) )
        BOOST_QVM_THROW_EXCEPTION(zero_magnitude_error());
    T const rm=scalar_traits<T>::value(1)/sqrt(m2);
    typedef typename deduce_vec<A>::type R;
    R r;
    write_vec_element<0>(r,a0*rm);
    write_vec_element<1>(r,a1*rm);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::normalized;
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    vec_traits<A>::dim==2,
    void>::type
normalize( A & a )
    {
    typedef typename vec_traits<A>::scalar_type T;
    T const a0=vec_traits<A>::template read_element<0>(a);
    T const a1=vec_traits<A>::template read_element<1>(a);
    T const m2=a0*a0+a1*a1;
    if( m2==scalar_traits<typename vec_traits<A>::scalar_type>::value(0) )
        BOOST_QVM_THROW_EXCEPTION(zero_magnitude_error());
    T const rm=scalar_traits<T>::value(1)/sqrt(m2);
    write_vec_element<0>(a,vec_traits<A>::template read_element<0>(a)*rm);
    write_vec_element<1>(a,vec_traits<A>::template read_element<1>(a)*rm);
    }

namespace
sfinae
    {
    using ::boost::qvm::normalize;
    }

namespace
qvm_detail
    {
    template <int D>
    struct normalize_v_defined;

    template <>
    struct
    normalize_v_defined<2>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    vec_traits<A>::dim==2 && vec_traits<B>::dim==2,
    deduce_scalar<typename vec_traits<A>::scalar_type,typename vec_traits<B>::scalar_type> >::type
dot( A const & a, B const & b )
    {
    typedef typename vec_traits<A>::scalar_type Ta;
    typedef typename vec_traits<B>::scalar_type Tb;
    typedef typename deduce_scalar<Ta,Tb>::type Tr;
    Ta const a0=vec_traits<A>::template read_element<0>(a);
    Ta const a1=vec_traits<A>::template read_element<1>(a);
    Tb const b0=vec_traits<B>::template read_element<0>(b);
    Tb const b1=vec_traits<B>::template read_element<1>(b);
    Tr const dot=a0*b0+a1*b1;
    return dot;
    }

namespace
sfinae
    {
    using ::boost::qvm::dot;
    }

namespace
qvm_detail
    {
    template <int D>
    struct dot_vv_defined;

    template <>
    struct
    dot_vv_defined<2>
        {
        static bool const value=true;
        };
    }

} }

#endif
// <<< #include <boost/qvm/gen/vec_operations2.hpp>
// #line 6 "boost/qvm/vec_operations2.hpp"
// <<< #include <boost/qvm/vec_operations2.hpp>
// #line 11 "boost/qvm/vec_operations.hpp"
// >>> #include <boost/qvm/vec_operations3.hpp>
// #line 5 "boost/qvm/vec_operations3.hpp"
// >>> #include <boost/qvm/gen/vec_operations3.hpp>
#ifndef BOOST_QVM_GEN_VEC_OPERATIONS3_HPP_INCLUDED
#define BOOST_QVM_GEN_VEC_OPERATIONS3_HPP_INCLUDED

// #line 9 "boost/qvm/gen/vec_operations3.hpp"
// #include <boost/qvm/deduce_scalar.hpp> // Expanded at line 904
// #include <boost/qvm/deduce_vec.hpp> // Expanded at line 1450
// #include <boost/qvm/error.hpp> // Expanded at line 17
// #include <boost/qvm/gen/vec_assign3.hpp> // Expanded at line 13268
// #include <boost/qvm/math.hpp> // Expanded at line 7399
// #include <boost/qvm/static_assert.hpp> // Expanded at line 1180
// #include <boost/qvm/throw_exception.hpp> // Expanded at line 1546

namespace boost { namespace qvm {

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    vec_traits<A>::dim==3 && vec_traits<B>::dim==3,
    deduce_vec2<A,B,3> >::type
operator+( A const & a, B const & b )
    {
    typedef typename deduce_vec2<A,B,3>::type R;
    BOOST_QVM_STATIC_ASSERT(vec_traits<R>::dim==3);
    R r;
    write_vec_element<0>(r,vec_traits<A>::template read_element<0>(a)+vec_traits<B>::template read_element<0>(b));
    write_vec_element<1>(r,vec_traits<A>::template read_element<1>(a)+vec_traits<B>::template read_element<1>(b));
    write_vec_element<2>(r,vec_traits<A>::template read_element<2>(a)+vec_traits<B>::template read_element<2>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator+;
    }

namespace
qvm_detail
    {
    template <int D>
    struct plus_vv_defined;

    template <>
    struct
    plus_vv_defined<3>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    vec_traits<A>::dim==3 && vec_traits<B>::dim==3,
    deduce_vec2<A,B,3> >::type
operator-( A const & a, B const & b )
    {
    typedef typename deduce_vec2<A,B,3>::type R;
    BOOST_QVM_STATIC_ASSERT(vec_traits<R>::dim==3);
    R r;
    write_vec_element<0>(r,vec_traits<A>::template read_element<0>(a)-vec_traits<B>::template read_element<0>(b));
    write_vec_element<1>(r,vec_traits<A>::template read_element<1>(a)-vec_traits<B>::template read_element<1>(b));
    write_vec_element<2>(r,vec_traits<A>::template read_element<2>(a)-vec_traits<B>::template read_element<2>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator-;
    }

namespace
qvm_detail
    {
    template <int D>
    struct minus_vv_defined;

    template <>
    struct
    minus_vv_defined<3>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    vec_traits<A>::dim==3 && vec_traits<B>::dim==3,
    A &>::type
operator+=( A & a, B const & b )
    {
    write_vec_element<0>(a,vec_traits<A>::template read_element<0>(a)+vec_traits<B>::template read_element<0>(b));
    write_vec_element<1>(a,vec_traits<A>::template read_element<1>(a)+vec_traits<B>::template read_element<1>(b));
    write_vec_element<2>(a,vec_traits<A>::template read_element<2>(a)+vec_traits<B>::template read_element<2>(b));
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator+=;
    }

namespace
qvm_detail
    {
    template <int D>
    struct plus_eq_vv_defined;

    template <>
    struct
    plus_eq_vv_defined<3>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    vec_traits<A>::dim==3 && vec_traits<B>::dim==3,
    A &>::type
operator-=( A & a, B const & b )
    {
    write_vec_element<0>(a,vec_traits<A>::template read_element<0>(a)-vec_traits<B>::template read_element<0>(b));
    write_vec_element<1>(a,vec_traits<A>::template read_element<1>(a)-vec_traits<B>::template read_element<1>(b));
    write_vec_element<2>(a,vec_traits<A>::template read_element<2>(a)-vec_traits<B>::template read_element<2>(b));
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator-=;
    }

namespace
qvm_detail
    {
    template <int D>
    struct minus_eq_vv_defined;

    template <>
    struct
    minus_eq_vv_defined<3>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    vec_traits<A>::dim==3 && is_scalar<B>::value,
    deduce_vec2<A,B,vec_traits<A>::dim> >::type
operator*( A const & a, B b )
    {
    typedef typename deduce_vec2<A,B,vec_traits<A>::dim>::type R;
    R r;
    write_vec_element<0>(r,vec_traits<A>::template read_element<0>(a)*b);
    write_vec_element<1>(r,vec_traits<A>::template read_element<1>(a)*b);
    write_vec_element<2>(r,vec_traits<A>::template read_element<2>(a)*b);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

namespace
qvm_detail
    {
    template <int D>
    struct mul_vs_defined;

    template <>
    struct
    mul_vs_defined<3>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_scalar<A>::value && vec_traits<B>::dim==3,
    deduce_vec2<A,B,vec_traits<B>::dim> >::type
operator*( A a, B const & b )
    {
    typedef typename deduce_vec2<A,B,vec_traits<B>::dim>::type R;
    R r;
    write_vec_element<0>(r,a*vec_traits<B>::template read_element<0>(b));
    write_vec_element<1>(r,a*vec_traits<B>::template read_element<1>(b));
    write_vec_element<2>(r,a*vec_traits<B>::template read_element<2>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

namespace
qvm_detail
    {
    template <int D>
    struct mul_sv_defined;

    template <>
    struct
    mul_sv_defined<3>
        {
        static bool const value=true;
        };
    }

template <class A,class  B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    vec_traits<A>::dim==3 && is_scalar<B>::value,
    A &>::type
operator*=( A & a, B b )
    {
    write_vec_element<0>(a, vec_traits<A>::template read_element<0>(a)*b);
    write_vec_element<1>(a, vec_traits<A>::template read_element<1>(a)*b);
    write_vec_element<2>(a, vec_traits<A>::template read_element<2>(a)*b);
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*=;
    }

namespace
qvm_detail
    {
    template <int D>
    struct mul_eq_vs_defined;

    template <>
    struct
    mul_eq_vs_defined<3>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    vec_traits<A>::dim==3 && is_scalar<B>::value,
    deduce_vec2<A,B,vec_traits<A>::dim> >::type
operator/( A const & a, B b )
    {
    typedef typename deduce_vec2<A,B,vec_traits<A>::dim>::type R;
    R r;
    write_vec_element<0>(r,vec_traits<A>::template read_element<0>(a)/b);
    write_vec_element<1>(r,vec_traits<A>::template read_element<1>(a)/b);
    write_vec_element<2>(r,vec_traits<A>::template read_element<2>(a)/b);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator/;
    }

namespace
qvm_detail
    {
    template <int D>
    struct div_vs_defined;

    template <>
    struct
    div_vs_defined<3>
        {
        static bool const value=true;
        };
    }

template <class A,class  B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    vec_traits<A>::dim==3 && is_scalar<B>::value,
    A &>::type
operator/=( A & a, B b )
    {
    write_vec_element<0>(a, vec_traits<A>::template read_element<0>(a)/b);
    write_vec_element<1>(a, vec_traits<A>::template read_element<1>(a)/b);
    write_vec_element<2>(a, vec_traits<A>::template read_element<2>(a)/b);
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator/=;
    }

namespace
qvm_detail
    {
    template <int D>
    struct div_eq_vs_defined;

    template <>
    struct
    div_eq_vs_defined<3>
        {
        static bool const value=true;
        };
    }

template <class R,class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_vec<A>::value &&
    vec_traits<R>::dim==3 && vec_traits<A>::dim==3,
    R>::type
convert_to( A const & a )
    {
    R r;
    write_vec_element<0>(r,vec_traits<A>::template read_element<0>(a));
    write_vec_element<1>(r,vec_traits<A>::template read_element<1>(a));
    write_vec_element<2>(r,vec_traits<A>::template read_element<2>(a));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::convert_to;
    }

namespace
qvm_detail
    {
    template <int D>
    struct convert_to_v_defined;

    template <>
    struct
    convert_to_v_defined<3>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    vec_traits<A>::dim==3 && vec_traits<B>::dim==3,
bool>::type
operator==( A const & a, B const & b )
    {
    return
        vec_traits<A>::template read_element<0>(a)==vec_traits<B>::template read_element<0>(b) &&
        vec_traits<A>::template read_element<1>(a)==vec_traits<B>::template read_element<1>(b) &&
        vec_traits<A>::template read_element<2>(a)==vec_traits<B>::template read_element<2>(b);
    }

namespace
sfinae
    {
    using ::boost::qvm::operator==;
    }

namespace
qvm_detail
    {
    template <int D>
    struct eq_vv_defined;

    template <>
    struct
    eq_vv_defined<3>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    vec_traits<A>::dim==3 && vec_traits<B>::dim==3,
bool>::type
operator!=( A const & a, B const & b )
    {
    return
        !(vec_traits<A>::template read_element<0>(a)==vec_traits<B>::template read_element<0>(b)) ||
        !(vec_traits<A>::template read_element<1>(a)==vec_traits<B>::template read_element<1>(b)) ||
        !(vec_traits<A>::template read_element<2>(a)==vec_traits<B>::template read_element<2>(b));
    }

namespace
sfinae
    {
    using ::boost::qvm::operator!=;
    }

namespace
qvm_detail
    {
    template <int D>
    struct neq_vv_defined;

    template <>
    struct
    neq_vv_defined<3>
        {
        static bool const value=true;
        };
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    vec_traits<A>::dim==3,
    deduce_vec<A> >::type
operator-( A const & a )
    {
    typedef typename deduce_vec<A>::type R;
    R r;
    write_vec_element<0>(r,-vec_traits<A>::template read_element<0>(a));
    write_vec_element<1>(r,-vec_traits<A>::template read_element<1>(a));
    write_vec_element<2>(r,-vec_traits<A>::template read_element<2>(a));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator-;
    }

namespace
qvm_detail
    {
    template <int D>
    struct minus_v_defined;

    template <>
    struct
    minus_v_defined<3>
        {
        static bool const value=true;
        };
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_vec<A>::value && vec_traits<A>::dim==3,
    typename vec_traits<A>::scalar_type>::type
mag( A const & a )
    {
    typedef typename vec_traits<A>::scalar_type T;
    T const a0=vec_traits<A>::template read_element<0>(a);
    T const a1=vec_traits<A>::template read_element<1>(a);
    T const a2=vec_traits<A>::template read_element<2>(a);
    T const m2=a0*a0+a1*a1+a2*a2;
    T const mag=sqrt(m2);
    return mag;
    }

namespace
sfinae
    {
    using ::boost::qvm::mag;
    }

namespace
qvm_detail
    {
    template <int D>
    struct mag_v_defined;

    template <>
    struct
    mag_v_defined<3>
        {
        static bool const value=true;
        };
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_vec<A>::value && vec_traits<A>::dim==3,
    typename vec_traits<A>::scalar_type>::type
mag_sqr( A const & a )
    {
    typedef typename vec_traits<A>::scalar_type T;
    T const a0=vec_traits<A>::template read_element<0>(a);
    T const a1=vec_traits<A>::template read_element<1>(a);
    T const a2=vec_traits<A>::template read_element<2>(a);
    T const m2=a0*a0+a1*a1+a2*a2;
    return m2;
    }

namespace
sfinae
    {
    using ::boost::qvm::mag_sqr;
    }

namespace
qvm_detail
    {
    template <int D>
    struct mag_sqr_v_defined;

    template <>
    struct
    mag_sqr_v_defined<3>
        {
        static bool const value=true;
        };
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    vec_traits<A>::dim==3,
    deduce_vec<A> >::type
normalized( A const & a )
    {
    typedef typename vec_traits<A>::scalar_type T;
    T const a0=vec_traits<A>::template read_element<0>(a);
    T const a1=vec_traits<A>::template read_element<1>(a);
    T const a2=vec_traits<A>::template read_element<2>(a);
    T const m2=a0*a0+a1*a1+a2*a2;
    if( m2==scalar_traits<typename vec_traits<A>::scalar_type>::value(0) )
        BOOST_QVM_THROW_EXCEPTION(zero_magnitude_error());
    T const rm=scalar_traits<T>::value(1)/sqrt(m2);
    typedef typename deduce_vec<A>::type R;
    R r;
    write_vec_element<0>(r,a0*rm);
    write_vec_element<1>(r,a1*rm);
    write_vec_element<2>(r,a2*rm);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::normalized;
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    vec_traits<A>::dim==3,
    void>::type
normalize( A & a )
    {
    typedef typename vec_traits<A>::scalar_type T;
    T const a0=vec_traits<A>::template read_element<0>(a);
    T const a1=vec_traits<A>::template read_element<1>(a);
    T const a2=vec_traits<A>::template read_element<2>(a);
    T const m2=a0*a0+a1*a1+a2*a2;
    if( m2==scalar_traits<typename vec_traits<A>::scalar_type>::value(0) )
        BOOST_QVM_THROW_EXCEPTION(zero_magnitude_error());
    T const rm=scalar_traits<T>::value(1)/sqrt(m2);
    write_vec_element<0>(a,vec_traits<A>::template read_element<0>(a)*rm);
    write_vec_element<1>(a,vec_traits<A>::template read_element<1>(a)*rm);
    write_vec_element<2>(a,vec_traits<A>::template read_element<2>(a)*rm);
    }

namespace
sfinae
    {
    using ::boost::qvm::normalize;
    }

namespace
qvm_detail
    {
    template <int D>
    struct normalize_v_defined;

    template <>
    struct
    normalize_v_defined<3>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    vec_traits<A>::dim==3 && vec_traits<B>::dim==3,
    deduce_scalar<typename vec_traits<A>::scalar_type,typename vec_traits<B>::scalar_type> >::type
dot( A const & a, B const & b )
    {
    typedef typename vec_traits<A>::scalar_type Ta;
    typedef typename vec_traits<B>::scalar_type Tb;
    typedef typename deduce_scalar<Ta,Tb>::type Tr;
    Ta const a0=vec_traits<A>::template read_element<0>(a);
    Ta const a1=vec_traits<A>::template read_element<1>(a);
    Ta const a2=vec_traits<A>::template read_element<2>(a);
    Tb const b0=vec_traits<B>::template read_element<0>(b);
    Tb const b1=vec_traits<B>::template read_element<1>(b);
    Tb const b2=vec_traits<B>::template read_element<2>(b);
    Tr const dot=a0*b0+a1*b1+a2*b2;
    return dot;
    }

namespace
sfinae
    {
    using ::boost::qvm::dot;
    }

namespace
qvm_detail
    {
    template <int D>
    struct dot_vv_defined;

    template <>
    struct
    dot_vv_defined<3>
        {
        static bool const value=true;
        };
    }

} }

#endif
// <<< #include <boost/qvm/gen/vec_operations3.hpp>
// #line 6 "boost/qvm/vec_operations3.hpp"
// <<< #include <boost/qvm/vec_operations3.hpp>
// #line 12 "boost/qvm/vec_operations.hpp"
// >>> #include <boost/qvm/vec_operations4.hpp>
// #line 5 "boost/qvm/vec_operations4.hpp"
// >>> #include <boost/qvm/gen/vec_operations4.hpp>
#ifndef BOOST_QVM_GEN_VEC_OPERATIONS4_HPP_INCLUDED
#define BOOST_QVM_GEN_VEC_OPERATIONS4_HPP_INCLUDED

// #line 9 "boost/qvm/gen/vec_operations4.hpp"
// #include <boost/qvm/deduce_scalar.hpp> // Expanded at line 904
// #include <boost/qvm/deduce_vec.hpp> // Expanded at line 1450
// #include <boost/qvm/error.hpp> // Expanded at line 17
// #include <boost/qvm/gen/vec_assign4.hpp> // Expanded at line 13317
// #include <boost/qvm/math.hpp> // Expanded at line 7399
// #include <boost/qvm/static_assert.hpp> // Expanded at line 1180
// #include <boost/qvm/throw_exception.hpp> // Expanded at line 1546

namespace boost { namespace qvm {

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    vec_traits<A>::dim==4 && vec_traits<B>::dim==4,
    deduce_vec2<A,B,4> >::type
operator+( A const & a, B const & b )
    {
    typedef typename deduce_vec2<A,B,4>::type R;
    BOOST_QVM_STATIC_ASSERT(vec_traits<R>::dim==4);
    R r;
    write_vec_element<0>(r,vec_traits<A>::template read_element<0>(a)+vec_traits<B>::template read_element<0>(b));
    write_vec_element<1>(r,vec_traits<A>::template read_element<1>(a)+vec_traits<B>::template read_element<1>(b));
    write_vec_element<2>(r,vec_traits<A>::template read_element<2>(a)+vec_traits<B>::template read_element<2>(b));
    write_vec_element<3>(r,vec_traits<A>::template read_element<3>(a)+vec_traits<B>::template read_element<3>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator+;
    }

namespace
qvm_detail
    {
    template <int D>
    struct plus_vv_defined;

    template <>
    struct
    plus_vv_defined<4>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    vec_traits<A>::dim==4 && vec_traits<B>::dim==4,
    deduce_vec2<A,B,4> >::type
operator-( A const & a, B const & b )
    {
    typedef typename deduce_vec2<A,B,4>::type R;
    BOOST_QVM_STATIC_ASSERT(vec_traits<R>::dim==4);
    R r;
    write_vec_element<0>(r,vec_traits<A>::template read_element<0>(a)-vec_traits<B>::template read_element<0>(b));
    write_vec_element<1>(r,vec_traits<A>::template read_element<1>(a)-vec_traits<B>::template read_element<1>(b));
    write_vec_element<2>(r,vec_traits<A>::template read_element<2>(a)-vec_traits<B>::template read_element<2>(b));
    write_vec_element<3>(r,vec_traits<A>::template read_element<3>(a)-vec_traits<B>::template read_element<3>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator-;
    }

namespace
qvm_detail
    {
    template <int D>
    struct minus_vv_defined;

    template <>
    struct
    minus_vv_defined<4>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    vec_traits<A>::dim==4 && vec_traits<B>::dim==4,
    A &>::type
operator+=( A & a, B const & b )
    {
    write_vec_element<0>(a,vec_traits<A>::template read_element<0>(a)+vec_traits<B>::template read_element<0>(b));
    write_vec_element<1>(a,vec_traits<A>::template read_element<1>(a)+vec_traits<B>::template read_element<1>(b));
    write_vec_element<2>(a,vec_traits<A>::template read_element<2>(a)+vec_traits<B>::template read_element<2>(b));
    write_vec_element<3>(a,vec_traits<A>::template read_element<3>(a)+vec_traits<B>::template read_element<3>(b));
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator+=;
    }

namespace
qvm_detail
    {
    template <int D>
    struct plus_eq_vv_defined;

    template <>
    struct
    plus_eq_vv_defined<4>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    vec_traits<A>::dim==4 && vec_traits<B>::dim==4,
    A &>::type
operator-=( A & a, B const & b )
    {
    write_vec_element<0>(a,vec_traits<A>::template read_element<0>(a)-vec_traits<B>::template read_element<0>(b));
    write_vec_element<1>(a,vec_traits<A>::template read_element<1>(a)-vec_traits<B>::template read_element<1>(b));
    write_vec_element<2>(a,vec_traits<A>::template read_element<2>(a)-vec_traits<B>::template read_element<2>(b));
    write_vec_element<3>(a,vec_traits<A>::template read_element<3>(a)-vec_traits<B>::template read_element<3>(b));
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator-=;
    }

namespace
qvm_detail
    {
    template <int D>
    struct minus_eq_vv_defined;

    template <>
    struct
    minus_eq_vv_defined<4>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    vec_traits<A>::dim==4 && is_scalar<B>::value,
    deduce_vec2<A,B,vec_traits<A>::dim> >::type
operator*( A const & a, B b )
    {
    typedef typename deduce_vec2<A,B,vec_traits<A>::dim>::type R;
    R r;
    write_vec_element<0>(r,vec_traits<A>::template read_element<0>(a)*b);
    write_vec_element<1>(r,vec_traits<A>::template read_element<1>(a)*b);
    write_vec_element<2>(r,vec_traits<A>::template read_element<2>(a)*b);
    write_vec_element<3>(r,vec_traits<A>::template read_element<3>(a)*b);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

namespace
qvm_detail
    {
    template <int D>
    struct mul_vs_defined;

    template <>
    struct
    mul_vs_defined<4>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_scalar<A>::value && vec_traits<B>::dim==4,
    deduce_vec2<A,B,vec_traits<B>::dim> >::type
operator*( A a, B const & b )
    {
    typedef typename deduce_vec2<A,B,vec_traits<B>::dim>::type R;
    R r;
    write_vec_element<0>(r,a*vec_traits<B>::template read_element<0>(b));
    write_vec_element<1>(r,a*vec_traits<B>::template read_element<1>(b));
    write_vec_element<2>(r,a*vec_traits<B>::template read_element<2>(b));
    write_vec_element<3>(r,a*vec_traits<B>::template read_element<3>(b));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

namespace
qvm_detail
    {
    template <int D>
    struct mul_sv_defined;

    template <>
    struct
    mul_sv_defined<4>
        {
        static bool const value=true;
        };
    }

template <class A,class  B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    vec_traits<A>::dim==4 && is_scalar<B>::value,
    A &>::type
operator*=( A & a, B b )
    {
    write_vec_element<0>(a, vec_traits<A>::template read_element<0>(a)*b);
    write_vec_element<1>(a, vec_traits<A>::template read_element<1>(a)*b);
    write_vec_element<2>(a, vec_traits<A>::template read_element<2>(a)*b);
    write_vec_element<3>(a, vec_traits<A>::template read_element<3>(a)*b);
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*=;
    }

namespace
qvm_detail
    {
    template <int D>
    struct mul_eq_vs_defined;

    template <>
    struct
    mul_eq_vs_defined<4>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    vec_traits<A>::dim==4 && is_scalar<B>::value,
    deduce_vec2<A,B,vec_traits<A>::dim> >::type
operator/( A const & a, B b )
    {
    typedef typename deduce_vec2<A,B,vec_traits<A>::dim>::type R;
    R r;
    write_vec_element<0>(r,vec_traits<A>::template read_element<0>(a)/b);
    write_vec_element<1>(r,vec_traits<A>::template read_element<1>(a)/b);
    write_vec_element<2>(r,vec_traits<A>::template read_element<2>(a)/b);
    write_vec_element<3>(r,vec_traits<A>::template read_element<3>(a)/b);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator/;
    }

namespace
qvm_detail
    {
    template <int D>
    struct div_vs_defined;

    template <>
    struct
    div_vs_defined<4>
        {
        static bool const value=true;
        };
    }

template <class A,class  B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    vec_traits<A>::dim==4 && is_scalar<B>::value,
    A &>::type
operator/=( A & a, B b )
    {
    write_vec_element<0>(a, vec_traits<A>::template read_element<0>(a)/b);
    write_vec_element<1>(a, vec_traits<A>::template read_element<1>(a)/b);
    write_vec_element<2>(a, vec_traits<A>::template read_element<2>(a)/b);
    write_vec_element<3>(a, vec_traits<A>::template read_element<3>(a)/b);
    return a;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator/=;
    }

namespace
qvm_detail
    {
    template <int D>
    struct div_eq_vs_defined;

    template <>
    struct
    div_eq_vs_defined<4>
        {
        static bool const value=true;
        };
    }

template <class R,class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_vec<A>::value &&
    vec_traits<R>::dim==4 && vec_traits<A>::dim==4,
    R>::type
convert_to( A const & a )
    {
    R r;
    write_vec_element<0>(r,vec_traits<A>::template read_element<0>(a));
    write_vec_element<1>(r,vec_traits<A>::template read_element<1>(a));
    write_vec_element<2>(r,vec_traits<A>::template read_element<2>(a));
    write_vec_element<3>(r,vec_traits<A>::template read_element<3>(a));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::convert_to;
    }

namespace
qvm_detail
    {
    template <int D>
    struct convert_to_v_defined;

    template <>
    struct
    convert_to_v_defined<4>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    vec_traits<A>::dim==4 && vec_traits<B>::dim==4,
bool>::type
operator==( A const & a, B const & b )
    {
    return
        vec_traits<A>::template read_element<0>(a)==vec_traits<B>::template read_element<0>(b) &&
        vec_traits<A>::template read_element<1>(a)==vec_traits<B>::template read_element<1>(b) &&
        vec_traits<A>::template read_element<2>(a)==vec_traits<B>::template read_element<2>(b) &&
        vec_traits<A>::template read_element<3>(a)==vec_traits<B>::template read_element<3>(b);
    }

namespace
sfinae
    {
    using ::boost::qvm::operator==;
    }

namespace
qvm_detail
    {
    template <int D>
    struct eq_vv_defined;

    template <>
    struct
    eq_vv_defined<4>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    vec_traits<A>::dim==4 && vec_traits<B>::dim==4,
bool>::type
operator!=( A const & a, B const & b )
    {
    return
        !(vec_traits<A>::template read_element<0>(a)==vec_traits<B>::template read_element<0>(b)) ||
        !(vec_traits<A>::template read_element<1>(a)==vec_traits<B>::template read_element<1>(b)) ||
        !(vec_traits<A>::template read_element<2>(a)==vec_traits<B>::template read_element<2>(b)) ||
        !(vec_traits<A>::template read_element<3>(a)==vec_traits<B>::template read_element<3>(b));
    }

namespace
sfinae
    {
    using ::boost::qvm::operator!=;
    }

namespace
qvm_detail
    {
    template <int D>
    struct neq_vv_defined;

    template <>
    struct
    neq_vv_defined<4>
        {
        static bool const value=true;
        };
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    vec_traits<A>::dim==4,
    deduce_vec<A> >::type
operator-( A const & a )
    {
    typedef typename deduce_vec<A>::type R;
    R r;
    write_vec_element<0>(r,-vec_traits<A>::template read_element<0>(a));
    write_vec_element<1>(r,-vec_traits<A>::template read_element<1>(a));
    write_vec_element<2>(r,-vec_traits<A>::template read_element<2>(a));
    write_vec_element<3>(r,-vec_traits<A>::template read_element<3>(a));
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator-;
    }

namespace
qvm_detail
    {
    template <int D>
    struct minus_v_defined;

    template <>
    struct
    minus_v_defined<4>
        {
        static bool const value=true;
        };
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_vec<A>::value && vec_traits<A>::dim==4,
    typename vec_traits<A>::scalar_type>::type
mag( A const & a )
    {
    typedef typename vec_traits<A>::scalar_type T;
    T const a0=vec_traits<A>::template read_element<0>(a);
    T const a1=vec_traits<A>::template read_element<1>(a);
    T const a2=vec_traits<A>::template read_element<2>(a);
    T const a3=vec_traits<A>::template read_element<3>(a);
    T const m2=a0*a0+a1*a1+a2*a2+a3*a3;
    T const mag=sqrt(m2);
    return mag;
    }

namespace
sfinae
    {
    using ::boost::qvm::mag;
    }

namespace
qvm_detail
    {
    template <int D>
    struct mag_v_defined;

    template <>
    struct
    mag_v_defined<4>
        {
        static bool const value=true;
        };
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_vec<A>::value && vec_traits<A>::dim==4,
    typename vec_traits<A>::scalar_type>::type
mag_sqr( A const & a )
    {
    typedef typename vec_traits<A>::scalar_type T;
    T const a0=vec_traits<A>::template read_element<0>(a);
    T const a1=vec_traits<A>::template read_element<1>(a);
    T const a2=vec_traits<A>::template read_element<2>(a);
    T const a3=vec_traits<A>::template read_element<3>(a);
    T const m2=a0*a0+a1*a1+a2*a2+a3*a3;
    return m2;
    }

namespace
sfinae
    {
    using ::boost::qvm::mag_sqr;
    }

namespace
qvm_detail
    {
    template <int D>
    struct mag_sqr_v_defined;

    template <>
    struct
    mag_sqr_v_defined<4>
        {
        static bool const value=true;
        };
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    vec_traits<A>::dim==4,
    deduce_vec<A> >::type
normalized( A const & a )
    {
    typedef typename vec_traits<A>::scalar_type T;
    T const a0=vec_traits<A>::template read_element<0>(a);
    T const a1=vec_traits<A>::template read_element<1>(a);
    T const a2=vec_traits<A>::template read_element<2>(a);
    T const a3=vec_traits<A>::template read_element<3>(a);
    T const m2=a0*a0+a1*a1+a2*a2+a3*a3;
    if( m2==scalar_traits<typename vec_traits<A>::scalar_type>::value(0) )
        BOOST_QVM_THROW_EXCEPTION(zero_magnitude_error());
    T const rm=scalar_traits<T>::value(1)/sqrt(m2);
    typedef typename deduce_vec<A>::type R;
    R r;
    write_vec_element<0>(r,a0*rm);
    write_vec_element<1>(r,a1*rm);
    write_vec_element<2>(r,a2*rm);
    write_vec_element<3>(r,a3*rm);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::normalized;
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    vec_traits<A>::dim==4,
    void>::type
normalize( A & a )
    {
    typedef typename vec_traits<A>::scalar_type T;
    T const a0=vec_traits<A>::template read_element<0>(a);
    T const a1=vec_traits<A>::template read_element<1>(a);
    T const a2=vec_traits<A>::template read_element<2>(a);
    T const a3=vec_traits<A>::template read_element<3>(a);
    T const m2=a0*a0+a1*a1+a2*a2+a3*a3;
    if( m2==scalar_traits<typename vec_traits<A>::scalar_type>::value(0) )
        BOOST_QVM_THROW_EXCEPTION(zero_magnitude_error());
    T const rm=scalar_traits<T>::value(1)/sqrt(m2);
    write_vec_element<0>(a,vec_traits<A>::template read_element<0>(a)*rm);
    write_vec_element<1>(a,vec_traits<A>::template read_element<1>(a)*rm);
    write_vec_element<2>(a,vec_traits<A>::template read_element<2>(a)*rm);
    write_vec_element<3>(a,vec_traits<A>::template read_element<3>(a)*rm);
    }

namespace
sfinae
    {
    using ::boost::qvm::normalize;
    }

namespace
qvm_detail
    {
    template <int D>
    struct normalize_v_defined;

    template <>
    struct
    normalize_v_defined<4>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    vec_traits<A>::dim==4 && vec_traits<B>::dim==4,
    deduce_scalar<typename vec_traits<A>::scalar_type,typename vec_traits<B>::scalar_type> >::type
dot( A const & a, B const & b )
    {
    typedef typename vec_traits<A>::scalar_type Ta;
    typedef typename vec_traits<B>::scalar_type Tb;
    typedef typename deduce_scalar<Ta,Tb>::type Tr;
    Ta const a0=vec_traits<A>::template read_element<0>(a);
    Ta const a1=vec_traits<A>::template read_element<1>(a);
    Ta const a2=vec_traits<A>::template read_element<2>(a);
    Ta const a3=vec_traits<A>::template read_element<3>(a);
    Tb const b0=vec_traits<B>::template read_element<0>(b);
    Tb const b1=vec_traits<B>::template read_element<1>(b);
    Tb const b2=vec_traits<B>::template read_element<2>(b);
    Tb const b3=vec_traits<B>::template read_element<3>(b);
    Tr const dot=a0*b0+a1*b1+a2*b2+a3*b3;
    return dot;
    }

namespace
sfinae
    {
    using ::boost::qvm::dot;
    }

namespace
qvm_detail
    {
    template <int D>
    struct dot_vv_defined;

    template <>
    struct
    dot_vv_defined<4>
        {
        static bool const value=true;
        };
    }

} }

#endif
// <<< #include <boost/qvm/gen/vec_operations4.hpp>
// #line 6 "boost/qvm/vec_operations4.hpp"
// <<< #include <boost/qvm/vec_operations4.hpp>
// #line 13 "boost/qvm/vec_operations.hpp"
// #include <boost/qvm/assert.hpp> // Expanded at line 887
// #include <boost/qvm/scalar_traits.hpp> // Expanded at line 909
// #include <boost/qvm/to_string.hpp> // Expanded at line 49

namespace boost { namespace qvm {

namespace
qvm_detail
    {
    BOOST_QVM_INLINE_CRITICAL
    void const *
    get_valid_ptr_vec_operations()
        {
        static int const obj=0;
        return &obj;
        }
    }

////////////////////////////////////////////////


namespace
qvm_detail
    {
    template <int D>
    struct
    to_string_v_defined
        {
        static bool const value=false;
        };

    template <int I,int DimMinusOne>
    struct
    to_string_vector_elements
        {
        template <class A>
        static
        std::string
        f( A const & a )
            {
            using namespace qvm_to_string_detail;
            return to_string(vec_traits<A>::template read_element<I>(a))+','+to_string_vector_elements<I+1,DimMinusOne>::f(a);
            }
        };

    template <int DimMinusOne>
    struct
    to_string_vector_elements<DimMinusOne,DimMinusOne>
        {
        template <class A>
        static
        std::string
        f( A const & a )
            {
            using namespace qvm_to_string_detail;
            return to_string(vec_traits<A>::template read_element<DimMinusOne>(a));
            }
        };
    }

template <class A>
inline
typename enable_if_c<
    is_vec<A>::value  &&
    !qvm_detail::to_string_v_defined<vec_traits<A>::dim>::value,
    std::string>::type
to_string( A const & a )
    {
    return '('+qvm_detail::to_string_vector_elements<0,vec_traits<A>::dim-1>::f(a)+')';
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int D>
    struct
    convert_to_v_defined
        {
        static bool const value=false;
        };
    }

template <class R,class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
typename enable_if_c<
    is_vec<R>::value && is_vec<A>::value &&
    vec_traits<R>::dim==vec_traits<A>::dim &&
    !qvm_detail::convert_to_v_defined<vec_traits<R>::dim>::value,
    R>::type
convert_to( A const & a )
    {
    R r; assign(r,a);
    return r;
    }

////////////////////////////////////////////////

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_vec<A>::value && is_vec<B>::value &&
    vec_traits<A>::dim==3 && vec_traits<B>::dim==3,
    deduce_vec2<A,B,3> >::type
cross( A const & a, B const & b )
    {
    typedef typename deduce_vec2<A,B,3>::type R;
    R r;
    write_vec_element<0>(r,
        vec_traits<A>::template read_element<1>(a)*vec_traits<B>::template read_element<2>(b)-
        vec_traits<A>::template read_element<2>(a)*vec_traits<B>::template read_element<1>(b));
    write_vec_element<1>(r,
        vec_traits<A>::template read_element<2>(a)*vec_traits<B>::template read_element<0>(b)-
        vec_traits<A>::template read_element<0>(a)*vec_traits<B>::template read_element<2>(b));
    write_vec_element<2>(r,
        vec_traits<A>::template read_element<0>(a)*vec_traits<B>::template read_element<1>(b)-
        vec_traits<A>::template read_element<1>(a)*vec_traits<B>::template read_element<0>(b));
    return r;
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_vec<A>::value && is_vec<B>::value &&
    vec_traits<A>::dim==2 && vec_traits<B>::dim==2,
    deduce_scalar<typename vec_traits<A>::scalar_type,typename vec_traits<B>::scalar_type> >::type
cross( A const & a, B const & b )
    {
    typedef typename deduce_scalar<typename vec_traits<A>::scalar_type,typename vec_traits<B>::scalar_type>::type R;
    R const r =
        vec_traits<A>::template read_element<0>(a)*vec_traits<B>::template read_element<1>(b)-
        vec_traits<A>::template read_element<1>(a)*vec_traits<B>::template read_element<0>(b);
    return r;
    }

////////////////////////////////////////////////

template <class A,class B,class Cmp>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_vec<A>::value && is_vec<B>::value &&
    vec_traits<A>::dim==vec_traits<B>::dim,
    bool>::type
cmp( A const & a, B const & b, Cmp f )
    {
    for( int i=0; i!=vec_traits<A>::dim; ++i )
        if( !f(
            vec_traits<A>::read_element_idx(i,a),
            vec_traits<B>::read_element_idx(i,b)) )
            return false;
    return true;
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <class T,int Dim>
    class
    zero_vec_
        {
        zero_vec_( zero_vec_ const & );
        zero_vec_ & operator=( zero_vec_ const & );
        ~zero_vec_();

        public:

        template <class R
#if __cplusplus >= 201103L
            , class = typename enable_if<is_vec<R> >::type
#endif
        >
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        operator R() const
            {
            R r;
            assign(r,*this);
            return r;
            }
        };
    }

template <class V>
struct vec_traits;

template <class T,int Dim>
struct
vec_traits< qvm_detail::zero_vec_<T,Dim> >
    {
    typedef qvm_detail::zero_vec_<T,Dim> this_vector;
    typedef T scalar_type;
    static int const dim=Dim;

    template <int I>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_vector const & )
        {
        BOOST_QVM_STATIC_ASSERT(I>=0);
        BOOST_QVM_STATIC_ASSERT(I<Dim);
        return scalar_traits<scalar_type>::value(0);
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int i, this_vector const & )
        {
        BOOST_QVM_ASSERT(i>=0); (void)i;
        BOOST_QVM_ASSERT(i<Dim);
        return scalar_traits<scalar_type>::value(0);
        }
    };

template <class T,int Dim,int D>
struct
deduce_vec<qvm_detail::zero_vec_<T,Dim>,D>
    {
    typedef vec<T,D> type;
    };

template <class T,int Dim>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
qvm_detail::zero_vec_<T,Dim> const &
zero_vec()
    {
    return *(qvm_detail::zero_vec_<T,Dim> const *)qvm_detail::get_valid_ptr_vec_operations();
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_vec<A>::value,
    void>::type
set_zero( A & a )
    {
    assign(a,zero_vec<typename vec_traits<A>::scalar_type,vec_traits<A>::dim>());
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <class OriginalType,class Scalar>
    class
    vector_scalar_cast_
        {
        vector_scalar_cast_( vector_scalar_cast_ const & );
        vector_scalar_cast_ & operator=( vector_scalar_cast_ const & );
        ~vector_scalar_cast_();

        public:

        template <class T>
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        vector_scalar_cast_ &
        operator=( T const & x )
            {
            assign(*this,x);
            return *this;
            }

        template <class R
#if __cplusplus >= 201103L
            , class = typename enable_if<is_vec<R> >::type
#endif
        >
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        operator R() const
            {
            R r;
            assign(r,*this);
            return r;
            }
        };

    template <bool> struct scalar_cast_vector_filter { };
    template <> struct scalar_cast_vector_filter<true> { typedef int type; };
    }

template <class OriginalType,class Scalar>
struct
vec_traits< qvm_detail::vector_scalar_cast_<OriginalType,Scalar> >
    {
    typedef Scalar scalar_type;
    typedef qvm_detail::vector_scalar_cast_<OriginalType,Scalar> this_vector;
    static int const dim=vec_traits<OriginalType>::dim;

    template <int I>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_vector const & x )
        {
        BOOST_QVM_STATIC_ASSERT(I>=0);
        BOOST_QVM_STATIC_ASSERT(I<dim);
        return scalar_type(vec_traits<OriginalType>::template read_element<I>(reinterpret_cast<OriginalType const &>(x)));
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int i, this_vector const & x )
        {
        BOOST_QVM_ASSERT(i>=0);
        BOOST_QVM_ASSERT(i<dim);
        return scalar_type(vec_traits<OriginalType>::read_element_idx(i,reinterpret_cast<OriginalType const &>(x)));
        }
    };

template <class OriginalType,class Scalar,int D>
struct
deduce_vec<qvm_detail::vector_scalar_cast_<OriginalType,Scalar>,D>
    {
    typedef vec<Scalar,D> type;
    };

template <class Scalar,class T>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
qvm_detail::vector_scalar_cast_<T,Scalar> const &
scalar_cast( T const & x, typename qvm_detail::scalar_cast_vector_filter<is_vec<T>::value>::type=0 )
    {
    return reinterpret_cast<qvm_detail::vector_scalar_cast_<T,Scalar> const &>(x);
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int D>
    struct
    div_eq_vs_defined
        {
        static bool const value=false;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_vec<A>::value && is_scalar<B>::value &&
    !qvm_detail::div_eq_vs_defined<vec_traits<A>::dim>::value,
    A &>::type
operator/=( A & a, B b )
    {
    for( int i=0; i!=vec_traits<A>::dim; ++i )
        write_vec_element_idx(i,a,vec_traits<A>::read_element_idx(i,a)/b);
    return a;
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int D>
    struct
    div_vs_defined
        {
        static bool const value=false;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_vec<A>::value && is_scalar<B>::value &&
    !qvm_detail::div_vs_defined<vec_traits<A>::dim>::value,
    deduce_vec2<A,B,vec_traits<A>::dim> >::type
operator/( A const & a, B b )
    {
    typedef typename deduce_vec2<A,B,vec_traits<A>::dim>::type R;
    R r;
    for( int i=0; i!=vec_traits<A>::dim; ++i )
        write_vec_element_idx(i,r,vec_traits<A>::read_element_idx(i,a)/b);
    return r;
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int D>
    struct
    dot_vv_defined
        {
        static bool const value=false;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_vec<A>::value && is_vec<B>::value &&
    vec_traits<A>::dim==vec_traits<B>::dim &&
    !qvm_detail::dot_vv_defined<vec_traits<A>::dim>::value,
    deduce_scalar<typename vec_traits<A>::scalar_type,typename vec_traits<B>::scalar_type> >::type
dot( A const & a, B const & b )
    {
    typedef typename deduce_scalar<typename vec_traits<A>::scalar_type,typename vec_traits<B>::scalar_type>::type T;
    T m(scalar_traits<T>::value(0));
    for( int i=0; i!=vec_traits<A>::dim; ++i )
        m+=vec_traits<A>::read_element_idx(i,a)*vec_traits<B>::read_element_idx(i,b);
    return m;
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int D>
    struct
    eq_vv_defined
        {
        static bool const value=false;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_vec<A>::value && is_vec<B>::value &&
    vec_traits<A>::dim==vec_traits<B>::dim &&
    !qvm_detail::eq_vv_defined<vec_traits<A>::dim>::value,
    bool>::type
operator==( A const & a, B const & b )
    {
    for( int i=0; i!=vec_traits<A>::dim; ++i )
        if( vec_traits<A>::read_element_idx(i,a)!=vec_traits<B>::read_element_idx(i,b) )
            return false;
    return true;
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int D>
    struct
    mag_sqr_v_defined
        {
        static bool const value=false;
        };
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_vec<A>::value &&
    !qvm_detail::mag_sqr_v_defined<vec_traits<A>::dim>::value,
    typename vec_traits<A>::scalar_type>::type
mag_sqr( A const & a )
    {
    typedef typename vec_traits<A>::scalar_type T;
    T m(scalar_traits<T>::value(0));
    for( int i=0; i!=vec_traits<A>::dim; ++i )
        {
        T x=vec_traits<A>::read_element_idx(i,a);
        m+=x*x;
        }
    return m;
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int D>
    struct
    mag_v_defined
        {
        static bool const value=false;
        };
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_vec<A>::value &&
    !qvm_detail::mag_v_defined<vec_traits<A>::dim>::value,
    typename vec_traits<A>::scalar_type>::type
mag( A const & a )
    {
    typedef typename vec_traits<A>::scalar_type T;
    T m(scalar_traits<T>::value(0));
    for( int i=0; i!=vec_traits<A>::dim; ++i )
        {
        T x=vec_traits<A>::read_element_idx(i,a);
        m+=x*x;
        }
    return sqrt(m);
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int D>
    struct
    minus_eq_vv_defined
        {
        static bool const value=false;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_vec<A>::value && is_vec<B>::value &&
    vec_traits<A>::dim==vec_traits<B>::dim &&
    !qvm_detail::minus_eq_vv_defined<vec_traits<A>::dim>::value,
    A &>::type
operator-=( A & a, B const & b )
    {
    for( int i=0; i!=vec_traits<A>::dim; ++i )
        write_vec_element_idx(i,a,vec_traits<A>::read_element_idx(i,a)-vec_traits<B>::read_element_idx(i,b));
    return a;
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int D>
    struct
    minus_v_defined
        {
        static bool const value=false;
        };
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_vec<A>::value &&
    !qvm_detail::minus_v_defined<vec_traits<A>::dim>::value,
    deduce_vec<A> >::type
operator-( A const & a )
    {
    typedef typename deduce_vec<A>::type R;
    R r;
    for( int i=0; i!=vec_traits<A>::dim; ++i )
        write_vec_element_idx(i,r,-vec_traits<A>::read_element_idx(i,a));
    return r;
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int D>
    struct
    minus_vv_defined
        {
        static bool const value=false;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_vec<A>::value && is_vec<B>::value &&
    vec_traits<A>::dim==vec_traits<B>::dim &&
    !qvm_detail::minus_vv_defined<vec_traits<A>::dim>::value,
    deduce_vec2<A,B,vec_traits<A>::dim> >::type
operator-( A const & a, B const & b )
    {
    typedef typename deduce_vec2<A,B,vec_traits<A>::dim>::type R;
    R r;
    for( int i=0; i!=vec_traits<A>::dim; ++i )
        write_vec_element_idx(i,r,vec_traits<A>::read_element_idx(i,a)-vec_traits<B>::read_element_idx(i,b));
    return r;
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int D>
    struct
    mul_eq_vs_defined
        {
        static bool const value=false;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_vec<A>::value && is_scalar<B>::value &&
    !qvm_detail::mul_eq_vs_defined<vec_traits<A>::dim>::value,
    A &>::type
operator*=( A & a, B b )
    {
    for( int i=0; i!=vec_traits<A>::dim; ++i )
        write_vec_element_idx(i,a,vec_traits<A>::read_element_idx(i,a)*b);
    return a;
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int D>
    struct
    mul_vs_defined
        {
        static bool const value=false;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_vec<A>::value && is_scalar<B>::value &&
    !qvm_detail::mul_vs_defined<vec_traits<A>::dim>::value,
    deduce_vec2<A,B,vec_traits<A>::dim> >::type
operator*( A const & a, B b )
    {
    typedef typename deduce_vec2<A,B,vec_traits<A>::dim>::type R;
    R r;
    for( int i=0; i!=vec_traits<A>::dim; ++i )
        write_vec_element_idx(i,r,vec_traits<A>::read_element_idx(i,a)*b);
    return r;
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int D>
    struct
    mul_sv_defined
        {
        static bool const value=false;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_scalar<A>::value && is_vec<B>::value &&
    !qvm_detail::mul_sv_defined<vec_traits<B>::dim>::value,
    deduce_vec2<A,B,vec_traits<B>::dim> >::type
operator*( A a, B const & b )
    {
    typedef typename deduce_vec2<A,B,vec_traits<B>::dim>::type R;
    R r;
    for( int i=0; i!=vec_traits<B>::dim; ++i )
        write_vec_element_idx(i,r,a*vec_traits<B>::read_element_idx(i,b));
    return r;
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int D>
    struct
    neq_vv_defined
        {
        static bool const value=false;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_vec<A>::value && is_vec<B>::value &&
    vec_traits<A>::dim==vec_traits<B>::dim &&
    !qvm_detail::neq_vv_defined<vec_traits<A>::dim>::value,
    bool>::type
operator!=( A const & a, B const & b )
    {
    for( int i=0; i!=vec_traits<A>::dim; ++i )
        if( vec_traits<A>::read_element_idx(i,a)!=vec_traits<B>::read_element_idx(i,b) )
            return true;
    return false;
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int D>
    struct
    normalize_v_defined
        {
        static bool const value=false;
        };
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_vec<A>::value &&
    !qvm_detail::normalize_v_defined<vec_traits<A>::dim>::value,
    deduce_vec<A> >::type
normalized( A const & a )
    {
    typedef typename vec_traits<A>::scalar_type T;
    T m(scalar_traits<T>::value(0));
    for( int i=0; i!=vec_traits<A>::dim; ++i )
        {
        T x=vec_traits<A>::read_element_idx(i,a);
        m+=x*x;
        }
    if( m==scalar_traits<T>::value(0) )
        BOOST_QVM_THROW_EXCEPTION(zero_magnitude_error());
    T rm=scalar_traits<T>::value(1)/sqrt(m);
    typedef typename deduce_vec<A>::type R;
    R r;
    for( int i=0; i!=vec_traits<A>::dim; ++i )
        write_vec_element_idx(i,r,vec_traits<A>::read_element_idx(i,a)*rm);
    return r;
    }

template <class A>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_vec<A>::value &&
    !qvm_detail::normalize_v_defined<vec_traits<A>::dim>::value,
    void>::type
normalize( A & a )
    {
    typedef typename vec_traits<A>::scalar_type T;
    T m(scalar_traits<T>::value(0));
    for( int i=0; i!=vec_traits<A>::dim; ++i )
        {
        T x=vec_traits<A>::read_element_idx(i,a);
        m+=x*x;
        }
    if( m==scalar_traits<T>::value(0) )
        BOOST_QVM_THROW_EXCEPTION(zero_magnitude_error());
    T rm=scalar_traits<T>::value(1)/sqrt(m);
    for( int i=0; i!=vec_traits<A>::dim; ++i )
        write_vec_element_idx(i,a,vec_traits<A>::read_element_idx(i,a)*rm);
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int D>
    struct
    plus_eq_vv_defined
        {
        static bool const value=false;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename enable_if_c<
    is_vec<A>::value && is_vec<B>::value &&
    vec_traits<A>::dim==vec_traits<B>::dim &&
    !qvm_detail::plus_eq_vv_defined<vec_traits<A>::dim>::value,
    A &>::type
operator+=( A & a, B const & b )
    {
    for( int i=0; i!=vec_traits<A>::dim; ++i )
        write_vec_element_idx(i,a,vec_traits<A>::read_element_idx(i,a)+vec_traits<B>::read_element_idx(i,b));
    return a;
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int D>
    struct
    plus_vv_defined
        {
        static bool const value=false;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_vec<A>::value && is_vec<B>::value &&
    vec_traits<A>::dim==vec_traits<B>::dim &&
    !qvm_detail::plus_vv_defined<vec_traits<A>::dim>::value,
    deduce_vec2<A,B,vec_traits<A>::dim> >::type
operator+( A const & a, B const & b )
    {
    typedef typename deduce_vec2<A,B,vec_traits<A>::dim>::type R;
    R r;
    for( int i=0; i!=vec_traits<A>::dim; ++i )
        write_vec_element_idx(i,r,vec_traits<A>::read_element_idx(i,a)+vec_traits<B>::read_element_idx(i,b));
    return r;
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <class T>
    class
    vref_
        {
        vref_( vref_ const & );
        vref_ & operator=( vref_ const & );
        ~vref_();

        public:

        template <class R>
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        vref_ &
        operator=( R const & x )
            {
            assign(*this,x);
            return *this;
            }

        template <class R
#if __cplusplus >= 201103L
            , class = typename enable_if<is_vec<R> >::type
#endif
        >
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        operator R() const
            {
            R r;
            assign(r,*this);
            return r;
            }
        };

    template <class V,bool WriteElementRef=vec_write_element_ref<V>::value>
    struct vref_write_traits;

    template <class V>
    struct
    vref_write_traits<V,true>
        {
        typedef typename vec_traits<V>::scalar_type scalar_type;
        typedef qvm_detail::vref_<V> this_vector;
        static int const dim=vec_traits<V>::dim;

        template <int I>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        scalar_type &
        write_element( this_vector & x )
            {
            BOOST_QVM_STATIC_ASSERT(I>=0);
            BOOST_QVM_STATIC_ASSERT(I<dim);
            return vec_traits<V>::template write_element<I>(reinterpret_cast<V &>(x));
            }

        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        scalar_type &
        write_element_idx( int i, this_vector & x )
            {
            BOOST_QVM_ASSERT(i>=0);
            BOOST_QVM_ASSERT(i<dim);
            return vec_traits<V>::write_element_idx(i,reinterpret_cast<V &>(x));
            }
        };

    template <class V>
    struct
    vref_write_traits<V,false>
        {
        typedef typename vec_traits<V>::scalar_type scalar_type;
        typedef qvm_detail::vref_<V> this_vector;
        static int const dim=vec_traits<V>::dim;

        template <int I>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        void
        write_element( this_vector & x, scalar_type s )
            {
            BOOST_QVM_STATIC_ASSERT(I>=0);
            BOOST_QVM_STATIC_ASSERT(I<dim);
            vec_traits<V>::template write_element<I>(reinterpret_cast<V &>(x), s);
            }

        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        void
        write_element_idx( int i, this_vector & x, scalar_type s )
            {
            BOOST_QVM_ASSERT(i>=0);
            BOOST_QVM_ASSERT(i<dim);
            vec_traits<V>::write_element_idx(i,reinterpret_cast<V &>(x), s);
            }
        };
    }

template <class V>
struct
vec_traits< qvm_detail::vref_<V> >:
    qvm_detail::vref_write_traits<V>
    {
    typedef typename vec_traits<V>::scalar_type scalar_type;
    typedef qvm_detail::vref_<V> this_vector;
    static int const dim=vec_traits<V>::dim;

    template <int I>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_vector const & x )
        {
        BOOST_QVM_STATIC_ASSERT(I>=0);
        BOOST_QVM_STATIC_ASSERT(I<dim);
        return vec_traits<V>::template read_element<I>(reinterpret_cast<V const &>(x));
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int i, this_vector const & x )
        {
        BOOST_QVM_ASSERT(i>=0);
        BOOST_QVM_ASSERT(i<dim);
        return vec_traits<V>::read_element_idx(i,reinterpret_cast<V const &>(x));
        }
    };

template <class V,int D>
struct
deduce_vec<qvm_detail::vref_<V>,D>
    {
    typedef vec<typename vec_traits<V>::scalar_type,D> type;
    };

template <class V>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
typename enable_if_c<
    is_vec<V>::value,
    qvm_detail::vref_<V> const &>::type
vref( V const & a )
    {
    return reinterpret_cast<qvm_detail::vref_<V> const &>(a);
    }

template <class V>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
typename enable_if_c<
    is_vec<V>::value,
    qvm_detail::vref_<V> &>::type
vref( V & a )
    {
    return reinterpret_cast<qvm_detail::vref_<V> &>(a);
    }

////////////////////////////////////////////////

namespace
sfinae
    {
    using ::boost::qvm::to_string;
    using ::boost::qvm::assign;
    using ::boost::qvm::convert_to;
    using ::boost::qvm::cross;
    using ::boost::qvm::cmp;
    using ::boost::qvm::set_zero;
    using ::boost::qvm::scalar_cast;
    using ::boost::qvm::operator/=;
    using ::boost::qvm::operator/;
    using ::boost::qvm::dot;
    using ::boost::qvm::operator==;
    using ::boost::qvm::mag_sqr;
    using ::boost::qvm::mag;
    using ::boost::qvm::operator-=;
    using ::boost::qvm::operator-;
    using ::boost::qvm::operator*=;
    using ::boost::qvm::operator*;
    using ::boost::qvm::operator!=;
    using ::boost::qvm::normalized;
    using ::boost::qvm::normalize;
    using ::boost::qvm::operator+=;
    using ::boost::qvm::operator+;
    using ::boost::qvm::vref;
    }

} }

#endif
// <<< #include <boost/qvm/vec_operations.hpp>
// #line 20 "boost/qvm/lite.hpp"
// >>> #include <boost/qvm/vec_access.hpp>
#ifndef BOOST_QVM_VEC_ACCESS_HPP_INCLUDED
#define BOOST_QVM_VEC_ACCESS_HPP_INCLUDED

// #line 8 "boost/qvm/vec_access.hpp"
// #include <boost/qvm/vec_traits.hpp> // Expanded at line 1010
// #include <boost/qvm/config.hpp> // Expanded at line 109
// #include <boost/qvm/static_assert.hpp> // Expanded at line 1180
// #include <boost/qvm/enable_if.hpp> // Expanded at line 177

namespace boost { namespace qvm {

template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value,typename vec_traits<V>::scalar_type>::type X( V const & a ) { BOOST_QVM_STATIC_ASSERT(0<vec_traits<V>::dim); return vec_traits<V>::template read_element<0>(a); }
template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value,typename vec_traits<V>::scalar_type>::type Y( V const & a ) { BOOST_QVM_STATIC_ASSERT(1<vec_traits<V>::dim); return vec_traits<V>::template read_element<1>(a); }
template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value,typename vec_traits<V>::scalar_type>::type Z( V const & a ) { BOOST_QVM_STATIC_ASSERT(2<vec_traits<V>::dim); return vec_traits<V>::template read_element<2>(a); }
template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value,typename vec_traits<V>::scalar_type>::type W( V const & a ) { BOOST_QVM_STATIC_ASSERT(3<vec_traits<V>::dim); return vec_traits<V>::template read_element<3>(a); }

template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value,typename vec_traits<V>::scalar_type>::type A0( V const & a ) { BOOST_QVM_STATIC_ASSERT(0<vec_traits<V>::dim); return vec_traits<V>::template read_element<0>(a); }
template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value,typename vec_traits<V>::scalar_type>::type A1( V const & a ) { BOOST_QVM_STATIC_ASSERT(1<vec_traits<V>::dim); return vec_traits<V>::template read_element<1>(a); }
template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value,typename vec_traits<V>::scalar_type>::type A2( V const & a ) { BOOST_QVM_STATIC_ASSERT(2<vec_traits<V>::dim); return vec_traits<V>::template read_element<2>(a); }
template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value,typename vec_traits<V>::scalar_type>::type A3( V const & a ) { BOOST_QVM_STATIC_ASSERT(3<vec_traits<V>::dim); return vec_traits<V>::template read_element<3>(a); }
template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value,typename vec_traits<V>::scalar_type>::type A4( V const & a ) { BOOST_QVM_STATIC_ASSERT(4<vec_traits<V>::dim); return vec_traits<V>::template read_element<4>(a); }
template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value,typename vec_traits<V>::scalar_type>::type A5( V const & a ) { BOOST_QVM_STATIC_ASSERT(5<vec_traits<V>::dim); return vec_traits<V>::template read_element<5>(a); }
template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value,typename vec_traits<V>::scalar_type>::type A6( V const & a ) { BOOST_QVM_STATIC_ASSERT(6<vec_traits<V>::dim); return vec_traits<V>::template read_element<6>(a); }
template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value,typename vec_traits<V>::scalar_type>::type A7( V const & a ) { BOOST_QVM_STATIC_ASSERT(7<vec_traits<V>::dim); return vec_traits<V>::template read_element<7>(a); }
template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value,typename vec_traits<V>::scalar_type>::type A8( V const & a ) { BOOST_QVM_STATIC_ASSERT(8<vec_traits<V>::dim); return vec_traits<V>::template read_element<8>(a); }
template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value,typename vec_traits<V>::scalar_type>::type A9( V const & a ) { BOOST_QVM_STATIC_ASSERT(9<vec_traits<V>::dim); return vec_traits<V>::template read_element<9>(a); }

namespace
qvm_detail
    {
    template <int I,class V>
    struct
    v_element_access
        {
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        void
        operator=( typename vec_traits<V>::scalar_type s )
            {
            vec_traits<V>::template write_element<I>(*reinterpret_cast<V *>(this), s);
            }

        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        operator typename vec_traits<V>::scalar_type() const
            {
            return vec_traits<V>::template read_element<I>(*reinterpret_cast<V const *>(this));
            }
        };
    }

template <int I,class V>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
typename enable_if_c<
    is_vec<V>::value,
    typename vec_traits<V>::scalar_type>::type
A( V const & a )
    {
    BOOST_QVM_STATIC_ASSERT(I>=0);
    BOOST_QVM_STATIC_ASSERT(I<vec_traits<V>::dim);
    return vec_traits<V>::template read_element<I>(a);
    }

template <int I,class V>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
typename enable_if_c<
    is_vec<V>::value && vec_write_element_ref<V>::value,
    typename vec_traits<V>::scalar_type &>::type
A( V & a )
    {
    BOOST_QVM_STATIC_ASSERT(I>=0);
    BOOST_QVM_STATIC_ASSERT(I<vec_traits<V>::dim);
    return vec_traits<V>::template write_element<I>(a);
    }

template <int I,class V>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
typename enable_if_c<
    is_vec<V>::value && !vec_write_element_ref<V>::value,
    qvm_detail::v_element_access<I,V> &>::type
A( V & a )
    {
    BOOST_QVM_STATIC_ASSERT(I>=0);
    BOOST_QVM_STATIC_ASSERT(I<vec_traits<V>::dim);
    return *reinterpret_cast<qvm_detail::v_element_access<I,V> *>(&a);
    }

template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value && vec_write_element_ref<V>::value,typename vec_traits<V>::scalar_type &>::type X( V & a ) { BOOST_QVM_STATIC_ASSERT(0<vec_traits<V>::dim); return vec_traits<V>::template write_element<0>(a); }
template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value && vec_write_element_ref<V>::value,typename vec_traits<V>::scalar_type &>::type Y( V & a ) { BOOST_QVM_STATIC_ASSERT(1<vec_traits<V>::dim); return vec_traits<V>::template write_element<1>(a); }
template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value && vec_write_element_ref<V>::value,typename vec_traits<V>::scalar_type &>::type Z( V & a ) { BOOST_QVM_STATIC_ASSERT(2<vec_traits<V>::dim); return vec_traits<V>::template write_element<2>(a); }
template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value && vec_write_element_ref<V>::value,typename vec_traits<V>::scalar_type &>::type W( V & a ) { BOOST_QVM_STATIC_ASSERT(3<vec_traits<V>::dim); return vec_traits<V>::template write_element<3>(a); }

template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value && vec_write_element_ref<V>::value,typename vec_traits<V>::scalar_type &>::type A0( V & a ) {  BOOST_QVM_STATIC_ASSERT(0<vec_traits<V>::dim); return vec_traits<V>::template write_element<0>(a); }
template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value && vec_write_element_ref<V>::value,typename vec_traits<V>::scalar_type &>::type A1( V & a ) {  BOOST_QVM_STATIC_ASSERT(1<vec_traits<V>::dim); return vec_traits<V>::template write_element<1>(a); }
template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value && vec_write_element_ref<V>::value,typename vec_traits<V>::scalar_type &>::type A2( V & a ) {  BOOST_QVM_STATIC_ASSERT(2<vec_traits<V>::dim); return vec_traits<V>::template write_element<2>(a); }
template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value && vec_write_element_ref<V>::value,typename vec_traits<V>::scalar_type &>::type A3( V & a ) {  BOOST_QVM_STATIC_ASSERT(3<vec_traits<V>::dim); return vec_traits<V>::template write_element<3>(a); }
template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value && vec_write_element_ref<V>::value,typename vec_traits<V>::scalar_type &>::type A4( V & a ) {  BOOST_QVM_STATIC_ASSERT(4<vec_traits<V>::dim); return vec_traits<V>::template write_element<4>(a); }
template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value && vec_write_element_ref<V>::value,typename vec_traits<V>::scalar_type &>::type A5( V & a ) {  BOOST_QVM_STATIC_ASSERT(5<vec_traits<V>::dim); return vec_traits<V>::template write_element<5>(a); }
template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value && vec_write_element_ref<V>::value,typename vec_traits<V>::scalar_type &>::type A6( V & a ) {  BOOST_QVM_STATIC_ASSERT(6<vec_traits<V>::dim); return vec_traits<V>::template write_element<6>(a); }
template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value && vec_write_element_ref<V>::value,typename vec_traits<V>::scalar_type &>::type A7( V & a ) {  BOOST_QVM_STATIC_ASSERT(7<vec_traits<V>::dim); return vec_traits<V>::template write_element<7>(a); }
template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value && vec_write_element_ref<V>::value,typename vec_traits<V>::scalar_type &>::type A8( V & a ) {  BOOST_QVM_STATIC_ASSERT(8<vec_traits<V>::dim); return vec_traits<V>::template write_element<8>(a); }
template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value && vec_write_element_ref<V>::value,typename vec_traits<V>::scalar_type &>::type A9( V & a ) {  BOOST_QVM_STATIC_ASSERT(9<vec_traits<V>::dim); return vec_traits<V>::template write_element<9>(a); }

template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value && !vec_write_element_ref<V>::value,qvm_detail::v_element_access<0,V> &>::type X( V & a ) { BOOST_QVM_STATIC_ASSERT(0<vec_traits<V>::dim); return *reinterpret_cast<qvm_detail::v_element_access<0, V> *>(&a); }
template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value && !vec_write_element_ref<V>::value,qvm_detail::v_element_access<1,V> &>::type Y( V & a ) { BOOST_QVM_STATIC_ASSERT(1<vec_traits<V>::dim); return *reinterpret_cast<qvm_detail::v_element_access<1, V> *>(&a); }
template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value && !vec_write_element_ref<V>::value,qvm_detail::v_element_access<2,V> &>::type Z( V & a ) { BOOST_QVM_STATIC_ASSERT(2<vec_traits<V>::dim); return *reinterpret_cast<qvm_detail::v_element_access<2, V> *>(&a); }
template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value && !vec_write_element_ref<V>::value,qvm_detail::v_element_access<3,V> &>::type W( V & a ) { BOOST_QVM_STATIC_ASSERT(3<vec_traits<V>::dim); return *reinterpret_cast<qvm_detail::v_element_access<3, V> *>(&a); }

template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value && !vec_write_element_ref<V>::value,qvm_detail::v_element_access<0,V> &>::type A0( V & a ) {  BOOST_QVM_STATIC_ASSERT(0<vec_traits<V>::dim); return *reinterpret_cast<qvm_detail::v_element_access<0, V> *>(&a); }
template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value && !vec_write_element_ref<V>::value,qvm_detail::v_element_access<1,V> &>::type A1( V & a ) {  BOOST_QVM_STATIC_ASSERT(1<vec_traits<V>::dim); return *reinterpret_cast<qvm_detail::v_element_access<1, V> *>(&a); }
template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value && !vec_write_element_ref<V>::value,qvm_detail::v_element_access<2,V> &>::type A2( V & a ) {  BOOST_QVM_STATIC_ASSERT(2<vec_traits<V>::dim); return *reinterpret_cast<qvm_detail::v_element_access<2, V> *>(&a); }
template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value && !vec_write_element_ref<V>::value,qvm_detail::v_element_access<3,V> &>::type A3( V & a ) {  BOOST_QVM_STATIC_ASSERT(3<vec_traits<V>::dim); return *reinterpret_cast<qvm_detail::v_element_access<3, V> *>(&a); }
template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value && !vec_write_element_ref<V>::value,qvm_detail::v_element_access<4,V> &>::type A4( V & a ) {  BOOST_QVM_STATIC_ASSERT(4<vec_traits<V>::dim); return *reinterpret_cast<qvm_detail::v_element_access<4, V> *>(&a); }
template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value && !vec_write_element_ref<V>::value,qvm_detail::v_element_access<5,V> &>::type A5( V & a ) {  BOOST_QVM_STATIC_ASSERT(5<vec_traits<V>::dim); return *reinterpret_cast<qvm_detail::v_element_access<5, V> *>(&a); }
template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value && !vec_write_element_ref<V>::value,qvm_detail::v_element_access<6,V> &>::type A6( V & a ) {  BOOST_QVM_STATIC_ASSERT(6<vec_traits<V>::dim); return *reinterpret_cast<qvm_detail::v_element_access<6, V> *>(&a); }
template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value && !vec_write_element_ref<V>::value,qvm_detail::v_element_access<7,V> &>::type A7( V & a ) {  BOOST_QVM_STATIC_ASSERT(7<vec_traits<V>::dim); return *reinterpret_cast<qvm_detail::v_element_access<7, V> *>(&a); }
template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value && !vec_write_element_ref<V>::value,qvm_detail::v_element_access<8,V> &>::type A8( V & a ) {  BOOST_QVM_STATIC_ASSERT(8<vec_traits<V>::dim); return *reinterpret_cast<qvm_detail::v_element_access<8, V> *>(&a); }
template <class V> BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL typename enable_if_c<is_vec<V>::value && !vec_write_element_ref<V>::value,qvm_detail::v_element_access<9,V> &>::type A9( V & a ) {  BOOST_QVM_STATIC_ASSERT(9<vec_traits<V>::dim); return *reinterpret_cast<qvm_detail::v_element_access<9, V> *>(&a); }

} }

#endif
// <<< #include <boost/qvm/vec_access.hpp>
// #line 21 "boost/qvm/lite.hpp"
// >>> #include <boost/qvm/vec_traits_defaults.hpp>
#ifndef BOOST_QVM_VEC_TRAITS_DEFAULTS_HPP_INCLUDED
#define BOOST_QVM_VEC_TRAITS_DEFAULTS_HPP_INCLUDED

// #line 8 "boost/qvm/vec_traits_defaults.hpp"
// #include <boost/qvm/config.hpp> // Expanded at line 109
// #include <boost/qvm/assert.hpp> // Expanded at line 887

namespace boost { namespace qvm {

template <class>
struct vec_traits;

namespace
qvm_detail
    {
    template <int I,int N>
    struct
    vector_w
        {
        template <class A>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        typename vec_traits<A>::scalar_type &
        write_element_idx( int i, A & a )
            {
            return I==i?
                vec_traits<A>::template write_element<I>(a) :
                vector_w<I+1,N>::write_element_idx(i,a);
            }
        };

    template <int N>
    struct
    vector_w<N,N>
        {
        template <class A>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        typename vec_traits<A>::scalar_type &
        write_element_idx( int, A & a )
            {
            BOOST_QVM_ASSERT(0);
            return vec_traits<A>::template write_element<0>(a);
            }
        };
    }

template <class VecType,class ScalarType,int Dim>
struct
vec_traits_defaults
    {
    typedef VecType vec_type;
    typedef ScalarType scalar_type;
    static int const dim=Dim;

    template <int I>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( vec_type const & x )
        {
        return vec_traits<vec_type>::template write_element<I>(const_cast<vec_type &>(x));
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int i, vec_type const & x )
        {
        return vec_traits<vec_type>::write_element_idx(i,const_cast<vec_type &>(x));
        }

    protected:

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
    scalar_type &
    write_element_idx( int i, vec_type & m )
        {
        return qvm_detail::vector_w<0,vec_traits<vec_type>::dim>::write_element_idx(i,m);
        }
    };

} }

#endif
// <<< #include <boost/qvm/vec_traits_defaults.hpp>
// #line 22 "boost/qvm/lite.hpp"
// >>> #include <boost/qvm/vec_traits_array.hpp>
#ifndef BOOST_QVM_VEC_TRAITS_ARRAY_HPP_INCLUDED
#define BOOST_QVM_VEC_TRAITS_ARRAY_HPP_INCLUDED

// #line 8 "boost/qvm/vec_traits_array.hpp"
// #include <boost/qvm/config.hpp> // Expanded at line 109
// #include <boost/qvm/deduce_vec.hpp> // Expanded at line 1450
// #include <boost/qvm/assert.hpp> // Expanded at line 887

#if __cplusplus > 199711L

#include <array>

namespace boost { namespace qvm {

template <class T,std::size_t M,std::size_t N>
struct
vec_traits<std::array<std::array<T,M>,N> >
    {
    static int const dim=0;
    typedef void scalar_type;
    };

template <class T,std::size_t Dim>
struct
vec_traits<std::array<T, Dim> >
    {
    typedef std::array<T, Dim> this_vector;
    typedef T scalar_type;
    static int const dim=int(Dim);

    template <int I>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_vector const & x )
        {
        BOOST_QVM_STATIC_ASSERT(I>=0);
        BOOST_QVM_STATIC_ASSERT(I<int(Dim));
        return x[I];
        }

    template <int I>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type &
    write_element( this_vector & x )
        {
        BOOST_QVM_STATIC_ASSERT(I>=0);
        BOOST_QVM_STATIC_ASSERT(I<int(Dim));
        return x[I];
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int i, this_vector const & x )
        {
        BOOST_QVM_ASSERT(i>=0);
        BOOST_QVM_ASSERT(i<int(Dim));
        return x[i];
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type &
    write_element_idx( int i, this_vector & x )
        {
        BOOST_QVM_ASSERT(i>=0);
        BOOST_QVM_ASSERT(i<int(Dim));
        return x[i];
        }
    };

template <class T,std::size_t Dim>
struct
vec_traits<std::array<T, Dim> const>
    {
    typedef std::array<T, Dim> const this_vector;
    typedef T scalar_type;
    static int const dim=int(Dim);

    template <int I>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_vector & x )
        {
        BOOST_QVM_STATIC_ASSERT(I>=0);
        BOOST_QVM_STATIC_ASSERT(I<int(Dim));
        return x[I];
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int i, this_vector & x )
        {
        BOOST_QVM_ASSERT(i>=0);
        BOOST_QVM_ASSERT(i<int(Dim));
        return x[i];
        }
    };

template <class T,std::size_t Dim,int D>
struct
deduce_vec<std::array<T,Dim>,D>
    {
    typedef vec<T,D> type;
    };

template <class T,std::size_t Dim,int D>
struct
deduce_vec<std::array<T,Dim> const,D>
    {
    typedef vec<T,D> type;
    };

template <class T1,class T2,std::size_t Dim,int D>
struct
deduce_vec2<std::array<T1,Dim>,std::array<T2,Dim>,D>
    {
    typedef vec<typename deduce_scalar<T1,T2>::type,D> type;
    };

template <class T1,class T2,std::size_t Dim,int D>
struct
deduce_vec2<std::array<T1,Dim> const,std::array<T2,Dim>,D>
    {
    typedef vec<typename deduce_scalar<T1,T2>::type,D> type;
    };

template <class T1,class T2,std::size_t Dim,int D>
struct
deduce_vec2<std::array<T1,Dim>,std::array<T2,Dim> const,D>
    {
    typedef vec<typename deduce_scalar<T1,T2>::type,D> type;
    };

template <class T1,class T2,std::size_t Dim,int D>
struct
deduce_vec2<std::array<T1,Dim> const,std::array<T2,Dim> const,D>
    {
    typedef vec<typename deduce_scalar<T1,T2>::type,D> type;
    };

} }

#endif

namespace boost { namespace qvm {

template <class T,int M,int N>
struct
vec_traits<T[M][N]>
    {
    static int const dim=0;
    typedef void scalar_type;
    };

template <class T,int Dim>
struct
vec_traits<T[Dim]>
    {
    typedef T this_vector[Dim];
    typedef T scalar_type;
    static int const dim=Dim;

    template <int I>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_vector const & x )
        {
        BOOST_QVM_STATIC_ASSERT(I>=0);
        BOOST_QVM_STATIC_ASSERT(I<Dim);
        return x[I];
        }

    template <int I>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type &
    write_element( this_vector & x )
        {
        BOOST_QVM_STATIC_ASSERT(I>=0);
        BOOST_QVM_STATIC_ASSERT(I<Dim);
        return x[I];
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int i, this_vector const & x )
        {
        BOOST_QVM_ASSERT(i>=0);
        BOOST_QVM_ASSERT(i<Dim);
        return x[i];
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type &
    write_element_idx( int i, this_vector & x )
        {
        BOOST_QVM_ASSERT(i>=0);
        BOOST_QVM_ASSERT(i<Dim);
        return x[i];
        }
    };

template <class T,int Dim,int D>
struct
deduce_vec<T[Dim],D>
    {
    typedef vec<T,D> type;
    };

template <class T,int Dim,int D>
struct
deduce_vec<T const[Dim],D>
    {
    typedef vec<T,D> type;
    };

template <class T1,class T2,int Dim,int D>
struct
deduce_vec2<T1[Dim],T2[Dim],D>
    {
    typedef vec<typename deduce_scalar<T1,T2>::type,D> type;
    };

template <int Dim,class T>
T (&ptr_vref( T * ptr ))[Dim]
    {
    return *reinterpret_cast<T (*)[Dim]>(ptr);
    }

} }

#endif
// <<< #include <boost/qvm/vec_traits_array.hpp>
// #line 23 "boost/qvm/lite.hpp"
// >>> #include <boost/qvm/vec_traits_gnuc.hpp>
#ifndef BOOST_QVM_VEC_TRAITS_GNUC_HPP_INCLUDED
#define BOOST_QVM_VEC_TRAITS_GNUC_HPP_INCLUDED

// #line 8 "boost/qvm/vec_traits_gnuc.hpp"
#if defined(__GNUC__) && defined(__SSE2__)

// #include <boost/qvm/config.hpp> // Expanded at line 109
// #include <boost/qvm/assert.hpp> // Expanded at line 887
// #include <boost/qvm/static_assert.hpp> // Expanded at line 1180

namespace boost { namespace qvm {

namespace
qvm_detail
    {
    template <class V, class T, int D>
    struct
    vec_traits_gnuc_impl
        {
        typedef T scalar_type;
        static int const dim=D;

        template <int I>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        scalar_type
        read_element( V const & x )
            {
            BOOST_QVM_STATIC_ASSERT(I>=0);
            BOOST_QVM_STATIC_ASSERT(I<dim);
            return x[I];
            }

        template <int I>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        void
        write_element( V & x, scalar_type s )
            {
            BOOST_QVM_STATIC_ASSERT(I>=0);
            BOOST_QVM_STATIC_ASSERT(I<dim);
            x[I] = s;
            }

        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        scalar_type
        read_element_idx( int i, V const & x )
            {
            BOOST_QVM_ASSERT(i>=0);
            BOOST_QVM_ASSERT(i<dim);
            return x[i];
            }

        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        void
        write_element_idx( int i, V & x, scalar_type s )
            {
            BOOST_QVM_ASSERT(i>=0);
            BOOST_QVM_ASSERT(i<dim);
            x[i] = s;
            }
        };
    }

template <class> struct vec_traits;
template <class> struct is_vec;

#define BOOST_QVM_GNUC_VEC_TYPE(T,D)\
    template <>\
    struct\
    vec_traits<T __attribute__((vector_size(sizeof(T)*D)))>:\
        qvm_detail::vec_traits_gnuc_impl<T __attribute__((vector_size(sizeof(T)*D))),T,D>\
        {\
        };\
    template <>\
    struct\
    is_vec<T __attribute__((vector_size(sizeof(T)*D)))>\
        {\
        enum { value = true };\
        };

BOOST_QVM_GNUC_VEC_TYPE(float,2)
BOOST_QVM_GNUC_VEC_TYPE(float,4)
BOOST_QVM_GNUC_VEC_TYPE(double,2)
BOOST_QVM_GNUC_VEC_TYPE(double,4)

#undef BOOST_QVM_GNUC_VEC_TYPE

} }

#endif

#endif
// <<< #include <boost/qvm/vec_traits_gnuc.hpp>
// #line 24 "boost/qvm/lite.hpp"
// >>> #include <boost/qvm/vec.hpp>
#ifndef BOOST_QVM_VEC_HPP_INCLUDED
#define BOOST_QVM_VEC_HPP_INCLUDED

// #line 8 "boost/qvm/vec.hpp"
// #include <boost/qvm/detail/vec_assign.hpp> // Expanded at line 13215
// #include <boost/qvm/assert.hpp> // Expanded at line 887
// #include <boost/qvm/static_assert.hpp> // Expanded at line 1180

namespace boost { namespace qvm {

template <class T,int D>
struct
vec
    {
    T a[D];
    template <class R
#if __cplusplus >= 201103L
        , class = typename enable_if<is_vec<R> >::type
#endif
    >
    operator R() const
        {
        R r;
        assign(r,*this);
        return r;
        }
    };

template <class V>
struct vec_traits;

template <class T,int Dim>
struct
vec_traits< vec<T,Dim> >
    {
    typedef vec<T,Dim> this_vector;
    typedef T scalar_type;
    static int const dim=Dim;

    template <int I>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_vector const & x )
        {
        BOOST_QVM_STATIC_ASSERT(I>=0);
        BOOST_QVM_STATIC_ASSERT(I<dim);
        return x.a[I];
        }

    template <int I>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type &
    write_element( this_vector & x )
        {
        BOOST_QVM_STATIC_ASSERT(I>=0);
        BOOST_QVM_STATIC_ASSERT(I<dim);
        return x.a[I];
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int i, this_vector const & x )
        {
        BOOST_QVM_ASSERT(i>=0);
        BOOST_QVM_ASSERT(i<dim);
        return x.a[i];
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type &
    write_element_idx( int i, this_vector & x )
        {
        BOOST_QVM_ASSERT(i>=0);
        BOOST_QVM_ASSERT(i<dim);
        return x.a[i];
        }
    };

} }

#endif
// <<< #include <boost/qvm/vec.hpp>
// #line 25 "boost/qvm/lite.hpp"
// >>> #include <boost/qvm/vec_mat_operations.hpp>
#ifndef BOOST_QVM_VEC_MAT_OPERATIONS_HPP_INCLUDED
#define BOOST_QVM_VEC_MAT_OPERATIONS_HPP_INCLUDED

// #line 8 "boost/qvm/vec_mat_operations.hpp"
// >>> #include <boost/qvm/vec_mat_operations2.hpp>
// #line 5 "boost/qvm/vec_mat_operations2.hpp"
// >>> #include <boost/qvm/gen/vec_mat_operations2.hpp>
#ifndef BOOST_QVM_GEN_VEC_MAT_OPERATIONS2_HPP_INCLUDED
#define BOOST_QVM_GEN_VEC_MAT_OPERATIONS2_HPP_INCLUDED

// #line 9 "boost/qvm/gen/vec_mat_operations2.hpp"
// #include <boost/qvm/config.hpp> // Expanded at line 109
// #include <boost/qvm/deduce_vec.hpp> // Expanded at line 1450
// #include <boost/qvm/enable_if.hpp> // Expanded at line 177
// #include <boost/qvm/mat_traits.hpp> // Expanded at line 303
// #include <boost/qvm/vec_traits.hpp> // Expanded at line 1010

namespace boost { namespace qvm {

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==2 && mat_traits<A>::cols==2 &&
    vec_traits<B>::dim==2,
    deduce_vec2<A,B,2> >::type
operator*( A const & a, B const & b )
    {
    typedef typename mat_traits<A>::scalar_type Ta;
    typedef typename vec_traits<B>::scalar_type Tb;
    Ta const a00 = mat_traits<A>::template read_element<0,0>(a);
    Ta const a01 = mat_traits<A>::template read_element<0,1>(a);
    Ta const a10 = mat_traits<A>::template read_element<1,0>(a);
    Ta const a11 = mat_traits<A>::template read_element<1,1>(a);
    Tb const b0 = vec_traits<B>::template read_element<0>(b);
    Tb const b1 = vec_traits<B>::template read_element<1>(b);
    typedef typename deduce_vec2<A,B,2>::type R;
    BOOST_QVM_STATIC_ASSERT(vec_traits<R>::dim==2);
    R r;
    write_vec_element<0>(r,a00*b0+a01*b1);
    write_vec_element<1>(r,a10*b0+a11*b1);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct mul_mv_defined;

    template <>
    struct
    mul_mv_defined<2,2>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<B>::rows==2 && mat_traits<B>::cols==2 &&
    vec_traits<A>::dim==2,
    deduce_vec2<A,B,2> >::type
operator*( A const & a, B const & b )
    {
    typedef typename vec_traits<A>::scalar_type Ta;
    typedef typename mat_traits<B>::scalar_type Tb;
    Ta const a0 = vec_traits<A>::template read_element<0>(a);
    Ta const a1 = vec_traits<A>::template read_element<1>(a);
    Tb const b00 = mat_traits<B>::template read_element<0,0>(b);
    Tb const b01 = mat_traits<B>::template read_element<0,1>(b);
    Tb const b10 = mat_traits<B>::template read_element<1,0>(b);
    Tb const b11 = mat_traits<B>::template read_element<1,1>(b);
    typedef typename deduce_vec2<A,B,2>::type R;
    BOOST_QVM_STATIC_ASSERT(vec_traits<R>::dim==2);
    R r;
    write_vec_element<0>(r,a0*b00+a1*b10);
    write_vec_element<1>(r,a0*b01+a1*b11);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct mul_vm_defined;

    template <>
    struct
    mul_vm_defined<2,2>
        {
        static bool const value=true;
        };
    }

} }

#endif
// <<< #include <boost/qvm/gen/vec_mat_operations2.hpp>
// #line 6 "boost/qvm/vec_mat_operations2.hpp"
// <<< #include <boost/qvm/vec_mat_operations2.hpp>
// #line 9 "boost/qvm/vec_mat_operations.hpp"
// >>> #include <boost/qvm/vec_mat_operations3.hpp>
// #line 5 "boost/qvm/vec_mat_operations3.hpp"
// >>> #include <boost/qvm/gen/vec_mat_operations3.hpp>
#ifndef BOOST_QVM_GEN_VEC_MAT_OPERATIONS3_HPP_INCLUDED
#define BOOST_QVM_GEN_VEC_MAT_OPERATIONS3_HPP_INCLUDED

// #line 9 "boost/qvm/gen/vec_mat_operations3.hpp"
// #include <boost/qvm/config.hpp> // Expanded at line 109
// #include <boost/qvm/deduce_vec.hpp> // Expanded at line 1450
// #include <boost/qvm/enable_if.hpp> // Expanded at line 177
// #include <boost/qvm/mat_traits.hpp> // Expanded at line 303
// #include <boost/qvm/vec_traits.hpp> // Expanded at line 1010

namespace boost { namespace qvm {

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==3 && mat_traits<A>::cols==3 &&
    vec_traits<B>::dim==3,
    deduce_vec2<A,B,3> >::type
operator*( A const & a, B const & b )
    {
    typedef typename mat_traits<A>::scalar_type Ta;
    typedef typename vec_traits<B>::scalar_type Tb;
    Ta const a00 = mat_traits<A>::template read_element<0,0>(a);
    Ta const a01 = mat_traits<A>::template read_element<0,1>(a);
    Ta const a02 = mat_traits<A>::template read_element<0,2>(a);
    Ta const a10 = mat_traits<A>::template read_element<1,0>(a);
    Ta const a11 = mat_traits<A>::template read_element<1,1>(a);
    Ta const a12 = mat_traits<A>::template read_element<1,2>(a);
    Ta const a20 = mat_traits<A>::template read_element<2,0>(a);
    Ta const a21 = mat_traits<A>::template read_element<2,1>(a);
    Ta const a22 = mat_traits<A>::template read_element<2,2>(a);
    Tb const b0 = vec_traits<B>::template read_element<0>(b);
    Tb const b1 = vec_traits<B>::template read_element<1>(b);
    Tb const b2 = vec_traits<B>::template read_element<2>(b);
    typedef typename deduce_vec2<A,B,3>::type R;
    BOOST_QVM_STATIC_ASSERT(vec_traits<R>::dim==3);
    R r;
    write_vec_element<0>(r,a00*b0+a01*b1+a02*b2);
    write_vec_element<1>(r,a10*b0+a11*b1+a12*b2);
    write_vec_element<2>(r,a20*b0+a21*b1+a22*b2);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct mul_mv_defined;

    template <>
    struct
    mul_mv_defined<3,3>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<B>::rows==3 && mat_traits<B>::cols==3 &&
    vec_traits<A>::dim==3,
    deduce_vec2<A,B,3> >::type
operator*( A const & a, B const & b )
    {
    typedef typename vec_traits<A>::scalar_type Ta;
    typedef typename mat_traits<B>::scalar_type Tb;
    Ta const a0 = vec_traits<A>::template read_element<0>(a);
    Ta const a1 = vec_traits<A>::template read_element<1>(a);
    Ta const a2 = vec_traits<A>::template read_element<2>(a);
    Tb const b00 = mat_traits<B>::template read_element<0,0>(b);
    Tb const b01 = mat_traits<B>::template read_element<0,1>(b);
    Tb const b02 = mat_traits<B>::template read_element<0,2>(b);
    Tb const b10 = mat_traits<B>::template read_element<1,0>(b);
    Tb const b11 = mat_traits<B>::template read_element<1,1>(b);
    Tb const b12 = mat_traits<B>::template read_element<1,2>(b);
    Tb const b20 = mat_traits<B>::template read_element<2,0>(b);
    Tb const b21 = mat_traits<B>::template read_element<2,1>(b);
    Tb const b22 = mat_traits<B>::template read_element<2,2>(b);
    typedef typename deduce_vec2<A,B,3>::type R;
    BOOST_QVM_STATIC_ASSERT(vec_traits<R>::dim==3);
    R r;
    write_vec_element<0>(r,a0*b00+a1*b10+a2*b20);
    write_vec_element<1>(r,a0*b01+a1*b11+a2*b21);
    write_vec_element<2>(r,a0*b02+a1*b12+a2*b22);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct mul_vm_defined;

    template <>
    struct
    mul_vm_defined<3,3>
        {
        static bool const value=true;
        };
    }

} }

#endif
// <<< #include <boost/qvm/gen/vec_mat_operations3.hpp>
// #line 6 "boost/qvm/vec_mat_operations3.hpp"
// <<< #include <boost/qvm/vec_mat_operations3.hpp>
// #line 10 "boost/qvm/vec_mat_operations.hpp"
// >>> #include <boost/qvm/vec_mat_operations4.hpp>
// #line 5 "boost/qvm/vec_mat_operations4.hpp"
// >>> #include <boost/qvm/gen/vec_mat_operations4.hpp>
#ifndef BOOST_QVM_GEN_VEC_MAT_OPERATIONS4_HPP_INCLUDED
#define BOOST_QVM_GEN_VEC_MAT_OPERATIONS4_HPP_INCLUDED

// #line 9 "boost/qvm/gen/vec_mat_operations4.hpp"
// #include <boost/qvm/config.hpp> // Expanded at line 109
// #include <boost/qvm/deduce_vec.hpp> // Expanded at line 1450
// #include <boost/qvm/enable_if.hpp> // Expanded at line 177
// #include <boost/qvm/mat_traits.hpp> // Expanded at line 303
// #include <boost/qvm/vec_traits.hpp> // Expanded at line 1010

namespace boost { namespace qvm {

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==4 && mat_traits<A>::cols==4 &&
    vec_traits<B>::dim==4,
    deduce_vec2<A,B,4> >::type
operator*( A const & a, B const & b )
    {
    typedef typename mat_traits<A>::scalar_type Ta;
    typedef typename vec_traits<B>::scalar_type Tb;
    Ta const a00 = mat_traits<A>::template read_element<0,0>(a);
    Ta const a01 = mat_traits<A>::template read_element<0,1>(a);
    Ta const a02 = mat_traits<A>::template read_element<0,2>(a);
    Ta const a03 = mat_traits<A>::template read_element<0,3>(a);
    Ta const a10 = mat_traits<A>::template read_element<1,0>(a);
    Ta const a11 = mat_traits<A>::template read_element<1,1>(a);
    Ta const a12 = mat_traits<A>::template read_element<1,2>(a);
    Ta const a13 = mat_traits<A>::template read_element<1,3>(a);
    Ta const a20 = mat_traits<A>::template read_element<2,0>(a);
    Ta const a21 = mat_traits<A>::template read_element<2,1>(a);
    Ta const a22 = mat_traits<A>::template read_element<2,2>(a);
    Ta const a23 = mat_traits<A>::template read_element<2,3>(a);
    Ta const a30 = mat_traits<A>::template read_element<3,0>(a);
    Ta const a31 = mat_traits<A>::template read_element<3,1>(a);
    Ta const a32 = mat_traits<A>::template read_element<3,2>(a);
    Ta const a33 = mat_traits<A>::template read_element<3,3>(a);
    Tb const b0 = vec_traits<B>::template read_element<0>(b);
    Tb const b1 = vec_traits<B>::template read_element<1>(b);
    Tb const b2 = vec_traits<B>::template read_element<2>(b);
    Tb const b3 = vec_traits<B>::template read_element<3>(b);
    typedef typename deduce_vec2<A,B,4>::type R;
    BOOST_QVM_STATIC_ASSERT(vec_traits<R>::dim==4);
    R r;
    write_vec_element<0>(r,a00*b0+a01*b1+a02*b2+a03*b3);
    write_vec_element<1>(r,a10*b0+a11*b1+a12*b2+a13*b3);
    write_vec_element<2>(r,a20*b0+a21*b1+a22*b2+a23*b3);
    write_vec_element<3>(r,a30*b0+a31*b1+a32*b2+a33*b3);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct mul_mv_defined;

    template <>
    struct
    mul_mv_defined<4,4>
        {
        static bool const value=true;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<B>::rows==4 && mat_traits<B>::cols==4 &&
    vec_traits<A>::dim==4,
    deduce_vec2<A,B,4> >::type
operator*( A const & a, B const & b )
    {
    typedef typename vec_traits<A>::scalar_type Ta;
    typedef typename mat_traits<B>::scalar_type Tb;
    Ta const a0 = vec_traits<A>::template read_element<0>(a);
    Ta const a1 = vec_traits<A>::template read_element<1>(a);
    Ta const a2 = vec_traits<A>::template read_element<2>(a);
    Ta const a3 = vec_traits<A>::template read_element<3>(a);
    Tb const b00 = mat_traits<B>::template read_element<0,0>(b);
    Tb const b01 = mat_traits<B>::template read_element<0,1>(b);
    Tb const b02 = mat_traits<B>::template read_element<0,2>(b);
    Tb const b03 = mat_traits<B>::template read_element<0,3>(b);
    Tb const b10 = mat_traits<B>::template read_element<1,0>(b);
    Tb const b11 = mat_traits<B>::template read_element<1,1>(b);
    Tb const b12 = mat_traits<B>::template read_element<1,2>(b);
    Tb const b13 = mat_traits<B>::template read_element<1,3>(b);
    Tb const b20 = mat_traits<B>::template read_element<2,0>(b);
    Tb const b21 = mat_traits<B>::template read_element<2,1>(b);
    Tb const b22 = mat_traits<B>::template read_element<2,2>(b);
    Tb const b23 = mat_traits<B>::template read_element<2,3>(b);
    Tb const b30 = mat_traits<B>::template read_element<3,0>(b);
    Tb const b31 = mat_traits<B>::template read_element<3,1>(b);
    Tb const b32 = mat_traits<B>::template read_element<3,2>(b);
    Tb const b33 = mat_traits<B>::template read_element<3,3>(b);
    typedef typename deduce_vec2<A,B,4>::type R;
    BOOST_QVM_STATIC_ASSERT(vec_traits<R>::dim==4);
    R r;
    write_vec_element<0>(r,a0*b00+a1*b10+a2*b20+a3*b30);
    write_vec_element<1>(r,a0*b01+a1*b11+a2*b21+a3*b31);
    write_vec_element<2>(r,a0*b02+a1*b12+a2*b22+a3*b32);
    write_vec_element<3>(r,a0*b03+a1*b13+a2*b23+a3*b33);
    return r;
    }

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    }

namespace
qvm_detail
    {
    template <int R,int C>
    struct mul_vm_defined;

    template <>
    struct
    mul_vm_defined<4,4>
        {
        static bool const value=true;
        };
    }

} }

#endif
// <<< #include <boost/qvm/gen/vec_mat_operations4.hpp>
// #line 6 "boost/qvm/vec_mat_operations4.hpp"
// <<< #include <boost/qvm/vec_mat_operations4.hpp>
// #line 11 "boost/qvm/vec_mat_operations.hpp"

namespace boost { namespace qvm {

namespace
qvm_detail
    {
    template <int M,int N>
    struct
    mul_mv_defined
        {
        static bool const value=false;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_mat<A>::value && is_vec<B>::value &&
    mat_traits<A>::cols==vec_traits<B>::dim &&
    !qvm_detail::mul_mv_defined<mat_traits<A>::rows,mat_traits<A>::cols>::value,
    deduce_vec2<A,B,mat_traits<A>::rows> >::type
operator*( A const & a, B const & b )
    {
    typedef typename deduce_vec2<A,B,mat_traits<A>::rows>::type R;
    R r;
    for( int i=0; i<mat_traits<A>::rows; ++i )
        {
        typedef typename vec_traits<R>::scalar_type Tr;
        Tr x(scalar_traits<Tr>::value(0));
        for( int j=0; j<mat_traits<A>::cols; ++j )
            x += mat_traits<A>::read_element_idx(i,j,a)*vec_traits<B>::read_element_idx(j,b);
        write_vec_element_idx(i,r,x);
        }
    return r;
    }

namespace
qvm_detail
    {
    template <int M,int N>
    struct
    mul_vm_defined
        {
        static bool const value=false;
        };
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    is_vec<A>::value && is_mat<B>::value &&
    vec_traits<A>::dim==mat_traits<B>::rows &&
    !qvm_detail::mul_vm_defined<mat_traits<B>::rows,mat_traits<B>::cols>::value,
    deduce_vec2<A,B,mat_traits<B>::cols> >::type
operator*( A const & a, B const & b )
    {
    typedef typename deduce_vec2<A,B,mat_traits<B>::cols>::type R;
    R r;
    for( int i=0; i<mat_traits<B>::cols; ++i )
        {
        typedef typename vec_traits<R>::scalar_type Tr;
        Tr x(scalar_traits<Tr>::value(0));
        for( int j=0; j<mat_traits<B>::rows; ++j )
            x += vec_traits<A>::read_element_idx(j,a)*mat_traits<B>::read_element_idx(j,i,b);
        write_vec_element_idx(i,r,x);
        }
    return r;
    }

////////////////////////////////////////////////

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==4 && mat_traits<A>::cols==4 &&
    vec_traits<B>::dim==3,
    deduce_vec2<A,B,3> >::type
transform_point( A const & a, B const & b )
    {
    typedef typename mat_traits<A>::scalar_type Ta;
    typedef typename vec_traits<B>::scalar_type Tb;
    Ta const a00 = mat_traits<A>::template read_element<0,0>(a);
    Ta const a01 = mat_traits<A>::template read_element<0,1>(a);
    Ta const a02 = mat_traits<A>::template read_element<0,2>(a);
    Ta const a03 = mat_traits<A>::template read_element<0,3>(a);
    Ta const a10 = mat_traits<A>::template read_element<1,0>(a);
    Ta const a11 = mat_traits<A>::template read_element<1,1>(a);
    Ta const a12 = mat_traits<A>::template read_element<1,2>(a);
    Ta const a13 = mat_traits<A>::template read_element<1,3>(a);
    Ta const a20 = mat_traits<A>::template read_element<2,0>(a);
    Ta const a21 = mat_traits<A>::template read_element<2,1>(a);
    Ta const a22 = mat_traits<A>::template read_element<2,2>(a);
    Ta const a23 = mat_traits<A>::template read_element<2,3>(a);
    Tb const b0 = vec_traits<B>::template read_element<0>(b);
    Tb const b1 = vec_traits<B>::template read_element<1>(b);
    Tb const b2 = vec_traits<B>::template read_element<2>(b);
    typedef typename deduce_vec2<A,B,3>::type R;
    BOOST_QVM_STATIC_ASSERT(vec_traits<R>::dim==3);
    R r;
    write_vec_element<0>(r, a00*b0+a01*b1+a02*b2+a03);
    write_vec_element<1>(r, a10*b0+a11*b1+a12*b2+a13);
    write_vec_element<2>(r, a20*b0+a21*b1+a22*b2+a23);
    return r;
    }

template <class A,class B>
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_OPERATIONS
typename lazy_enable_if_c<
    mat_traits<A>::rows==4 && mat_traits<A>::cols==4 &&
    vec_traits<B>::dim==3,
    deduce_vec2<A,B,3> >::type
transform_vector( A const & a, B const & b )
    {
    typedef typename mat_traits<A>::scalar_type Ta;
    typedef typename vec_traits<B>::scalar_type Tb;
    Ta const a00 = mat_traits<A>::template read_element<0,0>(a);
    Ta const a01 = mat_traits<A>::template read_element<0,1>(a);
    Ta const a02 = mat_traits<A>::template read_element<0,2>(a);
    Ta const a10 = mat_traits<A>::template read_element<1,0>(a);
    Ta const a11 = mat_traits<A>::template read_element<1,1>(a);
    Ta const a12 = mat_traits<A>::template read_element<1,2>(a);
    Ta const a20 = mat_traits<A>::template read_element<2,0>(a);
    Ta const a21 = mat_traits<A>::template read_element<2,1>(a);
    Ta const a22 = mat_traits<A>::template read_element<2,2>(a);
    Tb const b0 = vec_traits<B>::template read_element<0>(b);
    Tb const b1 = vec_traits<B>::template read_element<1>(b);
    Tb const b2 = vec_traits<B>::template read_element<2>(b);
    typedef typename deduce_vec2<A,B,3>::type R;
    BOOST_QVM_STATIC_ASSERT(vec_traits<R>::dim==3);
    R r;
    write_vec_element<0>(r, a00*b0+a01*b1+a02*b2);
    write_vec_element<1>(r, a10*b0+a11*b1+a12*b2);
    write_vec_element<2>(r, a20*b0+a21*b1+a22*b2);
    return r;
    }

////////////////////////////////////////////////

namespace
sfinae
    {
    using ::boost::qvm::operator*;
    using ::boost::qvm::transform_point;
    using ::boost::qvm::transform_vector;
    }

} }

#endif
// <<< #include <boost/qvm/vec_mat_operations.hpp>
// #line 26 "boost/qvm/lite.hpp"
// >>> #include <boost/qvm/vec_register.hpp>
#ifndef BOOST_QVM_VEC_REGISTER_HPP_INCLUDED
#define BOOST_QVM_VEC_REGISTER_HPP_INCLUDED

// #line 9 "boost/qvm/vec_register.hpp"
// >>> #include <boost/qvm/vec_register2.hpp>
#ifndef BOOST_QVM_VEC_REGISTER2_HPP
#define BOOST_QVM_VEC_REGISTER2_HPP

// #line 9 "boost/qvm/vec_register2.hpp"
// >>> #include <boost/qvm/detail/vec_register_impl.hpp>
#ifndef BOOST_QVM_DETAIL_VEC_REGISTER_IMPL_HPP
#define BOOST_QVM_DETAIL_VEC_REGISTER_IMPL_HPP

// #line 9 "boost/qvm/detail/vec_register_impl.hpp"
// #include <boost/qvm/assert.hpp> // Expanded at line 887
// #include <boost/qvm/config.hpp> // Expanded at line 109
// #include <boost/qvm/static_assert.hpp> // Expanded at line 1180
// #include <boost/qvm/vec_traits.hpp> // Expanded at line 1010

namespace boost { namespace qvm { namespace qvm_detail {

template<class VecType, class ScalarType, int Dim>
struct vec_register_common
{
    typedef VecType vec_type;
    typedef ScalarType scalar_type;
    static int const dim = Dim;
};

template<class VecType, class ScalarType, int Dim>
struct vec_register_read
{
    template<int I> static ScalarType read_element(VecType const& v);

    template<int I, int N> struct read_element_idx_detail
    {
        static BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL ScalarType impl(int const i, VecType const& v)
        {
            return I == i
                ? read_element<I>(v)
                : read_element_idx_detail<I + 1, N>::impl(i, v);
        }
    };

    template<int N> struct read_element_idx_detail<N, N>
    {
        static BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL ScalarType impl(int, VecType const& v)
        {
            BOOST_QVM_ASSERT(0);
            return read_element<0>(v);
        }
    };

    static BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL ScalarType read_element_idx(int const i, VecType const& v)
    {
        return read_element_idx_detail<0, Dim>::impl(i, v);
    }
};

template<class VecType, class ScalarType, int Dim>
struct vec_register_write
{
    template<int I> static ScalarType& write_element(VecType& v);

    template<int I, int N> struct write_element_idx_detail
    {
        static BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL ScalarType& impl(int const i, VecType& v)
        {
            return I == i
                ? write_element<I>(v)
                : write_element_idx_detail<I + 1, N>::impl(i, v);
        }
    };

    template<int N> struct write_element_idx_detail<N, N>
    {
        static BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL ScalarType& impl(int, VecType& v)
        {
            BOOST_QVM_ASSERT(0);
            return write_element<0>(v);
        }
    };

    static BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL ScalarType& write_element_idx(int const i, VecType& v)
    {
        return write_element_idx_detail<0, Dim>::impl(i, v);
    }
};

}}}

#define BOOST_QVM_DETAIL_SPECIALIZE_QVM_DETAIL_VEC_REGISTER_READ(VecType, ScalarType, Dim, I, Read) \
namespace boost { namespace qvm {namespace qvm_detail{ \
template<> \
template<> \
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL \
ScalarType vec_register_read<VecType, ScalarType, Dim>::read_element<I>(VecType const& v) \
{ \
    BOOST_QVM_STATIC_ASSERT(I>=0); \
    BOOST_QVM_STATIC_ASSERT(I<Dim); \
    return v. Read; \
} \
}}}

#define BOOST_QVM_DETAIL_SPECIALIZE_QVM_DETAIL_VEC_REGISTER_WRITE(VecType, ScalarType, Dim, I, Write) \
namespace boost { namespace qvm {namespace qvm_detail{ \
template<> \
template<> \
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL \
ScalarType& vec_register_write<VecType, ScalarType, Dim>::write_element<I>(VecType& v) \
{ \
    BOOST_QVM_STATIC_ASSERT(I>=0); \
    BOOST_QVM_STATIC_ASSERT(I<Dim); \
    return v. Write; \
}; \
}}}

#define BOOST_QVM_DETAIL_SPECIALIZE_QVM_DETAIL_VEC_REGISTER_READ_WRITE(VecType, ScalarType, Dim, I, Read, Write)\
BOOST_QVM_DETAIL_SPECIALIZE_QVM_DETAIL_VEC_REGISTER_READ(VecType, ScalarType, Dim, I, Read) \
BOOST_QVM_DETAIL_SPECIALIZE_QVM_DETAIL_VEC_REGISTER_WRITE(VecType, ScalarType, Dim, I, Write)

#define BOOST_QVM_DETAIL_REGISTER_VEC_SPECIALIZE_VEC_TRAITS_READ(VecType, ScalarType, Dim) \
namespace boost { namespace qvm { \
template<> \
struct vec_traits<VecType> \
: qvm_detail::vec_register_common<VecType, ScalarType, Dim> \
, qvm_detail::vec_register_read<VecType, ScalarType, Dim> \
{ \
}; \
}}

#define BOOST_QVM_DETAIL_REGISTER_VEC_SPECIALIZE_VEC_TRAITS_READ_WRITE(VecType, ScalarType, Dim)\
namespace boost { namespace qvm { \
template<> \
struct vec_traits<VecType> \
: qvm_detail::vec_register_common<VecType, ScalarType, Dim> \
, qvm_detail::vec_register_read<VecType, ScalarType, Dim> \
, qvm_detail::vec_register_write<VecType, ScalarType, Dim> \
{ \
}; \
}}

#endif
// <<< #include <boost/qvm/detail/vec_register_impl.hpp>
// #line 10 "boost/qvm/vec_register2.hpp"

#define BOOST_QVM_REGISTER_VEC_2_READ(VecType, ScalarType, Read0, Read1)                    \
BOOST_QVM_DETAIL_REGISTER_VEC_SPECIALIZE_VEC_TRAITS_READ(VecType, ScalarType, 2)            \
BOOST_QVM_DETAIL_SPECIALIZE_QVM_DETAIL_VEC_REGISTER_READ(VecType, ScalarType, 2, 0, Read0)  \
BOOST_QVM_DETAIL_SPECIALIZE_QVM_DETAIL_VEC_REGISTER_READ(VecType, ScalarType, 2, 1, Read1)

#define BOOST_QVM_REGISTER_VEC_2_READ_WRITE(VecType, ScalarType, Read0, Read1, Write0, Write1)          \
BOOST_QVM_DETAIL_REGISTER_VEC_SPECIALIZE_VEC_TRAITS_READ_WRITE(VecType, ScalarType, 2)                  \
BOOST_QVM_DETAIL_SPECIALIZE_QVM_DETAIL_VEC_REGISTER_READ_WRITE(VecType, ScalarType, 2, 0, Read0, Write0)\
BOOST_QVM_DETAIL_SPECIALIZE_QVM_DETAIL_VEC_REGISTER_READ_WRITE(VecType, ScalarType, 2, 1, Read1, Write1)

#define BOOST_QVM_REGISTER_VEC_2(VecType, ScalarType, Element0, Element1)   \
BOOST_QVM_REGISTER_VEC_2_READ_WRITE(VecType, ScalarType, Element0, Element1, Element0, Element1)

#endif
// <<< #include <boost/qvm/vec_register2.hpp>
// #line 10 "boost/qvm/vec_register.hpp"
// >>> #include <boost/qvm/vec_register3.hpp>
#ifndef BOOST_QVM_VEC_REGISTER3_HPP
#define BOOST_QVM_VEC_REGISTER3_HPP

// #line 9 "boost/qvm/vec_register3.hpp"
// #include <boost/qvm/detail/vec_register_impl.hpp> // Expanded at line 17577

#define BOOST_QVM_REGISTER_VEC_3_READ(VecType, ScalarType, Read0, Read1, Read2)             \
BOOST_QVM_DETAIL_REGISTER_VEC_SPECIALIZE_VEC_TRAITS_READ(VecType, ScalarType, 3)            \
BOOST_QVM_DETAIL_SPECIALIZE_QVM_DETAIL_VEC_REGISTER_READ(VecType, ScalarType, 3, 0, Read0)  \
BOOST_QVM_DETAIL_SPECIALIZE_QVM_DETAIL_VEC_REGISTER_READ(VecType, ScalarType, 3, 1, Read1)  \
BOOST_QVM_DETAIL_SPECIALIZE_QVM_DETAIL_VEC_REGISTER_READ(VecType, ScalarType, 3, 2, Read2)

#define BOOST_QVM_REGISTER_VEC_3_READ_WRITE(VecType, ScalarType, Read0, Read1, Read2, Write0, Write1, Write2)   \
BOOST_QVM_DETAIL_REGISTER_VEC_SPECIALIZE_VEC_TRAITS_READ_WRITE(VecType, ScalarType, 3)                          \
BOOST_QVM_DETAIL_SPECIALIZE_QVM_DETAIL_VEC_REGISTER_READ_WRITE(VecType, ScalarType, 3, 0, Read0, Write0)        \
BOOST_QVM_DETAIL_SPECIALIZE_QVM_DETAIL_VEC_REGISTER_READ_WRITE(VecType, ScalarType, 3, 1, Read1, Write1)        \
BOOST_QVM_DETAIL_SPECIALIZE_QVM_DETAIL_VEC_REGISTER_READ_WRITE(VecType, ScalarType, 3, 2, Read2, Write2)

#define BOOST_QVM_REGISTER_VEC_3(VecType, ScalarType, Element0, Element1, Element2)   \
BOOST_QVM_REGISTER_VEC_3_READ_WRITE(VecType, ScalarType, Element0, Element1, Element2, Element0, Element1, Element2)

#endif
// <<< #include <boost/qvm/vec_register3.hpp>
// #line 11 "boost/qvm/vec_register.hpp"
// >>> #include <boost/qvm/vec_register4.hpp>
#ifndef BOOST_QVM_VEC_REGISTER4_HPP
#define BOOST_QVM_VEC_REGISTER4_HPP

// #line 9 "boost/qvm/vec_register4.hpp"
// #include <boost/qvm/detail/vec_register_impl.hpp> // Expanded at line 17577

#define BOOST_QVM_REGISTER_VEC_4_READ(VecType, ScalarType, Read0, Read1, Read2, Read3)      \
BOOST_QVM_DETAIL_REGISTER_VEC_SPECIALIZE_VEC_TRAITS_READ(VecType, ScalarType, 4)            \
BOOST_QVM_DETAIL_SPECIALIZE_QVM_DETAIL_VEC_REGISTER_READ(VecType, ScalarType, 4, 0, Read0)  \
BOOST_QVM_DETAIL_SPECIALIZE_QVM_DETAIL_VEC_REGISTER_READ(VecType, ScalarType, 4, 1, Read1)  \
BOOST_QVM_DETAIL_SPECIALIZE_QVM_DETAIL_VEC_REGISTER_READ(VecType, ScalarType, 4, 2, Read2)  \
BOOST_QVM_DETAIL_SPECIALIZE_QVM_DETAIL_VEC_REGISTER_READ(VecType, ScalarType, 4, 3, Read3)

#define BOOST_QVM_REGISTER_VEC_4_READ_WRITE(VecType, ScalarType, Read0, Read1, Read2, Read3, Write0, Write1, Write2, Write3)\
BOOST_QVM_DETAIL_REGISTER_VEC_SPECIALIZE_VEC_TRAITS_READ_WRITE(VecType, ScalarType, 4)                                      \
BOOST_QVM_DETAIL_SPECIALIZE_QVM_DETAIL_VEC_REGISTER_READ_WRITE(VecType, ScalarType, 4, 0, Read0, Write0)                    \
BOOST_QVM_DETAIL_SPECIALIZE_QVM_DETAIL_VEC_REGISTER_READ_WRITE(VecType, ScalarType, 4, 1, Read1, Write1)                    \
BOOST_QVM_DETAIL_SPECIALIZE_QVM_DETAIL_VEC_REGISTER_READ_WRITE(VecType, ScalarType, 4, 2, Read2, Write2)                    \
BOOST_QVM_DETAIL_SPECIALIZE_QVM_DETAIL_VEC_REGISTER_READ_WRITE(VecType, ScalarType, 4, 3, Read3, Write3)

#define BOOST_QVM_REGISTER_VEC_4(VecType, ScalarType, Element0, Element1, Element2, Element3)   \
BOOST_QVM_REGISTER_VEC_4_READ_WRITE(VecType, ScalarType, Element0, Element1, Element2, Element3, Element0, Element1, Element2, Element3)

#endif
// <<< #include <boost/qvm/vec_register4.hpp>
// #line 12 "boost/qvm/vec_register.hpp"

#endif
// <<< #include <boost/qvm/vec_register.hpp>
// #line 27 "boost/qvm/lite.hpp"
// >>> #include <boost/qvm/map.hpp>
#ifndef BOOST_QVM_MAP_HPP_INCLUDED
#define BOOST_QVM_MAP_HPP_INCLUDED

// #line 8 "boost/qvm/map.hpp"
// >>> #include <boost/qvm/map_vec_mat.hpp>
#ifndef BOOST_QVM_MAP_VEC_MAT_HPP_INCLUDED
#define BOOST_QVM_MAP_VEC_MAT_HPP_INCLUDED

// #line 8 "boost/qvm/map_vec_mat.hpp"
// #include <boost/qvm/config.hpp> // Expanded at line 109
// #include <boost/qvm/deduce_mat.hpp> // Expanded at line 899
// #include <boost/qvm/vec_traits.hpp> // Expanded at line 1010
// #include <boost/qvm/assert.hpp> // Expanded at line 887
// #include <boost/qvm/enable_if.hpp> // Expanded at line 177

namespace boost { namespace qvm {

namespace
qvm_detail
    {
    template <class OriginalVector>
    class
    col_mat_
        {
        col_mat_( col_mat_ const & );
        col_mat_ & operator=( col_mat_ const & );
        ~col_mat_();

        public:

        template <class T>
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        col_mat_ &
        operator=( T const & x )
            {
            assign(*this,x);
            return *this;
            }

        template <class R
#if __cplusplus >= 201103L
            , class = typename enable_if<is_mat<R> >::type
#endif
        >
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        operator R() const
            {
            R r;
            assign(r,*this);
            return r;
            }
        };

    template <class OriginalVector,bool WriteElementRef=vec_write_element_ref<OriginalVector>::value>
    struct col_mat_write_traits;

    template <class OriginalVector>
    struct
    col_mat_write_traits<OriginalVector,true>
        {
        typedef qvm_detail::col_mat_<OriginalVector> this_matrix;
        typedef typename vec_traits<OriginalVector>::scalar_type scalar_type;
        static int const rows=vec_traits<OriginalVector>::dim;
        static int const cols=1;

        template <int Row,int Col>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        scalar_type &
        write_element( this_matrix & x )
            {
            BOOST_QVM_STATIC_ASSERT(Col==0);
            BOOST_QVM_STATIC_ASSERT(Row>=0);
            BOOST_QVM_STATIC_ASSERT(Row<rows);
            return vec_traits<OriginalVector>::template write_element<Row>(reinterpret_cast<OriginalVector &>(x));
            }

        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        scalar_type &
        write_element_idx( int row, int col, this_matrix & x )
            {
            BOOST_QVM_ASSERT(col==0); (void)col;
            BOOST_QVM_ASSERT(row>=0);
            BOOST_QVM_ASSERT(row<rows);
            return vec_traits<OriginalVector>::write_element_idx(row,reinterpret_cast<OriginalVector &>(x));
            }
        };

    template <class OriginalVector>
    struct
    col_mat_write_traits<OriginalVector,false>
        {
        typedef qvm_detail::col_mat_<OriginalVector> this_matrix;
        typedef typename vec_traits<OriginalVector>::scalar_type scalar_type;
        static int const rows=vec_traits<OriginalVector>::dim;
        static int const cols=1;

        template <int Row,int Col>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        void
        write_element( this_matrix & x, scalar_type s )
            {
            BOOST_QVM_STATIC_ASSERT(Col==0);
            BOOST_QVM_STATIC_ASSERT(Row>=0);
            BOOST_QVM_STATIC_ASSERT(Row<rows);
            vec_traits<OriginalVector>::template write_element<Row>(reinterpret_cast<OriginalVector &>(x), s);
            }

        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        void
        write_element_idx( int row, int col, this_matrix & x, scalar_type s )
            {
            BOOST_QVM_ASSERT(col==0); (void)col;
            BOOST_QVM_ASSERT(row>=0);
            BOOST_QVM_ASSERT(row<rows);
            vec_traits<OriginalVector>::write_element_idx(row,reinterpret_cast<OriginalVector &>(x), s);
            }
        };
    }

template <class OriginalVector>
struct
mat_traits< qvm_detail::col_mat_<OriginalVector> >:
    qvm_detail::col_mat_write_traits<OriginalVector>
    {
    typedef qvm_detail::col_mat_<OriginalVector> this_matrix;
    typedef typename vec_traits<OriginalVector>::scalar_type scalar_type;
    static int const rows=vec_traits<OriginalVector>::dim;
    static int const cols=1;

    template <int Row,int Col>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_matrix const & x )
        {
        BOOST_QVM_STATIC_ASSERT(Col==0);
        BOOST_QVM_STATIC_ASSERT(Row>=0);
        BOOST_QVM_STATIC_ASSERT(Row<rows);
        return vec_traits<OriginalVector>::template read_element<Row>(reinterpret_cast<OriginalVector const &>(x));
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int row, int col, this_matrix const & x )
        {
        BOOST_QVM_ASSERT(col==0); (void)col;
        BOOST_QVM_ASSERT(row>=0);
        BOOST_QVM_ASSERT(row<rows);
        return vec_traits<OriginalVector>::read_element_idx(row,reinterpret_cast<OriginalVector const &>(x));
        }
    };

template <class OriginalVector,int R,int C>
struct
deduce_mat<qvm_detail::col_mat_<OriginalVector>,R,C>
    {
    typedef mat<typename vec_traits<OriginalVector>::scalar_type,R,C> type;
    };

template <class OriginalVector,int R,int C>
struct
deduce_mat2<qvm_detail::col_mat_<OriginalVector>,qvm_detail::col_mat_<OriginalVector>,R,C>
    {
    typedef mat<typename vec_traits<OriginalVector>::scalar_type,R,C> type;
    };

template <class A>
typename enable_if_c<
    is_vec<A>::value,
    qvm_detail::col_mat_<A> const &>::type
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
col_mat( A const & a )
    {
    return reinterpret_cast<typename qvm_detail::col_mat_<A> const &>(a);
    }

template <class A>
typename enable_if_c<
    is_vec<A>::value,
    qvm_detail::col_mat_<A> &>::type
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
col_mat( A & a )
    {
    return reinterpret_cast<typename qvm_detail::col_mat_<A> &>(a);
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <class OriginalVector>
    class
    row_mat_
        {
        row_mat_( row_mat_ const & );
        row_mat_ & operator=( row_mat_ const & );
        ~row_mat_();

        public:

        template <class T>
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        row_mat_ &
        operator=( T const & x )
            {
            assign(*this,x);
            return *this;
            }

        template <class R
#if __cplusplus >= 201103L
            , class = typename enable_if<is_mat<R> >::type
#endif
        >
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        operator R() const
            {
            R r;
            assign(r,*this);
            return r;
            }
        };

    template <class OriginalVector,bool WriteElementRef=vec_write_element_ref<OriginalVector>::value>
    struct row_mat_write_traits;

    template <class OriginalVector>
    struct
    row_mat_write_traits<OriginalVector,true>
        {
        typedef qvm_detail::row_mat_<OriginalVector> this_matrix;
        typedef typename vec_traits<OriginalVector>::scalar_type scalar_type;
        static int const rows=1;
        static int const cols=vec_traits<OriginalVector>::dim;

        template <int Row,int Col>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        scalar_type &
        write_element( this_matrix & x )
            {
            BOOST_QVM_STATIC_ASSERT(Row==0);
            BOOST_QVM_STATIC_ASSERT(Col>=0);
            BOOST_QVM_STATIC_ASSERT(Col<cols);
            return vec_traits<OriginalVector>::template write_element<Col>(reinterpret_cast<OriginalVector &>(x));
            }

        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        scalar_type &
        write_element_idx( int row, int col, this_matrix & x )
            {
            BOOST_QVM_ASSERT(row==0); (void)row;
            BOOST_QVM_ASSERT(col>=0);
            BOOST_QVM_ASSERT(col<cols);
            return vec_traits<OriginalVector>::write_element_idx(col,reinterpret_cast<OriginalVector &>(x));
            }
        };

    template <class OriginalVector>
    struct
    row_mat_write_traits<OriginalVector,false>
        {
        typedef qvm_detail::row_mat_<OriginalVector> this_matrix;
        typedef typename vec_traits<OriginalVector>::scalar_type scalar_type;
        static int const rows=1;
        static int const cols=vec_traits<OriginalVector>::dim;

        template <int Row,int Col>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        void
        write_element( this_matrix & x, scalar_type s )
            {
            BOOST_QVM_STATIC_ASSERT(Row==0);
            BOOST_QVM_STATIC_ASSERT(Col>=0);
            BOOST_QVM_STATIC_ASSERT(Col<cols);
            vec_traits<OriginalVector>::template write_element<Col>(reinterpret_cast<OriginalVector &>(x), s);
            }

        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        void
        write_element_idx( int row, int col, this_matrix & x, scalar_type s )
            {
            BOOST_QVM_ASSERT(row==0); (void)row;
            BOOST_QVM_ASSERT(col>=0);
            BOOST_QVM_ASSERT(col<cols);
            vec_traits<OriginalVector>::write_element_idx(col,reinterpret_cast<OriginalVector &>(x), s);
            }
        };
    }

template <class OriginalVector>
struct
mat_traits< qvm_detail::row_mat_<OriginalVector> >:
    qvm_detail::row_mat_write_traits<OriginalVector>
    {
    typedef qvm_detail::row_mat_<OriginalVector> this_matrix;
    typedef typename vec_traits<OriginalVector>::scalar_type scalar_type;
    static int const rows=1;
    static int const cols=vec_traits<OriginalVector>::dim;

    template <int Row,int Col>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_matrix const & x )
        {
        BOOST_QVM_STATIC_ASSERT(Row==0);
        BOOST_QVM_STATIC_ASSERT(Col>=0);
        BOOST_QVM_STATIC_ASSERT(Col<cols);
        return vec_traits<OriginalVector>::template read_element<Col>(reinterpret_cast<OriginalVector const &>(x));
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int row, int col, this_matrix const & x )
        {
        BOOST_QVM_ASSERT(row==0); (void)row;
        BOOST_QVM_ASSERT(col>=0);
        BOOST_QVM_ASSERT(col<cols);
        return vec_traits<OriginalVector>::read_element_idx(col,reinterpret_cast<OriginalVector const &>(x));
        }
    };

template <class OriginalVector,int R,int C>
struct
deduce_mat<qvm_detail::row_mat_<OriginalVector>,R,C>
    {
    typedef mat<typename vec_traits<OriginalVector>::scalar_type,R,C> type;
    };

template <class OriginalVector,int R,int C>
struct
deduce_mat2<qvm_detail::row_mat_<OriginalVector>,qvm_detail::row_mat_<OriginalVector>,R,C>
    {
    typedef mat<typename vec_traits<OriginalVector>::scalar_type,R,C> type;
    };

template <class A>
typename enable_if_c<
    is_vec<A>::value,
    qvm_detail::row_mat_<A> const &>::type
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
row_mat( A const & a )
    {
    return reinterpret_cast<typename qvm_detail::row_mat_<A> const &>(a);
    }

template <class A>
typename enable_if_c<
    is_vec<A>::value,
    qvm_detail::row_mat_<A> &>::type
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
row_mat( A & a )
    {
    return reinterpret_cast<typename qvm_detail::row_mat_<A> &>(a);
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <class OriginalVector>
    class
    translation_mat_
        {
        translation_mat_( translation_mat_ const & );
        translation_mat_ & operator=( translation_mat_ const & );
        ~translation_mat_();

        public:

        template <class T>
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        translation_mat_ &
        operator=( T const & x )
            {
            assign(*this,x);
            return *this;
            }

        template <class R
#if __cplusplus >= 201103L
            , class = typename enable_if<is_mat<R> >::type
#endif
        >
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        operator R() const
            {
            R r;
            assign(r,*this);
            return r;
            }
        };

    template <class M,int Row,int Col,bool TransCol=(Col==mat_traits<M>::cols-1)>
    struct read_translation_matat;

    template <class OriginalVector,int Row,int Col,bool TransCol>
    struct
    read_translation_matat<translation_mat_<OriginalVector>,Row,Col,TransCol>
        {
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        typename mat_traits< translation_mat_<OriginalVector> >::scalar_type
        f( translation_mat_<OriginalVector> const & )
            {
            return scalar_traits<typename mat_traits< translation_mat_<OriginalVector> >::scalar_type>::value(0);
            }
        };

    template <class OriginalVector,int D>
    struct
    read_translation_matat<translation_mat_<OriginalVector>,D,D,false>
        {
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        typename mat_traits< translation_mat_<OriginalVector> >::scalar_type
        f( translation_mat_<OriginalVector> const & )
            {
            return scalar_traits<typename mat_traits< translation_mat_<OriginalVector> >::scalar_type>::value(1);
            }
        };

    template <class OriginalVector,int D>
    struct
    read_translation_matat<translation_mat_<OriginalVector>,D,D,true>
        {
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        typename mat_traits< translation_mat_<OriginalVector> >::scalar_type
        f( translation_mat_<OriginalVector> const & )
            {
            return scalar_traits<typename mat_traits< translation_mat_<OriginalVector> >::scalar_type>::value(1);
            }
        };

    template <class OriginalVector,int Row,int Col>
    struct
    read_translation_matat<translation_mat_<OriginalVector>,Row,Col,true>
        {
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        typename mat_traits< translation_mat_<OriginalVector> >::scalar_type
        f( translation_mat_<OriginalVector> const & x )
            {
            return vec_traits<OriginalVector>::template read_element<Row>(reinterpret_cast<OriginalVector const &>(x));
            }
        };

    template <class OriginalVector,bool WriteElementRef=vec_write_element_ref<OriginalVector>::value>
    struct translation_mat_write_traits;

    template <class OriginalVector>
    struct
    translation_mat_write_traits<OriginalVector,true>
        {
        typedef qvm_detail::translation_mat_<OriginalVector> this_matrix;
        typedef typename vec_traits<OriginalVector>::scalar_type scalar_type;
        static int const rows=vec_traits<OriginalVector>::dim+1;
        static int const cols=vec_traits<OriginalVector>::dim+1;

        template <int Row,int Col>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        scalar_type &
        write_element( this_matrix & x )
            {
            BOOST_QVM_STATIC_ASSERT(Row>=0);
            BOOST_QVM_STATIC_ASSERT(Row<rows-1);
            BOOST_QVM_STATIC_ASSERT(Col==cols-1 || Col==0);
            //The following should be a static_assert, but this is a constexpr
            //function and it gets instantiated with Row=0 and Col=0 in the
            //mat_write_element_ref test (in a sizeof expression).
            BOOST_QVM_ASSERT(Col==cols-1);
            return vec_traits<OriginalVector>::template write_element<Row>(reinterpret_cast<OriginalVector &>(x));
            }

        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        scalar_type &
        write_element_idx( int row, int col, this_matrix const & x )
            {
            BOOST_QVM_ASSERT(row>=0);
            BOOST_QVM_ASSERT(row<rows-1);
            BOOST_QVM_ASSERT(col==cols-1); (void)col;
            return vec_traits<OriginalVector>::write_element_idx(row,reinterpret_cast<OriginalVector &>(x));
            }
        };

    template <class OriginalVector>
    struct
    translation_mat_write_traits<OriginalVector,false>
        {
        typedef qvm_detail::translation_mat_<OriginalVector> this_matrix;
        typedef typename vec_traits<OriginalVector>::scalar_type scalar_type;
        static int const rows=vec_traits<OriginalVector>::dim+1;
        static int const cols=vec_traits<OriginalVector>::dim+1;

        template <int Row,int Col>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        void
        write_element( this_matrix & x, scalar_type s )
            {
            BOOST_QVM_STATIC_ASSERT(Row>=0);
            BOOST_QVM_STATIC_ASSERT(Row<rows-1);
            BOOST_QVM_STATIC_ASSERT(Col==cols-1 || Col==0);
            //The following should be a static_assert, but this is a constexpr
            //function and it gets instantiated with Row=0 and Col=0 in the
            //mat_write_element_ref test (in a sizeof expression).
            BOOST_QVM_ASSERT(Col==cols-1);
            vec_traits<OriginalVector>::template write_element<Row>(reinterpret_cast<OriginalVector &>(x), s);
            }

        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        void
        write_element_idx( int row, int col, this_matrix const & x, scalar_type s )
            {
            BOOST_QVM_ASSERT(row>=0);
            BOOST_QVM_ASSERT(row<rows);
            BOOST_QVM_ASSERT(col==cols-1); (void)col;
            BOOST_QVM_ASSERT(col!=row);
            vec_traits<OriginalVector>::write_element_idx(row,reinterpret_cast<OriginalVector &>(x), s);
            }
        };
    }

template <class OriginalVector>
struct
mat_traits< qvm_detail::translation_mat_<OriginalVector> >:
    qvm_detail::translation_mat_write_traits<OriginalVector>
    {
    typedef qvm_detail::translation_mat_<OriginalVector> this_matrix;
    typedef typename vec_traits<OriginalVector>::scalar_type scalar_type;
    static int const rows=vec_traits<OriginalVector>::dim+1;
    static int const cols=vec_traits<OriginalVector>::dim+1;

    template <int Row,int Col>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_matrix const & x )
        {
        BOOST_QVM_STATIC_ASSERT(Row>=0);
        BOOST_QVM_STATIC_ASSERT(Row<rows);
        BOOST_QVM_STATIC_ASSERT(Col>=0);
        BOOST_QVM_STATIC_ASSERT(Col<cols);
        return qvm_detail::read_translation_matat<qvm_detail::translation_mat_<OriginalVector>,Row,Col>::f(x);
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int row, int col, this_matrix const & x )
        {
        BOOST_QVM_ASSERT(row>=0);
        BOOST_QVM_ASSERT(row<rows);
        BOOST_QVM_ASSERT(col>=0);
        BOOST_QVM_ASSERT(col<cols);
        return
            row==col?
                scalar_traits<scalar_type>::value(1):
                (col==cols-1?
                    vec_traits<OriginalVector>::read_element_idx(row,reinterpret_cast<OriginalVector const &>(x)):
                    scalar_traits<scalar_type>::value(0));
        }
    };

template <class OriginalVector,int R,int C>
struct
deduce_mat<qvm_detail::translation_mat_<OriginalVector>,R,C>
    {
    typedef mat<typename vec_traits<OriginalVector>::scalar_type,R,C> type;
    };

template <class OriginalVector,int R,int C>
struct
deduce_mat2<qvm_detail::translation_mat_<OriginalVector>,qvm_detail::translation_mat_<OriginalVector>,R,C>
    {
    typedef mat<typename vec_traits<OriginalVector>::scalar_type,R,C> type;
    };

template <class A>
typename enable_if_c<
    is_vec<A>::value,
    qvm_detail::translation_mat_<A> const &>::type
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
translation_mat( A const & a )
    {
    return reinterpret_cast<typename qvm_detail::translation_mat_<A> const &>(a);
    }

template <class A>
typename enable_if_c<
    is_vec<A>::value,
    qvm_detail::translation_mat_<A> &>::type
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
translation_mat( A & a )
    {
    return reinterpret_cast<typename qvm_detail::translation_mat_<A> &>(a);
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <class OriginalVector>
    class
    diag_mat_
        {
        diag_mat_( diag_mat_ const & );
        diag_mat_ & operator=( diag_mat_ const & );
        ~diag_mat_();

        public:

        template <class T>
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        diag_mat_ &
        operator=( T const & x )
            {
            assign(*this,x);
            return *this;
            }

        template <class R
#if __cplusplus >= 201103L
            , class = typename enable_if<is_mat<R> >::type
#endif
        >
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        operator R() const
            {
            R r;
            assign(r,*this);
            return r;
            }
        };

    template <class OriginalVector,bool WriteElementRef=vec_write_element_ref<OriginalVector>::value>
    struct diag_mat_write_traits;

    template <class OriginalVector>
    struct
    diag_mat_write_traits<OriginalVector,true>
        {
        typedef qvm_detail::diag_mat_<OriginalVector> this_matrix;
        typedef typename vec_traits<OriginalVector>::scalar_type scalar_type;
        static int const rows=vec_traits<OriginalVector>::dim;
        static int const cols=vec_traits<OriginalVector>::dim;

        template <int Row,int Col>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        scalar_type &
        write_element( this_matrix & x )
            {
            BOOST_QVM_STATIC_ASSERT(Row>=0);
            BOOST_QVM_STATIC_ASSERT(Row<rows);
            BOOST_QVM_STATIC_ASSERT(Row==Col);
            return vec_traits<OriginalVector>::template write_element<Row>(reinterpret_cast<OriginalVector &>(x));
            }

        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        scalar_type &
        write_element_idx( int row, int col, this_matrix & x )
            {
            BOOST_QVM_ASSERT(row>=0);
            BOOST_QVM_ASSERT(row<rows);
            BOOST_QVM_ASSERT(row==col); (void)col;
            return vec_traits<OriginalVector>::write_element_idx(row,reinterpret_cast<OriginalVector &>(x));
            }
        };

    template <class OriginalVector>
    struct
    diag_mat_write_traits<OriginalVector,false>
        {
        typedef qvm_detail::diag_mat_<OriginalVector> this_matrix;
        typedef typename vec_traits<OriginalVector>::scalar_type scalar_type;
        static int const rows=vec_traits<OriginalVector>::dim;
        static int const cols=vec_traits<OriginalVector>::dim;

        template <int Row,int Col>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        void
        write_element( this_matrix & x, scalar_type s )
            {
            BOOST_QVM_STATIC_ASSERT(Row>=0);
            BOOST_QVM_STATIC_ASSERT(Row<rows);
            BOOST_QVM_STATIC_ASSERT(Row==Col);
            vec_traits<OriginalVector>::template write_element<Row>(reinterpret_cast<OriginalVector &>(x), s);
            }

        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        void
        write_element_idx( int row, int col, this_matrix & x, scalar_type s )
            {
            BOOST_QVM_ASSERT(row>=0);
            BOOST_QVM_ASSERT(row<rows);
            BOOST_QVM_ASSERT(row==col); (void)col;
            vec_traits<OriginalVector>::write_element_idx(row,reinterpret_cast<OriginalVector &>(x), s);
            }
        };
    }

template <class OriginalVector>
struct
mat_traits< qvm_detail::diag_mat_<OriginalVector> >:
    qvm_detail::diag_mat_write_traits<OriginalVector>
    {
    typedef qvm_detail::diag_mat_<OriginalVector> this_matrix;
    typedef typename vec_traits<OriginalVector>::scalar_type scalar_type;
    static int const rows=vec_traits<OriginalVector>::dim;
    static int const cols=vec_traits<OriginalVector>::dim;

    template <int Row,int Col>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_matrix const & x )
        {
        BOOST_QVM_STATIC_ASSERT(Row>=0);
        BOOST_QVM_STATIC_ASSERT(Row<rows);
        BOOST_QVM_STATIC_ASSERT(Col>=0);
        BOOST_QVM_STATIC_ASSERT(Col<cols);
        return Row==Col?vec_traits<OriginalVector>::template read_element<Row>(reinterpret_cast<OriginalVector const &>(x)):scalar_traits<scalar_type>::value(0);
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int row, int col, this_matrix const & x )
        {
        BOOST_QVM_ASSERT(row>=0);
        BOOST_QVM_ASSERT(row<rows);
        BOOST_QVM_ASSERT(col>=0);
        BOOST_QVM_ASSERT(col<cols);
        return row==col?vec_traits<OriginalVector>::read_element_idx(row,reinterpret_cast<OriginalVector const &>(x)):scalar_traits<scalar_type>::value(0);
        }
    };

template <class OriginalVector,int R,int C>
struct
deduce_mat<qvm_detail::diag_mat_<OriginalVector>,R,C>
    {
    typedef mat<typename vec_traits<OriginalVector>::scalar_type,R,C> type;
    };

template <class OriginalVector,int R,int C>
struct
deduce_mat2<qvm_detail::diag_mat_<OriginalVector>,qvm_detail::diag_mat_<OriginalVector>,R,C>
    {
    typedef mat<typename vec_traits<OriginalVector>::scalar_type,R,C> type;
    };

template <class A>
typename enable_if_c<
    is_vec<A>::value,
    qvm_detail::diag_mat_<A> const &>::type
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
diag_mat( A const & a )
    {
    return reinterpret_cast<typename qvm_detail::diag_mat_<A> const &>(a);
    }

template <class A>
typename enable_if_c<
    is_vec<A>::value,
    qvm_detail::diag_mat_<A> &>::type
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
diag_mat( A & a )
    {
    return reinterpret_cast<typename qvm_detail::diag_mat_<A> &>(a);
    }

} }

#endif
// <<< #include <boost/qvm/map_vec_mat.hpp>
// #line 9 "boost/qvm/map.hpp"
// >>> #include <boost/qvm/map_mat_vec.hpp>
#ifndef BOOST_QVM_MAP_MAT_VEC_HPP_INCLUDED
#define BOOST_QVM_MAP_MAT_VEC_HPP_INCLUDED

// #line 8 "boost/qvm/map_mat_vec.hpp"
// #include <boost/qvm/config.hpp> // Expanded at line 109
// #include <boost/qvm/mat_traits.hpp> // Expanded at line 303
// #include <boost/qvm/deduce_vec.hpp> // Expanded at line 1450
// #include <boost/qvm/assert.hpp> // Expanded at line 887
// #include <boost/qvm/enable_if.hpp> // Expanded at line 177

namespace boost { namespace qvm {

namespace
qvm_detail
    {
    template <int Col,class OriginalMatrix>
    class
    col_
        {
        col_( col_ const & );
        col_ & operator=( col_ const & );
        ~col_();

        public:

        template <class T>
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        col_ &
        operator=( T const & x )
            {
            assign(*this,x);
            return *this;
            }

        template <class R
#if __cplusplus >= 201103L
            , class = typename enable_if<is_vec<R> >::type
#endif
        >
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        operator R() const
            {
            R r;
            assign(r,*this);
            return r;
            }
        };

    template <int Col,class OriginalMatrix,bool WriteElementRef=mat_write_element_ref<OriginalMatrix>::value>
    struct col_write_traits;

    template <int Col,class OriginalMatrix>
    struct
    col_write_traits<Col,OriginalMatrix,true>
        {
        typedef qvm_detail::col_<Col,OriginalMatrix> this_vector;
        typedef typename mat_traits<OriginalMatrix>::scalar_type scalar_type;
        static int const dim=mat_traits<OriginalMatrix>::rows;
        BOOST_QVM_STATIC_ASSERT(Col>=0);
        BOOST_QVM_STATIC_ASSERT(Col<mat_traits<OriginalMatrix>::cols);

        template <int I>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        scalar_type &
        write_element( this_vector & x )
            {
            BOOST_QVM_STATIC_ASSERT(I>=0);
            BOOST_QVM_STATIC_ASSERT(I<dim);
            return mat_traits<OriginalMatrix>::template write_element<I,Col>(reinterpret_cast<OriginalMatrix &>(x));
            }

        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        scalar_type &
        write_element_idx( int i, this_vector & x )
            {
            BOOST_QVM_ASSERT(i>=0);
            BOOST_QVM_ASSERT(i<dim);
            return mat_traits<OriginalMatrix>::write_element_idx(i,Col,reinterpret_cast<OriginalMatrix &>(x));
            }
        };

    template <int Col,class OriginalMatrix>
    struct
    col_write_traits<Col,OriginalMatrix,false>
        {
        typedef qvm_detail::col_<Col,OriginalMatrix> this_vector;
        typedef typename mat_traits<OriginalMatrix>::scalar_type scalar_type;
        static int const dim=mat_traits<OriginalMatrix>::rows;
        BOOST_QVM_STATIC_ASSERT(Col>=0);
        BOOST_QVM_STATIC_ASSERT(Col<mat_traits<OriginalMatrix>::cols);

        template <int I>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        void
        write_element( this_vector & x, scalar_type s )
            {
            BOOST_QVM_STATIC_ASSERT(I>=0);
            BOOST_QVM_STATIC_ASSERT(I<dim);
            mat_traits<OriginalMatrix>::template write_element<I,Col>(reinterpret_cast<OriginalMatrix &>(x), s);
            }

        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        void
        write_element_idx( int i, this_vector & x, scalar_type s )
            {
            BOOST_QVM_ASSERT(i>=0);
            BOOST_QVM_ASSERT(i<dim);
            mat_traits<OriginalMatrix>::write_element_idx(i,Col,reinterpret_cast<OriginalMatrix &>(x), s);
            }
        };
    }

template <int Col,class OriginalMatrix>
struct
vec_traits< qvm_detail::col_<Col,OriginalMatrix> >:
    qvm_detail::col_write_traits<Col,OriginalMatrix>
    {
    typedef qvm_detail::col_<Col,OriginalMatrix> this_vector;
    typedef typename mat_traits<OriginalMatrix>::scalar_type scalar_type;
    static int const dim=mat_traits<OriginalMatrix>::rows;
    BOOST_QVM_STATIC_ASSERT(Col>=0);
    BOOST_QVM_STATIC_ASSERT(Col<mat_traits<OriginalMatrix>::cols);

    template <int I>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_vector const & x )
        {
        BOOST_QVM_STATIC_ASSERT(I>=0);
        BOOST_QVM_STATIC_ASSERT(I<dim);
        return mat_traits<OriginalMatrix>::template read_element<I,Col>(reinterpret_cast<OriginalMatrix const &>(x));
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int i, this_vector const & x )
        {
        BOOST_QVM_ASSERT(i>=0);
        BOOST_QVM_ASSERT(i<dim);
        return mat_traits<OriginalMatrix>::read_element_idx(i,Col,reinterpret_cast<OriginalMatrix const &>(x));
        }
    };

template <int Col,class OriginalMatrix,int D>
struct
deduce_vec<qvm_detail::col_<Col,OriginalMatrix>,D>
    {
    typedef vec<typename mat_traits<OriginalMatrix>::scalar_type,D> type;
    };

template <int Col,class OriginalMatrix,int D>
struct
deduce_vec2<qvm_detail::col_<Col,OriginalMatrix>,qvm_detail::col_<Col,OriginalMatrix>,D>
    {
    typedef vec<typename mat_traits<OriginalMatrix>::scalar_type,D> type;
    };

template <int Col,class A>
typename enable_if_c<
    is_mat<A>::value,
    qvm_detail::col_<Col,A> const &>::type
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
col( A const & a )
    {
    return reinterpret_cast<typename qvm_detail::col_<Col,A> const &>(a);
    }

template <int Col,class A>
typename enable_if_c<
    is_mat<A>::value,
    qvm_detail::col_<Col,A> &>::type
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
col( A & a )
    {
    return reinterpret_cast<typename qvm_detail::col_<Col,A> &>(a);
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int Row,class OriginalMatrix>
    class
    row_
        {
        row_( row_ const & );
        row_ & operator=( row_ const & );
        ~row_();

        public:

        template <class T>
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        row_ &
        operator=( T const & x )
            {
            assign(*this,x);
            return *this;
            }

        template <class R
#if __cplusplus >= 201103L
            , class = typename enable_if<is_vec<R> >::type
#endif
        >
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        operator R() const
            {
            R r;
            assign(r,*this);
            return r;
            }
        };

    template <int Row,class OriginalMatrix,bool WriteElementRef=mat_write_element_ref<OriginalMatrix>::value>
    struct row_write_traits;

    template <int Row,class OriginalMatrix>
    struct
    row_write_traits<Row,OriginalMatrix,true>
        {
        typedef qvm_detail::row_<Row,OriginalMatrix> this_vector;
        typedef typename mat_traits<OriginalMatrix>::scalar_type scalar_type;
        static int const dim=mat_traits<OriginalMatrix>::cols;
        BOOST_QVM_STATIC_ASSERT(Row>=0);
        BOOST_QVM_STATIC_ASSERT(Row<mat_traits<OriginalMatrix>::rows);

        template <int I>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        scalar_type &
        write_element( this_vector & x )
            {
            BOOST_QVM_STATIC_ASSERT(I>=0);
            BOOST_QVM_STATIC_ASSERT(I<dim);
            return mat_traits<OriginalMatrix>::template write_element<Row,I>(reinterpret_cast<OriginalMatrix &>(x));
            }

        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        scalar_type &
        write_element_idx( int i, this_vector & x )
            {
            BOOST_QVM_ASSERT(i>=0);
            BOOST_QVM_ASSERT(i<dim);
            return mat_traits<OriginalMatrix>::write_element_idx(Row,i,reinterpret_cast<OriginalMatrix &>(x));
            }
        };

    template <int Row,class OriginalMatrix>
    struct
    row_write_traits<Row,OriginalMatrix,false>
        {
        typedef qvm_detail::row_<Row,OriginalMatrix> this_vector;
        typedef typename mat_traits<OriginalMatrix>::scalar_type scalar_type;
        static int const dim=mat_traits<OriginalMatrix>::cols;
        BOOST_QVM_STATIC_ASSERT(Row>=0);
        BOOST_QVM_STATIC_ASSERT(Row<mat_traits<OriginalMatrix>::rows);

        template <int I>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        void
        write_element( this_vector & x, scalar_type s )
            {
            BOOST_QVM_STATIC_ASSERT(I>=0);
            BOOST_QVM_STATIC_ASSERT(I<dim);
            mat_traits<OriginalMatrix>::template write_element<Row,I>(reinterpret_cast<OriginalMatrix &>(x), s);
            }

        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        void
        write_element_idx( int i, this_vector & x, scalar_type s )
            {
            BOOST_QVM_ASSERT(i>=0);
            BOOST_QVM_ASSERT(i<dim);
            mat_traits<OriginalMatrix>::write_element_idx(Row,i,reinterpret_cast<OriginalMatrix &>(x), s);
            }
        };
    }

template <int Row,class OriginalMatrix>
struct
vec_traits< qvm_detail::row_<Row,OriginalMatrix> >:
    qvm_detail::row_write_traits<Row,OriginalMatrix>
    {
    typedef qvm_detail::row_<Row,OriginalMatrix> this_vector;
    typedef typename mat_traits<OriginalMatrix>::scalar_type scalar_type;
    static int const dim=mat_traits<OriginalMatrix>::cols;
    BOOST_QVM_STATIC_ASSERT(Row>=0);
    BOOST_QVM_STATIC_ASSERT(Row<mat_traits<OriginalMatrix>::rows);

    template <int I>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_vector const & x )
        {
        BOOST_QVM_STATIC_ASSERT(I>=0);
        BOOST_QVM_STATIC_ASSERT(I<dim);
        return mat_traits<OriginalMatrix>::template read_element<Row,I>(reinterpret_cast<OriginalMatrix const &>(x));
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int i, this_vector const & x )
        {
        BOOST_QVM_ASSERT(i>=0);
        BOOST_QVM_ASSERT(i<dim);
        return mat_traits<OriginalMatrix>::read_element_idx(Row,i,reinterpret_cast<OriginalMatrix const &>(x));
        }
    };

template <int Row,class OriginalMatrix,int D>
struct
deduce_vec<qvm_detail::row_<Row,OriginalMatrix>,D>
    {
    typedef vec<typename mat_traits<OriginalMatrix>::scalar_type,D> type;
    };

template <int Row,class OriginalMatrix,int D>
struct
deduce_vec2<qvm_detail::row_<Row,OriginalMatrix>,qvm_detail::row_<Row,OriginalMatrix>,D>
    {
    typedef vec<typename mat_traits<OriginalMatrix>::scalar_type,D> type;
    };

template <int Row,class A>
typename enable_if_c<
    is_mat<A>::value,
    qvm_detail::row_<Row,A> const &>::type
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
row( A const & a )
    {
    return reinterpret_cast<typename qvm_detail::row_<Row,A> const &>(a);
    }

template <int Row,class A>
typename enable_if_c<
    is_mat<A>::value,
    qvm_detail::row_<Row,A> &>::type
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
row( A & a )
    {
    return reinterpret_cast<typename qvm_detail::row_<Row,A> &>(a);
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <class OriginalMatrix>
    class
    diag_
        {
        diag_( diag_ const & );
        diag_ & operator=( diag_ const & );
        ~diag_();

        public:

        template <class T>
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        diag_ &
        operator=( T const & x )
            {
            assign(*this,x);
            return *this;
            }

        template <class R
#if __cplusplus >= 201103L
            , class = typename enable_if<is_vec<R> >::type
#endif
        >
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        operator R() const
            {
            R r;
            assign(r,*this);
            return r;
            }
        };

    template <int X,int Y,bool Which>
    struct diag_bool_dispatch;

    template <int X,int Y>
    struct
    diag_bool_dispatch<X,Y,true>
        {
        static int const value=X;
        };

    template <int X,int Y>
    struct
    diag_bool_dispatch<X,Y,false>
        {
        static int const value=Y;
        };

    template <class OriginalMatrix,bool WriteElementRef=mat_write_element_ref<OriginalMatrix>::value>
    struct diag_write_traits;

    template <class OriginalMatrix>
    struct
    diag_write_traits<OriginalMatrix,true>
        {
        typedef qvm_detail::diag_<OriginalMatrix> this_vector;
        typedef typename mat_traits<OriginalMatrix>::scalar_type scalar_type;
        static int const dim=qvm_detail::diag_bool_dispatch<
                mat_traits<OriginalMatrix>::rows,
                mat_traits<OriginalMatrix>::cols,
                mat_traits<OriginalMatrix>::rows<=mat_traits<OriginalMatrix>::cols>::value;

        template <int I>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        scalar_type &
        write_element( this_vector & x )
            {
            BOOST_QVM_STATIC_ASSERT(I>=0);
            BOOST_QVM_STATIC_ASSERT(I<dim);
            return mat_traits<OriginalMatrix>::template write_element<I,I>(reinterpret_cast<OriginalMatrix &>(x));
            }

        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        scalar_type &
        write_element_idx( int i, this_vector & x )
            {
            BOOST_QVM_ASSERT(i>=0);
            BOOST_QVM_ASSERT(i<dim);
            return mat_traits<OriginalMatrix>::write_element_idx(i,i,reinterpret_cast<OriginalMatrix &>(x));
            }
        };

    template <class OriginalMatrix>
    struct
    diag_write_traits<OriginalMatrix,false>
        {
        typedef qvm_detail::diag_<OriginalMatrix> this_vector;
        typedef typename mat_traits<OriginalMatrix>::scalar_type scalar_type;
        static int const dim=qvm_detail::diag_bool_dispatch<
                mat_traits<OriginalMatrix>::rows,
                mat_traits<OriginalMatrix>::cols,
                mat_traits<OriginalMatrix>::rows<=mat_traits<OriginalMatrix>::cols>::value;

        template <int I>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        void
        write_element( this_vector & x, scalar_type s )
            {
            BOOST_QVM_STATIC_ASSERT(I>=0);
            BOOST_QVM_STATIC_ASSERT(I<dim);
            mat_traits<OriginalMatrix>::template write_element<I,I>(reinterpret_cast<OriginalMatrix &>(x), s);
            }

        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        void
        write_element_idx( int i, this_vector & x, scalar_type s )
            {
            BOOST_QVM_ASSERT(i>=0);
            BOOST_QVM_ASSERT(i<dim);
            mat_traits<OriginalMatrix>::write_element_idx(i,i,reinterpret_cast<OriginalMatrix &>(x), s);
            }
        };
    }

template <class OriginalMatrix>
struct
vec_traits< qvm_detail::diag_<OriginalMatrix> >:
    qvm_detail::diag_write_traits<OriginalMatrix>
    {
    typedef qvm_detail::diag_<OriginalMatrix> this_vector;
    typedef typename mat_traits<OriginalMatrix>::scalar_type scalar_type;
    static int const dim=qvm_detail::diag_bool_dispatch<
            mat_traits<OriginalMatrix>::rows,
            mat_traits<OriginalMatrix>::cols,
            mat_traits<OriginalMatrix>::rows<=mat_traits<OriginalMatrix>::cols>::value;

    template <int I>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_vector const & x )
        {
        BOOST_QVM_STATIC_ASSERT(I>=0);
        BOOST_QVM_STATIC_ASSERT(I<dim);
        return mat_traits<OriginalMatrix>::template read_element<I,I>(reinterpret_cast<OriginalMatrix const &>(x));
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int i, this_vector const & x )
        {
        BOOST_QVM_ASSERT(i>=0);
        BOOST_QVM_ASSERT(i<dim);
        return mat_traits<OriginalMatrix>::read_element_idx(i,i,reinterpret_cast<OriginalMatrix const &>(x));
        }
    };

template <class OriginalMatrix,int D>
struct
deduce_vec<qvm_detail::diag_<OriginalMatrix>,D>
    {
    typedef vec<typename mat_traits<OriginalMatrix>::scalar_type,D> type;
    };

template <class OriginalMatrix,int D>
struct
deduce_vec2<qvm_detail::diag_<OriginalMatrix>,qvm_detail::diag_<OriginalMatrix>,D>
    {
    typedef vec<typename mat_traits<OriginalMatrix>::scalar_type,D> type;
    };

template <class A>
typename enable_if_c<
    is_mat<A>::value,
    qvm_detail::diag_<A> const &>::type
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
diag( A const & a )
    {
    return reinterpret_cast<typename qvm_detail::diag_<A> const &>(a);
    }

template <class A>
typename enable_if_c<
    is_mat<A>::value,
    qvm_detail::diag_<A> &>::type
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
diag( A & a )
    {
    return reinterpret_cast<typename qvm_detail::diag_<A> &>(a);
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <class OriginalMatrix>
    class
    translation_
        {
        translation_( translation_ const & );
        ~translation_();

        public:

        translation_ &
        operator=( translation_ const & x )
            {
            assign(*this,x);
            return *this;
            }

        template <class T>
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        translation_ &
        operator=( T const & x )
            {
            assign(*this,x);
            return *this;
            }

        template <class R
#if __cplusplus >= 201103L
            , class = typename enable_if<is_vec<R> >::type
#endif
        >
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        operator R() const
            {
            R r;
            assign(r,*this);
            return r;
            }
        };

    template <class OriginalMatrix,bool WriteElementRef=mat_write_element_ref<OriginalMatrix>::value>
    struct translation_write_traits;

    template <class OriginalMatrix>
    struct
    translation_write_traits<OriginalMatrix,true>
        {
        typedef qvm_detail::translation_<OriginalMatrix> this_vector;
        typedef typename mat_traits<OriginalMatrix>::scalar_type scalar_type;
        static int const dim=mat_traits<OriginalMatrix>::cols-1;
        BOOST_QVM_STATIC_ASSERT(mat_traits<OriginalMatrix>::rows==mat_traits<OriginalMatrix>::cols || mat_traits<OriginalMatrix>::rows+1==mat_traits<OriginalMatrix>::cols);
        BOOST_QVM_STATIC_ASSERT(mat_traits<OriginalMatrix>::cols>=3);

        template <int I>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        scalar_type &
        write_element( this_vector & x )
            {
            BOOST_QVM_STATIC_ASSERT(I>=0);
            BOOST_QVM_STATIC_ASSERT(I<dim);
            return mat_traits<OriginalMatrix>::template write_element<I,dim>(reinterpret_cast<OriginalMatrix &>(x));
            }

        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        scalar_type &
        write_element_idx( int i, this_vector & x )
            {
            BOOST_QVM_ASSERT(i>=0);
            BOOST_QVM_ASSERT(i<dim);
            return mat_traits<OriginalMatrix>::write_element_idx(i,dim,reinterpret_cast<OriginalMatrix &>(x));
            }
        };

    template <class OriginalMatrix>
    struct
    translation_write_traits<OriginalMatrix,false>
        {
        typedef qvm_detail::translation_<OriginalMatrix> this_vector;
        typedef typename mat_traits<OriginalMatrix>::scalar_type scalar_type;
        static int const dim=mat_traits<OriginalMatrix>::cols-1;
        BOOST_QVM_STATIC_ASSERT(mat_traits<OriginalMatrix>::rows==mat_traits<OriginalMatrix>::cols || mat_traits<OriginalMatrix>::rows+1==mat_traits<OriginalMatrix>::cols);
        BOOST_QVM_STATIC_ASSERT(mat_traits<OriginalMatrix>::cols>=3);

        template <int I>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        void
        write_element( this_vector & x, scalar_type s )
            {
            BOOST_QVM_STATIC_ASSERT(I>=0);
            BOOST_QVM_STATIC_ASSERT(I<dim);
            mat_traits<OriginalMatrix>::template write_element<I,dim>(reinterpret_cast<OriginalMatrix &>(x), s);
            }

        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        void
        write_element_idx( int i, this_vector & x, scalar_type s )
            {
            BOOST_QVM_ASSERT(i>=0);
            BOOST_QVM_ASSERT(i<dim);
            mat_traits<OriginalMatrix>::write_element_idx(i,dim,reinterpret_cast<OriginalMatrix &>(x), s);
            }
        };
    }

template <class OriginalMatrix>
struct
vec_traits< qvm_detail::translation_<OriginalMatrix> >:
    qvm_detail::translation_write_traits<OriginalMatrix>
    {
    typedef qvm_detail::translation_<OriginalMatrix> this_vector;
    typedef typename mat_traits<OriginalMatrix>::scalar_type scalar_type;
    static int const dim=mat_traits<OriginalMatrix>::cols-1;
        BOOST_QVM_STATIC_ASSERT(mat_traits<OriginalMatrix>::rows==mat_traits<OriginalMatrix>::cols || mat_traits<OriginalMatrix>::rows+1==mat_traits<OriginalMatrix>::cols);
    BOOST_QVM_STATIC_ASSERT(mat_traits<OriginalMatrix>::cols>=3);

    template <int I>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_vector const & x )
        {
        BOOST_QVM_STATIC_ASSERT(I>=0);
        BOOST_QVM_STATIC_ASSERT(I<dim);
        return mat_traits<OriginalMatrix>::template read_element<I,dim>(reinterpret_cast<OriginalMatrix const &>(x));
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int i, this_vector const & x )
        {
        BOOST_QVM_ASSERT(i>=0);
        BOOST_QVM_ASSERT(i<dim);
        return mat_traits<OriginalMatrix>::read_element_idx(i,dim,reinterpret_cast<OriginalMatrix const &>(x));
        }
    };

template <class OriginalMatrix,int D>
struct
deduce_vec<qvm_detail::translation_<OriginalMatrix>,D>
    {
    typedef vec<typename mat_traits<OriginalMatrix>::scalar_type,D> type;
    };

template <class OriginalMatrix,int D>
struct
deduce_vec2<qvm_detail::translation_<OriginalMatrix>,qvm_detail::translation_<OriginalMatrix>,D>
    {
    typedef vec<typename mat_traits<OriginalMatrix>::scalar_type,D> type;
    };

template <class A>
typename enable_if_c<
    is_mat<A>::value && (mat_traits<A>::rows==mat_traits<A>::cols || mat_traits<A>::rows+1==mat_traits<A>::cols) && mat_traits<A>::cols>=3,
    qvm_detail::translation_<A> const &>::type
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
translation( A const & a )
    {
    return reinterpret_cast<typename qvm_detail::translation_<A> const &>(a);
    }

template <class A>
typename enable_if_c<
    is_mat<A>::value && (mat_traits<A>::rows==mat_traits<A>::cols || mat_traits<A>::rows+1==mat_traits<A>::cols) && mat_traits<A>::cols>=3,
    qvm_detail::translation_<A> &>::type
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
translation( A & a )
    {
    return reinterpret_cast<typename qvm_detail::translation_<A> &>(a);
    }

} }

#endif
// <<< #include <boost/qvm/map_mat_vec.hpp>
// #line 10 "boost/qvm/map.hpp"
// >>> #include <boost/qvm/map_mat_mat.hpp>
#ifndef BOOST_QVM_MAP_MAT_MAT_HPP_INCLUDED
#define BOOST_QVM_MAP_MAT_MAT_HPP_INCLUDED

// #line 8 "boost/qvm/map_mat_mat.hpp"
// #include <boost/qvm/config.hpp> // Expanded at line 109
// #include <boost/qvm/deduce_mat.hpp> // Expanded at line 899
// #include <boost/qvm/assert.hpp> // Expanded at line 887
// #include <boost/qvm/enable_if.hpp> // Expanded at line 177
// #include <boost/qvm/detail/transp_impl.hpp> // Expanded at line 7903

namespace boost { namespace qvm {

namespace
qvm_detail
    {
    template <int Row,class OriginalMatrix>
    class
    del_row_
        {
        del_row_( del_row_ const & );
        del_row_ & operator=( del_row_ const & );
        ~del_row_();

        public:

        template <class T>
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        del_row_ &
        operator=( T const & x )
            {
            assign(*this,x);
            return *this;
            }

        template <class R
#if __cplusplus >= 201103L
            , class = typename enable_if<is_mat<R> >::type
#endif
        >
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        operator R() const
            {
            R r;
            assign(r,*this);
            return r;
            }
        };

    template <int I,class OriginalMatrix,bool WriteElementRef=mat_write_element_ref<OriginalMatrix>::value>
    struct del_row_write_traits;

    template <int I,class OriginalMatrix>
    struct
    del_row_write_traits<I,OriginalMatrix,true>
        {
        typedef del_row_<I,OriginalMatrix> this_matrix;
        typedef typename mat_traits<OriginalMatrix>::scalar_type scalar_type;
        static int const rows=mat_traits<OriginalMatrix>::rows-1;
        static int const cols=mat_traits<OriginalMatrix>::cols;

        template <int Row,int Col>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        scalar_type &
        write_element( this_matrix & x )
            {
            BOOST_QVM_STATIC_ASSERT(Row>=0);
            BOOST_QVM_STATIC_ASSERT(Row<rows);
            BOOST_QVM_STATIC_ASSERT(Col>=0);
            BOOST_QVM_STATIC_ASSERT(Col<cols);
            return mat_traits<OriginalMatrix>::template write_element<Row+(Row>=I),Col>(reinterpret_cast<OriginalMatrix &>(x));
            }

        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        scalar_type &
        write_element_idx( int row, int col, this_matrix & x )
            {
            BOOST_QVM_ASSERT(row>=0);
            BOOST_QVM_ASSERT(row<rows);
            BOOST_QVM_ASSERT(col>=0);
            BOOST_QVM_ASSERT(col<cols);
            return mat_traits<OriginalMatrix>::write_element_idx(row+(row>=I),col,reinterpret_cast<OriginalMatrix &>(x));
            }
        };

    template <int I,class OriginalMatrix>
    struct
    del_row_write_traits<I,OriginalMatrix,false>
        {
        typedef del_row_<I,OriginalMatrix> this_matrix;
        typedef typename mat_traits<OriginalMatrix>::scalar_type scalar_type;
        static int const rows=mat_traits<OriginalMatrix>::rows-1;
        static int const cols=mat_traits<OriginalMatrix>::cols;

        template <int Row,int Col>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        void
        write_element( this_matrix & x, scalar_type s )
            {
            BOOST_QVM_STATIC_ASSERT(Row>=0);
            BOOST_QVM_STATIC_ASSERT(Row<rows);
            BOOST_QVM_STATIC_ASSERT(Col>=0);
            BOOST_QVM_STATIC_ASSERT(Col<cols);
            mat_traits<OriginalMatrix>::template write_element<Row+(Row>=I),Col>(reinterpret_cast<OriginalMatrix &>(x), s);
            }

        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        void
        write_element_idx( int row, int col, this_matrix & x, scalar_type s )
            {
            BOOST_QVM_ASSERT(row>=0);
            BOOST_QVM_ASSERT(row<rows);
            BOOST_QVM_ASSERT(col>=0);
            BOOST_QVM_ASSERT(col<cols);
            mat_traits<OriginalMatrix>::write_element_idx(row+(row>=I),col,reinterpret_cast<OriginalMatrix &>(x), s);
            }
        };
    }

template <int I,class OriginalMatrix>
struct
mat_traits<qvm_detail::del_row_<I,OriginalMatrix> >:
    qvm_detail::del_row_write_traits<I,OriginalMatrix>
    {
    typedef qvm_detail::del_row_<I,OriginalMatrix> this_matrix;
    typedef typename mat_traits<OriginalMatrix>::scalar_type scalar_type;
    static int const rows=mat_traits<OriginalMatrix>::rows-1;
    static int const cols=mat_traits<OriginalMatrix>::cols;

    template <int Row,int Col>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_matrix const & x )
        {
        BOOST_QVM_STATIC_ASSERT(Row>=0);
        BOOST_QVM_STATIC_ASSERT(Row<rows);
        BOOST_QVM_STATIC_ASSERT(Col>=0);
        BOOST_QVM_STATIC_ASSERT(Col<cols);
        return mat_traits<OriginalMatrix>::template read_element<Row+(Row>=I),Col>(reinterpret_cast<OriginalMatrix const &>(x));
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int row, int col, this_matrix const & x )
        {
        BOOST_QVM_ASSERT(row>=0);
        BOOST_QVM_ASSERT(row<rows);
        BOOST_QVM_ASSERT(col>=0);
        BOOST_QVM_ASSERT(col<cols);
        return mat_traits<OriginalMatrix>::read_element_idx(row+(row>=I),col,reinterpret_cast<OriginalMatrix const &>(x));
        }
    };

template <int J,class OriginalMatrix,int R,int C>
struct
deduce_mat<qvm_detail::del_row_<J,OriginalMatrix>,R,C>
    {
    typedef mat<typename mat_traits<OriginalMatrix>::scalar_type,R,C> type;
    };

template <int J,class OriginalMatrix,int R,int C>
struct
deduce_mat2<qvm_detail::del_row_<J,OriginalMatrix>,qvm_detail::del_row_<J,OriginalMatrix>,R,C>
    {
    typedef mat<typename mat_traits<OriginalMatrix>::scalar_type,R,C> type;
    };

template <int Row,class A>
typename enable_if_c<
    is_mat<A>::value,
    qvm_detail::del_row_<Row,A> const &>::type
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
del_row( A const & a )
    {
    return reinterpret_cast<typename qvm_detail::del_row_<Row,A> const &>(a);
    }

template <int Row,class A>
typename enable_if_c<
    is_mat<A>::value,
    qvm_detail::del_row_<Row,A> &>::type
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
del_row( A & a )
    {
    return reinterpret_cast<typename qvm_detail::del_row_<Row,A> &>(a);
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int Col,class OriginalMatrix>
    class
    del_col_
        {
        del_col_( del_col_ const & );
        del_col_ & operator=( del_col_ const & );
        ~del_col_();

        public:

        template <class T>
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        del_col_ &
        operator=( T const & x )
            {
            assign(*this,x);
            return *this;
            }

        template <class R
#if __cplusplus >= 201103L
            , class = typename enable_if<is_mat<R> >::type
#endif
        >
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        operator R() const
            {
            R r;
            assign(r,*this);
            return r;
            }
        };

    template <int J,class OriginalMatrix,bool WriteElementRef=mat_write_element_ref<OriginalMatrix>::value>
    struct del_col_write_traits;

    template <int J,class OriginalMatrix>
    struct
    del_col_write_traits<J,OriginalMatrix,true>
        {
        typedef del_col_<J,OriginalMatrix> this_matrix;
        typedef typename mat_traits<OriginalMatrix>::scalar_type scalar_type;
        static int const rows=mat_traits<OriginalMatrix>::rows;
        static int const cols=mat_traits<OriginalMatrix>::cols-1;

        template <int Row,int Col>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        scalar_type &
        write_element( this_matrix & x )
            {
            BOOST_QVM_STATIC_ASSERT(Row>=0);
            BOOST_QVM_STATIC_ASSERT(Row<rows);
            BOOST_QVM_STATIC_ASSERT(Col>=0);
            BOOST_QVM_STATIC_ASSERT(Col<cols);
            return mat_traits<OriginalMatrix>::template write_element<Row,Col+(Col>=J)>(reinterpret_cast<OriginalMatrix &>(x));
            }

        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        scalar_type &
        write_element_idx( int row, int col, this_matrix & x )
            {
            BOOST_QVM_ASSERT(row>=0);
            BOOST_QVM_ASSERT(row<rows);
            BOOST_QVM_ASSERT(col>=0);
            BOOST_QVM_ASSERT(col<cols);
            return mat_traits<OriginalMatrix>::write_element_idx(row,col+(col>=J),reinterpret_cast<OriginalMatrix &>(x));
            }
        };

    template <int J,class OriginalMatrix>
    struct
    del_col_write_traits<J,OriginalMatrix,false>
        {
        typedef del_col_<J,OriginalMatrix> this_matrix;
        typedef typename mat_traits<OriginalMatrix>::scalar_type scalar_type;
        static int const rows=mat_traits<OriginalMatrix>::rows;
        static int const cols=mat_traits<OriginalMatrix>::cols-1;

        template <int Row,int Col>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        void
        write_element( this_matrix & x, scalar_type s )
            {
            BOOST_QVM_STATIC_ASSERT(Row>=0);
            BOOST_QVM_STATIC_ASSERT(Row<rows);
            BOOST_QVM_STATIC_ASSERT(Col>=0);
            BOOST_QVM_STATIC_ASSERT(Col<cols);
            mat_traits<OriginalMatrix>::template write_element<Row,Col+(Col>=J)>(reinterpret_cast<OriginalMatrix &>(x), s);
            }

        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        void
        write_element_idx( int row, int col, this_matrix & x, scalar_type s )
            {
            BOOST_QVM_ASSERT(row>=0);
            BOOST_QVM_ASSERT(row<rows);
            BOOST_QVM_ASSERT(col>=0);
            BOOST_QVM_ASSERT(col<cols);
            mat_traits<OriginalMatrix>::write_element_idx(row,col+(col>=J),reinterpret_cast<OriginalMatrix &>(x), s);
            }
        };
    }

template <int J,class OriginalMatrix>
struct
mat_traits<qvm_detail::del_col_<J,OriginalMatrix> >:
    qvm_detail::del_col_write_traits<J,OriginalMatrix>
    {
    typedef qvm_detail::del_col_<J,OriginalMatrix> this_matrix;
    typedef typename mat_traits<OriginalMatrix>::scalar_type scalar_type;
    static int const rows=mat_traits<OriginalMatrix>::rows;
    static int const cols=mat_traits<OriginalMatrix>::cols-1;

    template <int Row,int Col>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_matrix const & x )
        {
        BOOST_QVM_STATIC_ASSERT(Row>=0);
        BOOST_QVM_STATIC_ASSERT(Row<rows);
        BOOST_QVM_STATIC_ASSERT(Col>=0);
        BOOST_QVM_STATIC_ASSERT(Col<cols);
        return mat_traits<OriginalMatrix>::template read_element<Row,Col+(Col>=J)>(reinterpret_cast<OriginalMatrix const &>(x));
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int row, int col, this_matrix const & x )
        {
        BOOST_QVM_ASSERT(row>=0);
        BOOST_QVM_ASSERT(row<rows);
        BOOST_QVM_ASSERT(col>=0);
        BOOST_QVM_ASSERT(col<cols);
        return mat_traits<OriginalMatrix>::read_element_idx(row,col+(col>=J),reinterpret_cast<OriginalMatrix const &>(x));
        }
    };

template <int J,class OriginalMatrix,int R,int C>
struct
deduce_mat<qvm_detail::del_col_<J,OriginalMatrix>,R,C>
    {
    typedef mat<typename mat_traits<OriginalMatrix>::scalar_type,R,C> type;
    };

template <int J,class OriginalMatrix,int R,int C>
struct
deduce_mat2<qvm_detail::del_col_<J,OriginalMatrix>,qvm_detail::del_col_<J,OriginalMatrix>,R,C>
    {
    typedef mat<typename mat_traits<OriginalMatrix>::scalar_type,R,C> type;
    };

template <int Col,class A>
typename enable_if_c<
    is_mat<A>::value,
    qvm_detail::del_col_<Col,A> const &>::type
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
del_col( A const & a )
    {
    return reinterpret_cast<typename qvm_detail::del_col_<Col,A> const &>(a);
    }

template <int Col,class A>
typename enable_if_c<
    is_mat<A>::value,
    qvm_detail::del_col_<Col,A> &>::type
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
del_col( A & a )
    {
    return reinterpret_cast<typename qvm_detail::del_col_<Col,A> &>(a);
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int Row,int Col,class OriginalMatrix>
    class
    del_row_col_
        {
        del_row_col_( del_row_col_ const & );
        ~del_row_col_();

        public:

        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        del_row_col_ &
        operator=( del_row_col_ const & x )
            {
            assign(*this,x);
            return *this;
            }

        template <class T>
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        del_row_col_ &
        operator=( T const & x )
            {
            assign(*this,x);
            return *this;
            }

        template <class R
#if __cplusplus >= 201103L
            , class = typename enable_if<is_mat<R> >::type
#endif
        >
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        operator R() const
            {
            R r;
            assign(r,*this);
            return r;
            }
        };

    template <int I,int J,class OriginalMatrix,bool WriteElementRef=mat_write_element_ref<OriginalMatrix>::value>
    struct del_row_col_write_traits;

    template <int I,int J,class OriginalMatrix>
    struct
    del_row_col_write_traits<I,J,OriginalMatrix,true>
        {
        typedef del_row_col_<I,J,OriginalMatrix> this_matrix;
        typedef typename mat_traits<OriginalMatrix>::scalar_type scalar_type;
        static int const rows=mat_traits<OriginalMatrix>::rows-1;
        static int const cols=mat_traits<OriginalMatrix>::cols-1;

        template <int Row,int Col>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        scalar_type &
        write_element( this_matrix & x )
            {
            BOOST_QVM_STATIC_ASSERT(Row>=0);
            BOOST_QVM_STATIC_ASSERT(Row<rows);
            BOOST_QVM_STATIC_ASSERT(Col>=0);
            BOOST_QVM_STATIC_ASSERT(Col<cols);
            return mat_traits<OriginalMatrix>::template write_element<Row+(Row>=I),Col+(Col>=J)>(reinterpret_cast<OriginalMatrix &>(x));
            }

        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        scalar_type &
        write_element_idx( int row, int col, this_matrix & x )
            {
            BOOST_QVM_ASSERT(row>=0);
            BOOST_QVM_ASSERT(row<rows);
            BOOST_QVM_ASSERT(col>=0);
            BOOST_QVM_ASSERT(col<cols);
            return mat_traits<OriginalMatrix>::write_element_idx(row+(row>=I),col+(col>=J),reinterpret_cast<OriginalMatrix &>(x));
            }
        };

    template <int I,int J,class OriginalMatrix>
    struct
    del_row_col_write_traits<I,J,OriginalMatrix,false>
        {
        typedef del_row_col_<I,J,OriginalMatrix> this_matrix;
        typedef typename mat_traits<OriginalMatrix>::scalar_type scalar_type;
        static int const rows=mat_traits<OriginalMatrix>::rows-1;
        static int const cols=mat_traits<OriginalMatrix>::cols-1;

        template <int Row,int Col>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        void
        write_element( this_matrix & x, scalar_type s )
            {
            BOOST_QVM_STATIC_ASSERT(Row>=0);
            BOOST_QVM_STATIC_ASSERT(Row<rows);
            BOOST_QVM_STATIC_ASSERT(Col>=0);
            BOOST_QVM_STATIC_ASSERT(Col<cols);
            mat_traits<OriginalMatrix>::template write_element<Row+(Row>=I),Col+(Col>=J)>(reinterpret_cast<OriginalMatrix &>(x), s);
            }

        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        void
        write_element_idx( int row, int col, this_matrix & x, scalar_type s )
            {
            BOOST_QVM_ASSERT(row>=0);
            BOOST_QVM_ASSERT(row<rows);
            BOOST_QVM_ASSERT(col>=0);
            BOOST_QVM_ASSERT(col<cols);
            mat_traits<OriginalMatrix>::write_element_idx(row+(row>=I),col+(col>=J),reinterpret_cast<OriginalMatrix &>(x), s);
            }
        };
    }

template <int I,int J,class OriginalMatrix>
struct
mat_traits<qvm_detail::del_row_col_<I,J,OriginalMatrix> >:
    qvm_detail::del_row_col_write_traits<I,J,OriginalMatrix>
    {
    typedef qvm_detail::del_row_col_<I,J,OriginalMatrix> this_matrix;
    typedef typename mat_traits<OriginalMatrix>::scalar_type scalar_type;
    static int const rows=mat_traits<OriginalMatrix>::rows-1;
    static int const cols=mat_traits<OriginalMatrix>::cols-1;

    template <int Row,int Col>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_matrix const & x )
        {
        BOOST_QVM_STATIC_ASSERT(Row>=0);
        BOOST_QVM_STATIC_ASSERT(Row<rows);
        BOOST_QVM_STATIC_ASSERT(Col>=0);
        BOOST_QVM_STATIC_ASSERT(Col<cols);
        return mat_traits<OriginalMatrix>::template read_element<Row+(Row>=I),Col+(Col>=J)>(reinterpret_cast<OriginalMatrix const &>(x));
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int row, int col, this_matrix const & x )
        {
        BOOST_QVM_ASSERT(row>=0);
        BOOST_QVM_ASSERT(row<rows);
        BOOST_QVM_ASSERT(col>=0);
        BOOST_QVM_ASSERT(col<cols);
        return mat_traits<OriginalMatrix>::read_element_idx(row+(row>=I),col+(col>=J),reinterpret_cast<OriginalMatrix const &>(x));
        }
    };

template <int I,int J,class OriginalMatrix,int R,int C>
struct
deduce_mat<qvm_detail::del_row_col_<I,J,OriginalMatrix>,R,C>
    {
    typedef mat<typename mat_traits<OriginalMatrix>::scalar_type,R,C> type;
    };

template <int I,int J,class OriginalMatrix,int R,int C>
struct
deduce_mat2<qvm_detail::del_row_col_<I,J,OriginalMatrix>,qvm_detail::del_row_col_<I,J,OriginalMatrix>,R,C>
    {
    typedef mat<typename mat_traits<OriginalMatrix>::scalar_type,R,C> type;
    };

template <int Row,int Col,class A>
typename enable_if_c<
    is_mat<A>::value,
    qvm_detail::del_row_col_<Row,Col,A> const &>::type
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
del_row_col( A const & a )
    {
    return reinterpret_cast<typename qvm_detail::del_row_col_<Row,Col,A> const &>(a);
    }

template <int Row,int Col,class A>
typename enable_if_c<
    is_mat<A>::value,
    qvm_detail::del_row_col_<Row,Col,A> &>::type
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
del_row_col( A & a )
    {
    return reinterpret_cast<typename qvm_detail::del_row_col_<Row,Col,A> &>(a);
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int Row,class OriginalMatrix>
    class
    neg_row_
        {
        neg_row_( neg_row_ const & );
        neg_row_ & operator=( neg_row_ const & );
        ~neg_row_();

        public:

        template <class T>
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        neg_row_ &
        operator=( T const & x )
            {
            assign(*this,x);
            return *this;
            }

        template <class R
#if __cplusplus >= 201103L
            , class = typename enable_if<is_mat<R> >::type
#endif
        >
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        operator R() const
            {
            R r;
            assign(r,*this);
            return r;
            }
        };
    }

template <int I,class OriginalMatrix>
struct
mat_traits<qvm_detail::neg_row_<I,OriginalMatrix> >
    {
    typedef qvm_detail::neg_row_<I,OriginalMatrix> this_matrix;
    typedef typename mat_traits<OriginalMatrix>::scalar_type scalar_type;
    static int const rows=mat_traits<OriginalMatrix>::rows;
    static int const cols=mat_traits<OriginalMatrix>::cols;

    template <int Row,int Col>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_matrix const & x )
        {
        BOOST_QVM_STATIC_ASSERT(Row>=0);
        BOOST_QVM_STATIC_ASSERT(Row<rows);
        BOOST_QVM_STATIC_ASSERT(Col>=0);
        BOOST_QVM_STATIC_ASSERT(Col<cols);
        return Row==I ?
            -mat_traits<OriginalMatrix>::template read_element<Row,Col>(reinterpret_cast<OriginalMatrix const &>(x)) :
            mat_traits<OriginalMatrix>::template read_element<Row,Col>(reinterpret_cast<OriginalMatrix const &>(x));
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int row, int col, this_matrix const & x )
        {
        BOOST_QVM_ASSERT(row>=0);
        BOOST_QVM_ASSERT(row<rows);
        BOOST_QVM_ASSERT(col>=0);
        BOOST_QVM_ASSERT(col<cols);
        return row==I?
            -mat_traits<OriginalMatrix>::read_element_idx(row,col,reinterpret_cast<OriginalMatrix const &>(x)) :
            mat_traits<OriginalMatrix>::read_element_idx(row,col,reinterpret_cast<OriginalMatrix const &>(x));
        }
    };

template <int J,class OriginalMatrix,int R,int C>
struct
deduce_mat<qvm_detail::neg_row_<J,OriginalMatrix>,R,C>
    {
    typedef mat<typename mat_traits<OriginalMatrix>::scalar_type,R,C> type;
    };

template <int J,class OriginalMatrix,int R,int C>
struct
deduce_mat2<qvm_detail::neg_row_<J,OriginalMatrix>,qvm_detail::neg_row_<J,OriginalMatrix>,R,C>
    {
    typedef mat<typename mat_traits<OriginalMatrix>::scalar_type,R,C> type;
    };

template <int Row,class A>
typename enable_if_c<
    is_mat<A>::value,
    qvm_detail::neg_row_<Row,A> const &>::type
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
neg_row( A const & a )
    {
    return reinterpret_cast<typename qvm_detail::neg_row_<Row,A> const &>(a);
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int Col,class OriginalMatrix>
    class
    neg_col_
        {
        neg_col_( neg_col_ const & );
        neg_col_ & operator=( neg_col_ const & );
        ~neg_col_();

        public:

        template <class T>
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        neg_col_ &
        operator=( T const & x )
            {
            assign(*this,x);
            return *this;
            }

        template <class R
#if __cplusplus >= 201103L
            , class = typename enable_if<is_mat<R> >::type
#endif
        >
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        operator R() const
            {
            R r;
            assign(r,*this);
            return r;
            }
        };
    }

template <int J,class OriginalMatrix>
struct
mat_traits<qvm_detail::neg_col_<J,OriginalMatrix> >
    {
    typedef qvm_detail::neg_col_<J,OriginalMatrix> this_matrix;
    typedef typename mat_traits<OriginalMatrix>::scalar_type scalar_type;
    static int const rows=mat_traits<OriginalMatrix>::rows;
    static int const cols=mat_traits<OriginalMatrix>::cols;

    template <int Row,int Col>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_matrix const & x )
        {
        BOOST_QVM_STATIC_ASSERT(Row>=0);
        BOOST_QVM_STATIC_ASSERT(Row<rows);
        BOOST_QVM_STATIC_ASSERT(Col>=0);
        BOOST_QVM_STATIC_ASSERT(Col<cols);
        return Col==J?
            -mat_traits<OriginalMatrix>::template read_element<Row,Col>(reinterpret_cast<OriginalMatrix const &>(x)) :
            mat_traits<OriginalMatrix>::template read_element<Row,Col>(reinterpret_cast<OriginalMatrix const &>(x));
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int row, int col, this_matrix const & x )
        {
        BOOST_QVM_ASSERT(row>=0);
        BOOST_QVM_ASSERT(row<rows);
        BOOST_QVM_ASSERT(col>=0);
        BOOST_QVM_ASSERT(col<cols);
        return col==J?
            -mat_traits<OriginalMatrix>::read_element_idx(row,col,reinterpret_cast<OriginalMatrix const &>(x)) :
            mat_traits<OriginalMatrix>::read_element_idx(row,col,reinterpret_cast<OriginalMatrix const &>(x));
        }
    };

template <int J,class OriginalMatrix,int R,int C>
struct
deduce_mat<qvm_detail::neg_col_<J,OriginalMatrix>,R,C>
    {
    typedef mat<typename mat_traits<OriginalMatrix>::scalar_type,R,C> type;
    };

template <int J,class OriginalMatrix,int R,int C>
struct
deduce_mat2<qvm_detail::neg_col_<J,OriginalMatrix>,qvm_detail::neg_col_<J,OriginalMatrix>,R,C>
    {
    typedef mat<typename mat_traits<OriginalMatrix>::scalar_type,R,C> type;
    };

template <int Col,class A>
typename enable_if_c<
    is_mat<A>::value,
    qvm_detail::neg_col_<Col,A> const &>::type
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
neg_col( A const & a )
    {
    return reinterpret_cast<typename qvm_detail::neg_col_<Col,A> const &>(a);
    }

////////////////////////////////////////////////

template <class A>
typename enable_if_c<
    is_mat<A>::value,
    qvm_detail::transposed_<A> const &>::type
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
transposed( A const & a )
    {
    return reinterpret_cast<typename qvm_detail::transposed_<A> const &>(a);
    }

template <class A>
typename enable_if_c<
    is_mat<A>::value,
    qvm_detail::transposed_<A> &>::type
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
transposed( A & a )
    {
    return reinterpret_cast<typename qvm_detail::transposed_<A> &>(a);
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int Row1,int Row2,class OriginalMatrix>
    class
    swap_rows_
        {
        swap_rows_( swap_rows_ const & );
        swap_rows_ & operator=( swap_rows_ const & );
        ~swap_rows_();

        public:

        template <class T>
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        swap_rows_ &
        operator=( T const & x )
            {
            assign(*this,x);
            return *this;
            }

        template <class R
#if __cplusplus >= 201103L
            , class = typename enable_if<is_mat<R> >::type
#endif
        >
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        operator R() const
            {
            R r;
            assign(r,*this);
            return r;
            }
        };

    template <int R1,int R2,class OriginalMatrix,bool WriteElementRef=mat_write_element_ref<OriginalMatrix>::value>
    struct swap_rows_write_traits;

    template <int R1,int R2,class OriginalMatrix>
    struct
    swap_rows_write_traits<R1,R2,OriginalMatrix,true>
        {
        typedef swap_rows_<R1,R2,OriginalMatrix> this_matrix;
        typedef typename mat_traits<OriginalMatrix>::scalar_type scalar_type;
        static int const rows=mat_traits<OriginalMatrix>::rows;
        static int const cols=mat_traits<OriginalMatrix>::cols;

        template <int Row,int Col>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        scalar_type &
        write_element( this_matrix & x )
            {
            BOOST_QVM_STATIC_ASSERT(Row>=0);
            BOOST_QVM_STATIC_ASSERT(Row<rows);
            BOOST_QVM_STATIC_ASSERT(Col>=0);
            BOOST_QVM_STATIC_ASSERT(Col<cols);
            return mat_traits<OriginalMatrix>::template write_element<(Row==R1 && R1!=R2)*R2+(Row==R2 && R1!=R2)*R1+((Row!=R1 && Row!=R2) || R1==R2)*Row,Col>(reinterpret_cast<OriginalMatrix &>(x));
            }

        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        scalar_type &
        write_element_idx( int row, int col, this_matrix & x )
            {
            BOOST_QVM_ASSERT(row>=0);
            BOOST_QVM_ASSERT(row<rows);
            BOOST_QVM_ASSERT(col>=0);
            BOOST_QVM_ASSERT(col<cols);
            return mat_traits<OriginalMatrix>::write_element_idx(row==R1?R2:row==R2?R1:row,col,reinterpret_cast<OriginalMatrix &>(x));
            }
        };

    template <int R1,int R2,class OriginalMatrix>
    struct
        swap_rows_write_traits<R1,R2,OriginalMatrix,false>
        {
        typedef swap_rows_<R1,R2,OriginalMatrix> this_matrix;
        typedef typename mat_traits<OriginalMatrix>::scalar_type scalar_type;
        static int const rows=mat_traits<OriginalMatrix>::rows;
        static int const cols=mat_traits<OriginalMatrix>::cols;

        template <int Row,int Col>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        void
        write_element( this_matrix & x, scalar_type s )
            {
            BOOST_QVM_STATIC_ASSERT(Row>=0);
            BOOST_QVM_STATIC_ASSERT(Row<rows);
            BOOST_QVM_STATIC_ASSERT(Col>=0);
            BOOST_QVM_STATIC_ASSERT(Col<cols);
            mat_traits<OriginalMatrix>::template write_element<(Row==R1 && R1!=R2)*R2+(Row==R2 && R1!=R2)*R1+((Row!=R1 && Row!=R2) || R1==R2)*Row,Col>(reinterpret_cast<OriginalMatrix &>(x), s);
            }

        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        void
        write_element_idx( int row, int col, this_matrix & x, scalar_type s )
            {
            BOOST_QVM_ASSERT(row>=0);
            BOOST_QVM_ASSERT(row<rows);
            BOOST_QVM_ASSERT(col>=0);
            BOOST_QVM_ASSERT(col<cols);
            mat_traits<OriginalMatrix>::write_element_idx(row==R1?R2:row==R2?R1:row,col,reinterpret_cast<OriginalMatrix &>(x), s);
            }
        };
    }

template <int R1,int R2,class OriginalMatrix>
struct
mat_traits<qvm_detail::swap_rows_<R1,R2,OriginalMatrix> >:
    qvm_detail::swap_rows_write_traits<R1,R2,OriginalMatrix>
    {
    typedef qvm_detail::swap_rows_<R1,R2,OriginalMatrix> this_matrix;
    typedef typename mat_traits<OriginalMatrix>::scalar_type scalar_type;
    static int const rows=mat_traits<OriginalMatrix>::rows;
    static int const cols=mat_traits<OriginalMatrix>::cols;

    template <int Row,int Col>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_matrix const & x )
        {
        BOOST_QVM_STATIC_ASSERT(Row>=0);
        BOOST_QVM_STATIC_ASSERT(Row<rows);
        BOOST_QVM_STATIC_ASSERT(Col>=0);
        BOOST_QVM_STATIC_ASSERT(Col<cols);
        return mat_traits<OriginalMatrix>::template read_element<(Row==R1 && R1!=R2)*R2+(Row==R2 && R1!=R2)*R1+((Row!=R1 && Row!=R2) || R1==R2)*Row,Col>(reinterpret_cast<OriginalMatrix const &>(x));
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int row, int col, this_matrix const & x )
        {
        BOOST_QVM_ASSERT(row>=0);
        BOOST_QVM_ASSERT(row<rows);
        BOOST_QVM_ASSERT(col>=0);
        BOOST_QVM_ASSERT(col<cols);
        return mat_traits<OriginalMatrix>::read_element_idx(row==R1?R2:row==R2?R1:row,col,reinterpret_cast<OriginalMatrix const &>(x));
        }
    };

template <int R1,int R2,class OriginalMatrix,int R,int C>
struct
deduce_mat<qvm_detail::swap_rows_<R1,R2,OriginalMatrix>,R,C>
    {
    typedef mat<typename mat_traits<OriginalMatrix>::scalar_type,R,C> type;
    };

template <int R1,int R2,class OriginalMatrix,int R,int C>
struct
deduce_mat2<qvm_detail::swap_rows_<R1,R2,OriginalMatrix>,qvm_detail::swap_rows_<R1,R2,OriginalMatrix>,R,C>
    {
    typedef mat<typename mat_traits<OriginalMatrix>::scalar_type,R,C> type;
    };

template <int R1,int R2,class A>
typename enable_if_c<
    is_mat<A>::value,
    qvm_detail::swap_rows_<R1,R2,A> const &>::type
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
swap_rows( A const & a )
    {
    return reinterpret_cast<typename qvm_detail::swap_rows_<R1,R2,A> const &>(a);
    }

template <int R1,int R2,class A>
typename enable_if_c<
    is_mat<A>::value,
    qvm_detail::swap_rows_<R1,R2,A> &>::type
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
swap_rows( A & a )
    {
    return reinterpret_cast<typename qvm_detail::swap_rows_<R1,R2,A> &>(a);
    }

////////////////////////////////////////////////

namespace
qvm_detail
    {
    template <int Row1,int Row2,class OriginalMatrix>
    class
    swap_cols_
        {
        swap_cols_( swap_cols_ const & );
        swap_cols_ & operator=( swap_cols_ const & );
        ~swap_cols_();

        public:

        template <class T>
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        swap_cols_ &
        operator=( T const & x )
            {
            assign(*this,x);
            return *this;
            }

        template <class R
#if __cplusplus >= 201103L
            , class = typename enable_if<is_mat<R> >::type
#endif
        >
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
        operator R() const
            {
            R r;
            assign(r,*this);
            return r;
            }
        };

    template <int C1,int C2,class OriginalMatrix,bool WriteElementRef=mat_write_element_ref<OriginalMatrix>::value>
    struct swap_cols_write_traits;

    template <int C1,int C2,class OriginalMatrix>
    struct
    swap_cols_write_traits<C1,C2,OriginalMatrix,true>
        {
        typedef swap_cols_<C1,C2,OriginalMatrix> this_matrix;
        typedef typename mat_traits<OriginalMatrix>::scalar_type scalar_type;
        static int const rows=mat_traits<OriginalMatrix>::rows;
        static int const cols=mat_traits<OriginalMatrix>::cols;

        template <int Row,int Col>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        scalar_type &
        write_element( this_matrix & x )
            {
            BOOST_QVM_STATIC_ASSERT(Row>=0);
            BOOST_QVM_STATIC_ASSERT(Row<rows);
            BOOST_QVM_STATIC_ASSERT(Col>=0);
            BOOST_QVM_STATIC_ASSERT(Col<cols);
            return mat_traits<OriginalMatrix>::template write_element<Row,(Col==C1 && C1!=C2)*C2+(Col==C2 && C1!=C2)*C1+((Col!=C1 && Col!=C2) || C1==C2)*Col>(reinterpret_cast<OriginalMatrix &>(x));
            }

        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        scalar_type &
        write_element_idx( int row, int col, this_matrix & x )
            {
            BOOST_QVM_ASSERT(row>=0);
            BOOST_QVM_ASSERT(row<rows);
            BOOST_QVM_ASSERT(col>=0);
            BOOST_QVM_ASSERT(col<cols);
            return mat_traits<OriginalMatrix>::write_element_idx(row,col==C1?C2:col==C2?C1:col,reinterpret_cast<OriginalMatrix &>(x));
            }
        };

    template <int C1,int C2,class OriginalMatrix>
    struct
    swap_cols_write_traits<C1,C2,OriginalMatrix,false>
        {
        typedef swap_cols_<C1,C2,OriginalMatrix> this_matrix;
        typedef typename mat_traits<OriginalMatrix>::scalar_type scalar_type;
        static int const rows=mat_traits<OriginalMatrix>::rows;
        static int const cols=mat_traits<OriginalMatrix>::cols;

        template <int Row,int Col>
        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        void
        write_element( this_matrix & x, scalar_type s )
            {
            BOOST_QVM_STATIC_ASSERT(Row>=0);
            BOOST_QVM_STATIC_ASSERT(Row<rows);
            BOOST_QVM_STATIC_ASSERT(Col>=0);
            BOOST_QVM_STATIC_ASSERT(Col<cols);
            mat_traits<OriginalMatrix>::template write_element<Row,(Col==C1 && C1!=C2)*C2+(Col==C2 && C1!=C2)*C1+((Col!=C1 && Col!=C2) || C1==C2)*Col>(reinterpret_cast<OriginalMatrix &>(x), s);
            }

        static
        BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
        void
        write_element_idx( int row, int col, this_matrix & x, scalar_type s )
            {
            BOOST_QVM_ASSERT(row>=0);
            BOOST_QVM_ASSERT(row<rows);
            BOOST_QVM_ASSERT(col>=0);
            BOOST_QVM_ASSERT(col<cols);
            mat_traits<OriginalMatrix>::write_element_idx(row,col==C1?C2:col==C2?C1:col,reinterpret_cast<OriginalMatrix &>(x), s);
            }
        };
    }

template <int C1,int C2,class OriginalMatrix>
struct
mat_traits<qvm_detail::swap_cols_<C1,C2,OriginalMatrix> >:
    qvm_detail::swap_cols_write_traits<C1,C2,OriginalMatrix>
    {
    typedef qvm_detail::swap_cols_<C1,C2,OriginalMatrix> this_matrix;
    typedef typename mat_traits<OriginalMatrix>::scalar_type scalar_type;
    static int const rows=mat_traits<OriginalMatrix>::rows;
    static int const cols=mat_traits<OriginalMatrix>::cols;

    template <int Row,int Col>
    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element( this_matrix const & x )
        {
        BOOST_QVM_STATIC_ASSERT(Row>=0);
        BOOST_QVM_STATIC_ASSERT(Row<rows);
        BOOST_QVM_STATIC_ASSERT(Col>=0);
        BOOST_QVM_STATIC_ASSERT(Col<cols);
        return mat_traits<OriginalMatrix>::template read_element<Row,(Col==C1 && C1!=C2)*C2+(Col==C2 && C1!=C2)*C1+((Col!=C1 && Col!=C2) || C1==C2)*Col>(reinterpret_cast<OriginalMatrix const &>(x));
        }

    static
    BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_CRITICAL
    scalar_type
    read_element_idx( int row, int col, this_matrix const & x )
        {
        BOOST_QVM_ASSERT(row>=0);
        BOOST_QVM_ASSERT(row<rows);
        BOOST_QVM_ASSERT(col>=0);
        BOOST_QVM_ASSERT(col<cols);
        return mat_traits<OriginalMatrix>::read_element_idx(row,col==C1?C2:col==C2?C1:col,reinterpret_cast<OriginalMatrix const &>(x));
        }
    };

template <int C1,int C2,class OriginalMatrix,int R,int C>
struct
deduce_mat<qvm_detail::swap_cols_<C1,C2,OriginalMatrix>,R,C>
    {
    typedef mat<typename mat_traits<OriginalMatrix>::scalar_type,R,C> type;
    };

template <int C1,int C2,class OriginalMatrix,int R,int C>
struct
deduce_mat2<qvm_detail::swap_cols_<C1,C2,OriginalMatrix>,qvm_detail::swap_cols_<C1,C2,OriginalMatrix>,R,C>
    {
    typedef mat<typename mat_traits<OriginalMatrix>::scalar_type,R,C> type;
    };

template <int C1,int C2,class A>
typename enable_if_c<
    is_mat<A>::value,
    qvm_detail::swap_cols_<C1,C2,A> const &>::type
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
swap_cols( A const & a )
    {
    return reinterpret_cast<typename qvm_detail::swap_cols_<C1,C2,A> const &>(a);
    }

template <int C1,int C2,class A>
typename enable_if_c<
    is_mat<A>::value,
    qvm_detail::swap_cols_<C1,C2,A> &>::type
BOOST_QVM_CONSTEXPR BOOST_QVM_INLINE_TRIVIAL
swap_cols( A & a )
    {
    return reinterpret_cast<typename qvm_detail::swap_cols_<C1,C2,A> &>(a);
    }

} }

#endif
// <<< #include <boost/qvm/map_mat_mat.hpp>
// #line 11 "boost/qvm/map.hpp"

#endif
// <<< #include <boost/qvm/map.hpp>
// #line 28 "boost/qvm/lite.hpp"

#endif
