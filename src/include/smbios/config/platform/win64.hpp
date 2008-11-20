//  (C) Copyright John Maddock 2001 - 2003.
//  (C) Copyright Bill Kempf 2001.
//  (C) Copyright Aleksey Gurtovoy 2003.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  Win64 specific config options:

#define LIBSMBIOS_PLATFORM "Win64"
#define LIBSMBIOS_PLATFORM_WIN64

// Windows has a special platform-specific smbios table accessor method
// that can be used if generic memory access fails.
#define LIBSMBIOS_HAS_ARCH_TABLE_CLASS

// Enable 64-bit file access (changes off_t to 64-bit)
#ifndef FSEEK
#define FSEEK(fh, pos, whence) fseek(fh, static_cast<long>(pos), whence)
#endif

#if defined(__GNUC__) && !defined(LIBSMBIOS_NO_SWPRINTF)
#  define LIBSMBIOS_NO_SWPRINTF
#endif

#if !defined(__GNUC__) && !defined(LIBSMBIOS_HAS_DECLSPEC)
#  define LIBSMBIOS_HAS_DECLSPEC
#endif

#if defined(__MINGW32__) && ((__GNUC__ > 3) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 2)))
#  define LIBSMBIOS_HAS_STDINT_H
#  define __STDC_LIMIT_MACROS
#endif

// workaround for broken namespace on win64 compiler
#define LIBSMBIOS_NO_STDC_NAMESPACE

//
// Win64 will normally be using native Win64 threads,
// but there is a pthread library available as an option,
// we used to disable this when LIBSMBIOS_DISABLE_WIN64 was
// defined but no longer - this should allow some
// files to be compiled in strict mode - while maintaining
// a consistent setting of LIBSMBIOS_HAS_THREADS across
// all translation units (needed for shared_ptr etc).
//
#ifndef LIBSMBIOS_HAS_PTHREADS
#  define LIBSMBIOS_HAS_WINTHREADS
#endif

#ifndef LIBSMBIOS_DISABLE_WIN64
// WEK: Added
#define LIBSMBIOS_HAS_FTIME

#endif

//
// disable min/max macros:
//
#ifdef min
#  undef min
#endif
#ifdef max
#  undef max
#endif
#ifndef NOMINMAX
#  define NOMINMAX
#endif

#ifdef LIBSMBIOS_MSVC
#include <algorithm> // for existing std::min and std::max
namespace std{
  // Apparently, something in the Microsoft libraries requires the "long"
  // overload, because it calls the min/max functions with arguments of
  // slightly different type.  (If this proves to be incorrect, this
  // whole "LIBSMBIOS_MSVC" section can be removed.)
  inline long min(long __a, long __b) {
    return __b < __a ? __b : __a;
  }
  inline long max(long __a, long __b) {
    return  __a < __b ? __b : __a;
  }
  // The "long double" overload is required, otherwise user code calling
  // min/max for floating-point numbers will use the "long" overload.
  // (SourceForge bug #495495)
  inline long double min(long double __a, long double __b) {
    return __b < __a ? __b : __a;
  }
  inline long double max(long double __a, long double __b) {
    return  __a < __b ? __b : __a;
  }
}
using std::min;
using std::max;
#     endif


