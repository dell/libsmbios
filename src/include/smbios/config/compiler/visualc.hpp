//  (C) Copyright John Maddock 2001 - 2003. 
//  (C) Copyright Darin Adler 2001 - 2002. 
//  (C) Copyright Peter Dimov 2001. 
//  (C) Copyright Aleksey Gurtovoy 2002. 
//  (C) Copyright David Abrahams 2002 - 2003. 
//  (C) Copyright Beman Dawes 2002 - 2003. 
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  Microsoft Visual C++ compiler setup:

#define LIBSMBIOS_MSVC _MSC_VER

// turn off the warnings before we #include anything
// 4503: warning: decorated name length exceeded
// 4250: 'class1' : inherits 'class2::member' via dominance
// 4201: nonstandard extension used : nameless struct/union
// 4127: warning: conditional expression is constant
#pragma warning( disable : 4201 4250 4503 4127 )
#ifndef DEBUG
// 4702: unreachable code
#pragma warning( disable : 4702 ) // disable in release because MS headers have tons of unreachable code
#endif

// Only new MSVC has _strtoi64. Older compilers are screwed (will get undefined ref error)
#if _MSC_VER >= 1300
#define strtoll(p, e, b) _strtoi64(p, e, b) 
#endif

#define UNREFERENCED_PARAMETER(P)  (P)
#define MARK_UNUSED
#define LIBSMBIOS_PACKED_ATTR
#define _dbg_iostream_out(stream, line) do { stream << line; } while(0)
#define _dbg_cout(line) _dbg_iostream_out(cout, line)
#define _dbg_cerr(line) _dbg_iostream_out(cerr, line)
#define _null_call(...) do {} while(0)
#ifdef DEBUG_OUTPUT_ALL
#include <iostream>
#define DCOUT _dbg_cout
#define DCERR _dbg_cerr
#else
#define DCOUT _null_call
#define DCERR _null_call
#endif



// we require RTTI, guard against users:
#ifndef _CPPRTTI
#error RTTI required
#endif

#if _MSC_VER <= 1200  // 1200 == VC++ 6.0
#pragma warning( disable : 4786 ) // ident trunc to '255' chars in debug info
#  define LIBSMBIOS_NO_DEPENDENT_TYPES_IN_TEMPLATE_VALUE_PARAMETERS
#  define LIBSMBIOS_NO_VOID_RETURNS
#  define LIBSMBIOS_NO_EXCEPTION_STD_NAMESPACE
#  define LIBSMBIOS_NO_STDC_NAMESPACE
   // disable min/max macro defines on vc6:
   //
#endif

#if (_MSC_VER <= 1300)  // 1300 == VC++ 7.0

#if !defined(_MSC_EXTENSIONS) && !defined(LIBSMBIOS_NO_DEPENDENT_TYPES_IN_TEMPLATE_VALUE_PARAMETERS)      // VC7 bug with /Za
#  define LIBSMBIOS_NO_DEPENDENT_TYPES_IN_TEMPLATE_VALUE_PARAMETERS
#endif

#  define LIBSMBIOS_NO_EXPLICIT_FUNCTION_TEMPLATE_ARGUMENTS
#  define LIBSMBIOS_NO_INCLASS_MEMBER_INITIALIZATION
#  define LIBSMBIOS_NO_PRIVATE_IN_AGGREGATE
#  define LIBSMBIOS_NO_ARGUMENT_DEPENDENT_LOOKUP
#  define LIBSMBIOS_NO_INTEGRAL_INT64_T
#  define LIBSMBIOS_NO_DEDUCED_TYPENAME
#  define LIBSMBIOS_NO_USING_DECLARATION_OVERLOADS_FROM_TYPENAME_BASE

//    VC++ 6/7 has member templates but they have numerous problems including
//    cases of silent failure, so for safety we define:
#  define LIBSMBIOS_NO_MEMBER_TEMPLATES
//    For VC++ experts wishing to attempt workarounds, we define:
#  define LIBSMBIOS_MSVC6_MEMBER_TEMPLATES

