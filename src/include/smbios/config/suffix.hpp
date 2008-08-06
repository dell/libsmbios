//  Boost config.hpp configuration header file  ------------------------------//

//  (C) Copyright John Maddock 2001 - 2003. 
//  (C) Copyright Darin Adler 2001. 
//  (C) Copyright Peter Dimov 2001. 
//  (C) Copyright Bill Kempf 2002. 
//  (C) Copyright Jens Maurer 2002. 
//  (C) Copyright David Abrahams 2002 - 2003. 
//  (C) Copyright Gennaro Prota 2003. 
//  (C) Copyright Eric Friedman 2003. 
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  Boost config.hpp policy and rationale documentation has been moved to
//  http://www.boost.org/libs/config
//
//  This file is intended to be stable, and relatively unchanging.
//  It should contain boilerplate code only - no compiler specific
//  code unless it is unavoidable - no changes unless unavoidable.

#ifndef LIBSMBIOS_CONFIG_SUFFIX_HPP
#define LIBSMBIOS_CONFIG_SUFFIX_HPP

//
// look for long long by looking for the appropriate macros in <limits.h>.
// Note that we use limits.h rather than climits for maximal portability,
// remember that since these just declare a bunch of macros, there should be
// no namespace issues from this.
//
#include <limits.h>
# if !defined(LIBSMBIOS_HAS_LONG_LONG)                                              \
   && !(defined(LIBSMBIOS_MSVC) && LIBSMBIOS_MSVC <=1300) && !defined(__BORLANDC__)     \
   && (defined(ULLONG_MAX) || defined(ULONG_LONG_MAX) || defined(ULONGLONG_MAX))
#  define LIBSMBIOS_HAS_LONG_LONG
#endif
#if !defined(LIBSMBIOS_HAS_LONG_LONG) && !defined(LIBSMBIOS_NO_INTEGRAL_INT64_T)
#  define LIBSMBIOS_NO_INTEGRAL_INT64_T
#endif

// GCC 3.x will clean up all of those nasty macro definitions that
// LIBSMBIOS_NO_CTYPE_FUNCTIONS is intended to help work around, so undefine
// it under GCC 3.x.
#if defined(__GNUC__) && (__GNUC__ >= 3) && defined(LIBSMBIOS_NO_CTYPE_FUNCTIONS)
#  undef LIBSMBIOS_NO_CTYPE_FUNCTIONS
#endif


//
// Assume any extensions are in namespace std:: unless stated otherwise:
//
#  ifndef LIBSMBIOS_STD_EXTENSION_NAMESPACE
#    define LIBSMBIOS_STD_EXTENSION_NAMESPACE std
#  endif

//
// If cv-qualified specializations are not allowed, then neither are cv-void ones:
//
#  if defined(LIBSMBIOS_NO_CV_SPECIALIZATIONS) \
      && !defined(LIBSMBIOS_NO_CV_VOID_SPECIALIZATIONS)
#     define LIBSMBIOS_NO_CV_VOID_SPECIALIZATIONS
#  endif

//
// If there is no numeric_limits template, then it can't have any compile time
// constants either!
//
#  if defined(LIBSMBIOS_NO_LIMITS) \
      && !defined(LIBSMBIOS_NO_LIMITS_COMPILE_TIME_CONSTANTS)
#     define LIBSMBIOS_NO_LIMITS_COMPILE_TIME_CONSTANTS
#     define LIBSMBIOS_NO_MS_INT64_NUMERIC_LIMITS
#     define LIBSMBIOS_NO_LONG_LONG_NUMERIC_LIMITS
#  endif

//
// if there is no long long then there is no specialisation
// for numeric_limits<long long> either:
//
#if !defined(LIBSMBIOS_HAS_LONG_LONG) && !defined(LIBSMBIOS_NO_LONG_LONG_NUMERIC_LIMITS)
#  define LIBSMBIOS_NO_LONG_LONG_NUMERIC_LIMITS
#endif

//
// if there is no __int64 then there is no specialisation
// for numeric_limits<__int64> either:
//
#if !defined(LIBSMBIOS_HAS_MS_INT64) && !defined(LIBSMBIOS_NO_MS_INT64_NUMERIC_LIMITS)
#  define LIBSMBIOS_NO_MS_INT64_NUMERIC_LIMITS
#endif

//
// if member templates are supported then so is the
// VC6 subset of member templates:
//
#  if !defined(LIBSMBIOS_NO_MEMBER_TEMPLATES) \
       && !defined(LIBSMBIOS_MSVC6_MEMBER_TEMPLATES)
