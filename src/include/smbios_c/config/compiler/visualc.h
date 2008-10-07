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

#define LIBSMBIOS_C_MSVC _MSC_VER
#define LIBSMBIOS_C_HAS_FUNCTION
#define LIBSMBIOS_C_HAS_LONG_LONG
#define LIBSMBIOS_C_HAS_DECLSPEC
#define LIBSMBIOS_C_PACKED_ATTR

#ifndef _NATIVE_WCHAR_T_DEFINED
#  define LIBSMBIOS_C_NO_INTRINSIC_WCHAR_T
#endif

//
// prefix and suffix headers:
//
#ifndef LIBSMBIOS_C_ABI_PREFIX
#  define LIBSMBIOS_C_ABI_PREFIX "smbios_c/config/abi/msvc_prefix.h"
#endif
#ifndef LIBSMBIOS_C_ABI_SUFFIX
#  define LIBSMBIOS_C_ABI_SUFFIX "smbios_c/config/abi/msvc_suffix.h"
#endif

#if _MSC_VER == 1310
#   define LIBSMBIOS_C_COMPILER_VERSION 7.1
# elif _MSC_VER == 1400
#   define LIBSMBIOS_C_COMPILER_VERSION 8.0
# elif _MSC_VER == 1500
#   define LIBSMBIOS_C_COMPILER_VERSION 9.0
# else
#   define LIBSMBIOS_C_COMPILER_VERSION _MSC_VER
#endif

#define LIBSMBIOS_C_COMPILER "Microsoft Visual C++ version " LIBSMBIOS_C_STRINGIZE(LIBSMBIOS_C_COMPILER_VERSION)

//
// versions check:
// we don't support Visual C++ prior to version 8:
// need variadic macros
#if _MSC_VER < 1400
#error "Compiler looks ancient. Sorry but we dont support it MSVC++ prior to version 8.0."
#endif
//
// last known and checked version is 1310:
#if (_MSC_VER > 1500)
#  if defined(LIBSMBIOS_C_ASSERT_CONFIG)
#     error "Unknown compiler version - please run the configure tests and report the results"
#  else
#     pragma message("Unknown compiler version - please run the configure tests and report the results")
#  endif
#endif