#  define LIBSMBIOS_NO_MEMBER_TEMPLATE_FRIENDS
#  define LIBSMBIOS_NO_TEMPLATE_PARTIAL_SPECIALIZATION
#  define LIBSMBIOS_NO_CV_VOID_SPECIALIZATIONS
#  define LIBSMBIOS_NO_FUNCTION_TEMPLATE_ORDERING
#  define LIBSMBIOS_NO_USING_TEMPLATE
#  define LIBSMBIOS_NO_SWPRINTF
#  define LIBSMBIOS_NO_TEMPLATE_TEMPLATES
#  define LIBSMBIOS_NO_SFINAE
#  if (_MSC_VER > 1200)
#     define LIBSMBIOS_NO_MEMBER_FUNCTION_SPECIALIZATIONS
#  endif

#endif

#if _MSC_VER >= 1300
// VC++ 7 and higher have __FUNCTION__ macro
#define LIBSMBIOS_HAS_FUNCTION
#endif

#if _MSC_VER < 1310 // 1310 == VC++ 7.1
#  define LIBSMBIOS_NO_SWPRINTF
#endif

#if _MSC_VER <= 1310
#  define LIBSMBIOS_NO_MEMBER_TEMPLATE_FRIENDS
#endif

#ifndef _NATIVE_WCHAR_T_DEFINED
#  define LIBSMBIOS_NO_INTRINSIC_WCHAR_T
#endif

//   
// check for exception handling support:   
#ifndef _CPPUNWIND   
#  define LIBSMBIOS_NO_EXCEPTIONS   
#endif 

//
// __int64 support:
//
#if (_MSC_VER >= 1200)
#   define LIBSMBIOS_HAS_MS_INT64
#endif
#if (_MSC_VER >= 1310) && defined(_MSC_EXTENSIONS)
#   define LIBSMBIOS_HAS_LONG_LONG
#endif
//
// disable Win32 API's if compiler extentions are
// turned off:
//
#ifndef _MSC_EXTENSIONS
#  define LIBSMBIOS_DISABLE_WIN32
#endif

//
// all versions support __declspec:
//
#define LIBSMBIOS_HAS_DECLSPEC
//
// prefix and suffix headers:
//
#ifndef LIBSMBIOS_ABI_PREFIX
#  define LIBSMBIOS_ABI_PREFIX "smbios/config/abi/msvc_prefix.hpp"
#endif
#ifndef LIBSMBIOS_ABI_SUFFIX
#  define LIBSMBIOS_ABI_SUFFIX "smbios/config/abi/msvc_suffix.hpp"
#endif

# if _MSC_VER == 1200
#   define LIBSMBIOS_COMPILER_VERSION 6.0
# elif _MSC_VER == 1300
#   define LIBSMBIOS_COMPILER_VERSION 7.0
# elif _MSC_VER == 1310
#   define LIBSMBIOS_COMPILER_VERSION 7.1
# else
#   define LIBSMBIOS_COMPILER_VERSION _MSC_VER
# endif

#define LIBSMBIOS_COMPILER "Microsoft Visual C++ version " LIBSMBIOS_STRINGIZE(LIBSMBIOS_COMPILER_VERSION)

//
// versions check:
// we don't support Visual C++ prior to version 6:
#if _MSC_VER < 1200
#error "Compiler looks ancient. Sorry but we dont support it MSVC++ prior to version 6."
#endif
//
// last known and checked version is 1310:
#if (_MSC_VER > 1400)
#  if defined(LIBSMBIOS_ASSERT_CONFIG)
#     error "Unknown compiler version - please run the configure tests and report the results"
#  else
#     pragma message("Unknown compiler version - please run the configure tests and report the results")
#  endif
#endif
