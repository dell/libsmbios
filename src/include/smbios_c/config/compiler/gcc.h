//  (C) Copyright John Maddock 2001 - 2003.
//  (C) Copyright Darin Adler 2001 - 2002.
//  (C) Copyright Jens Maurer 2001 - 2002.
//  (C) Copyright Beman Dawes 2001 - 2003.
//  (C) Copyright Douglas Gregor 2002.
//  (C) Copyright David Abrahams 2002 - 2003.
//  (C) Copyright Synge Todo 2003.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

#ifndef UNREFERENCED_PARAMETER
#   define UNREFERENCED_PARAMETER(P)  (void)(P)
#endif

#define LIBSMBIOS_C_HAS_PRETTY_FUNCTION
#define LIBSMBIOS_C_HAS_FUNCTION
#define LIBSMBIOS_C_PACKED_ATTR      __attribute__ ((packed))
#define LIBSMBIOS_C_COMPILER "GNU C++ version " __VERSION__
#define LIBSMBIOS_C_HAS_LONG_LONG

#define __internal __attribute__((visibility("internal")))
#define __hidden __attribute__((visibility("hidden")))
#define _null_call( args...) do {} while(0)
#define _dbg_printf(format, args...) do { fprintf(stderr , format , ## args);  } while(0)
// dbg_printf gets redefined to _dbg_printf on a source file by source file basis
#define dbg_printf _null_call
#define fnprintf(fmt, args...)  dbg_printf( "%s: " fmt, __PRETTY_FUNCTION__, ## args)

// versions check:
// we don't know gcc prior to version 2.90:
#if (__GNUC__ == 2) && (__GNUC_MINOR__ < 90)
#  error "GCC versions < 2.90 not supported"
#endif
//
// last known and checked version is 3.4:
#if (__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ > 4))
#  if defined(LIBSMBIOS_C_ASSERT_CONFIG)
#     error "Unknown compiler version - please run the configure tests and report the results"
#  else
#     warning "Unknown compiler version - please run the configure tests and report the results"
#  endif
#endif