#     define LIBSMBIOS_MSVC6_MEMBER_TEMPLATES
#  endif

//
// Without partial specialization, can't test for partial specialisation bugs:
//
#  if defined(LIBSMBIOS_NO_TEMPLATE_PARTIAL_SPECIALIZATION) \
      && !defined(LIBSMBIOS_BCB_PARTIAL_SPECIALIZATION_BUG)
#     define LIBSMBIOS_BCB_PARTIAL_SPECIALIZATION_BUG
#  endif

//
// Without partial specialization, we can't have array-type partial specialisations:
//
#  if defined(LIBSMBIOS_NO_TEMPLATE_PARTIAL_SPECIALIZATION) \
      && !defined(LIBSMBIOS_NO_ARRAY_TYPE_SPECIALIZATIONS)
#     define LIBSMBIOS_NO_ARRAY_TYPE_SPECIALIZATIONS
#  endif

//
// Without partial specialization, std::iterator_traits can't work:
//
#  if defined(LIBSMBIOS_NO_TEMPLATE_PARTIAL_SPECIALIZATION) \
      && !defined(LIBSMBIOS_NO_STD_ITERATOR_TRAITS)
#     define LIBSMBIOS_NO_STD_ITERATOR_TRAITS
#  endif

//
// Without member template support, we can't have template constructors
// in the standard library either:
//
#  if defined(LIBSMBIOS_NO_MEMBER_TEMPLATES) \
      && !defined(LIBSMBIOS_MSVC6_MEMBER_TEMPLATES) \
      && !defined(LIBSMBIOS_NO_TEMPLATED_ITERATOR_CONSTRUCTORS)
#     define LIBSMBIOS_NO_TEMPLATED_ITERATOR_CONSTRUCTORS
#  endif

//
// Without member template support, we can't have a conforming
// std::allocator template either:
//
#  if defined(LIBSMBIOS_NO_MEMBER_TEMPLATES) \
      && !defined(LIBSMBIOS_MSVC6_MEMBER_TEMPLATES) \
      && !defined(LIBSMBIOS_NO_STD_ALLOCATOR)
#     define LIBSMBIOS_NO_STD_ALLOCATOR
#  endif

//
// without ADL support then using declarations will break ADL as well:
//
#if defined(LIBSMBIOS_NO_ARGUMENT_DEPENDENT_LOOKUP) && !defined(LIBSMBIOS_FUNCTION_SCOPE_USING_DECLARATION_BREAKS_ADL)
#  define LIBSMBIOS_FUNCTION_SCOPE_USING_DECLARATION_BREAKS_ADL
#endif

//
// If we have a standard allocator, then we have a partial one as well:
//
#if !defined(LIBSMBIOS_NO_STD_ALLOCATOR)
#  define LIBSMBIOS_HAS_PARTIAL_STD_ALLOCATOR
#endif

//
// We can't have a working std::use_facet if there is no std::locale:
//
#  if defined(LIBSMBIOS_NO_STD_LOCALE) && !defined(LIBSMBIOS_NO_STD_USE_FACET)
#     define LIBSMBIOS_NO_STD_USE_FACET
#  endif

//
// We can't have a std::messages facet if there is no std::locale:
//
#  if defined(LIBSMBIOS_NO_STD_LOCALE) && !defined(LIBSMBIOS_NO_STD_MESSAGES)
#     define LIBSMBIOS_NO_STD_MESSAGES
#  endif

//
// We can't have a working std::wstreambuf if there is no std::locale:
//
#  if defined(LIBSMBIOS_NO_STD_LOCALE) && !defined(LIBSMBIOS_NO_STD_WSTREAMBUF)
#     define LIBSMBIOS_NO_STD_WSTREAMBUF
#  endif

//
// We can't have a <cwctype> if there is no <cwchar>:
//
#  if defined(LIBSMBIOS_NO_CWCHAR) && !defined(LIBSMBIOS_NO_CWCTYPE)
#     define LIBSMBIOS_NO_CWCTYPE
#  endif

//
// We can't have a swprintf if there is no <cwchar>:
//
#  if defined(LIBSMBIOS_NO_CWCHAR) && !defined(LIBSMBIOS_NO_SWPRINTF)
#     define LIBSMBIOS_NO_SWPRINTF
#  endif

//
// If Win32 support is turned off, then we must turn off
// threading support also, unless there is some other
// thread API enabled:
//
#if defined(LIBSMBIOS_DISABLE_WIN32) && defined(_WIN32) \
   && !defined(LIBSMBIOS_DISABLE_THREADS) && !defined(LIBSMBIOS_HAS_PTHREADS)
