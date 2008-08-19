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

#include <sys/io.h>

#include "CmosRWImpl.h"

using namespace std;

namespace cmos
{

    //
    // CmosRWIo functions
    //

    // REGULAR CONSTRUCTOR
    CmosRWIo::CmosRWIo ()
            : ICmosRW(), Suppressable()
    {}

    // COPY CONSTRUCTOR
    CmosRWIo::CmosRWIo (const CmosRWIo &)
            : ICmosRW(), Suppressable()
    {}

    // OVERLOADED ASSIGNMENT
    CmosRWIo & CmosRWIo::operator = (const CmosRWIo &)
    {
        return *this;
    }

    // TODO: need to throw exception on problem with iopl
    //
    u8 CmosRWIo::readByte (u32 indexPort, u32 dataPort, u32 offset) const
    {
        if(iopl(3) < 0)
            throw smbios::InternalErrorImpl("iopl() failed. probably not root.");
        outb_p (offset, indexPort);
        return (inb_p (dataPort));
    }

    // TODO: need to throw exception on problem with iopl
    //
    void CmosRWIo::writeByte (u32 indexPort, u32 dataPort, u32 offset, u8 byte) const
    {
        if(iopl(3) < 0)
            throw smbios::InternalErrorImpl("iopl() failed. probably not root.");
        outb_p (offset, indexPort);
        outb_p (byte, dataPort);

        if(! isNotifySuppressed() )
        {
            // writers are responsible for only writing changed values
            // otherwise we get to see how fast Linux can do an 
            // infinite loop. :-)
            notify(); 
        }
    }

}
