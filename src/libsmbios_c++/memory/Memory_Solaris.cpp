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

// compat header should always be first header if including system headers
#define LIBSMBIOS_SOURCE
#include "smbios/compat.h"

#include <errno.h>
#include <sys/mman.h>   /* mmap */
#include <unistd.h>     /* getpagesize */
#include <string.h>     /* memcpy etc. */
#include <cstdio>

#include "MemoryImpl.h"

// include this last
#include "smbios/message.h"

using namespace std;
using namespace factory;

namespace memory
{
    struct SolarisData
    {
        FILE *fd;
        void *lastMapping;
        unsigned long lastMappedOffset;
        unsigned long mappingSize;
        int reopenHint;
        string filename;
    };

    static void condOpenFd(struct SolarisData *data)
    {
        if(!data->fd)
        {
            data->lastMapping = 0;
            data->lastMappedOffset = 0;
            data->fd = fopen( data->filename.c_str(), "rb" );
            if(!data->fd)
            {
                AccessErrorImpl accessError;
                accessError.setMessageString( _("Unable to open memory. File: %(file)s, OS Error: %(err)s") );
                accessError.setParameter( "file", data->filename );
                accessError.setParameter( "err", strerror(errno) );
                throw accessError;
            }
        }
    }

    static void closeFd(struct SolarisData *data)
    {
        if(data->lastMapping)
        {
            munmap((char *)data->lastMapping, data->mappingSize);
            data->lastMapping = 0;
        }
        if (data->fd)
        {
            fclose(data->fd);
            data->fd = 0;
        }
        data->lastMappedOffset = 0;
    }

    MemoryFactoryImpl::MemoryFactoryImpl()
    {
        setParameter("memFile", "/dev/xsvc");
    }

    MemoryOsSpecific::MemoryOsSpecific( const string filename )
            : IMemory()
    {
        SolarisData *data = new SolarisData();
        data->fd = 0;
        data->filename = filename;
        data->mappingSize = getpagesize() * 16;
        data->reopenHint = 1;
        condOpenFd(data);
        closeFd(data);
        osData = static_cast<void *>(data);
    }

    MemoryOsSpecific::~MemoryOsSpecific()
    {
        SolarisData *data = static_cast<SolarisData *>(osData);
        closeFd(data);
        delete data;
        osData = 0;
    }

    int MemoryOsSpecific::incReopenHint()
    {
        SolarisData *data = static_cast<SolarisData *>(osData);
        return ++(data->reopenHint);
    }
    int MemoryOsSpecific::decReopenHint()
    {
        SolarisData *data = static_cast<SolarisData *>(osData);
        return --(data->reopenHint);
    }

    void MemoryOsSpecific::fillBuffer( u8 *buffer, u64 offset, unsigned int length) const
    {
        SolarisData *data = static_cast<SolarisData *>(osData);
        unsigned int bytesCopied = 0;

        condOpenFd(data);

        while( bytesCopied < length )
        {
            off_t mmoff = offset % data->mappingSize;

            if ((offset-mmoff) != data->lastMappedOffset)
            {
                data->lastMappedOffset = offset-mmoff;
                if (data->lastMapping)
                    munmap((char *)data->lastMapping, data->mappingSize);
                data->lastMapping = mmap( 0, data->mappingSize, PROT_READ, MAP_SHARED, fileno(data->fd), offset-mmoff);
                if ((data->lastMapping) == reinterpret_cast<void *>(-1))
                    throw AccessErrorImpl(_("mmap failed."));
            }

            unsigned long toCopy = length - bytesCopied;
            if( toCopy + mmoff > (data->mappingSize) )
                toCopy = (data->mappingSize) - mmoff;

            memcpy(buffer + bytesCopied, (reinterpret_cast<const u8 *>(data->lastMapping) + mmoff), toCopy);
            offset += toCopy;
            bytesCopied += toCopy;
        }

        if(data->reopenHint)
            closeFd(data);

    }

    u8 MemoryOsSpecific::getByte( u64 offset ) const
    {
        u8 value=0;
        fillBuffer( &value, offset, 1 );
        return value;
    }

    void MemoryOsSpecific::putByte( u64 offset, u8 value ) const
    {
        SolarisData *data = static_cast<SolarisData *>(osData);
        condOpenFd(data);
        int ret = fseek( data->fd, offset, 0 );
        if( 0 != ret )
        {
            OutOfBoundsImpl outOfBounds;
            outOfBounds.setMessageString(_("Seek error trying to seek to memory location. OS Error: %(err)s"));
            outOfBounds.setParameter("err", strerror(errno) );
            closeFd(data);
            throw outOfBounds;
        }
        ret = fwrite( &value, 1, 1, data->fd );
        if( 1 != ret )
        {
            AccessErrorImpl accessError;
            accessError.setMessageString(_("Error trying to write memory. OS Error: %(err)s"));
            accessError.setParameter("err", strerror(errno) );
            closeFd(data);
            throw accessError;
        }
        if(data->reopenHint)
            closeFd(data);
    }

}
