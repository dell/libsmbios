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

// system
//#include <stdlib.h>
//#include <string.h>

// public
#include "smbios_c/cmos.h"
#include "smbios_c/types.h"

// private
#include "token_impl.h"


// no error checking...
__internal u8 readByte(u32 indexPort, u32 dataPort, u32 offset)
{
    struct cmos_obj *c = cmos_factory(CMOS_GET_SINGLETON);
    u8 byte = 0;
    cmos_read_byte(c, &byte, indexPort, dataPort, offset);
    return byte;
}

__internal u8 byteChecksum( u32 start, u32 end, u32 indexPort, u32 dataPort )
{
    u8 running_checksum=0;
    for( u32 i = start; i <= end; i++)
        running_checksum += readByte( indexPort, dataPort, i );
    return running_checksum;
}


__internal u16 wordChecksum( u32 start, u32 end, u32 indexPort, u32 dataPort, bool complement )
{
    u16 running_checksum=0;
    for( u32 i = start; i <= end; i++)
        running_checksum += readByte( indexPort, dataPort, i );
    if( complement )
        running_checksum = (~running_checksum) + 1;
    return running_checksum;
}


__internal u16 wordCrc( u32 start, u32 end, u32 indexPort, u32 dataPort )
{
    u16 running_crc=0;

    for( u32 i = start; i <= end; i++)
    {
        running_crc ^= readByte( indexPort, dataPort, i );

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

    return running_crc;
}

