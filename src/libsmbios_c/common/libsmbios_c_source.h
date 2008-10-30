//  Boost config.h configuration header file  ------------------------------//

//  (C) Copyright John Maddock 2002.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/config for most recent version.

//  Boost config.h policy and rationale documentation has been moved to
//  http://www.boost.org/libs/config
//
//  CAUTION: This file is intended to be completely stable -
//           DO NOT MODIFY THIS FILE!
//
//  Modified for libsmbios project: 2004-03-28 by Michael Brown

#ifndef LIBSMBIOS_C_INTERNAL_CONFIG_H
#define LIBSMBIOS_C_INTERNAL_CONFIG_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

// if we don't have a compiler config set, try and find one:
#if !defined(LIBSMBIOS_C_INTERNAL_COMPILER_CONFIG) && !defined(LIBSMBIOS_C_INTERNAL_NO_COMPILER_CONFIG) && !defined(LIBSMBIOS_C_INTERNAL_NO_CONFIG)
#  include <select_compiler_config.h>
#endif
// if we have a compiler config, include it now:
#ifdef LIBSMBIOS_C_INTERNAL_COMPILER_CONFIG
#  include LIBSMBIOS_C_INTERNAL_COMPILER_CONFIG
#endif


// if we don't have a platform config set, try and find one:
#if !defined(LIBSMBIOS_C_INTERNAL_PLATFORM_CONFIG) && !defined(LIBSMBIOS_C_INTERNAL_NO_PLATFORM_CONFIG) && !defined(LIBSMBIOS_C_INTERNAL_NO_CONFIG)
#  include <select_platform_config.h>
#endif
// if we have a platform config, include it now:
#ifdef LIBSMBIOS_C_INTERNAL_PLATFORM_CONFIG
#  include LIBSMBIOS_C_INTERNAL_PLATFORM_CONFIG
#endif

#endif  // LIBSMBIOS_C_INTERNAL_CONFIG_H
