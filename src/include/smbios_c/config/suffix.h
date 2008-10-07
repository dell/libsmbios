//  Boost config.h configuration header file  ------------------------------//

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

//  Boost config.h policy and rationale documentation has been moved to
//  http://www.boost.org/libs/config
//
//  This file is intended to be stable, and relatively unchanging.
//  It should contain boilerplate code only - no compiler specific
//  code unless it is unavoidable - no changes unless unavoidable.

#ifndef LIBSMBIOS_C_CONFIG_SUFFIX_H
#define LIBSMBIOS_C_CONFIG_SUFFIX_H

//
// look for long long by looking for the appropriate macros in <limits.h>.
// Note that we use limits.h rather than climits for maximal portability,
// remember that since these just declare a bunch of macros, there should be
// no namespace issues from this.
//
#include <limits.h>
# if !defined(LIBSMBIOS_C_HAS_LONG_LONG)                                              \
   && !(defined(LIBSMBIOS_C_MSVC) && LIBSMBIOS_C_MSVC <=1300) && !defined(__BORLANDC__)     \
   && (defined(ULLONG_MAX) || defined(ULONG_LONG_MAX) || defined(ULONGLONG_MAX))
#  define LIBSMBIOS_C_HAS_LONG_LONG
#endif
#if !defined(LIBSMBIOS_C_HAS_LONG_LONG) && !defined(LIBSMBIOS_C_NO_INTEGRAL_INT64_T)
#  define LIBSMBIOS_C_NO_INTEGRAL_INT64_T
#endif

// GCC 3.x will clean up all of those nasty macro definitions that
// LIBSMBIOS_C_NO_CTYPE_FUNCTIONS is intended to help work around, so undefine
// it under GCC 3.x.
#if defined(__GNUC__) && (__GNUC__ >= 3) && defined(LIBSMBIOS_C_NO_CTYPE_FUNCTIONS)
#  undef LIBSMBIOS_C_NO_CTYPE_FUNCTIONS
#endif



//
// We can't have a <cwctype> if there is no <cwchar>:
//
#  if defined(LIBSMBIOS_C_NO_CWCHAR) && !defined(LIBSMBIOS_C_NO_CWCTYPE)
#     define LIBSMBIOS_C_NO_CWCTYPE
#  endif

//
// We can't have a swprintf if there is no <cwchar>:
//
#  if defined(LIBSMBIOS_C_NO_CWCHAR) && !defined(LIBSMBIOS_C_NO_SWPRINTF)
#     define LIBSMBIOS_C_NO_SWPRINTF
#  endif

//
// If Win32 support is turned off, then we must turn off
// threading support also, unless there is some other
// thread API enabled:
//
#if defined(LIBSMBIOS_C_DISABLE_WIN32) && defined(_WIN32) \
   && !defined(LIBSMBIOS_C_DISABLE_THREADS) && !defined(LIBSMBIOS_C_HAS_PTHREADS)
#  define LIBSMBIOS_C_DISABLE_THREADS
#endif

//
// Turn on threading support if the compiler thinks that it's in
// multithreaded mode.  We put this here because there are only a
// limited number of macros that identify this (if there's any missing
// from here then add to the appropriate compiler section):
//
#if (defined(__MT__) || defined(_MT) || defined(_REENTRANT) \
    || defined(_PTHREADS)) && !defined(LIBSMBIOS_C_HAS_THREADS)
#  define LIBSMBIOS_C_HAS_THREADS
#endif

//
// Turn threading support off if LIBSMBIOS_C_DISABLE_THREADS is defined:
//
#if defined(LIBSMBIOS_C_DISABLE_THREADS) && defined(LIBSMBIOS_C_HAS_THREADS)
#  undef LIBSMBIOS_C_HAS_THREADS
#endif

//
// Turn threading support off if we don't recognise the threading API:
//
#if defined(LIBSMBIOS_C_HAS_THREADS) && !defined(LIBSMBIOS_C_HAS_PTHREADS)\
      && !defined(LIBSMBIOS_C_HAS_WINTHREADS) && !defined(LIBSMBIOS_C_HAS_BETHREADS)\
      && !defined(LIBSMBIOS_C_HAS_MPTASKS)
