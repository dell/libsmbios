/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=c:cindent:textwidth=0:
 *
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

#define LIBSMBIOS_SOURCE

// Include compat.h first, then system headers, then public, then private
#include "smbios_c/compat.h"

#include <stdarg.h>     // va_list
#include <stdlib.h>
#include <stdio.h>
#include <string.h>     // memcpy
#include <errno.h>
#include <sys/mman.h>   // mmap


#include "smbios_c/memory.h"
#include "smbios_c/types.h"
#include "smbios_impl.h"

#if defined(DEBUG_MEMORY_C)
#   define dprintf(format, args...) do { fprintf(stdout , format , ## args);  } while(0)
#else
#   define dprintf(format, args...) do {} while(0)
#endif


