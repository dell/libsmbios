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

#ifndef LIBSMBIOS_C_CONFIG_H
#define LIBSMBIOS_C_CONFIG_H

// if we don't have a user config, then use the default location:
#if !defined(LIBSMBIOS_C_USER_CONFIG) && !defined(LIBSMBIOS_C_NO_USER_CONFIG)
#  define LIBSMBIOS_C_USER_CONFIG <smbios_c/config/user.h>
#endif
// include it first:
#ifdef LIBSMBIOS_C_USER_CONFIG
#  include LIBSMBIOS_C_USER_CONFIG
#endif

// if we don't have a compiler config set, try and find one:
#if !defined(LIBSMBIOS_C_COMPILER_CONFIG) && !defined(LIBSMBIOS_C_NO_COMPILER_CONFIG) && !defined(LIBSMBIOS_C_NO_CONFIG)
#  include <smbios_c/config/select_compiler_config.h>
#endif
// if we have a compiler config, include it now:
#ifdef LIBSMBIOS_C_COMPILER_CONFIG
#  include LIBSMBIOS_C_COMPILER_CONFIG
#endif


// if we don't have a platform config set, try and find one:
#if !defined(LIBSMBIOS_C_PLATFORM_CONFIG) && !defined(LIBSMBIOS_C_NO_PLATFORM_CONFIG) && !defined(LIBSMBIOS_C_NO_CONFIG)
#  include <smbios_c/config/select_platform_config.h>
#endif
// if we have a platform config, include it now:
#ifdef LIBSMBIOS_C_PLATFORM_CONFIG
#  include LIBSMBIOS_C_PLATFORM_CONFIG
#endif

// get config suffix code:
#include <smbios_c/config/suffix.h>

#endif  // LIBSMBIOS_C_CONFIG_H