#  undef LIBSMBIOS_C_HAS_THREADS
#endif

//
// Turn threading detail macros off if we don't (want to) use threading
//
#ifndef LIBSMBIOS_C_HAS_THREADS
#  undef LIBSMBIOS_C_HAS_PTHREADS
#  undef LIBSMBIOS_C_HAS_PTHREAD_MUTEXATTR_SETTYPE
#  undef LIBSMBIOS_C_HAS_WINTHREADS
#  undef LIBSMBIOS_C_HAS_BETHREADS
#  undef LIBSMBIOS_C_HAS_MPTASKS
#endif

//
// If the compiler claims to be C99 conformant, then it had better
// have a <stdint.h>:
//
#  if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
#     define LIBSMBIOS_C_HAS_STDINT_H
#  endif


//  LIBSMBIOS_C_HAS_ABI_HEADERS
//  This macro gets set if we have headers that fix the ABI,
//  and prevent ODR violations when linking to external libraries:
#if defined(LIBSMBIOS_C_ABI_PREFIX) && defined(LIBSMBIOS_C_ABI_SUFFIX) && !defined(LIBSMBIOS_C_HAS_ABI_HEADERS)
#  define LIBSMBIOS_C_HAS_ABI_HEADERS
#endif

#if defined(LIBSMBIOS_C_HAS_ABI_HEADERS) && defined(LIBSMBIOS_C_DISABLE_ABI_HEADERS)
#  undef LIBSMBIOS_C_HAS_ABI_HEADERS
#endif



// LIBSMBIOS_C_UNREACHABLE_RETURN(x) workaround -------------------------------------//
// Normally evaluates to nothing, unless LIBSMBIOS_C_NO_UNREACHABLE_RETURN_DETECTION
// is defined, in which case it evaluates to return x; Use when you have a return
// statement that can never be reached.

#ifdef LIBSMBIOS_C_NO_UNREACHABLE_RETURN_DETECTION
#  define LIBSMBIOS_C_UNREACHABLE_RETURN(x) return x;
#else
#  define LIBSMBIOS_C_UNREACHABLE_RETURN(x)
#endif


// ---------------------------------------------------------------------------//

//
// Helper macro LIBSMBIOS_C_STRINGIZE:
// Converts the parameter X to a string after macro replacement
// on X has been performed.
//
#define LIBSMBIOS_C_STRINGIZE(X) LIBSMBIOS_C_DO_STRINGIZE(X)
#define LIBSMBIOS_C_DO_STRINGIZE(X) #X

//
// Helper macro LIBSMBIOS_C_JOIN:
// The following piece of macro magic joins the two
// arguments together, even when one of the arguments is
// itself a macro (see 16.3.1 in C++ standard).  The key
// is that macro expansion of macro arguments does not
// occur in LIBSMBIOS_C_DO_JOIN2 but does in LIBSMBIOS_C_DO_JOIN.
//
#define LIBSMBIOS_C_JOIN( X, Y ) LIBSMBIOS_C_DO_JOIN( X, Y )
#define LIBSMBIOS_C_DO_JOIN( X, Y ) LIBSMBIOS_C_DO_JOIN2(X,Y)
#define LIBSMBIOS_C_DO_JOIN2( X, Y ) X##Y

//
// Set some default values for compiler/library/platform names.
// These are for debugging config setup only:
//
#  ifndef LIBSMBIOS_C_COMPILER
#     define LIBSMBIOS_C_COMPILER "Unknown ISO C++ Compiler"
#  endif
#  ifndef LIBSMBIOS_C_STDLIB
#     define LIBSMBIOS_C_STDLIB "Unknown ISO standard library"
#  endif
#  ifndef LIBSMBIOS_C_PLATFORM
#     if defined(unix) || defined(__unix) || defined(_XOPEN_SOURCE) \
         || defined(_POSIX_SOURCE)
#        define LIBSMBIOS_C_PLATFORM "Generic Unix"
#     else
#        define LIBSMBIOS_C_PLATFORM "Unknown"
#     endif
#  endif

#endif



