//  (C) Copyright John Maddock 2001.
//  (C) Copyright Jens Maurer 2001 - 2003.
//  (C) Copyright Peter Dimov 2002.
//  (C) Copyright Aleksey Gurtovoy 2002 - 2003.
//  (C) Copyright David Abrahams 2002.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  Sun C++ compiler setup:

#    if __SUNPRO_CC <= 0x500
#      define LIBSMBIOS_NO_MEMBER_TEMPLATES
#      define LIBSMBIOS_NO_FUNCTION_TEMPLATE_ORDERING
#    endif

#    if (__SUNPRO_CC <= 0x520)
       //
       // Sunpro 5.2 and earler:
       //
       // although sunpro 5.2 supports the syntax for
       // inline initialization it often gets the value
       // wrong, especially where the value is computed
       // from other constants (J Maddock 6th May 2001)
#      define LIBSMBIOS_NO_INCLASS_MEMBER_INITIALIZATION

       // Although sunpro 5.2 supports the syntax for
       // partial specialization, it often seems to
       // bind to the wrong specialization.  Better
       // to disable it until suppport becomes more stable
       // (J Maddock 6th May 2001).
#      define LIBSMBIOS_NO_TEMPLATE_PARTIAL_SPECIALIZATION
#    endif

#    if (__SUNPRO_CC <= 0x530)
       // Requesting debug info (-g) with Boost.Python results
       // in an internal compiler error for "static const"
       // initialized in-class.
       //    >> Assertion:   (../links/dbg_cstabs.cc, line 611)
       //         while processing ../test.cpp at line 0.
       // (Jens Maurer according to Gottfried Ganssauge 04 Mar 2002)
#      define LIBSMBIOS_NO_INCLASS_MEMBER_INITIALIZATION

       // SunPro 5.3 has better support for partial specialization,
       // but breaks when compiling std::less<shared_ptr<T> >
       // (Jens Maurer 4 Nov 2001).

       // std::less specialization fixed as reported by George
       // Heintzelman; partial specialization re-enabled
       // (Peter Dimov 17 Jan 2002)

//#      define LIBSMBIOS_NO_TEMPLATE_PARTIAL_SPECIALIZATION

       // integral constant expressions with 64 bit numbers fail
#      define LIBSMBIOS_NO_INTEGRAL_INT64_T
#    endif

#    if (__SUNPRO_CC < 0x570)
#      define LIBSMBIOS_NO_TEMPLATE_TEMPLATES
       // see http://lists.boost.org/MailArchives/boost/msg47184.php
       // and http://lists.boost.org/MailArchives/boost/msg47220.php
#      define LIBSMBIOS_NO_INCLASS_MEMBER_INITIALIZATION
#      define LIBSMBIOS_NO_SFINAE
#      define LIBSMBIOS_NO_ARRAY_TYPE_SPECIALIZATIONS
#    endif
#    if (__SUNPRO_CC <= 0x580)
#      define LIBSMBIOS_NO_IS_ABSTRACT
#    endif

//
// Issues that effect all known versions:
//
#define LIBSMBIOS_NO_TWO_PHASE_NAME_LOOKUP
#define LIBSMBIOS_NO_ADL_BARRIER

//
// C++0x features
//

#if(__SUNPRO_CC >= 0x590)
#  define LIBSMBIOS_HAS_LONG_LONG
#else
#  define LIBSMBIOS_NO_LONG_LONG
#endif

#define LIBSMBIOS_NO_AUTO_DECLARATIONS
#define LIBSMBIOS_NO_AUTO_MULTIDECLARATIONS
#define LIBSMBIOS_NO_CHAR16_T
#define LIBSMBIOS_NO_CHAR32_T
#define LIBSMBIOS_NO_CONCEPTS
#define LIBSMBIOS_NO_CONSTEXPR
#define LIBSMBIOS_NO_DECLTYPE
#define LIBSMBIOS_NO_DEFAULTED_FUNCTIONS
#define LIBSMBIOS_NO_DELETED_FUNCTIONS
#define LIBSMBIOS_NO_EXPLICIT_CONVERSION_OPERATORS
#define LIBSMBIOS_NO_EXTERN_TEMPLATE
#define LIBSMBIOS_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
#define LIBSMBIOS_NO_INITIALIZER_LISTS
#define LIBSMBIOS_NO_LAMBDAS
#define LIBSMBIOS_NO_NULLPTR
#define LIBSMBIOS_NO_RAW_LITERALS
#define LIBSMBIOS_NO_RVALUE_REFERENCES
#define LIBSMBIOS_NO_SCOPED_ENUMS
#define LIBSMBIOS_NO_SFINAE_EXPR
#define LIBSMBIOS_NO_STATIC_ASSERT
#define LIBSMBIOS_NO_TEMPLATE_ALIASES
#define LIBSMBIOS_NO_UNICODE_LITERALS
#define LIBSMBIOS_NO_VARIADIC_TEMPLATES

//
// Version
//

#define LIBSMBIOS_COMPILER "Sun compiler version " LIBSMBIOS_STRINGIZE(__SUNPRO_CC)

//
// versions check:
// we don't support sunpro prior to version 4:
#if __SUNPRO_CC < 0x400
#error "Compiler not supported or configured - please reconfigure"
#endif
//
// last known and checked version is 0x590:
#if (__SUNPRO_CC > 0x590)
#  if defined(LIBSMBIOS_ASSERT_CONFIG)
#     error "Unknown compiler version - please run the configure tests and report the results"
#  endif
#endif

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(P)  ((void)(P))
#endif

#define LIBSMBIOS_PACKED_ATTR      __attribute__ ((packed))

#define _dbg_iostream_out(stream, line) do { stream << line; } while(0)
#define _dbg_cout(line) _dbg_iostream_out(cout, line)
#define _dbg_cerr(line) _dbg_iostream_out(cerr, line)
#define _null_call( args...) do {} while(0)
#ifdef DEBUG_OUTPUT_ALL
#include <iostream>
#define DCOUT _dbg_cout
#define DCERR _dbg_cerr
#else
#define DCOUT _null_call
#define DCERR _null_call
#endif

#define MARK_UNUSED __attribute__((unused))
