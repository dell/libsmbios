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

// system
#include <sys/sysi86.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

void outb_p(int data, int port)
{
	__asm__ __volatile__("outb %b0,%w1" : : "a" (data), "Nd" (port));
}
uint8_t inb_p(int port)
{
	uint8_t v;
	__asm__ __volatile__("inb %w1,%b0" : "=a" (v) : "d" (port));
	return v;
}
int iopl(int v)
{
  return sysi86(SI86V86, V86SC_IOPL, 0x3000);
}

// public
#include "smbios_c/cmos.h"
#include "smbios_c/types.h"
#include "common_internal.h"
#include "libsmbios_c_intlize.h"

// private
#include "cmos_impl.h"

static int linux_read_fn(const struct cmos_access_obj *this, u8 *byte, u32 indexPort, u32 dataPort, u32 offset)
{
    outb_p (offset, indexPort);
    *byte = (inb_p (dataPort));
    fnprintf(" cmos read offset 0x%x = 0x%x\n", offset, *byte);
    return 0;
}

static int linux_write_fn(const struct cmos_access_obj *this, u8 byte, u32 indexPort, u32 dataPort, u32 offset)
{
    fnprintf(" cmos write: offset 0x%x = 0x%x\n", offset, byte);
    outb_p (offset, indexPort);
    outb_p (byte, dataPort);
    return 0;
}

int __internal init_cmos_struct(struct cmos_access_obj *m)
{
    char * errbuf;
    int retval = 0;

    fnprintf("\n");
    if(iopl(3) < 0)
        goto out_noprivs;

    m->read_fn = linux_read_fn;
    m->write_fn = linux_write_fn;

    retval = _init_cmos_std_stuff(m);
    goto out;

out_noprivs:
    fnprintf("out_noprivs:\n");
    retval = -1;
    errbuf = cmos_get_module_error_buf();
    if (errbuf)
    {
        strlcpy(errbuf, _("Error trying to raise IO Privilege level.\n"), ERROR_BUFSIZE);
        strlcat(errbuf, _("The OS Error string was: "), ERROR_BUFSIZE);
        fixed_strerror(errno, errbuf, ERROR_BUFSIZE);
        strlcat(errbuf, "\n", ERROR_BUFSIZE);
    }
    // nothing left to free
    goto out;

out:
    return retval;
}
