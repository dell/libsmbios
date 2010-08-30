//  (C) Copyright John Maddock 2001 - 2003.
//  (C) Copyright Jens Maurer 2001 - 2003.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  Aolaris specific config options:

#define LIBSMBIOS_PLATFORM "solaris"
#define LIBSMBIOS_PLATFORM_SOLARIS

// don't want to enable this just yet.
// // we can assume gettext on Solaris
// #define LIBSMBIOS_HAS_GETTEXT

// make sure we have __GLIBC_PREREQ if available at all
#include <cstdlib>

// If we are on IA64 we will need to macro define inb_p and outb_p
#if defined(__ia64__)
#    define outb_p outb
#    define inb_p  inb
#endif

// Enable 64-bit file access
#ifndef FSEEK
#define FSEEK(fh, pos, whence) fseeko(fh, static_cast<off_t>(pos), whence)
#endif

//
// <stdint.h> added to glibc 2.1.1
// We can only test for 2.1 though:
//
#if defined(__GLIBC__) && ((__GLIBC__ > 2) || ((__GLIBC__ == 2) && (__GLIBC_MINOR__ >= 1)))
   // <stdint.h> defines int64_t unconditionally, but <sys/types.h> defines
   // int64_t only if __GNUC__.  Thus, assume a fully usable <stdint.h>
   // only when using GCC.
#  if defined __GNUC__
#    define LIBSMBIOS_HAS_STDINT_H
#  endif
#endif

#if defined(__LIBCOMO__)
   //
   // como on linux doesn't have std:: c functions:
   // NOTE: versions of libcomo prior to beta28 have octal version numbering,
   // e.g. version 25 is 21 (dec)
   //
#  if __LIBCOMO_VERSION__ <= 20
#    define LIBSMBIOS_NO_STDC_NAMESPACE
#  endif

#  if __LIBCOMO_VERSION__ <= 21
#    define LIBSMBIOS_NO_SWPRINTF
#  endif

#endif

//
// If glibc is past version 2 then we definitely have
// gettimeofday, earlier versions may or may not have it:
//
#if defined(__GLIBC__) && (__GLIBC__ >= 2)
#  define LIBSMBIOS_HAS_GETTIMEOFDAY
#endif

#ifdef __USE_POSIX199309
#  define LIBSMBIOS_HAS_NANOSLEEP
#endif

#if defined(__GLIBC__) && defined(__GLIBC_PREREQ)
// __GLIBC_PREREQ is available since 2.1.2

   // swprintf is available since glibc 2.2.0
#  if !__GLIBC_PREREQ(2,2) || (!defined(__USE_ISOC99) && !defined(__USE_UNIX98))
#    define LIBSMBIOS_NO_SWPRINTF
#  endif
#else
#  define LIBSMBIOS_NO_SWPRINTF
#endif

// boilerplate code:
#define LIBSMBIOS_HAS_UNISTD_H
#include <smbios/config/posix_features.hpp>

#ifndef __GNUC__
//
// if the compiler is not gcc we still need to be able to parse
// the GNU system headers, some of which (mainly <stdint.h>)
// use GNU specific extensions:
//
#  ifndef __extension__
#     define __extension__
#  endif
#  ifndef __const__
#     define __const__ const
#  endif
#  ifndef __volatile__
#     define __volatile__ volatile
#  endif
#  ifndef __signed__
#     define __signed__ signed
#  endif
#  ifndef __typeof__
#     define __typeof__ typeof
#  endif
#  ifndef __inline__
#     define __inline__ inline
#  endif
#endif


