// vim:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=c:
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


#ifndef TYPES_H
#define TYPES_H

// compat header should always be first header
#include "smbios/compat.h"

typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;

typedef char   s8;
typedef short  s16;
typedef int    s32;

#if defined(LIBSMBIOS_HAS_LONG_LONG)
typedef unsigned long long  u64;
typedef long long  s64;
#elif defined(LIBSMBIOS_HAS_MS_INT64)
typedef unsigned __int64   u64;
typedef __int64 s64;
#else
#error  "No LONG LONG or __INT64 support. Current compiler config is not supported."
#endif

#endif /* TYPES_H */
