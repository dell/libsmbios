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

#define LIBSMBIOS_C_SOURCE

// Include compat.h first, then system headers, then public, then private
#include "smbios_c/compat.h"

#include <stdio.h>

// public
#include "smbios_c/cmos.h"
#include "smbios_c/types.h"
#include "common_internal.h"
#include "libsmbios_c_intlize.h"

// private
#include "cmos_impl.h"
#include "common_windows.h"

static int windows_read_fn(const struct cmos_access_obj *this, u8 *byte, u32 indexPort, u32 dataPort, u32 offset)
{
    NTSTATUS status;
    IO_STRUCT io;
    int retval = -1;

    memset(&io, 0, sizeof(io));
    io.Addr = indexPort;
    io.pBuf = &offset;
    io.NumBytes = 1;
    io.Reserved4 = 1;
    io.Reserved6 = 1;

    status = ZwSystemDebugControl(DebugSysWriteIoSpace, &io, sizeof(io), NULL, 0, NULL);
    if (!NT_SUCCESS(status))
        goto out;

    memset(&io, 0, sizeof(io));
    io.Addr = dataPort;
    io.pBuf = byte;
    io.NumBytes = 1;
    io.Reserved4 = 1;
    io.Reserved6 = 1;

    status = ZwSystemDebugControl(DebugSysReadIoSpace, &io, sizeof(io), NULL, 0, NULL);
    if (!NT_SUCCESS(status))
        goto out;

    retval = 0;

out:
    return retval;
}


int __hidden init_cmos_struct(struct cmos_access_obj *m)
{
    printf("Loaded Windows CMOS STUFF. not yet ready.\n");

    if (!LoadNtdllFuncs())
        goto out_err;

    if (!EnableDebug())
        goto out_err;

    m->read_fn = windows_read_fn;

    return -1; /// NOT YET IMPLEMENTED
    goto out;

out_err:
    return -1;

out:
    return 0;
}

