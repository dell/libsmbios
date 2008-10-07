/* vim:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=c:cindent:
 */
/*
 * Copyright (C) 2005 Dell Inc.
 *  by Michael Brown <Michael_E_Brown@dell.com>
 * Licensed under the Open Software License version 2.1
 *
 * Alternatively, you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.

 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 */


#ifndef LIBSMBIOS_C_COMPAT_H_INCLUDED
#define LIBSMBIOS_C_COMPAT_H_INCLUDED

#include "smbios_c/config/config.h"
#include "smbios_c/config/auto_link.h"

/*
 * CHANGES TO THIS FILE CAUSE A WHOLE-PROJECT REBUILD!
 *      Keep changes here to a minimum!
*/

#if defined(LIBSMBIOS_C_HAS_DECLSPEC) && defined(LIBSMBIOS_C_ALL_DYN_LINK)
#       if defined(LIBSMBIOS_SOURCE) || defined(LIBSMBIOS_SOURCE)
#           define DLL_SPEC  __declspec(dllexport)
#       else
#           define DLL_SPEC  __declspec(dllimport)
#       endif  /* SMBIOS_EXPORTS */
#endif

#ifndef DLL_SPEC
#   define DLL_SPEC
#endif

#ifdef __cplusplus
#define EXTERN_C_BEGIN extern "C" {
#define EXTERN_C_END   }
#else
#define EXTERN_C_BEGIN
#define EXTERN_C_END
#endif

#ifdef LIBSMBIOS_C_PLATFORM_WIN32
#   define WIN32_LEAN_AND_MEAN		/* Exclude rarely-used stuff from Windows headers */
#   include <windows.h>
/* Windows is lame. */
#   define _snprintf    snprintf

#endif /* LIBSMBIOS_C_PLATFORM_WIN32 */
#endif /* COMPAT_H */
