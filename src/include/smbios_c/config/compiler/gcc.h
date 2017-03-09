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

#define LIBSMBIOS_C_HAS_PRETTY_FUNCTION
#define LIBSMBIOS_C_HAS_FUNCTION
#define LIBSMBIOS_C_PACKED_ATTR      __attribute__ ((packed))
#define LIBSMBIOS_C_COMPILER "GNU C++ version " __VERSION__
#define LIBSMBIOS_C_HAS_LONG_LONG
#define LIBSMBIOS_C_HAS_VISIBILITY_ATTR

// versions check:
// we don't know gcc prior to version 2.90:
#if (__GNUC__ == 2) && (__GNUC_MINOR__ < 90)
#  error "GCC versions < 2.90 not supported"
#endif
//
