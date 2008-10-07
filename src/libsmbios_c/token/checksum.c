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
//#include <stdlib.h>
//#include <string.h>

// public
#include "smbios_c/obj/cmos.h"
#include "smbios_c/cmos.h"
#include "smbios_c/types.h"

// private
#include "token_impl.h"

__internal int update_checksum(const struct cmos_access_obj *c, bool do_update, void *userdata)
{
    int retval = -1;
    struct checksum_details *data = (struct checksum_details *)userdata;

    u16 wordRetval = data->csum_fn(data->start, data->end, data->indexPort, data->dataPort);
    const u8 *csum = (const u8 *)(&wordRetval);

    u32 actualcsum = 0;
    for( int i=0; i<data->csumlen; ++i )
    {
        u8 byte;
        int ret = cmos_read_byte(&byte, data->indexPort, data->dataPort, data->csumloc+i);
        if (ret)
            goto out;

        actualcsum = (actualcsum << 8) | byte;
    }

#ifdef DEBUG_TOKEN_C
    if (actualcsum != wordRetval)
    {
        u8 byteC = byteChecksum(data->start, data->end, data->indexPort, data->dataPort);
        u16 C = wordChecksum(data->start, data->end, data->indexPort, data->dataPort);
        u16 Cn = wordChecksum_n(data->start, data->end, data->indexPort, data->dataPort);
        u16 Crc = wordCrc(data->start, data->end, data->indexPort, data->dataPort);
        dprintf("%s - start %d end %d location %d indexPort %x\n", __PRETTY_FUNCTION__, data->start, data->end, data->csumloc,  data->indexPort);
        dprintf("%s - actual %x (%d), calculated %x\n", __PRETTY_FUNCTION__, actualcsum, data->csumlen, wordRetval);
        dprintf("%s - byte(%x) wordcsum(%x) wordcsum_n(%x) wordCrc(%x)\n", __PRETTY_FUNCTION__, byteC, C, Cn, Crc);
    }
#endif

    if(do_update && actualcsum != wordRetval)
        for( int i=0; i<data->csumlen; ++i )
        {
            int ret = cmos_write_byte( data->indexPort, data->dataPort, data->csumloc+i, csum[data->csumlen -i -1]);
            if (ret)
                goto out;
        }

    retval = 1;
    if (actualcsum != wordRetval)
        goto out;

    retval = 0;

out:
    return retval;
}

__internal u16 byteChecksum( u32 start, u32 end, u32 indexPort, u32 dataPort )
{
    u8 running_checksum=0;
    u8 byte;
    for( u32 i = start; i <= end; i++) {
        if(cmos_read_byte(&byte, indexPort, dataPort, i ))
            goto out;
        running_checksum += byte;
    }
out:
    return running_checksum;
}

__internal u16 wordChecksum( u32 start, u32 end, u32 indexPort, u32 dataPort)
{
    u16 running_checksum=0;
    u8 byte;
    for( u32 i = start; i <= end; i++) {
        if(cmos_read_byte(&byte, indexPort, dataPort, i ))
            goto out;
        running_checksum += byte;
    }
out:
    return running_checksum;
}

__internal u16 wordChecksum_n( u32 start, u32 end, u32 indexPort, u32 dataPort)
{
    return (~wordChecksum(start, end, indexPort, dataPort)) + 1;
}

__internal u16 wordCrc( u32 start, u32 end, u32 indexPort, u32 dataPort )
{
    u16 running_crc=0;
    u8 byte;

    for( u32 i = start; i <= end; i++)
    {
        if(cmos_read_byte(&byte, indexPort, dataPort, i ))
            goto out;
        running_crc ^= byte;

        for( int j=0; j<7; j++ )
        {
            u16 temp = running_crc & 0x0001;
            running_crc >>= 1;
            if( temp != 0 )
            {
                running_crc |= 0x8000;
                running_crc ^= 0xA001;
            }
        }
    }
out:
    return running_crc;
}