#  define LIBSMBIOS_DISABLE_THREADS
#endif

//
// Turn on threading support if the compiler thinks that it's in
// multithreaded mode.  We put this here because there are only a
// limited number of macros that identify this (if there's any missing
// from here then add to the appropriate compiler section):
//
#if (defined(__MT__) || defined(_MT) || defined(_REENTRANT) \
    || defined(_PTHREADS)) && !defined(LIBSMBIOS_HAS_THREADS)
#  define LIBSMBIOS_HAS_THREADS
#endif

//
// Turn threading support off if LIBSMBIOS_DISABLE_THREADS is defined:
//
#if defined(LIBSMBIOS_DISABLE_THREADS) && defined(LIBSMBIOS_HAS_THREADS)
#  undef LIBSMBIOS_HAS_THREADS
#endif

//
// Turn threading support off if we don't recognise the threading API:
//
#if defined(LIBSMBIOS_HAS_THREADS) && !defined(LIBSMBIOS_HAS_PTHREADS)\
      && !defined(LIBSMBIOS_HAS_WINTHREADS) && !defined(LIBSMBIOS_HAS_BETHREADS)\
      && !defined(LIBSMBIOS_HAS_MPTASKS)
#  undef LIBSMBIOS_HAS_THREADS
#endif

//
// Turn threading detail macros off if we don't (want to) use threading
//
#ifndef LIBSMBIOS_HAS_THREADS
#  undef LIBSMBIOS_HAS_PTHREADS
#  undef LIBSMBIOS_HAS_PTHREAD_MUTEXATTR_SETTYPE
#  undef LIBSMBIOS_HAS_WINTHREADS
#  undef LIBSMBIOS_HAS_BETHREADS
#  undef LIBSMBIOS_HAS_MPTASKS
#endif

//
// If the compiler claims to be C99 conformant, then it had better
// have a <stdint.h>:
//
#  if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
#     define LIBSMBIOS_HAS_STDINT_H
#  endif

//
// Define LIBSMBIOS_NO_SLIST and LIBSMBIOS_NO_HASH if required.
// Note that this is for backwards compatibility only.
//
#  ifndef LIBSMBIOS_HAS_SLIST
#     define LIBSMBIOS_NO_SLIST
#  endif

#  ifndef LIBSMBIOS_HAS_HASH
#     define LIBSMBIOS_NO_HASH
#  endif

//  LIBSMBIOS_HAS_ABI_HEADERS
//  This macro gets set if we have headers that fix the ABI,
//  and prevent ODR violations when linking to external libraries:
#if defined(LIBSMBIOS_ABI_PREFIX) && defined(LIBSMBIOS_ABI_SUFFIX) && !defined(LIBSMBIOS_HAS_ABI_HEADERS)
#  define LIBSMBIOS_HAS_ABI_HEADERS
#endif

#if defined(LIBSMBIOS_HAS_ABI_HEADERS) && defined(LIBSMBIOS_DISABLE_ABI_HEADERS)
#  undef LIBSMBIOS_HAS_ABI_HEADERS
#endif

//  LIBSMBIOS_NO_STDC_NAMESPACE workaround  --------------------------------------//
//  Because std::size_t usage is so common, even in headers which do not
//  otherwise use the C library, the <cstddef> workaround is included here so
//  that ugly workaround code need not appear in many other boost headers.
//  NOTE WELL: This is a workaround for non-conforming compilers; <cstddef> 
//  must still be #included in the usual places so that <cstddef> inclusion
//  works as expected with standard conforming compilers.  The resulting
//  double inclusion of <cstddef> is harmless.

# ifdef LIBSMBIOS_NO_STDC_NAMESPACE
#   include <cstddef>
    namespace std { using ::ptrdiff_t; using ::size_t; }
# endif

//  LIBSMBIOS_NO_STD_MIN_MAX workaround  -----------------------------------------//

#  ifdef LIBSMBIOS_NO_STD_MIN_MAX

namespace std {
  template <class _Tp>
  inline const _Tp& min(const _Tp& __a, const _Tp& __b) {
    return __b < __a ? __b : __a;
  }
  template <class _Tp>
  inline const _Tp& max(const _Tp& __a, const _Tp& __b) {
    return  __a < __b ? __b : __a;
  }
}

#  endif

// LIBSMBIOS_STATIC_CONSTANT workaround --------------------------------------- //
// On compilers which don't allow in-class initialization of static integral
// constant members, we must use enums as a workaround if we want the constants
// to be available at compile-time. This macro gives us a convenient way to
// declare such constants.

