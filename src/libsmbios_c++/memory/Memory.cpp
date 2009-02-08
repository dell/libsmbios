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
#include <stdio.h>
#include <string.h>
#include <cstdio>

#include "MemoryImpl.h"

// include this file last
#include "smbios/message.h"

using namespace std;

namespace memory
{
    MemoryFactory::~MemoryFactory() throw()
    {}
    MemoryFactory::MemoryFactory()
    {}

    MemoryFactory *MemoryFactory::getFactory()
    {
        // reinterpret_cast<...>(0) to ensure template parameter is correct
        // this is a workaround for VC6 which cannot use explicit member template
        // function initialization.
        return MemoryFactoryImpl::getFactory(reinterpret_cast<MemoryFactoryImpl *>(0));
    }

    IMemory           *MemoryFactoryImpl::_mem_instance = 0;

    MemoryFactoryImpl::~MemoryFactoryImpl() throw()
    {
        if( _mem_instance )
        {
            delete _mem_instance;
        }
        _mem_instance = 0;
    }

    //
    // MemoryFactoryImpl::MemoryFactoryImpl()   // Constructor
    //  -- Moved to the Memory_OSNAME.cc file
    //     so that the default parameters can be OS-Specific
    //


    IMemory *MemoryFactoryImpl::getSingleton()
    {
        if (! _mem_instance)
            _mem_instance = makeNew();

        return _mem_instance;
    }

    IMemory *MemoryFactoryImpl::makeNew()
    {
        if (mode == UnitTestMode)
        {
            return new MemoryFile( getParameterString("memFile") );
        }
        else if (mode == AutoDetectMode)
        {
            return new MemoryOsSpecific( getParameterString("memFile") );
        }
        else
        {
            throw smbios::NotImplementedImpl( _("Unknown Memory mode requested.") );
        }
    }


    //
    // IMemory
    //

    IMemory::~IMemory ()
    {}

    IMemory::IMemory ()
    {}

    MemoryFile::MemoryFile( const string initFilename )
            : IMemory(), filename(initFilename), fd(0), rw(false), reopenHint(1)
    {
        // workaround MSVC++ bugs...
        if( filename == "" )
        {
            throw AccessErrorImpl( _("File name passed in was null or zero-length.") );
        }

        // fopen portable to Windows if "b" is added to mode.
        fd = fopen( filename.c_str(), "rb" ); // open for read to start
        if (!fd)
        {
            AccessErrorImpl accessError;
            accessError.setMessageString( _("Unable to open memory. File: %(file)s, OS Error: %(err)s") );
            accessError.setParameter( "file", filename );
            accessError.setParameter( "err", strerror(errno) );
            throw accessError;
        }

        if (reopenHint>0)
        {
            fclose(fd);
            fd=0;
        }
    }

    MemoryFile::~MemoryFile()
    {
        if (fd)
        {
            fclose(fd);
            fd = 0;
        }
    }

    u8 MemoryFile::getByte( u64 offset ) const
    {
        u8 value = 0;
        fillBuffer( &value, offset, 1 );
        return value;
    }


    void MemoryFile::fillBuffer(u8 *buffer, u64 offset, unsigned int length) const
    {
        if (!fd)
        {
            // fopen portable to Windows if "b" is added to mode.
            fd = fopen( filename.c_str(), "rb" ); // open for read to start
            if (!fd)
            {
                AccessErrorImpl accessError;
                accessError.setMessageString( _("Unable to open memory. File: %(file)s, OS Error: %(err)s") );
                accessError.setParameter( "file", filename );
                accessError.setParameter( "err", strerror(errno) );
                throw accessError;
            }
        }
        // FSEEK is a macro defined in config/ for portability
        int ret = FSEEK(fd, offset, 0);
        if (ret)
        {
            OutOfBoundsImpl outOfBounds;
            outOfBounds.setMessageString(_("Seek error trying to seek to memory location. OS Error: %(err)s"));
            outOfBounds.setParameter("err", strerror(errno) );
            fclose(fd);
            fd = 0;
            throw outOfBounds;
        }
        size_t recordsRead = fread( buffer, length, 1, fd );

        if (reopenHint>0)
        {
            fclose(fd);
            fd=0;
        }

        // TODO: handle short reads
        if ((1 != recordsRead))
        {
            AccessErrorImpl accessError;
            accessError.setMessageString(_("Read error trying to read memory. OS Error: %(err)s"));
            accessError.setParameter("err", strerror(errno) );
            if(fd) 
            {
                fclose(fd);
                fd = 0;
            }
            throw accessError;
        }
    }

    void MemoryFile::putByte( u64 offset, u8 byte ) const
    {
        if(!rw || !fd)
        {
            if(fd)
            {
                fclose(fd);
                fd = 0;
            }

            fd = fopen( filename.c_str(), "r+b" ); // reopen for write
            if(!fd)
            {
                AccessErrorImpl accessError;
                accessError.setMessageString( _("Unable to re-open memory file for writing. File: %(file)s, OS Error: %(err)s") );
                accessError.setParameter( "file", filename );
                accessError.setParameter( "err", strerror(errno) );
                throw accessError;
            }
        }
        // FSEEK is a macro defined in config/ for portability
        int ret = FSEEK(fd, offset, 0);
        if( 0 != ret )
        {
            OutOfBoundsImpl outOfBounds;
            outOfBounds.setMessageString(_("Seek error trying to seek to memory location. OS Error: %(err)s"));
            outOfBounds.setParameter("err", strerror(errno) );
            fclose(fd);
            fd = 0;
            throw outOfBounds;
        }
        size_t recordsWritten = fwrite( &byte, 1, 1, fd );
        if (reopenHint > 0)
        {
            fclose(fd);
            fd = 0;
        }
        if( 1 != recordsWritten )
        {
            AccessErrorImpl accessError;
            accessError.setMessageString(_("Error trying to write memory. OS Error: %(err)s"));
            accessError.setParameter("err", strerror(errno) );
            if(fd)
            {
                fclose(fd);
                fd = 0;
            }
            throw accessError;
        }
    }
}