#  ifdef LIBSMBIOS_NO_INCLASS_MEMBER_INITIALIZATION
#       define LIBSMBIOS_STATIC_CONSTANT(type, assignment) enum { assignment }
#  else
#     define LIBSMBIOS_STATIC_CONSTANT(type, assignment) static const type assignment
#  endif

// LIBSMBIOS_USE_FACET workaround ----------------------------------------------//
// When the standard library does not have a conforming std::use_facet there
// are various workarounds available, but they differ from library to library.
// This macro provides a consistent way to access a locale's facets.
// Usage:
//    replace
//       std::use_facet<Type>(loc);
//    with
//       LIBSMBIOS_USE_FACET(Type, loc);
//    Note do not add a std:: prefix to the front of LIBSMBIOS_USE_FACET!

#if defined(LIBSMBIOS_NO_STD_USE_FACET)
#  ifdef LIBSMBIOS_HAS_TWO_ARG_USE_FACET
#     define LIBSMBIOS_USE_FACET(Type, loc) std::use_facet(loc, static_cast<Type*>(0))
#  elif defined(LIBSMBIOS_HAS_MACRO_USE_FACET)
#     define LIBSMBIOS_USE_FACET(Type, loc) std::_USE(loc, Type)
#  elif defined(LIBSMBIOS_HAS_STLP_USE_FACET)
#     define LIBSMBIOS_USE_FACET(Type, loc) (*std::_Use_facet<Type >(loc))
#  endif
#else
#  define LIBSMBIOS_USE_FACET(Type, loc) std::use_facet< Type >(loc)
#endif

// LIBSMBIOS_NESTED_TEMPLATE workaround ------------------------------------------//
// Member templates are supported by some compilers even though they can't use
// the A::template member<U> syntax, as a workaround replace:
//
// typedef typename A::template rebind<U> binder;
//
// with:
//
// typedef typename A::LIBSMBIOS_NESTED_TEMPLATE rebind<U> binder;

#ifndef LIBSMBIOS_NO_MEMBER_TEMPLATE_KEYWORD
#  define LIBSMBIOS_NESTED_TEMPLATE template
#else
#  define LIBSMBIOS_NESTED_TEMPLATE
#endif

// LIBSMBIOS_UNREACHABLE_RETURN(x) workaround -------------------------------------//
// Normally evaluates to nothing, unless LIBSMBIOS_NO_UNREACHABLE_RETURN_DETECTION
// is defined, in which case it evaluates to return x; Use when you have a return
// statement that can never be reached.

#ifdef LIBSMBIOS_NO_UNREACHABLE_RETURN_DETECTION
#  define LIBSMBIOS_UNREACHABLE_RETURN(x) return x;
#else
#  define LIBSMBIOS_UNREACHABLE_RETURN(x)
#endif

// LIBSMBIOS_DEDUCED_TYPENAME workaround ------------------------------------------//
//
// Some compilers don't support the use of `typename' for dependent
// types in deduced contexts, e.g.
//
//     template <class T> void f(T, typename T::type);
//                                  ^^^^^^^^
// Replace these declarations with:
//
//     template <class T> void f(T, LIBSMBIOS_DEDUCED_TYPENAME T::type);

#ifndef LIBSMBIOS_NO_DEDUCED_TYPENAME
#  define LIBSMBIOS_DEDUCED_TYPENAME typename
#else 
#  define LIBSMBIOS_DEDUCED_TYPENAME
#endif

// LIBSMBIOS_[APPEND_]EXPLICIT_TEMPLATE_[NON_]TYPE macros --------------------------//
//
// Some compilers have problems with function templates whose
// template parameters don't appear in the function parameter
// list (basically they just link one instantiation of the
// template in the final executable). These macros provide a
// uniform way to cope with the problem with no effects on the
// calling syntax.

// Example:
//
//  #include <iostream>
//  #include <ostream>
//  #include <typeinfo>
//
//  template <int n>
//  void f() { std::cout << n << ' '; }
//
//  template <typename T>
//  void g() { std::cout << typeid(T).name() << ' '; }
//
//  int main() {
//    f<1>();
//    f<2>();
//
//    g<int>();
//    g<double>();
//  }
//
// With VC++ 6.0 the output is:
//
//   2 2 double double
//
// To fix it, write
//
//   template <int n>
//   void f(LIBSMBIOS_EXPLICIT_TEMPLATE_NON_TYPE(int, n)) { ... }
//
//   template <typename T>
//   void g(LIBSMBIOS_EXPLICIT_TEMPLATE_TYPE(T)) { ... }
//


// Don't think we need the following boost constructs for libsmbios
//#if defined LIBSMBIOS_NO_EXPLICIT_FUNCTION_TEMPLATE_ARGUMENTS
//
//#  include "smbios/type.hpp"
//#  include "smbios/non_type.hpp"
//
//#  define LIBSMBIOS_EXPLICIT_TEMPLATE_TYPE(t)         smbios::type<t>* = 0
//#  define LIBSMBIOS_EXPLICIT_TEMPLATE_TYPE_SPEC(t)    smbios::type<t>*
//#  define LIBSMBIOS_EXPLICIT_TEMPLATE_NON_TYPE(t, v)  smbios::non_type<t, v>* = 0
//#  define LIBSMBIOS_EXPLICIT_TEMPLATE_NON_TYPE_SPEC(t, v)  smbios::non_type<t, v>*
//
//#  define LIBSMBIOS_APPEND_EXPLICIT_TEMPLATE_TYPE(t)  , LIBSMBIOS_EXPLICIT_TEMPLATE_TYPE(t)
//#  define LIBSMBIOS_APPEND_EXPLICIT_TEMPLATE_TYPE_SPEC(t)  , LIBSMBIOS_EXPLICIT_TEMPLATE_TYPE_SPEC(t)
//#  define LIBSMBIOS_APPEND_EXPLICIT_TEMPLATE_NON_TYPE(t, v)  , LIBSMBIOS_EXPLICIT_TEMPLATE_NON_TYPE(t, v)
//#  define LIBSMBIOS_APPEND_EXPLICIT_TEMPLATE_NON_TYPE_SPEC(t, v)  , LIBSMBIOS_EXPLICIT_TEMPLATE_NON_TYPE_SPEC(t, v)
//
//#else
//
//// no workaround needed: expand to nothing
//
//#  define LIBSMBIOS_EXPLICIT_TEMPLATE_TYPE(t)
//#  define LIBSMBIOS_EXPLICIT_TEMPLATE_TYPE_SPEC(t)
//#  define LIBSMBIOS_EXPLICIT_TEMPLATE_NON_TYPE(t, v)
//#  define LIBSMBIOS_EXPLICIT_TEMPLATE_NON_TYPE_SPEC(t, v)
//
//#  define LIBSMBIOS_APPEND_EXPLICIT_TEMPLATE_TYPE(t)
//#  define LIBSMBIOS_APPEND_EXPLICIT_TEMPLATE_TYPE_SPEC(t)
//#  define LIBSMBIOS_APPEND_EXPLICIT_TEMPLATE_NON_TYPE(t, v)
//#  define LIBSMBIOS_APPEND_EXPLICIT_TEMPLATE_NON_TYPE_SPEC(t, v)
//
//
//#endif // defined LIBSMBIOS_NO_EXPLICIT_FUNCTION_TEMPLATE_ARGUMENTS


// ---------------------------------------------------------------------------//

//
// Helper macro LIBSMBIOS_STRINGIZE:
// Converts the parameter X to a string after macro replacement
// on X has been performed.
//
#define LIBSMBIOS_STRINGIZE(X) LIBSMBIOS_DO_STRINGIZE(X)
#define LIBSMBIOS_DO_STRINGIZE(X) #X

//
// Helper macro LIBSMBIOS_JOIN:
// The following piece of macro magic joins the two 
// arguments together, even when one of the arguments is
// itself a macro (see 16.3.1 in C++ standard).  The key
// is that macro expansion of macro arguments does not
// occur in LIBSMBIOS_DO_JOIN2 but does in LIBSMBIOS_DO_JOIN.
//
#define LIBSMBIOS_JOIN( X, Y ) LIBSMBIOS_DO_JOIN( X, Y )
#define LIBSMBIOS_DO_JOIN( X, Y ) LIBSMBIOS_DO_JOIN2(X,Y)
#define LIBSMBIOS_DO_JOIN2( X, Y ) X##Y

//
// Set some default values for compiler/library/platform names.
// These are for debugging config setup only:
//
#  ifndef LIBSMBIOS_COMPILER
#     define LIBSMBIOS_COMPILER "Unknown ISO C++ Compiler"
#  endif
#  ifndef LIBSMBIOS_STDLIB
#     define LIBSMBIOS_STDLIB "Unknown ISO standard library"
#  endif
#  ifndef LIBSMBIOS_PLATFORM
#     if defined(unix) || defined(__unix) || defined(_XOPEN_SOURCE) \
         || defined(_POSIX_SOURCE)
#        define LIBSMBIOS_PLATFORM "Generic Unix"
#     else
#        define LIBSMBIOS_PLATFORM "Unknown"
#     endif
#  endif

#endif



